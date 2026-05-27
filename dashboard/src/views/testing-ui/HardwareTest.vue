<template>
  <main class="hardware-test">
    <h1>ESP8266 Hardware Test</h1>

    <section class="card">
      <label>ESP Base URL</label>
      <input
        v-model="espBaseUrl"
        placeholder="http://192.168.4.1"
      />

      <p class="hint">
        Use <b>http://192.168.4.1</b> if connected to ESP8266-TEST fallback WiFi.
      </p>

      <div class="buttons">
        <button @click="getStatus">Status</button>
        <button @click="toggleLed">Toggle LED</button>
        <button @click="testGy91">Test GY-91</button>
        <button @click="testGps">Test NEO-7M GPS</button>
        <button @click="testFsr">Read FSR</button>
      </div>

      <div class="oled-row">
        <input v-model="oledText" placeholder="OLED text" />
        <button @click="testOled">Send OLED Text</button>
      </div>
    </section>

    <section v-if="error" class="error">
      {{ error }}
    </section>

    <section v-if="result" class="card">
      <h2>Quick View</h2>

      <div class="grid">
        <div class="metric" v-if="result.test">
          <span>Sketch</span>
          <strong>{{ result.test }}</strong>
        </div>

        <div class="metric" v-if="result.wifiMode">
          <span>WiFi Mode</span>
          <strong>{{ result.wifiMode }}</strong>
        </div>

        <div class="metric" v-if="result.ip">
          <span>ESP IP</span>
          <strong>{{ result.ip }}</strong>
        </div>

        <div class="metric" v-if="result.freeHeap">
          <span>Free Heap</span>
          <strong>{{ result.freeHeap }}</strong>
        </div>

        <div class="metric" v-if="result.ledOn !== undefined">
          <span>LED</span>
          <strong>{{ result.ledOn ? "ON" : "OFF" }}</strong>
        </div>

        <div class="metric" v-if="result.raw !== undefined">
          <span>FSR Raw</span>
          <strong>{{ result.raw }}</strong>
        </div>

        <div class="metric" v-if="result.voltage !== undefined">
          <span>FSR Voltage</span>
          <strong>{{ result.voltage }} V</strong>
        </div>

        <div class="metric" v-if="result.i2cFound">
          <span>I2C Found</span>
          <strong>{{ result.i2cFound.join(", ") || "None" }}</strong>
        </div>

        <div class="metric" v-if="result.mpu">
          <span>MPU</span>
          <strong>
            {{ result.mpu.present ? `${result.mpu.address} / ${result.mpu.whoAmI}` : "Not found" }}
          </strong>
        </div>

        <div class="metric" v-if="result.bmp">
          <span>BMP/BME</span>
          <strong>
            {{ result.bmp.present ? `${result.bmp.address} / ${result.bmp.chipId}` : "Not found" }}
          </strong>
        </div>

        <div class="metric" v-if="result.gpsChars !== undefined">
          <span>GPS Chars</span>
          <strong>{{ result.gpsChars }}</strong>
        </div>

        <div class="metric" v-if="result.satellites !== undefined">
          <span>Satellites</span>
          <strong>{{ result.satellites }}</strong>
        </div>

        <div class="metric" v-if="result.locationValid !== undefined">
          <span>GPS Location</span>
          <strong>{{ result.locationValid ? "Valid" : "Not fixed yet" }}</strong>
        </div>
      </div>
    </section>

    <section class="card">
      <h2>Raw JSON</h2>
      <pre>{{ formattedResult }}</pre>
    </section>
  </main>
</template>

<script setup>
import { computed, ref } from "vue";

const espBaseUrl = ref("http://192.168.4.1");
const oledText = ref("Hello SH1106");
const result = ref(null);
const error = ref("");

const formattedResult = computed(() => {
  return result.value ? JSON.stringify(result.value, null, 2) : "No result yet.";
});

function cleanBaseUrl() {
  return espBaseUrl.value.replace(/\/+$/, "");
}

async function callEsp(endpoint) {
  error.value = "";

  try {
    const response = await fetch(`${cleanBaseUrl()}${endpoint}`);
    const data = await response.json();

    if (!response.ok) {
      throw new Error(data.error || `HTTP ${response.status}`);
    }

    result.value = data;
  } catch (err) {
    error.value =
      `${err.message}. Make sure your PC is connected to the same WiFi as the ESP. ` +
      `If using fallback mode, connect to ESP8266-TEST and use http://192.168.4.1.`;
  }
}

function getStatus() {
  callEsp("/api/status");
}

function toggleLed() {
  callEsp("/api/led?state=toggle");
}

function testGy91() {
  callEsp("/api/gy91");
}

function testGps() {
  callEsp("/api/gps");
}

function testFsr() {
  callEsp("/api/fsr");
}

function testOled() {
  callEsp(`/api/oled?text=${encodeURIComponent(oledText.value)}`);
}
</script>

<style scoped>
.hardware-test {
  max-width: 950px;
  margin: 0 auto;
  padding: 24px;
}

.card {
  padding: 18px;
  border: 1px solid #ddd;
  border-radius: 14px;
  margin-bottom: 16px;
  background: white;
}

label {
  display: block;
  font-weight: 700;
  margin-bottom: 6px;
}

input {
  padding: 10px;
  min-width: 280px;
  border: 1px solid #ccc;
  border-radius: 8px;
}

.hint {
  color: #555;
}

.buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 14px;
}

button {
  padding: 10px 14px;
  cursor: pointer;
  border: none;
  border-radius: 8px;
  background: #111827;
  color: white;
}

.oled-row {
  margin-top: 14px;
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
  gap: 12px;
}

.metric {
  padding: 12px;
  border: 1px solid #e5e7eb;
  border-radius: 10px;
  background: #f9fafb;
}

.metric span {
  display: block;
  color: #6b7280;
  font-size: 13px;
}

.metric strong {
  display: block;
  margin-top: 4px;
  font-size: 18px;
}

pre {
  background: #111827;
  color: #f9fafb;
  padding: 16px;
  border-radius: 10px;
  overflow-x: auto;
}

.error {
  color: #b91c1c;
  font-weight: bold;
  margin-bottom: 16px;
}
</style>