#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <U8g2lib.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

const int SDA_PIN = D2; // GPIO4
const int SCL_PIN = D1; // GPIO5

// For 1.3-inch OLED SH1106 I2C.
// Do not use an SSD1306 constructor for an SH1106 display.
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

ESP8266WebServer server(80);
String currentText = "SH1106 OK";
unsigned long drawCount = 0;

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

String jsonEscape(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\r", "\\r");
  s.replace("\n", "\\n");
  return s;
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

void drawOled(const String& line1, const String& line2 = "") {
  drawCount++;

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  u8g2.drawStr(0, 12, "ESP8266 OLED TEST");
  u8g2.drawHLine(0, 15, 128);

  u8g2.setCursor(0, 32);
  u8g2.print(line1);

  u8g2.setCursor(0, 46);
  u8g2.print(line2);

  u8g2.setCursor(0, 63);
  u8g2.print("draws: ");
  u8g2.print(drawCount);

  u8g2.sendBuffer();
}

void handleStatus() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"sh1106_oled_test\",";
  json += "\"millis\":" + String(millis()) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"wifiMode\":\"" + String((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + "\",";
  json += "\"ip\":\"" + ipToString(ip) + "\",";
  json += "\"driver\":\"SH1106 via U8g2\",";
  json += "\"i2cAddress\":\"0x3C\",";
  json += "\"drawCount\":" + String(drawCount) + ",";
  json += "\"text\":\"" + jsonEscape(currentText) + "\"";
  json += "}";

  sendJson(json);
}

void handleOled() {
  if (server.hasArg("text")) {
    currentText = server.arg("text");
    if (currentText.length() > 32) currentText = currentText.substring(0, 32);
  }

  drawOled(currentText, "SH1106 library");
  handleStatus();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  // U8g2 uses 8-bit I2C addresses. 0x3C becomes 0x78 internally.
  // If your display is 0x3D, change this to: 0x3D * 2
  u8g2.setI2CAddress(0x3C * 2);
  u8g2.begin();

  drawOled("Booting...", "");

  setupWiFi();

  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();
  drawOled("IP:", ipToString(ip));

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/oled", HTTP_GET, handleOled);
  server.on("/api/oled", HTTP_OPTIONS, handleOptions);
  server.begin();

  Serial.println("SH1106 OLED test ready. Open /api/oled?text=Hello");
}

void loop() {
  server.handleClient();
}
