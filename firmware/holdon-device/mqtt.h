#ifndef MQTT_H
#define MQTT_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// tripId received from dashboard via holdon/config topic
String activeTripId = "default";

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi failed — continuing without network.");
  }
}

// Forward declaration so mqttCallback can reference it
void onConfigReceived(String payload);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("[MQTT] Message on ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == TOPIC_CONFIG) {
    onConfigReceived(message);
  }
}

void connectMQTT() {
  wifiClient.setInsecure();
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  while (!mqttClient.connected()) {
    Serial.print("Connecting to HiveMQ...");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected.");
      mqttClient.subscribe(TOPIC_CONFIG);
      Serial.println("Subscribed to holdon/config");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 3s...");
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
    Serial.print("[MQTT] Published to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(payload);
  }
}

void onConfigReceived(String payload) {
  // Expects JSON: {"tripId":"abc123","destLat":14.5995,"destLng":120.9842}
  // Simple manual parse — no JSON library needed
  int tripIdStart = payload.indexOf("\"tripId\":\"");
  if (tripIdStart != -1) {
    tripIdStart += 10;
    int tripIdEnd = payload.indexOf("\"", tripIdStart);
    if (tripIdEnd != -1) {
      activeTripId = payload.substring(tripIdStart, tripIdEnd);
      Serial.print("Trip ID set to: ");
      Serial.println(activeTripId);
    }
  }
}

#endif