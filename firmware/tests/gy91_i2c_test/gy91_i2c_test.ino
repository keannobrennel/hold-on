#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

const int SDA_PIN = D2; // GPIO4
const int SCL_PIN = D1; // GPIO5

ESP8266WebServer server(80);

void addCors() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void sendJson(const String& json) {
  addCors();
  server.send(200, "application/json", json);
}

void handleOptions() {
  addCors();
  server.send(204);
}

String ipToString(IPAddress ip) {
  return String(ip[0]) + "." + ip[1] + "." + ip[2] + "." + ip[3];
}

void setupWiFi() {
  Serial.println();
  Serial.println("Starting WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
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

String hexByte(uint8_t v) {
  String s = String(v, HEX);
  s.toUpperCase();
  if (s.length() < 2) s = "0" + s;
  return "0x" + s;
}

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
  if (Wire.endTransmission(false) != 0) return false;

  uint8_t n = Wire.requestFrom((int)addr, (int)len);
  if (n != len) return false;

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
      json += "\"" + hexByte(addr) + "\"";
      first = false;
    }
  }

  json += "]";
  return json;
}

uint8_t findFirstPresent(const uint8_t* addresses, uint8_t count) {
  for (uint8_t i = 0; i < count; i++) {
    if (i2cPresent(addresses[i])) return addresses[i];
  }
  return 0;
}

void handleStatus() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"gy91_i2c_test\",";
  json += "\"millis\":" + String(millis()) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"wifiMode\":\"" + String((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + "\",";
  json += "\"ip\":\"" + ipToString(ip) + "\"";
  json += "}";

  sendJson(json);
}

void handleGy91() {
  uint8_t mpuCandidates[] = {0x68, 0x69};
  uint8_t bmpCandidates[] = {0x76, 0x77};

  uint8_t mpuAddr = findFirstPresent(mpuCandidates, 2);
  uint8_t bmpAddr = findFirstPresent(bmpCandidates, 2);

  bool mpuOk = false;
  uint8_t who = 0;
  int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
  float axg = 0, ayg = 0, azg = 0, gxdps = 0, gydps = 0, gzdps = 0;

  if (mpuAddr) {
    // Wake MPU-9250/MPU-9255/MPU-6500 style devices.
    writeReg(mpuAddr, 0x6B, 0x00);
    delay(20);

    who = readReg(mpuAddr, 0x75, &mpuOk);

    uint8_t b[14];
    if (readBytes(mpuAddr, 0x3B, b, 14)) {
      ax = be16(b[0], b[1]);
      ay = be16(b[2], b[3]);
      az = be16(b[4], b[5]);
      gx = be16(b[8], b[9]);
      gy = be16(b[10], b[11]);
      gz = be16(b[12], b[13]);

      // Default full-scale after reset: accel ±2g, gyro ±250 dps.
      axg = ax / 16384.0;
      ayg = ay / 16384.0;
      azg = az / 16384.0;
      gxdps = gx / 131.0;
      gydps = gy / 131.0;
      gzdps = gz / 131.0;
    }
  }

  bool bmpOk = false;
  uint8_t bmpId = 0;
  if (bmpAddr) {
    bmpId = readReg(bmpAddr, 0xD0, &bmpOk);
  }

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"gy91_i2c_test\",";
  json += "\"i2cFound\":" + scanI2cArrayJson() + ",";
  json += "\"mpu\":{";
  json += "\"present\":" + String(mpuAddr ? "true" : "false") + ",";
  json += "\"address\":\"" + String(mpuAddr ? hexByte(mpuAddr) : "") + "\",";
  json += "\"whoAmI\":\"" + String(mpuOk ? hexByte(who) : "") + "\",";
  json += "\"whoAmIExpected\":\"0x71 for MPU-9250, 0x73 for MPU-9255, 0x70 for MPU-6500\",";
  json += "\"accelRaw\":{\"x\":" + String(ax) + ",\"y\":" + String(ay) + ",\"z\":" + String(az) + "},";
  json += "\"accelG\":{\"x\":" + String(axg, 3) + ",\"y\":" + String(ayg, 3) + ",\"z\":" + String(azg, 3) + "},";
  json += "\"gyroRaw\":{\"x\":" + String(gx) + ",\"y\":" + String(gy) + ",\"z\":" + String(gz) + "},";
  json += "\"gyroDps\":{\"x\":" + String(gxdps, 2) + ",\"y\":" + String(gydps, 2) + ",\"z\":" + String(gzdps, 2) + "}";
  json += "},";
  json += "\"bmp\":{";
  json += "\"present\":" + String(bmpAddr ? "true" : "false") + ",";
  json += "\"address\":\"" + String(bmpAddr ? hexByte(bmpAddr) : "") + "\",";
  json += "\"chipId\":\"" + String(bmpOk ? hexByte(bmpId) : "") + "\",";
  json += "\"chipIdExpected\":\"0x58 for BMP280, 0x60 for BME280\"";
  json += "}";
  json += "}";

  sendJson(json);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  setupWiFi();

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/gy91", HTTP_GET, handleGy91);
  server.on("/api/gy91", HTTP_OPTIONS, handleOptions);
  server.begin();

  Serial.println("GY-91 test ready. Open /api/gy91");
}

void loop() {
  server.handleClient();
}
