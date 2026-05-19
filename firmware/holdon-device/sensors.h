#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"
#include "mqtt.h"

unsigned long lastSensorPublish = 0;
bool lastMovingState = true;

void initSensors() {
  pinMode(GRIP_PIN, INPUT);
  pinMode(PANIC_PIN, INPUT_PULLUP);
  Serial.println("Sensors initialized");
}

void publishGrip(int gripValue) {
  String payload = "{";
  payload += "\"value\":" + String(gripValue) + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";

  publishMessage(TOPIC_GRIP, payload);
}

void publishMotion(bool isMoving) {
  String payload = "{";
  payload += "\"moving\":" + String(isMoving ? "true" : "false") + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";

  publishMessage(TOPIC_MOTION, payload);
}

void publishEvent(String type, float lat, float lng) {
  String payload = "{";
  payload += "\"type\":\"" + type + "\",";
  payload += "\"lat\":" + String(lat, 6) + ",";
  payload += "\"lng\":" + String(lng, 6) + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";

  publishMessage(TOPIC_EVENT, payload);
}

void readSensors() {
  if (millis() - lastSensorPublish < SENSOR_PUBLISH_INTERVAL) return;
  lastSensorPublish = millis();

  // Read grip sensor
  int gripValue = analogRead(GRIP_PIN);
  publishGrip(gripValue);

  // Detect panic button press
  if (digitalRead(PANIC_PIN) == LOW) {
    Serial.println("[SENSOR] Panic button pressed!");
    publishEvent("panic", gps.location.lat(), gps.location.lng());
  }

  // Detect if not moving (grip below threshold)
  bool isMoving = gripValue > 100;
  if (isMoving != lastMovingState) {
    publishMotion(isMoving);
    if (!isMoving) {
      publishEvent("not_moving", gps.location.lat(), gps.location.lng());
    }
    lastMovingState = isMoving;
  }
}

#endif