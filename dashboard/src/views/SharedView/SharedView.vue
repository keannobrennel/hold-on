<template>
  <main class="shared-trip-shell">
    <div class="layout-header-slot"></div>
    <section class="shared-layout">
      <!-- LEFT PANEL -->
      <aside class="left-panel">
        <div class="stat-card duration-card">
          <p>TRIP DURATION</p>
          <h1>{{ formattedDuration }}</h1>
          <span>of est {{ estimatedDuration }}</span>
        </div>

        <div class="stat-card panic-card">
          <p>PANIC EVENTS</p>
          <h1>{{ panicEvents.length }}</h1>
          <span>last trigger at 12:10 PM</span>
        </div>

        <div class="event-section">
          <h3>EVENT LOGS</h3>

          <div class="event-scroll">
            <div
              v-for="event in visibleEvents"
              :key="event.id"
              class="event-card"
              :class="event.type"
            >
              <div class="event-icon">
                <!-- arrival: map-pin check -->
                <svg v-if="event.iconType === 'arrival'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M21 10c0 7-9 13-9 13s-9-6-9-13a9 9 0 0 1 18 0z"/>
                  <polyline points="9 10 11 12 15 8"/>
                </svg>
                <!-- success: checkmark -->
                <svg v-else-if="event.iconType === 'check'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
                  <polyline points="20 6 9 17 4 12"/>
                </svg>
                <!-- danger: alert triangle -->
                <svg v-else-if="event.iconType === 'alert'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
                  <line x1="12" y1="9" x2="12" y2="13"/>
                  <line x1="12" y1="17" x2="12.01" y2="17"/>
                </svg>
                <!-- flag: trip ended -->
                <svg v-else-if="event.iconType === 'flag'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M4 15s1-1 4-1 5 2 8 2 4-1 4-1V3s-1 1-4 1-5-2-8-2-4 1-4 1z"/>
                  <line x1="4" y1="22" x2="4" y2="15"/>
                </svg>
                <!-- default: clock -->
                <svg v-else xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <circle cx="12" cy="12" r="10"/>
                  <polyline points="12 6 12 12 16 14"/>
                </svg>
              </div>
              <div>
                <strong>{{ event.title }}</strong>
                <small>{{ event.time }}</small>
              </div>
            </div>
          </div>
        </div>
      </aside>

      <!-- CENTER MAP -->
      <section class="map-panel">
        <div v-if="showPanicAlert" class="map-alert">
          <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
            <line x1="12" y1="9" x2="12" y2="13"/>
            <line x1="12" y1="17" x2="12.01" y2="17"/>
          </svg>
          Panic event at 12:09 PM detected
        </div>

        <div id="shared-map" class="map-container"></div>
      </section>

      <!-- RIGHT PANEL -->
      <aside class="right-column">
        <div class="current-trip-card">
          <h2>CURRENT TRIP</h2>

          <div class="location-card">
            <span class="dot dot--active" aria-hidden="true"></span>
            <div>
              <strong>{{ pickupLocation }}</strong>
              <span>Pick-Up</span>
            </div>
            <!-- Pick-up: crosshair/target icon -->
            <span class="svg-icon location-svg-icon pickup-svg-icon" aria-hidden="true">
              <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                <circle cx="12" cy="12" r="10"/>
                <circle cx="12" cy="12" r="3"/>
                <line x1="12" y1="2" x2="12" y2="6"/>
                <line x1="12" y1="18" x2="12" y2="22"/>
                <line x1="2" y1="12" x2="6" y2="12"/>
                <line x1="18" y1="12" x2="22" y2="12"/>
              </svg>
            </span>
          </div>

          <div class="route-dots"></div>

          <div class="location-card">
            <span class="dot dot--inactive" aria-hidden="true"></span>
            <div>
              <strong>{{ destination }}</strong>
              <span>Drop-off</span>
            </div>
            <!-- Drop-off: map pin icon -->
            <span class="svg-icon location-svg-icon dropoff-svg-icon" aria-hidden="true">
              <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                <path d="M21 10c0 7-9 13-9 13s-9-6-9-13a9 9 0 0 1 18 0z"/>
                <circle cx="12" cy="10" r="3"/>
              </svg>
            </span>
          </div>

          <div class="trip-meta">
            <span>
              <!-- ETA: clock icon -->
              <span class="svg-icon meta-svg-icon eta-svg-icon" aria-hidden="true">
                <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <circle cx="12" cy="12" r="10"/>
                  <polyline points="12 6 12 12 16 14"/>
                </svg>
              </span>
              ETA {{ eta }}
            </span>
            <span>
              <!-- Distance: motorcycle icon -->
              <span class="svg-icon meta-svg-icon distance-svg-icon" aria-hidden="true">
                <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <circle cx="5.5" cy="17.5" r="2.5"/>
                  <circle cx="18.5" cy="17.5" r="2.5"/>
                  <path d="M15 6h-3l-2 6H5.5"/>
                  <path d="M9 12h6l2-3h2"/>
                  <path d="M15 6l1.5-3"/>
                </svg>
              </span>
              DISTANCE {{ distance }}
            </span>
          </div>
        </div>

        <div class="passenger-status">
          <div class="status-heading">
            <h3>PASSENGER STATUS</h3>
            <span class="status-pill" :class="passengerStatusClass">
              <span></span>
              {{ passengerStatus }}
            </span>
          </div>

          <button class="call-btn" @click="callPassenger">
            Call Passenger
          </button>
        </div>
      </aside>
    </section>
  </main>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, onMounted, ref } from "vue";
import L from "leaflet";
import "leaflet/dist/leaflet.css";

const pickupLocation = ref("SM Megamall");
const destination = ref("SMDC Shine Residences");
const eta = ref("12:15 PM");
const distance = ref("7.5 KM");
const estimatedDuration = ref("00:15:00");
const elapsedSeconds = ref(820);
const passengerStatus = ref("Arrived");
const showPanicAlert = ref(false);

let map = null;
let timer = null;

const panicEvents = ref([
  {
    id: 1,
    title: "Panic detected",
    time: "12:09 PM",
    type: "danger",
    iconType: "alert",
  },
]);

const events = ref([
  {
    id: 5,
    title: "Passenger Arrived",
    time: "12:10 PM",
    type: "arrival",
    iconType: "arrival",
  },
  {
    id: 4,
    title: "Movement resumed",
    time: "12:10 PM",
    type: "success",
    iconType: "check",
  },
  {
    id: 3,
    title: "Panic cleared",
    time: "12:10 PM",
    type: "success",
    iconType: "check",
  },
  {
    id: 2,
    title: "Panic detected",
    time: "12:09 PM",
    type: "danger faded",
    iconType: "alert",
  },
  {
    id: 1,
    title: "Trip started",
    time: "12:03 PM",
    type: "neutral",
    iconType: "clock",
  },
]);

const visibleEvents = computed(() => events.value);

const passengerStatusClass = computed(() => {
  if (passengerStatus.value === "Arrived") return "arrived";
  if (passengerStatus.value === "Panic") return "panic";
  if (passengerStatus.value === "Not Moving") return "danger";
  return "moving";
});

const formattedDuration = computed(() => {
  const hours = String(Math.floor(elapsedSeconds.value / 3600)).padStart(2, "0");
  const minutes = String(Math.floor((elapsedSeconds.value % 3600) / 60)).padStart(2, "0");
  const seconds = String(elapsedSeconds.value % 60).padStart(2, "0");
  return `${hours}:${minutes}:${seconds}`;
});

const callPassenger = () => {
  alert("Calling passenger...");
};

const initMap = async () => {
  await nextTick();

  map = L.map("shared-map", {
    zoomControl: false,
  }).setView([14.5869, 121.0568], 16);

  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution: "© OpenStreetMap",
  }).addTo(map);

  L.marker([14.5869, 121.0568]).addTo(map).bindPopup("Passenger location");
  L.marker([14.5897, 121.0598]).addTo(map).bindPopup("Destination");
};

onMounted(() => {
  initMap();
  timer = setInterval(() => {
    elapsedSeconds.value += 1;
  }, 1000);
});

onBeforeUnmount(() => {
  if (timer) clearInterval(timer);
  if (map) map.remove();
});
</script>

<style scoped src="./SharedView.css"></style>
