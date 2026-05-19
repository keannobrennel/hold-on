#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "mqtt.h"

TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

unsigned long lastGPSPublish = 0;

void initGPS() {
  gpsSerial.begin(9600);
  Serial.println("GPS initialized");
}

void readGPS() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
}

void publishLocation() {
  if (millis() - lastGPSPublish < GPS_PUBLISH_INTERVAL) return;
  lastGPSPublish = millis();

  if (gps.location.isValid()) {
    String payload = "{";
    payload += "\"lat\":" + String(gps.location.lat(), 6) + ",";
    payload += "\"lng\":" + String(gps.location.lng(), 6) + ",";
    payload += "\"timestamp\":" + String(millis());
    payload += "}";

    publishMessage(TOPIC_LOCATION, payload);
  } else {
    Serial.println("[GPS] Waiting for fix...");
  }
}

#endif