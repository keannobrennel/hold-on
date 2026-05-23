#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

const int GPS_RX_PIN = D5; // ESP RX, connect to GPS TX
const int GPS_TX_PIN = D6; // ESP TX, optional, connect to GPS RX if needed

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;
ESP8266WebServer server(80);

String lastSentence = "";
String currentSentence = "";
unsigned long gpsChars = 0;
unsigned long lastCharMillis = 0;

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

void pumpGps() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
    gpsChars++;
    lastCharMillis = millis();

    if (c == '\n') {
      lastSentence = currentSentence;
      currentSentence = "";
    } else if (c != '\r') {
      if (currentSentence.length() < 120) currentSentence += c;
      else currentSentence = "";
    }
  }
}

void handleStatus() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"neo7m_gps_test\",";
  json += "\"millis\":" + String(millis()) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"wifiMode\":\"" + String((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + "\",";
  json += "\"ip\":\"" + ipToString(ip) + "\"";
  json += "}";

  sendJson(json);
}

void handleGps() {
  pumpGps();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"neo7m_gps_test\",";
  json += "\"gpsChars\":" + String(gpsChars) + ",";
  json += "\"sentencesWithFix\":" + String(gps.sentencesWithFix()) + ",";
  json += "\"failedChecksum\":" + String(gps.failedChecksum()) + ",";
  json += "\"passedChecksum\":" + String(gps.passedChecksum()) + ",";
  json += "\"lastCharMillisAgo\":" + String(lastCharMillis ? (millis() - lastCharMillis) : 0) + ",";
  json += "\"locationValid\":" + String(gps.location.isValid() ? "true" : "false") + ",";
  json += "\"lat\":" + String(gps.location.isValid() ? gps.location.lat() : 0.0, 6) + ",";
  json += "\"lng\":" + String(gps.location.isValid() ? gps.location.lng() : 0.0, 6) + ",";
  json += "\"altitudeMeters\":" + String(gps.altitude.isValid() ? gps.altitude.meters() : 0.0, 2) + ",";
  json += "\"satellites\":" + String(gps.satellites.isValid() ? gps.satellites.value() : 0) + ",";
  json += "\"hdop\":" + String(gps.hdop.isValid() ? gps.hdop.hdop() : 0.0, 2) + ",";
  json += "\"date\":\"";
  if (gps.date.isValid()) {
    json += String(gps.date.year()) + "-" + String(gps.date.month()) + "-" + String(gps.date.day());
  }
  json += "\",";
  json += "\"time\":\"";
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) json += "0";
    json += String(gps.time.hour()) + ":";
    if (gps.time.minute() < 10) json += "0";
    json += String(gps.time.minute()) + ":";
    if (gps.time.second() < 10) json += "0";
    json += String(gps.time.second());
  }
  json += "\",";
  json += "\"lastNmea\":\"" + jsonEscape(lastSentence) + "\"";
  json += "}";

  sendJson(json);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  gpsSerial.begin(9600);

  setupWiFi();

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/gps", HTTP_GET, handleGps);
  server.on("/api/gps", HTTP_OPTIONS, handleOptions);
  server.begin();

  Serial.println("NEO-7M test ready. Open /api/gps");
}

void loop() {
  pumpGps();
  server.handleClient();
}
