#ifndef CONFIG_H
#define CONFIG_H

// WiFi
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

// MQTT
#define MQTT_HOST "1b2963ffcb6749ec911eab095a9bf97a.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USERNAME "holdon"
#define MQTT_PASSWORD "ask-keanno"
#define MQTT_CLIENT_ID "holdon-device-01"

// MQTT Topics
#define TOPIC_LOCATION "holdon/location"
#define TOPIC_GRIP     "holdon/grip"
#define TOPIC_MOTION   "holdon/motion"
#define TOPIC_EVENT    "holdon/event"

// Pin Definitions
#define GPS_RX_PIN     D6
#define GPS_TX_PIN     D7
#define GRIP_PIN       A0
#define PANIC_PIN      D3

// Intervals (ms)
#define GPS_PUBLISH_INTERVAL    3000
#define SENSOR_PUBLISH_INTERVAL 1000

#endif