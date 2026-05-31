#ifndef CONFIG_H
#define CONFIG_H

// ===================== WIFI =====================
#define WIFI_SSID "bbrouter"
#define WIFI_PASSWORD "107149090195"

// ===================== MQTT =====================
#define MQTT_HOST "87c9fdddd5c143b182358ae6f39ba5ab.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USERNAME "holdon"
#define MQTT_PASSWORD "Keanno&Pogi=999"
#define MQTT_CLIENT_ID "holdon-device-01"

// ===================== MQTT TOPICS =====================
#define TOPIC_LOCATION "holdon/location"
#define TOPIC_GRIP     "holdon/grip"
#define TOPIC_MOTION   "holdon/motion"
#define TOPIC_EVENT    "holdon/event"
#define TOPIC_CONFIG   "holdon/config"

// ===================== PINS =====================
#define SDA_PIN    D2
#define SCL_PIN    D1
#define PIN_BUTTON D5
#define PIN_BUZZER D6
#define PIN_GPS_RX D7
#define PIN_GPS_TX D8
#define PIN_FSR    A0

// ===================== OLED =====================
#define OLED_I2C_ADDR 0x3C

// ===================== SENSOR THRESHOLDS =====================
#define FSR_THRESHOLD        700
#define FSR_SUSTAIN_MS       2000
#define ACCEL_JOLT_G         2.5
#define GYRO_JOLT_DPS        150.0
#define SENSOR_WINDOW_MS     2000
#define SENSOR_PANIC_COOLDOWN_MS 10000

// ===================== TIMING =====================
#define SENSOR_SAMPLE_MS     100
#define OLED_REFRESH_MS      1000
#define GPS_PUBLISH_MS       5000
#define SENSOR_PUBLISH_MS    1000

// ===================== BUTTON =====================
#define DEBOUNCE_MS          35
#define DOUBLE_PRESS_MS      400
#define LONG_PRESS_MS        3000
#define COUNTDOWN_MS         5000

// ===================== TRIP LOGIC =====================
#define NOT_MOVING_DISTANCE_M   10.0
#define NOT_MOVING_WINDOW_MS    60000
#define ARRIVAL_RADIUS_M        50.0

// ===================== GPS FIX REQUIREMENT =====================
// Set false for bench testing, true for final demo
#define REQUIRE_GPS_FIX_TO_START false

#endif