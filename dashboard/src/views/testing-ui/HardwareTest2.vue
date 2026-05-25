<template>
  <main class="holdon-local">
    <section class="panel hero">
      <div>
        <p class="eyebrow">HoldON Local Prototype</p>
        <h1>Hardware Dashboard</h1>
        <p class="muted">Direct Vue → ESP8266. No Firebase and no backend required.</p>
      </div>

      <div class="connection">
        <label>ESP Base URL</label>
        <input v-model.trim="espBaseUrl" placeholder="http://192.168.4.1" />
        <button @click="connectOnce">Connect</button>
      </div>
    </section>

    <section class="panel controls">
      <div class="buttons">
        <button @click="startTrip">Start Trip</button>
        <button class="secondary" @click="cancelTrip">Cancel Trip</button>
        <button class="danger" @click="sendPanic">Manual Panic</button>
        <button class="secondary" @click="togglePolling">
          {{ polling ? "Stop Polling" : "Start Polling" }}
        </button>
      </div>

      <div class="destination">
        <input v-model.trim="tripId" placeholder="Trip ID" />
        <input v-model.number="destLat" type="number" step="any" placeholder="Destination lat" />
        <input v-model.number="destLng" type="number" step="any" placeholder="Destination lng" />
        <button @click="setDestination">Set Destination</button>
      </div>
    </section>

    <section v-if="error" class="error">{{ error }}</section>

    <section class="grid">
      <div class="panel status-card">
        <h2>Device Status</h2>
        <div class="status-row"><span>State</span><strong>{{ status?.state || "No data" }}</strong></div>
        <div class="status-row"><span>WiFi</span><strong>{{ status?.wifiMode || "-" }}</strong></div>
        <div class="status-row"><span>ESP IP</span><strong>{{ status?.ip || "-" }}</strong></div>
        <div class="status-row"><span>Trip</span><strong>{{ status?.tripId || "-" }}</strong></div>
        <div class="status-row"><span>Last Event</span><strong>{{ status?.event?.type || "-" }}</strong></div>
      </div>

      <div class="panel status-card">
        <h2>GPS</h2>
        <div class="status-row"><span>GPS Data</span><strong>{{ gpsConnected ? "Connected" : "No data" }}</strong></div>
        <div class="status-row"><span>Fix</span><strong>{{ gpsFix ? "Valid" : "Waiting" }}</strong></div>
        <div class="status-row"><span>Satellites</span><strong>{{ status?.gps?.satellites ?? "-" }}</strong></div>
        <div class="status-row"><span>HDOP</span><strong>{{ status?.gps?.hdop ?? "-" }}</strong></div>
        <div class="status-row"><span>Chars</span><strong>{{ status?.gps?.chars ?? 0 }}</strong></div>
      </div>

      <div class="panel status-card">
        <h2>Grip / FSR</h2>
        <div class="big-value">{{ status?.fsr?.raw ?? 0 }}</div>
        <div class="bar"><div class="bar-fill" :style="{ width: fsrPercent + '%' }"></div></div>
        <p class="muted">Threshold: {{ status?.fsr?.threshold ?? 700 }}</p>
      </div>

      <div class="panel status-card">
        <h2>Motion / MPU6050</h2>
        <div class="status-row"><span>MPU</span><strong>{{ status?.motion?.mpuOk ? "OK" : "Error" }}</strong></div>
        <div class="status-row"><span>Accel Mag</span><strong>{{ status?.motion?.accelMagG ?? 0 }} G</strong></div>
        <div class="status-row"><span>Gyro Mag</span><strong>{{ status?.motion?.gyroMagDps ?? 0 }} °/s</strong></div>
        <div class="status-row"><span>Jolt</span><strong>{{ status?.motion?.jolt ? "Yes" : "No" }}</strong></div>
      </div>
    </section>

    <section class="panel">
      <h2>Live Map</h2>
      <p class="muted">Leaflet works without Firebase. Map tiles still need internet unless you use offline tiles.</p>
      <div id="holdonMap"></div>
    </section>

    <section class="panel">
      <h2>Raw JSON</h2>
      <pre>{{ formattedStatus }}</pre>
    </section>
  </main>
</template>

<script setup>
import { computed, nextTick, onMounted, onUnmounted, ref } from "vue";
import L from "leaflet";
import "leaflet/dist/leaflet.css";

const espBaseUrl = ref("http://192.168.4.1");
const status = ref(null);
const error = ref("");
const polling = ref(false);
const pollTimer = ref(null);

const tripId = ref("local-demo-001");
const destLat = ref(14.599512);
const destLng = ref(120.984222);

let map;
let gpsMarker;
let destinationMarker;
let eventMarker;
let routeLine;

const formattedStatus = computed(() => status.value ? JSON.stringify(status.value, null, 2) : "No data yet.");
const gpsConnected = computed(() => (status.value?.gps?.chars || 0) > 0);
const gpsFix = computed(() => status.value?.gps?.valid === true);
const fsrPercent = computed(() => Math.max(0, Math.min(100, (Number(status.value?.fsr?.raw || 0) / 1023) * 100)));

function cleanBaseUrl() {
  return espBaseUrl.value.replace(/\/+$/, "");
}

async function callEsp(endpoint) {
  error.value = "";
  try {
    const response = await fetch(`${cleanBaseUrl()}${endpoint}`);
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || `HTTP ${response.status}`);
    status.value = data;
    updateMap(data);
    return data;
  } catch (err) {
    error.value = `${err.message}. Check ESP URL, WiFi, and firmware.`;
    throw err;
  }
}

function initMap() {
  map = L.map("holdonMap").setView([14.599512, 120.984222], 13);

  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution: "&copy; OpenStreetMap contributors",
  }).addTo(map);

  gpsMarker = L.marker([14.599512, 120.984222]).addTo(map).bindPopup("Waiting for GPS fix");
  routeLine = L.polyline([], { weight: 4 }).addTo(map);
}

function updateMap(data) {
  if (!map || !data) return;

  const gps = data.gps;
  if (gps?.valid && gps.lat !== null && gps.lng !== null) {
    const pos = [gps.lat, gps.lng];
    gpsMarker.setLatLng(pos);
    gpsMarker.bindPopup(`Passenger<br>${gps.lat}, ${gps.lng}`);

    const current = routeLine.getLatLngs();
    current.push(pos);
    routeLine.setLatLngs(current.slice(-150));

    map.setView(pos, Math.max(map.getZoom(), 16));
  }

  if (data.destination?.set) {
    const dest = [data.destination.lat, data.destination.lng];
    if (!destinationMarker) destinationMarker = L.marker(dest).addTo(map).bindPopup("Destination");
    else destinationMarker.setLatLng(dest);
  }

  if (data.event?.type && data.event.type !== "none" && data.event.hasGps) {
    const eventPos = [data.event.lat, data.event.lng];
    if (!eventMarker) eventMarker = L.marker(eventPos).addTo(map);
    else eventMarker.setLatLng(eventPos);
    eventMarker.bindPopup(`Event: ${data.event.type}`);
  }
}

async function connectOnce() { await callEsp("/api/status"); }
async function startTrip() { await callEsp("/api/start"); }
async function cancelTrip() { await callEsp("/api/cancel"); }
async function sendPanic() { await callEsp("/api/panic"); }

async function setDestination() {
  const q = new URLSearchParams({
    tripId: tripId.value,
    destLat: String(destLat.value),
    destLng: String(destLng.value),
  });
  await callEsp(`/api/config?${q.toString()}`);
}

function startPolling() {
  if (pollTimer.value) clearInterval(pollTimer.value);
  polling.value = true;
  connectOnce().catch(() => {});
  pollTimer.value = setInterval(() => callEsp("/api/status").catch(() => {}), 1000);
}

function stopPolling() {
  polling.value = false;
  if (pollTimer.value) clearInterval(pollTimer.value);
  pollTimer.value = null;
}

function togglePolling() {
  if (polling.value) stopPolling();
  else startPolling();
}

onMounted(async () => {
  await nextTick();
  initMap();
  startPolling();
});

onUnmounted(() => {
  stopPolling();
  if (map) map.remove();
});
</script>

<style scoped>
.holdon-local {
  min-height: 100vh;
  background: #f4f6fb;
  padding: 24px;
  color: #111827;
}

.panel {
  background: white;
  border: 1px solid #e5e7eb;
  border-radius: 18px;
  padding: 18px;
  margin-bottom: 16px;
  box-shadow: 0 12px 30px rgba(15, 23, 42, 0.06);
}

.hero {
  display: flex;
  justify-content: space-between;
  gap: 18px;
  align-items: flex-end;
}

.eyebrow {
  text-transform: uppercase;
  color: #2563eb;
  font-weight: 800;
  letter-spacing: 0.08em;
  margin: 0;
}

h1, h2 { margin-top: 0; }
.muted { color: #6b7280; }

.connection {
  display: grid;
  gap: 8px;
  min-width: 320px;
}

input {
  border: 1px solid #d1d5db;
  border-radius: 12px;
  padding: 10px 12px;
  font: inherit;
}

.buttons, .destination {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.destination { margin-top: 12px; }

button {
  border: none;
  border-radius: 12px;
  padding: 10px 14px;
  background: #111827;
  color: white;
  cursor: pointer;
  font-weight: 700;
}

button.secondary { background: #e5e7eb; color: #111827; }
button.danger { background: #dc2626; }

.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: 16px;
}

.status-row {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  padding: 8px 0;
  border-bottom: 1px solid #f3f4f6;
}

.big-value {
  font-size: 42px;
  font-weight: 900;
}

.bar {
  height: 14px;
  border-radius: 999px;
  background: #e5e7eb;
  overflow: hidden;
}

.bar-fill {
  height: 100%;
  background: #111827;
  transition: width 160ms ease;
}

#holdonMap {
  height: 440px;
  border-radius: 16px;
  overflow: hidden;
  border: 1px solid #e5e7eb;
}

pre {
  background: #111827;
  color: #f9fafb;
  padding: 16px;
  border-radius: 14px;
  overflow-x: auto;
}

.error {
  color: #991b1b;
  background: #fee2e2;
  border: 1px solid #fecaca;
  border-radius: 14px;
  padding: 12px 16px;
  margin-bottom: 16px;
}

@media (max-width: 760px) {
  .hero { display: block; }
  .connection { min-width: 0; }
}
</style>
