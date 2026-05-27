<template>
  <main class="shared-trip-shell">
    <!-- HEADER SLOT: replace this div with <AppHeader /> when your groupmate finishes the header -->
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
          <h3>EVENT LOG</h3>

          <div class="event-scroll">
            <div
              v-for="event in visibleEvents"
              :key="event.id"
              class="event-card"
              :class="event.type"
            >
              <div class="event-icon">
                <i :class="event.icon"></i>
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
          <i class="ti ti-alert-triangle"></i>
          Panic event at 12:09 PM detected
        </div>

        <div id="shared-map" class="map-container"></div>
      </section>

      <!-- RIGHT PANEL -->
      <aside class="right-column">
        <div class="current-trip-card">
          <h2>CURRENT TRIP</h2>

          <div class="location-card active-location">
            <div>
              <strong>{{ pickupLocation }}</strong>
              <span>Pick-Up</span>
            </div>
            <span class="svg-icon location-svg-icon pickup-svg-icon" aria-hidden="true">
            <!-- PASTE PICK-UP SVG HERE -->
            </span>
          </div>

          <div class="route-dots"></div>

          <div class="location-card">
            <div>
              <strong>{{ destination }}</strong>
              <span>Drop-off</span>
            </div>
            <span class="svg-icon location-svg-icon dropoff-svg-icon" aria-hidden="true">
            <!-- PASTE DROP-OFF SVG HERE -->
            </span>
          </div>

          <div class="trip-meta">
            <span>
              <span class="svg-icon meta-svg-icon eta-svg-icon" aria-hidden="true">
                <!-- PASTE ETA SVG HERE -->
              </span>
              ETA {{ eta }}
            </span>
            <span>
              <span class="svg-icon meta-svg-icon distance-svg-icon" aria-hidden="true">
                <!-- PASTE DISTANCE SVG HERE -->
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
  <!-- FOOTER SLOT: replace this div with <BottomNav /> when your groupmate finishes the footer/nav -->
  <div class="layout-footer-slot"></div>
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
    icon: "ti ti-alert-triangle",
  },
]);

const events = ref([
  {
    id: 5,
    title: "Passenger Arrived",
    time: "12:10 PM",
    type: "arrival",
    icon: "ti ti-map-pin-check",
  },
  {
    id: 4,
    title: "Movement resumed",
    time: "12:10 PM",
    type: "success",
    icon: "ti ti-check",
  },
  {
    id: 3,
    title: "Panic cleared",
    time: "12:10 PM",
    type: "success",
    icon: "ti ti-check",
  },
  {
    id: 2,
    title: "Panic detected",
    time: "12:09 PM",
    type: "danger faded",
    icon: "ti ti-alert-triangle",
  },
  {
    id: 1,
    title: "Trip started",
    time: "12:03 PM",
    type: "neutral",
    icon: "ti ti-clock",
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

<style scoped>
.shared-trip-shell {
  height: 100vh;
  background: #f4f4f2;
  padding: 0 32px;
  font-family: Inter, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
  color: #111;
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.layout-header-slot {
  height: 72px;
  flex: 0 0 72px;
}

.layout-footer-slot {
  height: 72px;
  flex: 0 0 72px;
}

.shared-layout {
  flex: 1;
  min-height: 0;
  display: grid;
  grid-template-columns: 286px minmax(520px, 1fr) 360px;
  gap: 32px;
  align-items: start;
}

.left-panel {
  height: 100%;
  min-height: 0;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.stat-card {
  position: relative;
  border-radius: 8px;
  padding: 14px 14px 16px;
  color: white;
  min-height: 84px;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.18);
}

.stat-card p {
  position: absolute;
  left: 14px;
  bottom: 14px;
  width: 62px;
  margin: 0;
  font-size: 10px;
  font-weight: 800;
  line-height: 1.05;
}

.stat-card h1 {
  margin: 2px 0 0;
  font-size: 32px;
  line-height: 1;
  text-align: right;
  font-weight: 900;
  letter-spacing: -1px;
}

.stat-card span {
  display: block;
  font-size: 10px;
  text-align: right;
  opacity: 0.95;
}

.duration-card {
  background: #2f9e48;
}

.panic-card {
  background: #6bd485;
}

.event-section {
  margin-top: 8px;
  border-top: 1px solid #a8a8a8;
  padding-top: 18px;
  min-height: 0;
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.event-section h3 {
  margin: 0 0 14px;
  font-size: 14px;
  color: #168b39;
  font-weight: 900;
  flex: 0 0 auto;
}

.event-scroll {
  flex: 1;
  min-height: 0;
  overflow-y: auto;
  padding-right: 6px;
}

.event-scroll::-webkit-scrollbar {
  width: 6px;
}

.event-scroll::-webkit-scrollbar-track {
  background: transparent;
}

.event-scroll::-webkit-scrollbar-thumb {
  background: #9a9a9a;
  border-radius: 999px;
}

.event-card {
  display: flex;
  align-items: center;
  gap: 10px;
  border-radius: 7px;
  padding: 10px;
  margin-bottom: 10px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.16);
}

.event-card strong {
  display: block;
  font-size: 14px;
}

.event-card small {
  display: block;
  font-size: 8px;
}

.event-icon {
  width: 20px;
  height: 20px;
  border-radius: 5px;
  display: grid;
  place-items: center;
}

.event-card.success {
  background: #d8ffd1;
  color: #151515;
}

.event-card.success .event-icon {
  background: #76cf86;
  color: #176b2c;
}

.event-card.arrival {
  background: #1f1f1f;
  color: #31ff62;
}

.event-card.arrival .event-icon {
  background: #dafcdc;
  color: #168b39;
}

.event-card.danger {
  background: #ffc6c6;
  color: #201010;
}

.event-card.danger .event-icon {
  background: #e46f6f;
  color: #9d1414;
}

.event-card.faded {
  opacity: 0.45;
}

.event-card.neutral {
  background: #d4d4d4;
  color: #222;
}

.event-card.neutral .event-icon {
  background: #bcbcbc;
  color: #777;
}

.map-panel {
  position: relative;
  height: 100%;
  min-height: 0;
  border-radius: 18px;
  overflow: hidden;
  align-self: start;
}

.map-container {
  width: 100%;
  height: 100%;
  min-height: 0;
  border-radius: 18px;
  overflow: hidden;
  filter: saturate(0.85);
}

.map-alert {
  position: absolute;
  z-index: 500;
  top: 20px;
  left: 50%;
  transform: translateX(-50%);
  background: #ffcaca;
  color: #b00000;
  border: 1.5px solid #ef5350;
  border-radius: 8px;
  font-size: 12px;
  padding: 10px 38px;
  display: flex;
  align-items: center;
  gap: 8px;
}

.right-column {
  width: 100%;
  max-width: 360px;
  align-self: start;
  display: flex;
  flex-direction: column;
  gap: 20px;
  box-sizing: border-box;
  overflow: hidden;
}

.current-trip-card {
  background: #1f1f1f;
  color: white;
  border-radius: 32px;
  padding: 38px 28px 30px;
  display: flex;
  flex-direction: column;
  box-sizing: border-box;
  overflow: hidden;
}

.current-trip-card h2 {
  margin: 0 0 16px;
  color: #31ff62;
  font-size: 18px;
  font-weight: 900;
}

.location-card {
  background: #454545;
  border-radius: 6px;
  padding: 12px;
  min-height: 48px;
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.location-card > div {
  flex: 1;
  min-width: 0;
  text-align: left;
}

.location-card strong {
  display: block;
  width: 100%;
  color: white;
  font-size: 14px;
  font-weight: 800;
  text-align: left;
}

.location-card span {
  display: block;
  color: #64ff7d;
  font-size: 8px;
}

.location-card i {
  color: #9dffb0;
}

.svg-icon {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  color: #9dffb0;
  flex: 0 0 auto;
}

.svg-icon svg {
  width: 100%;
  height: 100%;
  display: block;
}

.location-svg-icon {
  width: 14px;
  height: 14px;
  margin-left: 10px;
}

.meta-svg-icon {
  width: 11px;
  height: 11px;
  margin-right: 4px;
  vertical-align: -1px;
}

.active-location::before {
  content: "";
  width: 8px;
  height: 8px;
  background: #31ff62;
  border-radius: 999px;
  margin-right: 10px;
  flex: 0 0 auto;
}

.route-dots {
  height: 30px;
  margin-left: 17px;
  border-left: 2px dotted #777;
}

.trip-meta {
  display: flex;
  justify-content: space-between;
  gap: 10px;
  color: #d8f7dc;
  font-size: 10px;
  margin: 14px 0 0;
  overflow: hidden;
}

.trip-meta span {
  white-space: nowrap;
  font-size: 10px;
}

.trip-meta i {
  color: #9dffb0;
  margin-right: 4px;
}

.passenger-status {
  background: transparent;
  padding: 0 4px;
}

.status-heading {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 18px;
}

.status-heading h3 {
  margin: 0;
  font-size: 15px;
  font-weight: 900;
}

.status-pill {
  min-width: 88px;
  border-radius: 999px;
  padding: 5px 12px;
  font-size: 11px;
  font-weight: 700;
  display: inline-flex;
  justify-content: center;
  align-items: center;
  gap: 7px;
  border: 1px solid transparent;
}

.status-pill span {
  width: 7px;
  height: 7px;
  border-radius: 999px;
}

.status-pill.arrived {
  background: #baf5c5;
  color: #168b39;
  border-color: #62c875;
}

.status-pill.arrived span {
  background: #168b39;
}

.status-pill.panic,
.status-pill.danger {
  background: #ffc6c6;
  color: #b00000;
  border-color: #ef5350;
}

.status-pill.panic span,
.status-pill.danger span {
  background: #b00000;
}

.status-pill.moving {
  background: #d8ffd1;
  color: #168b39;
}

.status-pill.moving span {
  background: #168b39;
}

.call-btn {
  width: 100%;
  border: 0;
  border-radius: 8px;
  background: #31ff62;
  color: #000;
  padding: 11px 14px;
  font-weight: 500;
  cursor: pointer;
}

@media (min-width: 1001px) {
  .shared-trip-shell {
    --left-col: clamp(200px, 15.6vw, 300px);
    --right-col: clamp(300px, 25.25vw, 485px);
    --gap: clamp(20px, 1.65vw, 32px);
    --map-width: clamp(700px, 54vw, 1040px);

    padding: 0 0 0 clamp(24px, 2.5vw, 48px);
    overflow: hidden;
  }

  .shared-layout {
    flex: 1;
    min-height: 0;
    grid-template-columns: var(--left-col) minmax(0, 1fr) var(--right-col);
    gap: var(--gap);
    width: 100%;
  }

  .left-panel,
  .map-panel {
    height: 100%;
  }

  .map-panel {
    width: min(var(--map-width), 100%);
    max-width: 100%;
    height: 100%;
    justify-self: center;
    transform: none;
  }

  .map-container {
    width: 100%;
    height: 100%;
    min-height: 0;
  }

  .right-column {
    height: 100%;
    max-height: 100%;
    width: 100%;
    max-width: var(--right-col);
    justify-self: end;
    overflow: visible;
  }

  .current-trip-card {
    width: 100%;
    max-width: var(--right-col);
    box-sizing: border-box;
    overflow: hidden;
    background: #1f1f1f;
    border-radius: 32px 0 0 32px;
    padding: clamp(24px, 3.5vh, 32px) clamp(24px, 1.9vw, 36px);
  }

  .location-card,
  .trip-meta {
    width: 100%;
    max-width: 413px;
  }

  .location-card {
    min-height: clamp(54px, 7.5vh, 69px);
  }

  .route-dots {
    height: clamp(18px, 2.4vh, 24px);
  }

  .trip-meta {
    margin: 12px 0 0;
    overflow: visible;
    display: flex;
    justify-content: space-between;
    gap: 12px;
  }

  .trip-meta span {
    white-space: nowrap;
    line-height: 1.2;
    font-size: 10px;
  }

  .passenger-status {
    width: 100%;
    max-width: var(--right-col);
    box-sizing: border-box;
    padding: 18px clamp(24px, 1.9vw, 36px) 0;
  }

  .call-btn {
    width: min(229px, 100%);
    height: 42px;
    display: block;
    margin: 0 auto;
  }
}

@media (max-width: 1000px) {
  :global(html),
  :global(body),
  :global(#app) {
    height: auto;
    min-height: 100%;
    overflow-x: hidden;
    overflow-y: auto;
  }

  .shared-trip-shell {
    height: auto;
    min-height: 100vh;
    padding: 18px;
    overflow: visible;
  }

  .shared-layout {
    height: auto;
    display: grid;
    grid-template-columns: 1fr;
    gap: 18px;
  }

  .right-column {
    order: 1;
    width: 100%;
    max-width: none;
  }

  .current-trip-card {
    border-radius: 24px;
    padding: 28px 22px;
  }

  .map-panel {
    order: 2;
    width: 100%;
    height: 380px;
    min-height: 380px;
    align-self: stretch;
    border-radius: 18px;
  }

  .map-container {
    height: 100%;
    min-height: 380px;
  }

  .left-panel {
    order: 3;
    height: auto;
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 14px;
  }

  .event-section {
    grid-column: 1 / -1;
    max-height: 320px;
    min-height: 0;
  }

  .event-scroll {
    max-height: 250px;
  }

  .stat-card {
    min-height: 96px;
  }

  .stat-card h1 {
    font-size: 30px;
  }

  .map-alert {
    width: calc(100% - 36px);
    left: 18px;
    right: 18px;
    transform: none;
    justify-content: center;
    padding: 10px 14px;
    text-align: center;
  }
}

@media (max-width: 600px) {
  .shared-trip-shell {
    padding: 12px;
  }

  .current-trip-card {
    border-radius: 20px;
    padding: 24px 18px;
  }

  .current-trip-card h2 {
    font-size: 20px;
  }

  .location-card {
    min-height: 58px;
  }

  .map-panel {
    height: 320px;
    min-height: 320px;
  }

  .map-container {
    min-height: 320px;
  }

  .left-panel {
    grid-template-columns: 1fr;
  }

  .stat-card {
    min-height: 92px;
  }

  .stat-card h1 {
    font-size: 34px;
  }

  .event-section {
    max-height: 300px;
  }

  .event-scroll {
    max-height: 230px;
  }

  .status-heading {
    align-items: flex-start;
    flex-direction: column;
    gap: 10px;
  }
}
</style>