#ifndef MQTT_H
#define MQTT_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
}

void connectMQTT() {
  wifiClient.setInsecure(); // Use for testing; replace with CA cert for production
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  while (!mqttClient.connected()) {
    Serial.print("Connecting to HiveMQ...");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      delay(3000);
    }
  }
}

void ensureMQTTConnected() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
}

void publishMessage(const char* topic, String payload) {
  if (mqttClient.connected()) {
    mqttClient.publish(topic, payload.c_str());
    Serial.println("[MQTT] Published to " + String(topic) + ": " + payload);
  }
}

#endif