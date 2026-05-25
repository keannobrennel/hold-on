#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* WIFI_SSID = "bbrouter";
const char* WIFI_PASS = "107149090195";

ESP8266WebServer server(80);

bool ledOn = false;

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

void setLed(bool on) {
  ledOn = on;
  digitalWrite(LED_BUILTIN, on ? LOW : HIGH); // most NodeMCU onboard LEDs are active-low
}

void handleStatus() {
  IPAddress ip = (WiFi.getMode() == WIFI_AP) ? WiFi.softAPIP() : WiFi.localIP();

  String json = "{";
  json += "\"ok\":true,";
  json += "\"test\":\"esp8266_preflight\",";
  json += "\"millis\":" + String(millis()) + ",";
  json += "\"freeHeap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chipId\":\"0x" + String(ESP.getChipId(), HEX) + "\",";
  json += "\"flashSize\":" + String(ESP.getFlashChipSize()) + ",";
  json += "\"wifiMode\":\"" + String((WiFi.getMode() == WIFI_AP) ? "AP" : "STA") + "\",";
  json += "\"ip\":\"" + ipToString(ip) + "\",";
  json += "\"ledOn\":" + String(ledOn ? "true" : "false");
  json += "}";

  sendJson(json);
}

void handleLed() {
  String state = server.arg("state");
  state.toLowerCase();

  if (state == "on") setLed(true);
  else if (state == "off") setLed(false);
  else if (state == "toggle") setLed(!ledOn);

  handleStatus();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LED_BUILTIN, OUTPUT);
  setLed(false);

  setupWiFi();

  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/led", HTTP_GET, handleLed);
  server.on("/api/led", HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println("HTTP server ready.");
}

void loop() {
  server.handleClient();

  // Tiny heartbeat blink without blocking.
  static unsigned long last = 0;
  static bool pulse = false;
  if (millis() - last > 1000) {
    last = millis();
    pulse = !pulse;
    if (!ledOn) digitalWrite(LED_BUILTIN, pulse ? LOW : HIGH);
  }
}
