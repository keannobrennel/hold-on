<template>
  <main class="live-trip-shell">
    <div class="layout-header-slot"></div>

    <section class="trip-layout">
      <!-- LEFT PANEL -->
      <aside class="left-panel">
        <div class="stat-card duration-card">
          <p>TRIP DURATION</p>
          <h1>{{ tripStarted ? formattedDuration : "--:--:--" }}</h1>
          <span>
            {{ tripStarted ? `of est ${estimatedDuration}` : "start trip to begin timer" }}
          </span>
        </div>

        <div class="stat-card panic-card">
          <p>PANIC EVENTS</p>
          <h1>{{ panicEvents.length }}</h1>
          <span>
            {{ panicEvents.length ? "last trigger at 12:10 PM" : "no panic events yet" }}
          </span>
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
                <!-- SUCCESS: checkmark -->
                <svg v-if="event.type === 'success'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
                  <polyline points="20 6 9 17 4 12"/>
                </svg>
                <!-- DANGER: alert triangle -->
                <svg v-else-if="event.type === 'danger'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
                  <line x1="12" y1="9" x2="12" y2="13"/>
                  <line x1="12" y1="17" x2="12.01" y2="17"/>
                </svg>
                <!-- NEUTRAL: clock for trip started/ready, flag for trip ended -->
                <svg v-else-if="event.iconType === 'flag'" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M4 15s1-1 4-1 5 2 8 2 4-1 4-1V3s-1 1-4 1-5-2-8-2-4 1-4 1z"/>
                  <line x1="4" y1="22" x2="4" y2="15"/>
                </svg>
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
        <div v-if="tripStarted && latestAlert" class="map-alert">
          <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
            <line x1="12" y1="9" x2="12" y2="13"/>
            <line x1="12" y1="17" x2="12.01" y2="17"/>
          </svg>
          {{ latestAlert }}
        </div>

        <div id="home-map" class="map-container"></div>
      </section>

      <!-- RIGHT PANEL -->
      <aside class="right-panel">
        <h2>CURRENT TRIP</h2>

        <div class="location-card">
          <span class="dot dot--active" aria-hidden="true"></span>
          <div>
            <input
              v-model="pickupLocation"
              :disabled="tripStarted"
              placeholder="Search pick-up . . ."
            />
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
            <input
              v-model="destination"
              :disabled="tripStarted"
              placeholder="Search destination . . ."
            />
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
            ETA {{ tripStarted ? eta : "--:--" }}
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
            DISTANCE {{ tripStarted ? distance : "--" }}
          </span>
        </div>

        <div class="share-box">
          <span v-if="!tripStarted">Link will appear after starting trip</span>
          <span v-else>{{ shareableLink }}</span>
        </div>

        <button
          v-if="!tripStarted"
          class="primary-action"
          :disabled="!pickupLocation || !destination"
          @click="startTrip"
        >
          Start
        </button>

        <template v-else>
          <button class="primary-action" @click="copyLink">
            {{ copied ? "Copied!" : "Copy link" }}
          </button>

          <button class="secondary-action" @click="endTrip">
            End trip
          </button>
        </template>
      </aside>
    </section>
    <div class="layout-footer-slot"></div>
  </main>
</template>

<script setup>
import { computed, nextTick, onBeforeUnmount, onMounted, ref } from "vue";
import L from "leaflet";
import "leaflet/dist/leaflet.css";

const pickupLocation = ref("");
const destination = ref("");
const tripStarted = ref(false);
const copied = ref(false);
const elapsedSeconds = ref(0);
const estimatedDuration = ref("00:15:00");
const eta = ref("12:15 PM");
const distance = ref("7.5 KM");
const shareableLink = ref("");
const latestAlert = ref("");

let timer = null;
let map = null;

const panicEvents = ref([]);

const events = ref([
  {
    id: 1,
    title: "Trip ready",
    time: "Waiting",
    type: "neutral",
    iconType: "clock",
  },
]);

const visibleEvents = computed(() => events.value.slice(0, 5));

const formattedDuration = computed(() => {
  const hours = String(Math.floor(elapsedSeconds.value / 3600)).padStart(2, "0");
  const minutes = String(Math.floor((elapsedSeconds.value % 3600) / 60)).padStart(2, "0");
  const seconds = String(elapsedSeconds.value % 60).padStart(2, "0");
  return `${hours}:${minutes}:${seconds}`;
});

const startTrip = () => {
  tripStarted.value = true;
  elapsedSeconds.value = 0;
  shareableLink.value = `${window.location.origin}/sharedview`;

  events.value = [
    {
      id: Date.now(),
      title: "Trip started",
      time: "12:03 PM",
      type: "neutral",
      iconType: "clock",
    },
    ...events.value,
  ];

  timer = setInterval(() => {
    elapsedSeconds.value += 1;
  }, 1000);

  setTimeout(() => {
    addPanicEvent();
  }, 2500);
};

const addPanicEvent = () => {
  latestAlert.value = "Panic event at 12:09 PM detected";

  const panic = {
    id: Date.now(),
    title: "Panic detected",
    time: "12:09 PM",
    type: "danger",
    iconType: "alert",
  };

  panicEvents.value.push(panic);

  events.value = [
    {
      id: Date.now() + 1,
      title: "Panic cleared",
      time: "12:10 PM",
      type: "success",
      iconType: "check",
    },
    {
      id: Date.now() + 2,
      title: "Movement resumed",
      time: "12:10 PM",
      type: "success",
      iconType: "check",
    },
    panic,
    ...events.value,
  ];
};

const copyLink = async () => {
  await navigator.clipboard.writeText(shareableLink.value);
  copied.value = true;
  setTimeout(() => {
    copied.value = false;
  }, 1800);
};

const endTrip = () => {
  tripStarted.value = false;
  latestAlert.value = "";
  clearInterval(timer);
  timer = null;

  events.value = [
    {
      id: Date.now(),
      title: "Trip ended",
      time: "Now",
      type: "neutral",
      iconType: "flag",
    },
    ...events.value,
  ];
};

const initMap = async () => {
  await nextTick();

  map = L.map("home-map", {
    zoomControl: false,
  }).setView([14.5869, 121.0568], 16);

  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution: "© OpenStreetMap",
  }).addTo(map);

  L.marker([14.5869, 121.0568]).addTo(map).bindPopup("SM Megamall");
  L.marker([14.5897, 121.0598]).addTo(map).bindPopup("Destination");
};

onMounted(() => {
  initMap();
});

onBeforeUnmount(() => {
  if (timer) clearInterval(timer);
  if (map) map.remove();
});
</script>

<style scoped>
.live-trip-shell {
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

.trip-layout {
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
  width: 28px;
  height: 28px;
  border-radius: 6px;
  display: grid;
  place-items: center;
  flex: 0 0 auto;
}

.event-icon svg {
  width: 14px;
  height: 14px;
}

.event-card.success {
  background: #d8ffd1;
  color: #151515;
}

.event-card.success .event-icon {
  background: #76cf86;
  color: #176b2c;
}

.event-card.danger {
  background: #ffc6c6;
  color: #201010;
}

.event-card.danger .event-icon {
  background: #e46f6f;
  color: #9d1414;
}

.event-card.neutral {
  background: #d4d4d4;
  color: #222;
}

.event-card.neutral .event-icon {
  background: #bcbcbc;
  color: #777;
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

.right-panel {
  background: #1f1f1f;
  color: white;
  border-radius: 32px;
  padding: 38px 28px 30px;
  display: flex;
  flex-direction: column;
  align-self: start;
  min-height: 0;
  width: 100%;
  max-width: 360px;
  box-sizing: border-box;
  overflow: hidden;
}

.location-card > div {
  flex: 1;
  min-width: 0;
  text-align: left;
  display: flex;
  flex-direction: column;
  justify-content: center;
  gap: 2px;
}

.location-card strong,
.location-card input {
  text-align: left;
}
.share-box,
.primary-action,
.secondary-action {
  box-sizing: border-box;
}

.trip-meta {
  gap: 14px;
  overflow: hidden;
}

.trip-meta span {
  white-space: nowrap;
  font-size: 10px;
}

.right-panel h2 {
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

.location-card strong,
.location-card input {
  display: block;
  width: 100%;
  color: white;
  font-size: 14px;
  font-weight: 800;
  margin: 0;
  padding: 0;
  line-height: 1.2;
}

.location-card > div > span {
  display: block;
  color: #64ff7d;
  font-size: 8px;
  line-height: 1.2;
  margin: 0;
}

.dot {
  width: 10px;
  height: 10px;
  border-radius: 999px;
  flex: 0 0 auto;
  margin-right: 10px;
  display: inline-block;
}

.dot--active {
  background: #31ff62;
}

.dot--inactive {
  background: #888;
}

.location-card input {
  background: transparent;
  border: 0;
  outline: none;
}

.location-card input::placeholder {
  color: #cfcfcf;
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
  width: 18px;
  height: 18px;
  margin-left: 10px;
}

.meta-svg-icon {
  width: 12px;
  height: 12px;
  margin-right: 4px;
  vertical-align: -1px;
}

.route-dots {
  height: 30px;
  margin-left: 17px;
  border-left: 2px dotted #777;
}

.trip-meta {
  display: flex;
  justify-content: space-between;
  color: #d8f7dc;
  font-size: 10px;
  margin: 14px 0 22px;
}

.trip-meta i {
  color: #9dffb0;
  margin-right: 4px;
}

.share-box {
  height: 64px;
  border-radius: 6px;
  background: #6d9275;
  color: #dfffe4;
  display: grid;
  place-items: center;
  text-align: center;
  font-size: 11px;
  padding: 10px;
  margin-bottom: 22px;
  word-break: break-all;
}

/* ── BUTTONS: font-weight 500 ── */
.primary-action,
.secondary-action {
  width: 75%;
  align-self: center;
  border: 0;
  border-radius: 6px;
  padding: 9px 12px;
  font-weight: 500;
  cursor: pointer;
}

.primary-action {
  background: #31ff62;
  color: #000;
}

.primary-action:disabled {
  opacity: 0.45;
  cursor: not-allowed;
}

.secondary-action {
  margin-top: 14px;
  background: white;
  color: #168b39;
}

@media (min-width: 1001px) {
  .live-trip-shell {
    --left-col: clamp(200px, 15.6vw, 300px);
    --right-col: clamp(300px, 25.25vw, 485px);
    --gap: clamp(20px, 1.65vw, 32px);
    --map-width: clamp(700px, 54vw, 1040px);

    padding: 0 0 0 clamp(24px, 2.5vw, 48px);
    overflow: hidden;
  }

  .trip-layout {
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

  .right-panel {
    height: 100%;
    max-height: 100%;
    width: 100%;
    max-width: var(--right-col);
    justify-self: end;
    border-radius: 32px 0 0 32px;
    box-sizing: border-box;
    overflow: hidden;
    background: #1f1f1f;
    padding: clamp(24px, 3.5vh, 32px) clamp(24px, 1.9vw, 36px);
  }

  .location-card,
  .share-box,
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
    margin: 12px 0 18px;
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

  .share-box {
    height: clamp(72px, 10vh, 110px);
    margin-bottom: 18px;
  }

  .primary-action,
  .secondary-action {
    width: min(229px, 100%);
    height: 42px;
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

  .live-trip-shell {
    height: auto;
    min-height: 100vh;
    padding: 18px;
    overflow: visible;
  }

  .trip-layout {
    height: auto;
    display: grid;
    grid-template-columns: 1fr;
    gap: 18px;
  }

  .right-panel {
    order: 1;
    width: 100%;
    max-width: none;
    align-self: stretch;
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

  .trip-meta {
    gap: 10px;
    flex-wrap: wrap;
  }

  .primary-action,
  .secondary-action {
    width: 100%;
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
  .live-trip-shell {
    padding: 12px;
  }

  .right-panel {
    border-radius: 20px;
    padding: 24px 18px;
  }

  .right-panel h2 {
    font-size: 20px;
  }

  .location-card {
    min-height: 58px;
  }

  .location-card input {
    font-size: 14px;
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

  .share-box {
    height: auto;
    min-height: 64px;
  }
}
</style>