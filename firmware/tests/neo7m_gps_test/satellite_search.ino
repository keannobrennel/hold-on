#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

const int GPS_RX_PIN = D7;
const int GPS_TX_PIN = D8;

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
TinyGPSPlus gps;

unsigned long lastPrint  = 0;
unsigned long searchStart = 0;
bool firstFixAcquired = false;
unsigned long timeToFix = 0;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);
  searchStart = millis();

  Serial.println();
  Serial.println("================================");
  Serial.println("  NEO-7M SATELLITE SEARCH");
  Serial.println("================================");
  Serial.println("Point antenna UP, go outside.");
  Serial.println("Searching...");
  Serial.println();
}

void loop() {
  // Feed all available GPS bytes into TinyGPS
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  // Detect first fix the moment it happens
  if (!firstFixAcquired && gps.location.isValid()) {
    firstFixAcquired = true;
    timeToFix = millis() - searchStart;

    Serial.println();
    Serial.println("========================================");
    Serial.println("         *** FIX ACQUIRED! ***");
    Serial.println("========================================");
    Serial.print  ("  Time to fix : ");
    Serial.print  (timeToFix / 1000);
    Serial.println(" seconds");
    Serial.print  ("  Latitude    : ");
    Serial.println(gps.location.lat(), 6);
    Serial.print  ("  Longitude   : ");
    Serial.println(gps.location.lng(), 6);
    Serial.print  ("  Satellites  : ");
    Serial.println(gps.satellites.isValid() ? gps.satellites.value() : 0);
    Serial.print  ("  Altitude    : ");
    if (gps.altitude.isValid()) {
      Serial.print(gps.altitude.meters(), 1);
      Serial.println(" m");
    } else {
      Serial.println("--");
    }
    Serial.print  ("  HDOP        : ");
    Serial.println(gps.hdop.isValid() ? gps.hdop.hdop() : 99.99);
    Serial.println("========================================");
    Serial.println("Almanac saved. Future fixes will be faster.");
    Serial.println();
  }

  // Print status every 2 seconds
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    printSearchStatus();
  }
}

void printSearchStatus() {
  unsigned long elapsed = (millis() - searchStart) / 1000;
  int sats = gps.satellites.isValid() ? gps.satellites.value() : 0;

  // Progress bar — each satellite = one block, need 4 for fix
  Serial.print("[");
  for (int i = 0; i < 12; i++) {
    if (i < sats)        Serial.print("#");  // locked satellite
    else if (i < 4)      Serial.print("_");  // still needed
    else                 Serial.print(" ");  // bonus satellites
  }
  Serial.print("]  ");

  // Satellite count
  Serial.print(sats);
  Serial.print("/4 sats  ");

  // Elapsed time
  Serial.print(elapsed);
  Serial.print("s  ");

  // Status
  if (firstFixAcquired) {
    Serial.print("FIXED  hdop=");
    Serial.print(gps.hdop.isValid() ? gps.hdop.hdop() : 0, 1);
  } else if (gps.charsProcessed() < 10) {
    Serial.print("NO SIGNAL — check wiring");
  } else if (sats == 0) {
    Serial.print("searching...");
  } else {
    Serial.print("acquiring...");
  }

  Serial.println();

  // Warn if taking too long
  if (!firstFixAcquired && elapsed == 60) {
    Serial.println("  >> 1 min elapsed — make sure you are outdoors with clear sky.");
  }
  if (!firstFixAcquired && elapsed == 180) {
    Serial.println("  >> 3 min elapsed — try moving away from buildings or trees.");
  }
  if (!firstFixAcquired && elapsed == 300) {
    Serial.println("  >> 5 min elapsed — check antenna orientation (ceramic patch faces UP).");
  }
}