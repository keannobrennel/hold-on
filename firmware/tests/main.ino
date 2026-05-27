/*
  HoldON Local Prototype Firmware — GY-91 Version, NO Firebase

  Use this when your motion sensor is GY-91 / MPU9250-style, not MPU6050.

  What this sketch does:
  - ESP8266 HTTP API for Vue frontend, no Firebase and no backend
  - GY-91 raw I2C read:
      MPU9250/MPU9255/MPU6500 at 0x68 or 0x69
      BMP/BME chip ID check at 0x76 or 0x77
  - NEO-7M GPS using SoftwareSerial on D7
  - SH1106 OLED on I2C D1/D2
  - FSR analog sensor on A0
  - Button on D5
  - Buzzer on D6
  - Trip start/cancel/manual panic/sensor panic/not-moving/arrival states

  Required libraries:
  - TinyGPSPlus
  - U8g2

  No Adafruit_MPU6050 library needed.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <U8g2lib.h>
#include <math.h>

// ===================== USER CONFIG =====================

const char* WIFI_SSID = "bbrouter";
const char* WIFI_PASS = "107149090195";

// Set false for indoor bench testing.
// Set true for final demo so trip cannot start without GPS fix.
const bool REQUIRE_GPS_FIX_TO_START = false;

// OLED is your 1.3-inch SH1106.
const uint8_t OLED_I2C_ADDR = 0x3C;

// ===================== PIN MAP =====================

const int SDA_PIN = D2; // GPIO4, I2C SDA
const int SCL_PIN = D1; // GPIO5, I2C SCL

const uint8_t PIN_BUTTON = D5; // GPIO14, button to GND
const uint8_t PIN_BUZZER = D6; // GPIO12

const uint8_t PIN_GPS_RX = D7; // GPIO13, GPS TX goes here
const uint8_t PIN_GPS_TX = D8; // GPIO15, optional; leave GPS RX disconnected

const uint8_t PIN_FSR = A0;

// ===================== THRESHOLDS =====================

const int FSR_THRESHOLD = 700;
const unsigned long FSR_SUSTAIN_MS = 2000;

const float ACCEL_JOLT_G = 2.5;
const float GYRO_JOLT_DPS = 150.0;
const unsigned long SENSOR_WINDOW_MS = 2000;

const unsigned long SENSOR_SAMPLE_MS = 100;
const unsigned long OLED_REFRESH_MS = 1000;

const unsigned long DOUBLE_PRESS_MS = 400;
const unsigned long LONG_PRESS_MS = 3000;
const unsigned long COUNTDOWN_MS = 5000;

const double NOT_MOVING_DISTANCE_M = 10.0;
const unsigned long NOT_MOVING_WINDOW_MS = 60000;

const double ARRIVAL_RADIUS_M = 50.0;

// ===================== OBJECTS =====================

ESP8266WebServer server(80);
SoftwareSerial gpsSerial(PIN_GPS_RX, PIN_GPS_TX);
TinyGPSPlus gps;

// SH1106 constructor for 1.3-inch OLED.
// If you later use SSD1306, replace this constructor with:
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ===================== STATE =====================

enum DeviceState {
  STATE_GPS_ACQUIRING,
  STATE_READY,
  STATE_ACTIVE,
  STATE_CANCEL_PROMPT,
  STATE_COUNTDOWN,
  STATE_PANIC_MONITORING,
  STATE_NOT_MOVING,
  STATE_ARRIVED,
  STATE_CANCELLED
};

DeviceState state = STATE_GPS_ACQUIRING;

String tripId = "local-demo-001";

bool destinationSet = false;
double destLat = 14.599512;
double destLng = 120.984222;

// GY-91 addresses
uint8_t mpuAddr = 0;
uint8_t bmpAddr = 0;
uint8_t mpuWhoAmI = 0;
uint8_t bmpChipId = 0;

// GPS
bool gpsHasFix = false;
double currentLat = 0.0;
double currentLng = 0.0;
uint32_t gpsChars = 0;
String lastNmea = "";
String currentNmea = "";
unsigned long lastGpsCharMs = 0;

// Sensors
int fsrRaw = 0;
bool mpuOk = false;
bool bmpOk = false;

int16_t axRaw = 0, ayRaw = 0, azRaw = 0;
int16_t gxRaw = 0, gyRaw = 0, gzRaw = 0;

float axG = 0.0, ayG = 0.0, azG = 0.0;
float gxDps = 0.0, gyDps = 0.0, gzDps = 0.0;

float accelMagG = 0.0;
float gyroMagDps = 0.0;

// Timers
unsigned long lastSensorSampleMs = 0;
unsigned long lastOledRefreshMs = 0;

unsigned long fsrOverStartMs = 0;
unsigned long motionEventUntilMs = 0;
bool sensorPanicAlreadySent = false;

unsigned long movementWindowStartMs = 0;
double movementStartLat = 0.0;
double movementStartLng = 0.0;
bool movementWindowHasGps = false;

unsigned long countdownStartMs = 0;
int lastCountdownNumber = -1;

// Button debounce/click state
bool stableButtonPressed = false;
bool lastButtonReading = false;
unsigned long lastDebounceMs = 0;
const unsigned long DEBOUNCE_MS = 35;

unsigned long buttonDownAtMs = 0;
bool longPressStarted = false;
int clickCount = 0;
unsigned long lastReleaseMs = 0;

// Latest local event for Vue map/event display
String lastEventType = "none";
unsigned long lastEventMs = 0;
double lastEventLat = 0.0;
double lastEventLng = 0.0;
bool lastEventHasGps = false;

// ===================== JSON / STRING HELPERS =====================

String quote(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\r", "\\r");
  s.replace("\n", "\\n");
  return "\"" + s + "\"";
}

String boolJson(bool v) {
  return v ? "true" : "false";
}

String ipToString(IPAddress ip) {
  return String(ip[0]) + "." + ip[1] + "." + ip[2] + "." + ip[3];
}

String hexByte(uint8_t v) {
  String s = String(v, HEX);
  s.toUpperCase();
  if (s.length() < 2) s = "0" + s;
  return "0x" + s;
}

const char* stateName() {
  switch (state) {
    case STATE_GPS_ACQUIRING: return "gps_acquiring";
    case STATE_READY: return "ready";
    case STATE_ACTIVE: return "active";
    case STATE_CANCEL_PROMPT: return "cancel_prompt";
    case STATE_COUNTDOWN: return "countdown";
    case STATE_PANIC_MONITORING: return "panic_monitoring";
    case STATE_NOT_MOVING: return "not_moving";
    case STATE_ARRIVED: return "arrived";
    case STATE_CANCELLED: return "cancelled";
    default: return "unknown";
  }
}

// ===================== I2C / GY-91 HELPERS =====================

bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

uint8_t readReg(uint8_t addr, uint8_t reg, bool* ok = nullptr) {
  Wire.beginTransmission(addr);
  Wire.write(reg);

  uint8_t err = Wire.endTransmission(false);
  if (err != 0) {
    if (ok) *ok = false;
    return 0;
  }

  uint8_t n = Wire.requestFrom((int)addr, 1);
  if (n != 1) {
    if (ok) *ok = false;
    return 0;
  }

  if (ok) *ok = true;
  return Wire.read();
}

bool writeReg(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  uint8_t n = Wire.requestFrom((int)addr, (int)len);
  if (n != len) {
    return false;
  }

  for (uint8_t i = 0; i < len; i++) {
    buf[i] = Wire.read();
  }

  return true;
}

int16_t be16(uint8_t hi, uint8_t lo) {
  return (int16_t)((hi << 8) | lo);
}

String scanI2cArrayJson() {
  String json = "[";
  bool first = true;

  for (uint8_t addr = 1; addr < 127; addr++) {
    if (i2cPresent(addr)) {
      if (!first) json += ",";
      json += quote(hexByte(addr));
      first = false;
    }
  }

  json += "]";
  return json;
}

uint8_t findFirstPresent(const uint8_t* addresses, uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    if (i2cPresent(addresses[i])) {
      return addresses[i];
    }
  }

  return 0;
}

void initGy91() {
  uint8_t mpuCandidates[] = {0x68, 0x69};
  uint8_t bmpCandidates[] = {0x76, 0x77};

  mpuAddr = findFirstPresent(mpuCandidates, 2);
  bmpAddr = findFirstPresent(bmpCandidates, 2);

  if (mpuAddr) {
    // Wake MPU-9250/MPU-9255/MPU-6500-style device.
    writeReg(mpuAddr, 0x6B, 0x00);
    delay(50);

    // Configure known defaults explicitly.
    // ACCEL_CONFIG 0x1C = 0x00 means ±2G.
    // GYRO_CONFIG  0x1B = 0x00 means ±250 degrees/second.
    writeReg(mpuAddr, 0x1C, 0x00);
    writeReg(mpuAddr, 0x1B, 0x00);
    delay(20);

    bool ok = false;
    mpuWhoAmI = readReg(mpuAddr, 0x75, &ok);
    mpuOk = ok;

    Serial.print("MPU found at ");
    Serial.print(hexByte(mpuAddr));
    Serial.print(" WHO_AM_I=");
    Serial.println(hexByte(mpuWhoAmI));
  } else {
    mpuOk = false;
    Serial.println("MPU not found at 0x68 or 0x69.");
  }

  if (bmpAddr) {
    bool ok = false;
    bmpChipId = readReg(bmpAddr, 0xD0, &ok);
    bmpOk = ok;

    Serial.print("BMP/BME found at ");
    Serial.print(hexByte(bmpAddr));
    Serial.print(" chipId=");
    Serial.println(hexByte(bmpChipId));
  } else {
    bmpOk = false;
    Serial.println("BMP/BME not found at 0x76 or 0x77.");
  }
}

bool readGy91Mpu() {
  if (!mpuAddr) {
    mpuOk = false;
    return false;
  }

  uint8_t b[14];

  if (!readBytes(mpuAddr, 0x3B, b, 14)) {
    mpuOk = false;
    return false;
  }

  axRaw = be16(b[0], b[1]);
  ayRaw = be16(b[2], b[3]);
  azRaw = be16(b[4], b[5]);

  gxRaw = be16(b[8], b[9]);
  gyRaw = be16(b[10], b[11]);
  gzRaw = be16(b[12], b[13]);

  // Because initGy91 sets accel ±2G and gyro ±250 dps:
  axG = axRaw / 16384.0;
  ayG = ayRaw / 16384.0;
  azG = azRaw / 16384.0;

  gxDps = gxRaw / 131.0;
  gyDps = gyRaw / 131.0;
  gzDps = gzRaw / 131.0;

  accelMagG = sqrt(axG * axG + ayG * ayG + azG * azG);
  gyroMagDps = sqrt(gxDps * gxDps + gyDps * gyDps + gzDps * gzDps);

  mpuOk = true;
  return true;
}

// ===================== BUZZER / OLED =====================

void beepOnce(uint16_t freq = 2200, uint16_t ms = 80) {
  tone(PIN_BUZZER, freq, ms);
  delay(ms + 10);
  noTone(PIN_BUZZER);
}

void beepPattern(uint8_t count, uint16_t freq = 2200, uint16_t ms = 80, uint16_t gap = 70) {
  for (uint8_t i = 0; i < count; i++) {
    tone(PIN_BUZZER, freq, ms);
    delay(ms);
    noTone(PIN_BUZZER);
    delay(gap);
  }
}

String gpsSummary() {
  if (gpsHasFix) {
    return String(currentLat, 5) + "," + String(currentLng, 5);
  }

  if (gpsChars > 0) {
    return "GPS data, no fix";
  }

  return "GPS no data";
}

void drawOled(String title, String l1 = "", String l2 = "", String l3 = "", String l4 = "") {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  u8g2.setCursor(0, 12);
  u8g2.print(title);
  u8g2.drawHLine(0, 15, 128);

  u8g2.setCursor(0, 28);
  u8g2.print(l1);

  u8g2.setCursor(0, 40);
  u8g2.print(l2);

  u8g2.setCursor(0, 52);
  u8g2.print(l3);

  u8g2.setCursor(0, 64);
  u8g2.print(l4);

  u8g2.sendBuffer();
}

// ===================== GPS =====================

void pumpGps() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();

    gps.encode(c);
    gpsChars++;
    lastGpsCharMs = millis();

    if (c == '\n') {
      lastNmea = currentNmea;
      currentNmea = "";
    } else if (c != '\r') {
      if (currentNmea.length() < 120) {
        currentNmea += c;
      } else {
        currentNmea = "";
      }
    }
  }

  if (gps.location.isValid()) {
    gpsHasFix = true;
    currentLat = gps.location.lat();
    currentLng = gps.location.lng();

    if (state == STATE_GPS_ACQUIRING) {
      state = STATE_READY;
      beepPattern(1, 2400, 120);
      drawOled("GPS Fixed", "Trip ready", "Press to start", gpsSummary());
    }
  }
}

// ===================== SENSOR / EVENT LOGIC =====================

void recordEvent(String type) {
  lastEventType = type;
  lastEventMs = millis();
  lastEventHasGps = gpsHasFix;

  if (gpsHasFix) {
    lastEventLat = currentLat;
    lastEventLng = currentLng;
  } else {
    lastEventLat = 0.0;
    lastEventLng = 0.0;
  }

  Serial.print("EVENT: ");
  Serial.println(type);
}

void triggerPanic(String type) {
  sensorPanicAlreadySent = true;
  state = STATE_PANIC_MONITORING;

  if (gpsHasFix) {
    movementStartLat = currentLat;
    movementStartLng = currentLng;
    movementWindowHasGps = true;
  } else {
    movementWindowHasGps = false;
  }

  movementWindowStartMs = millis();

  drawOled("ALERT SENT", type, gpsSummary(), "Monitoring...");
  beepPattern(3, 1000, 220, 120);
  recordEvent(type);
}

void readSensors() {
  fsrRaw = analogRead(PIN_FSR);
  readGy91Mpu();
}

void updateSensorFusion() {
  unsigned long now = millis();

  if (fsrRaw > FSR_THRESHOLD) {
    if (fsrOverStartMs == 0) {
      fsrOverStartMs = now;
    }
  } else {
    fsrOverStartMs = 0;
  }

  bool gripSustained = fsrOverStartMs > 0 && (now - fsrOverStartMs >= FSR_SUSTAIN_MS);
  bool motionTriggered = accelMagG >= ACCEL_JOLT_G || gyroMagDps >= GYRO_JOLT_DPS;

  if (motionTriggered) {
    motionEventUntilMs = now + SENSOR_WINDOW_MS;
  }

  bool motionInWindow = now <= motionEventUntilMs;

  if ((state == STATE_ACTIVE || state == STATE_PANIC_MONITORING || state == STATE_NOT_MOVING) &&
      !sensorPanicAlreadySent &&
      gripSustained &&
      motionInWindow) {
    triggerPanic("sensor_panic");
  }
}

// ===================== TRIP LOGIC =====================

void startTrip() {
  if (REQUIRE_GPS_FIX_TO_START && !gpsHasFix) {
    drawOled("GPS acquiring", "Cannot start yet", "Need GPS fix", gpsSummary());
    beepPattern(2, 1200, 80, 80);
    return;
  }

  state = STATE_ACTIVE;
  sensorPanicAlreadySent = false;
  fsrOverStartMs = 0;
  motionEventUntilMs = 0;

  drawOled("TRIP ACTIVE", gpsSummary(), "FSR:" + String(fsrRaw), "Hold: panic");
  beepPattern(1, 2400, 120);
  recordEvent("trip_started");
}

void cancelTrip() {
  state = STATE_CANCELLED;
  drawOled("TRIP CANCELLED", "Returning idle", "", "");
  beepPattern(2, 1800, 90, 70);
  recordEvent("trip_cancelled");
  delay(600);

  state = gpsHasFix ? STATE_READY : STATE_GPS_ACQUIRING;
}

void arrived() {
  state = STATE_ARRIVED;
  drawOled("Safely arrived!", "Trip auto-ended", gpsSummary(), "");
  beepPattern(3, 2400, 100, 80);
  recordEvent("arrived");
}

void startCountdown() {
  state = STATE_COUNTDOWN;
  countdownStartMs = millis();
  lastCountdownNumber = -1;
  drawOled("EMERGENCY", "Hold to send", "Release = cancel", "5");
  beepOnce(2600, 80);
}

void cancelCountdown() {
  state = STATE_ACTIVE;
  drawOled("Countdown cancel", "No alert sent", "TRIP ACTIVE", gpsSummary());
  beepPattern(1, 1400, 80);
}

void updateCountdown() {
  unsigned long elapsed = millis() - countdownStartMs;
  int remaining = 5 - (elapsed / 1000);

  if (remaining < 0) {
    remaining = 0;
  }

  if (remaining != lastCountdownNumber) {
    lastCountdownNumber = remaining;
    drawOled("EMERGENCY", "Keep holding", "Release cancel", String(remaining));
    beepOnce(2600, 70);
  }

  if (elapsed >= COUNTDOWN_MS) {
    triggerPanic("manual_panic");
  }
}

void checkNotMoving() {
  if (state != STATE_PANIC_MONITORING && state != STATE_NOT_MOVING) {
    return;
  }

  unsigned long now = millis();

  if (!gpsHasFix) {
    if (now - movementWindowStartMs >= NOT_MOVING_WINDOW_MS) {
      state = STATE_NOT_MOVING;
      drawOled("NOT MOVING?", "No GPS fix", "Alert repeated", "");
      beepPattern(2, 900, 180, 150);
      recordEvent("not_moving_no_gps");
      movementWindowStartMs = now;
    }

    return;
  }

  if (!movementWindowHasGps) {
    movementStartLat = currentLat;
    movementStartLng = currentLng;
    movementWindowHasGps = true;
    movementWindowStartMs = now;
    return;
  }

  double d = TinyGPSPlus::distanceBetween(movementStartLat, movementStartLng, currentLat, currentLng);

  if (d >= NOT_MOVING_DISTANCE_M) {
    state = STATE_ACTIVE;
    drawOled("Movement resumed", String(d, 1) + " m", "TRIP ACTIVE", "");
    beepPattern(1, 2400, 120);
    recordEvent("movement_resumed");
    return;
  }

  if (now - movementWindowStartMs >= NOT_MOVING_WINDOW_MS) {
    state = STATE_NOT_MOVING;
    drawOled("NOT MOVING", String(d, 1) + "m/60s", "Alert repeated", "");
    beepPattern(2, 900, 180, 150);
    recordEvent("not_moving");

    movementWindowStartMs = now;
    movementStartLat = currentLat;
    movementStartLng = currentLng;
  }
}

void checkArrival() {
  if (!destinationSet || !gpsHasFix) {
    return;
  }

  if (state != STATE_ACTIVE && state != STATE_PANIC_MONITORING && state != STATE_NOT_MOVING) {
    return;
  }

  double d = TinyGPSPlus::distanceBetween(currentLat, currentLng, destLat, destLng);

  if (d <= ARRIVAL_RADIUS_M) {
    arrived();
  }
}

// ===================== BUTTON =====================

void handleSingleClick() {
  if (state == STATE_READY || state == STATE_GPS_ACQUIRING || state == STATE_CANCELLED || state == STATE_ARRIVED) {
    startTrip();
    return;
  }

  if (state == STATE_CANCEL_PROMPT) {
    cancelTrip();
    return;
  }
}

void handleDoubleClick() {
  if (state == STATE_ACTIVE || state == STATE_PANIC_MONITORING || state == STATE_NOT_MOVING) {
    state = STATE_CANCEL_PROMPT;
    drawOled("Cancel trip?", "Press once: yes", "Double: back", "");
    beepPattern(1, 1800, 80);
    return;
  }

  if (state == STATE_CANCEL_PROMPT) {
    state = STATE_ACTIVE;
    drawOled("TRIP ACTIVE", "Cancel aborted", gpsSummary(), "");
    beepPattern(1, 2400, 80);
    return;
  }
}

void updateButton() {
  unsigned long now = millis();
  bool readingPressed = digitalRead(PIN_BUTTON) == LOW;

  if (readingPressed != lastButtonReading) {
    lastDebounceMs = now;
    lastButtonReading = readingPressed;
  }

  if ((now - lastDebounceMs) > DEBOUNCE_MS) {
    if (readingPressed != stableButtonPressed) {
      stableButtonPressed = readingPressed;

      if (stableButtonPressed) {
        buttonDownAtMs = now;
        longPressStarted = false;
      } else {
        if (state == STATE_COUNTDOWN) {
          cancelCountdown();
        } else if (!longPressStarted) {
          clickCount++;
          lastReleaseMs = now;
        }
      }
    }
  }

  if (stableButtonPressed &&
      !longPressStarted &&
      state == STATE_ACTIVE &&
      (now - buttonDownAtMs >= LONG_PRESS_MS)) {
    longPressStarted = true;
    clickCount = 0;
    startCountdown();
  }

  if (state == STATE_COUNTDOWN) {
    if (stableButtonPressed) {
      updateCountdown();
    }

    return;
  }

  if (clickCount > 0 && (now - lastReleaseMs > DOUBLE_PRESS_MS)) {
    if (clickCount == 1) {
      handleSingleClick();
    } else {
      handleDoubleClick();
    }

    clickCount = 0;
  }
}

// ===================== API =====================

void addCors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void sendJson(String json) {
  addCors();
  server.send(200, "application/json", json);
}

void handleOptions() {
  addCors();
  server.send(204);
}

String gpsJson() {
  String json = "{";
  json += "\"valid\":" + boolJson(gpsHasFix) + ",";
  json += "\"chars\":" + String(gpsChars) + ",";
  json += "\"passedChecksum\":" + String(gps.passedChecksum()) + ",";
  json += "\"failedChecksum\":" + String(gps.failedChecksum()) + ",";

  json += "\"lastCharMillisAgo\":";
  if (lastGpsCharMs) {
    json += String(millis() - lastGpsCharMs);
  } else {
    json += "null";
  }
  json += ",";

  json += "\"satellites\":";
  if (gps.satellites.isValid()) {
    json += String(gps.satellites.value());
  } else {
    json += "null";
  }
  json += ",";

  json += "\"hdop\":";
  if (gps.hdop.isValid()) {
    json += String(gps.hdop.hdop(), 2);
  } else {
    json += "null";
  }
  json += ",";

  if (gpsHasFix) {
    json += "\"lat\":" + String(currentLat, 6) + ",";
    json += "\"lng\":" + String(currentLng, 6);
  } else {
    json += "\"lat\":null,\"lng\":null";
  }

  json += "}";
  return json;
}

String statusJson() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"project\":\"HoldON\",";
  json += "\"mode\":\"local_no_firebase_gy91\",";
  json += "\"state\":" + quote(stateName()) + ",";
  json += "\"tripId\":" + quote(tripId) + ",";
  json += "\"ip\":" + quote(ipToString(ip)) + ",";
  json += "\"wifiMode\":" + quote((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + ",";

  json += "\"gps\":" + gpsJson() + ",";
  json += "\"lastNmea\":" + quote(lastNmea) + ",";

  json += "\"fsr\":{";
  json += "\"raw\":" + String(fsrRaw) + ",";
  json += "\"threshold\":" + String(FSR_THRESHOLD) + ",";
  json += "\"hardGrip\":" + boolJson(fsrRaw > FSR_THRESHOLD);
  json += "},";

  json += "\"motion\":{";
  json += "\"mpuOk\":" + boolJson(mpuOk) + ",";
  json += "\"mpuAddress\":" + quote(mpuAddr ? hexByte(mpuAddr) : "") + ",";
  json += "\"whoAmI\":" + quote(mpuAddr ? hexByte(mpuWhoAmI) : "") + ",";
  json += "\"accelG\":{\"x\":" + String(axG, 3) + ",\"y\":" + String(ayG, 3) + ",\"z\":" + String(azG, 3) + "},";
  json += "\"gyroDps\":{\"x\":" + String(gxDps, 2) + ",\"y\":" + String(gyDps, 2) + ",\"z\":" + String(gzDps, 2) + "},";
  json += "\"accelMagG\":" + String(accelMagG, 3) + ",";
  json += "\"gyroMagDps\":" + String(gyroMagDps, 2) + ",";
  json += "\"jolt\":" + boolJson(accelMagG >= ACCEL_JOLT_G || gyroMagDps >= GYRO_JOLT_DPS);
  json += "},";

  json += "\"barometer\":{";
  json += "\"present\":" + boolJson(bmpAddr != 0) + ",";
  json += "\"address\":" + quote(bmpAddr ? hexByte(bmpAddr) : "") + ",";
  json += "\"chipId\":" + quote(bmpAddr ? hexByte(bmpChipId) : "");
  json += "},";

  json += "\"i2cFound\":" + scanI2cArrayJson() + ",";

  json += "\"destination\":{";
  json += "\"set\":" + boolJson(destinationSet) + ",";
  json += "\"lat\":" + String(destLat, 6) + ",";
  json += "\"lng\":" + String(destLng, 6);
  json += "},";

  json += "\"event\":{";
  json += "\"type\":" + quote(lastEventType) + ",";
  json += "\"millis\":" + String(lastEventMs) + ",";
  json += "\"hasGps\":" + boolJson(lastEventHasGps) + ",";
  json += "\"lat\":" + String(lastEventLat, 6) + ",";
  json += "\"lng\":" + String(lastEventLng, 6);
  json += "}";

  json += "}";

  return json;
}

void apiStatus() {
  sendJson(statusJson());
}

void apiStart() {
  startTrip();
  sendJson(statusJson());
}

void apiCancel() {
  cancelTrip();
  sendJson(statusJson());
}

void apiPanic() {
  triggerPanic("manual_panic_api");
  sendJson(statusJson());
}

void apiConfig() {
  if (server.hasArg("tripId")) {
    tripId = server.arg("tripId");
  }

  if (server.hasArg("destLat") && server.hasArg("destLng")) {
    destLat = server.arg("destLat").toFloat();
    destLng = server.arg("destLng").toFloat();
    destinationSet = true;
  }

  sendJson(statusJson());
}

// ===================== WIFI =====================

void setupWiFi() {
  Serial.println();
  Serial.println("Starting WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
    pumpGps();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi failed. Starting fallback AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP8266-TEST", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
  }
}

// ===================== SETUP / LOOP =====================

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  u8g2.setI2CAddress(OLED_I2C_ADDR * 2);
  u8g2.begin();
  u8g2.setPowerSave(0);
  drawOled("HoldON Boot", "GY-91 local", "No Firebase", "");

  Serial.println();
  Serial.println("HoldON Local GY-91 No-Firebase Firmware");

  initGy91();

  gpsSerial.begin(9600);
  Serial.println("GPS serial started at 9600 on D7.");

  setupWiFi();

  server.on("/api/status", HTTP_GET, apiStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);

  server.on("/api/start", HTTP_GET, apiStart);
  server.on("/api/start", HTTP_OPTIONS, handleOptions);

  server.on("/api/cancel", HTTP_GET, apiCancel);
  server.on("/api/cancel", HTTP_OPTIONS, handleOptions);

  server.on("/api/panic", HTTP_GET, apiPanic);
  server.on("/api/panic", HTTP_OPTIONS, handleOptions);

  server.on("/api/config", HTTP_GET, apiConfig);
  server.on("/api/config", HTTP_OPTIONS, handleOptions);

  server.begin();

  drawOled("GPS acquiring...", "API: /api/status", "Chars:0", "Press start ok");
  beepPattern(1, 1800, 80);

  Serial.println("HTTP server ready.");
  Serial.println("Open /api/status from Vue or browser.");
}

void loop() {
  unsigned long now = millis();

  pumpGps();
  server.handleClient();
  updateButton();

  if (now - lastSensorSampleMs >= SENSOR_SAMPLE_MS) {
    lastSensorSampleMs = now;
    readSensors();
    updateSensorFusion();
  }

  checkNotMoving();
  checkArrival();

  if (now - lastOledRefreshMs >= OLED_REFRESH_MS && state != STATE_COUNTDOWN) {
    lastOledRefreshMs = now;

    if (state == STATE_GPS_ACQUIRING) {
      drawOled("GPS acquiring...", gpsSummary(), "Chars:" + String(gpsChars), "FSR:" + String(fsrRaw));
    } else if (state == STATE_READY) {
      drawOled("Trip ready", "Press to start", gpsSummary(), "FSR:" + String(fsrRaw));
    } else if (state == STATE_ACTIVE) {
      drawOled("TRIP ACTIVE", gpsSummary(), "FSR:" + String(fsrRaw), "G:" + String(accelMagG, 2) + " Gy:" + String(gyroMagDps, 0));
    } else if (state == STATE_PANIC_MONITORING) {
      unsigned long sec = (millis() - movementWindowStartMs) / 1000;
      drawOled("ALERT SENT", gpsSummary(), "Monitoring " + String(sec) + "s", "Move >10m");
    } else if (state == STATE_NOT_MOVING) {
      unsigned long sec = (millis() - movementWindowStartMs) / 1000;
      drawOled("NOT MOVING", "Alert active", String(sec) + "s window", gpsSummary());
    } else if (state == STATE_CANCEL_PROMPT) {
      drawOled("Cancel trip?", "Press once: yes", "Double: back", "");
    } else if (state == STATE_ARRIVED) {
      drawOled("Safely arrived!", "Trip ended", gpsSummary(), "");
    } else if (state == STATE_CANCELLED) {
      drawOled("Trip cancelled", gpsSummary(), "", "");
    }
  }
}
