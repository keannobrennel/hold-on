#include "config.h"
#include "mqtt.h"
#include "gps.h"
#include "sensors.h"

void setup() {
  Serial.begin(115200);
  Serial.println("\nHoldON Device Starting...");

  connectWiFi();
  connectMQTT();
  initGPS();
  initSensors();

  Serial.println("HoldON Device Ready");
}

void loop() {
  ensureMQTTConnected();
  readGPS();
  publishLocation();
  readSensors();
}