#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include <math.h>
#include "config.h"
#include "mqtt.h"
#include "gps.h"

// ===================== MPU STATE =====================
uint8_t mpuAddr = 0;
uint8_t mpuWhoAmI = 0;
bool mpuOk = false;

int16_t axRaw = 0, ayRaw = 0, azRaw = 0;
int16_t gxRaw = 0, gyRaw = 0, gzRaw = 0;

float axG = 0.0, ayG = 0.0, azG = 0.0;
float gxDps = 0.0, gyDps = 0.0, gzDps = 0.0;
float accelMagG = 0.0;
float gyroMagDps = 0.0;

// ===================== FSR STATE =====================
int fsrRaw = 0;
unsigned long fsrOverStartMs = 0;
unsigned long motionEventUntilMs = 0;
unsigned long lastSensorPanicMs = 0;
unsigned long lastSensorPublishMs = 0;

// ===================== I2C HELPERS =====================
bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

uint8_t readReg(uint8_t addr, uint8_t reg, bool* ok = nullptr) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  uint8_t err = Wire.endTransmission(false);
  if (err != 0) { if (ok) *ok = false; return 0; }
  uint8_t n = Wire.requestFrom((int)addr, 1);
  if (n != 1) { if (ok) *ok = false; return 0; }
  if (ok) *ok = true;
  return Wire.read();
}

bool writeReg(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  uint8_t n = Wire.requestFrom((int)addr, (int)len);
  if (n != len) return false;
  for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

int16_t be16(uint8_t hi, uint8_t lo) {
  return (int16_t)((hi << 8) | lo);
}

// ===================== MPU INIT & READ =====================
void initMPU() {
  uint8_t candidates[] = {0x68, 0x69};
  for (uint8_t i = 0; i < 2; i++) {
    if (i2cPresent(candidates[i])) {
      mpuAddr = candidates[i];
      break;
    }
  }

  if (!mpuAddr) {
    Serial.println("MPU not found at 0x68 or 0x69.");
    mpuOk = false;
    return;
  }

  writeReg(mpuAddr, 0x6B, 0x00); // Wake up
  delay(50);
  writeReg(mpuAddr, 0x1C, 0x00); // Accel ±2G
  writeReg(mpuAddr, 0x1B, 0x00); // Gyro ±250 dps
  delay(20);

  bool ok = false;
  mpuWhoAmI = readReg(mpuAddr, 0x75, &ok);
  mpuOk = ok;

  Serial.print("MPU found at ");
  Serial.print(mpuAddr, HEX);
  Serial.print(" WHO_AM_I=");
  Serial.println(mpuWhoAmI, HEX);
}

bool readMPU() {
  if (!mpuAddr) { mpuOk = false; return false; }

  uint8_t b[14];
  if (!readBytes(mpuAddr, 0x3B, b, 14)) { mpuOk = false; return false; }

  axRaw = be16(b[0], b[1]);
  ayRaw = be16(b[2], b[3]);
  azRaw = be16(b[4], b[5]);
  gxRaw = be16(b[8], b[9]);
  gyRaw = be16(b[10], b[11]);
  gzRaw = be16(b[12], b[13]);

  axG = axRaw / 16384.0;
  ayG = ayRaw / 16384.0;
  azG = azRaw / 16384.0;
  gxDps = gxRaw / 131.0;
  gyDps = gyRaw / 131.0;
  gzDps = gzRaw / 131.0;

  accelMagG = sqrt(axG * axG + ayG * ayG + azG * azG);
  gyroMagDps = sqrt(gxDps * gxDps + gyDps * gyDps + gzDps * gzDps);

  mpuOk = true;
  return true;
}

// ===================== INIT SENSORS =====================
void initSensors() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  pinMode(PIN_FSR, INPUT);
  initMPU();
  Serial.println("Sensors initialized.");
}

// ===================== READ SENSORS =====================
void readSensors() {
  fsrRaw = analogRead(PIN_FSR);
  readMPU();
}

// ===================== PUBLISH SENSOR DATA =====================
void publishSensorData() {
  if (millis() - lastSensorPublishMs < SENSOR_PUBLISH_MS) return;
  lastSensorPublishMs = millis();

  // Grip
  String gripPayload = "{";
  gripPayload += "\"tripId\":\"" + activeTripId + "\",";
  gripPayload += "\"value\":" + String(fsrRaw) + ",";
  gripPayload += "\"timestamp\":" + String(millis());
  gripPayload += "}";
  publishMessage(TOPIC_GRIP, gripPayload);

  // Motion
  String motionPayload = "{";
  motionPayload += "\"tripId\":\"" + activeTripId + "\",";
  motionPayload += "\"accelMag\":" + String(accelMagG, 3) + ",";
  motionPayload += "\"angularVel\":" + String(gyroMagDps, 2) + ",";
  motionPayload += "\"timestamp\":" + String(millis());
  motionPayload += "}";
  publishMessage(TOPIC_MOTION, motionPayload);
}

// ===================== PUBLISH EVENT =====================
void publishEvent(String type) {
  String payload = "{";
  payload += "\"tripId\":\"" + activeTripId + "\",";
  payload += "\"type\":\"" + type + "\",";
  payload += "\"lat\":" + String(gpsHasFix ? currentLat : 0.0, 6) + ",";
  payload += "\"lng\":" + String(gpsHasFix ? currentLng : 0.0, 6) + ",";
  payload += "\"accelMag\":" + String(accelMagG, 3) + ",";
  payload += "\"gripValue\":" + String(fsrRaw) + ",";
  payload += "\"timestamp\":" + String(millis());
  payload += "}";
  publishMessage(TOPIC_EVENT, payload);
}

// ===================== DUAL-SENSOR FUSION =====================
void updateSensorFusion(bool tripActive) {
  if (!tripActive) return;

  unsigned long now = millis();

  if (fsrRaw > FSR_THRESHOLD) {
    if (fsrOverStartMs == 0) fsrOverStartMs = now;
  } else {
    fsrOverStartMs = 0;
  }

  bool gripSustained = fsrOverStartMs > 0 &&
                       (now - fsrOverStartMs >= FSR_SUSTAIN_MS);

  bool motionTriggered = accelMagG >= ACCEL_JOLT_G ||
                         gyroMagDps >= GYRO_JOLT_DPS;

  if (motionTriggered) {
    motionEventUntilMs = now + SENSOR_WINDOW_MS;
  }

  bool motionInWindow = now <= motionEventUntilMs;

  if (gripSustained &&
      motionInWindow &&
      (now - lastSensorPanicMs > SENSOR_PANIC_COOLDOWN_MS)) {
    lastSensorPanicMs = now;
    Serial.println("[SENSOR] Dual-sensor panic triggered!");
    publishEvent("panic");
  }
}

#endif