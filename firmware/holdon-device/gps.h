#ifndef GPS_H
#define GPS_H

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "mqtt.h"

TinyGPSPlus gps;
SoftwareSerial gpsSerial(PIN_GPS_RX, PIN_GPS_TX);

bool gpsHasFix = false;
double currentLat = 0.0;
double currentLng = 0.0;
uint32_t gpsChars = 0;
unsigned long lastGpsPublishMs = 0;

void initGPS() {
  gpsSerial.begin(9600);
  Serial.println("GPS serial started at 9600.");
}

void pumpGPS() {
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
    gpsChars++;
  }

  if (gps.location.isValid()) {
    gpsHasFix = true;
    currentLat = gps.location.lat();
    currentLng = gps.location.lng();
  }
}

void publishLocation() {
  if (millis() - lastGpsPublishMs < GPS_PUBLISH_MS) return;
  lastGpsPublishMs = millis();

  if (!gpsHasFix) {
    Serial.println("[GPS] Waiting for fix...");
    return;
  }

  String payload = "{";
  payload += "\"tripId\":\"" + activeTripId + "\",";
  payload += "\"lat\":" + String(currentLat, 6) + ",";
  payload += "\"lng\":" + String(currentLng, 6) + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";

  publishMessage(TOPIC_LOCATION, payload);
}

String gpsSummary() {
  if (gpsHasFix) {
    return String(currentLat, 5) + "," + String(currentLng, 5);
  }
  if (gpsChars > 0) return "GPS data, no fix";
  return "GPS no data";
}

double distanceTo(double destLat, double destLng) {
  if (!gpsHasFix) return -1;
  return TinyGPSPlus::distanceBetween(currentLat, currentLng, destLat, destLng);
}

#endif