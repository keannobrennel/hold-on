#include <U8g2lib.h>
#include "config.h"
#include "mqtt.h"
#include "gps.h"
#include "sensors.h"

// ===================== OLED =====================
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void drawOled(String title, String l1 = "", String l2 = "", String l3 = "", String l4 = "") {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.setCursor(0, 12); u8g2.print(title);
  u8g2.drawHLine(0, 15, 128);
  u8g2.setCursor(0, 28); u8g2.print(l1);
  u8g2.setCursor(0, 40); u8g2.print(l2);
  u8g2.setCursor(0, 52); u8g2.print(l3);
  u8g2.setCursor(0, 64); u8g2.print(l4);
  u8g2.sendBuffer();
}

// ===================== BUZZER =====================
void beepPattern(uint8_t count, uint16_t freq = 2200, uint16_t ms = 80, uint16_t gap = 70) {
  for (uint8_t i = 0; i < count; i++) {
    tone(PIN_BUZZER, freq, ms);
    delay(ms);
    noTone(PIN_BUZZER);
    delay(gap);
  }
}

void beepOnce(uint16_t freq = 2200, uint16_t ms = 80) {
  tone(PIN_BUZZER, freq, ms);
  delay(ms + 10);
  noTone(PIN_BUZZER);
}

// ===================== DEVICE STATE =====================
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

bool destinationSet = false;
double destLat = 0.0;
double destLng = 0.0;

unsigned long lastOledRefreshMs = 0;
unsigned long lastSensorSampleMs = 0;
unsigned long movementWindowStartMs = 0;
double movementStartLat = 0.0;
double movementStartLng = 0.0;
bool movementWindowHasGps = false;
unsigned long countdownStartMs = 0;
int lastCountdownNumber = -1;

// ===================== BUTTON STATE =====================
bool stableButtonPressed = false;
bool lastButtonReading = false;
unsigned long lastDebounceMs = 0;
unsigned long buttonDownAtMs = 0;
bool longPressStarted = false;
int clickCount = 0;
unsigned long lastReleaseMs = 0;

// ===================== TRIP ACTIONS =====================
void startTrip() {
  if (REQUIRE_GPS_FIX_TO_START && !gpsHasFix) {
    drawOled("GPS acquiring", "Cannot start yet", "Need GPS fix", gpsSummary());
    beepPattern(2, 1200, 80, 80);
    return;
  }

  state = STATE_ACTIVE;
  lastSensorPanicMs = 0;
  fsrOverStartMs = 0;
  motionEventUntilMs = 0;

  drawOled("TRIP ACTIVE", gpsSummary(), "FSR:" + String(fsrRaw), "Hold: panic");
  beepPattern(1, 2400, 120);
  publishEvent("trip_started");
}

void cancelTrip() {
  state = STATE_CANCELLED;
  drawOled("TRIP CANCELLED", "Returning idle", "", "");
  beepPattern(2, 1800, 90, 70);
  publishEvent("trip_cancelled");
  delay(600);
  state = gpsHasFix ? STATE_READY : STATE_GPS_ACQUIRING;
}

void arrived() {
  state = STATE_ARRIVED;
  drawOled("Safely arrived!", "Trip auto-ended", gpsSummary(), "");
  beepPattern(3, 2400, 100, 80);
  publishEvent("arrived");
}

void triggerPanic(String type) {
  lastSensorPanicMs = millis();
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
  publishEvent(type);
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
  drawOled("Cancelled", "No alert sent", "TRIP ACTIVE", gpsSummary());
  beepPattern(1, 1400, 80);
}

void updateCountdown() {
  unsigned long elapsed = millis() - countdownStartMs;
  int remaining = 5 - (int)(elapsed / 1000);
  if (remaining < 0) remaining = 0;

  if (remaining != lastCountdownNumber) {
    lastCountdownNumber = remaining;
    drawOled("EMERGENCY", "Keep holding", "Release cancel", String(remaining));
    beepOnce(2600, 70);
  }

  if (elapsed >= COUNTDOWN_MS) {
    triggerPanic("manual_panic");
  }
}

// ===================== NOT MOVING CHECK =====================
void checkNotMoving() {
  if (state != STATE_PANIC_MONITORING && state != STATE_NOT_MOVING) return;

  unsigned long now = millis();

  if (!gpsHasFix) {
    if (now - movementWindowStartMs >= NOT_MOVING_WINDOW_MS) {
      state = STATE_NOT_MOVING;
      drawOled("NOT MOVING?", "No GPS fix", "Alert repeated", "");
      beepPattern(2, 900, 180, 150);
      publishEvent("not_moving");
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

  double d = TinyGPSPlus::distanceBetween(
    movementStartLat, movementStartLng,
    currentLat, currentLng
  );

  if (d >= NOT_MOVING_DISTANCE_M) {
    state = STATE_ACTIVE;
    drawOled("Movement resumed", String(d, 1) + " m", "TRIP ACTIVE", "");
    beepPattern(1, 2400, 120);
    publishEvent("resumed");
    return;
  }

  if (now - movementWindowStartMs >= NOT_MOVING_WINDOW_MS) {
    state = STATE_NOT_MOVING;
    drawOled("NOT MOVING", String(d, 1) + "m/60s", "Alert repeated", "");
    beepPattern(2, 900, 180, 150);
    publishEvent("not_moving");
    movementWindowStartMs = now;
    movementStartLat = currentLat;
    movementStartLng = currentLng;
  }
}

// ===================== ARRIVAL CHECK =====================
void checkArrival() {
  if (!destinationSet || !gpsHasFix) return;
  if (state != STATE_ACTIVE &&
      state != STATE_PANIC_MONITORING &&
      state != STATE_NOT_MOVING) return;

  double d = TinyGPSPlus::distanceBetween(
    currentLat, currentLng, destLat, destLng
  );

  if (d <= ARRIVAL_RADIUS_M) {
    arrived();
  }
}

// ===================== BUTTON =====================
void handleSingleClick() {
  if (state == STATE_READY ||
      state == STATE_GPS_ACQUIRING ||
      state == STATE_CANCELLED ||
      state == STATE_ARRIVED) {
    startTrip();
    return;
  }
  if (state == STATE_CANCEL_PROMPT) {
    cancelTrip();
    return;
  }
}

void handleDoubleClick() {
  if (state == STATE_ACTIVE) {
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
      (state == STATE_ACTIVE ||
       state == STATE_PANIC_MONITORING ||
       state == STATE_NOT_MOVING) &&
      (now - buttonDownAtMs >= LONG_PRESS_MS)) {
    longPressStarted = true;
    clickCount = 0;
    startCountdown();
  }

  if (state == STATE_COUNTDOWN) {
    if (stableButtonPressed) updateCountdown();
    return;
  }

  if (clickCount > 0 && (now - lastReleaseMs > DOUBLE_PRESS_MS)) {
    if (clickCount == 1) handleSingleClick();
    else handleDoubleClick();
    clickCount = 0;
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  u8g2.setI2CAddress(OLED_I2C_ADDR * 2);
  u8g2.begin();
  u8g2.setPowerSave(0);
  drawOled("HoldON Boot", "Connecting...", "", "");

  Serial.println("HoldON MQTT Firmware starting...");

  initSensors();
  initGPS();
  connectWiFi();
  connectMQTT();

  drawOled("HoldON Ready", "GPS acquiring...", "MQTT connected", "Press to start");
  beepPattern(1, 2400, 120);
  Serial.println("HoldON device ready.");
}

// ===================== LOOP =====================
void loop() {
  unsigned long now = millis();

  ensureMQTTConnected();
  pumpGPS();
  updateButton();

  if (now - lastSensorSampleMs >= SENSOR_SAMPLE_MS) {
    lastSensorSampleMs = now;
    readSensors();

    bool tripActive = (state == STATE_ACTIVE ||
                       state == STATE_PANIC_MONITORING ||
                       state == STATE_NOT_MOVING);

    updateSensorFusion(tripActive);

    if (tripActive) {
      publishSensorData();
      publishLocation();
    }
  }

  checkNotMoving();
  checkArrival();

  if (now - lastOledRefreshMs >= OLED_REFRESH_MS &&
      state != STATE_COUNTDOWN) {
    lastOledRefreshMs = now;

    if (state == STATE_GPS_ACQUIRING) {
      drawOled("GPS acquiring...", gpsSummary(),
               "Chars:" + String(gpsChars),
               "FSR:" + String(fsrRaw));
    } else if (state == STATE_READY) {
      drawOled("Trip ready", "Press to start",
               gpsSummary(), "FSR:" + String(fsrRaw));
    } else if (state == STATE_ACTIVE) {
      drawOled("TRIP ACTIVE", gpsSummary(),
               "FSR:" + String(fsrRaw),
               "G:" + String(accelMagG, 2) +
               " Gy:" + String(gyroMagDps, 0));
    } else if (state == STATE_PANIC_MONITORING) {
      unsigned long sec = (millis() - movementWindowStartMs) / 1000;
      drawOled("ALERT SENT", gpsSummary(),
               "Monitoring " + String(sec) + "s", "Move >10m");
    } else if (state == STATE_NOT_MOVING) {
      unsigned long sec = (millis() - movementWindowStartMs) / 1000;
      drawOled("NOT MOVING", "Alert active",
               String(sec) + "s window", gpsSummary());
    } else if (state == STATE_CANCEL_PROMPT) {
      drawOled("Cancel trip?", "Press once: yes", "Double: back", "");
    } else if (state == STATE_ARRIVED) {
      drawOled("Safely arrived!", "Trip ended", gpsSummary(), "");
    } else if (state == STATE_CANCELLED) {
      drawOled("Trip cancelled", gpsSummary(), "", "");
    }
  }
}