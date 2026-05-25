#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* WIFI_SSID = "bbrouter";
const char* WIFI_PASS = "107149090195";

// Most NodeMCU dev boards scale A0 to about 0-3.3V.
// Bare ESP8266 ADC is 0-1.0V. Change this if needed.
const float A0_MAX_VOLTAGE = 3.3;

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

void handleStatus() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"fsr_analog_test\",";
  json += "\"millis\":" + String(millis()) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"wifiMode\":\"" + String((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + "\",";
  json += "\"ip\":\"" + ipToString(ip) + "\"";
  json += "}";

  sendJson(json);
}

void handleFsr() {
  const int samples = 20;
  long total = 0;

  for (int i = 0; i < samples; i++) {
    total += analogRead(A0);
    delay(2);
  }

  float raw = total / (float)samples;
  float voltage = raw * A0_MAX_VOLTAGE / 1023.0;
  float percent = raw * 100.0 / 1023.0;

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"fsr_analog_test\",";
  json += "\"raw\":" + String(raw, 1) + ",";
  json += "\"rawMax\":1023,";
  json += "\"voltage\":" + String(voltage, 3) + ",";
  json += "\"a0MaxVoltage\":" + String(A0_MAX_VOLTAGE, 1) + ",";
  json += "\"percent\":" + String(percent, 1) + ",";
  json += "\"note\":\"Connectivity test only; not calibrated force.\"";
  json += "}";

  sendJson(json);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  setupWiFi();

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/fsr", HTTP_GET, handleFsr);
  server.on("/api/fsr", HTTP_OPTIONS, handleOptions);
  server.begin();

  Serial.println("FSR test ready. Open /api/fsr");
}

void loop() {
  server.handleClient();
}
