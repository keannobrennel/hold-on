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
            {{
              tripStarted
                ? `of est ${estimatedDuration}`
                : "start trip to begin timer"
            }}
          </span>
        </div>

        <div class="stat-card panic-card">
          <p>PANIC EVENTS</p>
          <h1>{{ panicEvents.length }}</h1>
          <span>
            {{
              panicEvents.length
                ? "last trigger at 12:10 PM"
                : "no panic events yet"
            }}
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
                <svg
                  v-if="event.type === 'success'"
                  xmlns="http://www.w3.org/2000/svg"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="3"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                >
                  <polyline points="20 6 9 17 4 12" />
                </svg>
                <!-- DANGER: alert triangle -->
                <svg
                  v-else-if="event.type === 'danger'"
                  xmlns="http://www.w3.org/2000/svg"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2.5"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                >
                  <path
                    d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"
                  />
                  <line x1="12" y1="9" x2="12" y2="13" />
                  <line x1="12" y1="17" x2="12.01" y2="17" />
                </svg>
                <!-- NEUTRAL: clock for trip started/ready, flag for trip ended -->
                <svg
                  v-else-if="event.iconType === 'flag'"
                  xmlns="http://www.w3.org/2000/svg"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2.5"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                >
                  <path
                    d="M4 15s1-1 4-1 5 2 8 2 4-1 4-1V3s-1 1-4 1-5-2-8-2-4 1-4 1z"
                  />
                  <line x1="4" y1="22" x2="4" y2="15" />
                </svg>
                <svg
                  v-else
                  xmlns="http://www.w3.org/2000/svg"
                  viewBox="0 0 24 24"
                  fill="none"
                  stroke="currentColor"
                  stroke-width="2.5"
                  stroke-linecap="round"
                  stroke-linejoin="round"
                >
                  <circle cx="12" cy="12" r="10" />
                  <polyline points="12 6 12 12 16 14" />
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
          <svg
            xmlns="http://www.w3.org/2000/svg"
            width="16"
            height="16"
            viewBox="0 0 24 24"
            fill="none"
            stroke="currentColor"
            stroke-width="2.5"
            stroke-linecap="round"
            stroke-linejoin="round"
          >
            <path
              d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"
            />
            <line x1="12" y1="9" x2="12" y2="13" />
            <line x1="12" y1="17" x2="12.01" y2="17" />
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
          <div style="position: relative; flex: 1; min-width: 0">
            <input
              v-model="pickupLocation"
              :disabled="tripStarted"
              placeholder="Search pick-up . . ."
              @input="searchPlace(pickupLocation, 'pickup')"
              autocomplete="off"
            />
            <span>Pick-Up</span>
            <div v-if="pickupSuggestions.length" class="suggestions-dropdown">
              <div
                v-for="place in pickupSuggestions"
                :key="place.place_id"
                class="suggestion-item"
                @mousedown.prevent="selectPlace(place, 'pickup')"
              >
                {{ place.display_name.split(",").slice(0, 3).join(",") }}
              </div>
            </div>
          </div>
          <span
            class="svg-icon location-svg-icon pickup-svg-icon"
            aria-hidden="true"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              viewBox="0 0 24 24"
              fill="none"
              stroke="currentColor"
              stroke-width="2.5"
              stroke-linecap="round"
              stroke-linejoin="round"
            >
              <circle cx="12" cy="12" r="10" />
              <circle cx="12" cy="12" r="3" />
              <line x1="12" y1="2" x2="12" y2="6" />
              <line x1="12" y1="18" x2="12" y2="22" />
              <line x1="2" y1="12" x2="6" y2="12" />
              <line x1="18" y1="12" x2="22" y2="12" />
            </svg>
          </span>
        </div>

        <div class="route-dots"></div>

        <div class="location-card">
          <span class="dot dot--inactive" aria-hidden="true"></span>
          <div style="position: relative; flex: 1; min-width: 0">
            <input
              v-model="destination"
              :disabled="tripStarted"
              placeholder="Search destination . . ."
              @input="searchPlace(destination, 'destination')"
              autocomplete="off"
            />
            <span>Drop-off</span>
            <div
              v-if="destinationSuggestions.length"
              class="suggestions-dropdown"
            >
              <div
                v-for="place in destinationSuggestions"
                :key="place.place_id"
                class="suggestion-item"
                @mousedown.prevent="selectPlace(place, 'destination')"
              >
                {{ place.display_name.split(",").slice(0, 3).join(",") }}
              </div>
            </div>
          </div>
          <span
            class="svg-icon location-svg-icon dropoff-svg-icon"
            aria-hidden="true"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              viewBox="0 0 24 24"
              fill="none"
              stroke="currentColor"
              stroke-width="2.5"
              stroke-linecap="round"
              stroke-linejoin="round"
            >
              <path d="M21 10c0 7-9 13-9 13s-9-6-9-13a9 9 0 0 1 18 0z" />
              <circle cx="12" cy="10" r="3" />
            </svg>
          </span>
        </div>

        <p v-if="tripError" style="color: red; font-size: 13px">
          {{ tripError }}
        </p>

        <div class="trip-meta">
          <span>
            <!-- ETA: clock icon -->
            <span
              class="svg-icon meta-svg-icon eta-svg-icon"
              aria-hidden="true"
            >
              <svg
                xmlns="http://www.w3.org/2000/svg"
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                stroke-width="2.5"
                stroke-linecap="round"
                stroke-linejoin="round"
              >
                <circle cx="12" cy="12" r="10" />
                <polyline points="12 6 12 12 16 14" />
              </svg>
            </span>
            ETA {{ tripStarted ? eta : "--:--" }}
          </span>
          <span>
            <!-- Distance: motorcycle icon -->
            <span
              class="svg-icon meta-svg-icon distance-svg-icon"
              aria-hidden="true"
            >
              <svg
                xmlns="http://www.w3.org/2000/svg"
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                stroke-width="2"
                stroke-linecap="round"
                stroke-linejoin="round"
              >
                <circle cx="5.5" cy="17.5" r="2.5" />
                <circle cx="18.5" cy="17.5" r="2.5" />
                <path d="M15 6h-3l-2 6H5.5" />
                <path d="M9 12h6l2-3h2" />
                <path d="M15 6l1.5-3" />
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

          <button class="secondary-action" @click="endTrip">End trip</button>
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
import { auth, firestore } from "../../firebase";
import {
  collection,
  addDoc,
  doc,
  updateDoc,
  serverTimestamp,
} from "firebase/firestore";

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
const tripId = ref(null);
const tripError = ref("");

// Location search
const pickupCoords = ref(null);
const destinationCoords = ref(null);
const pickupSuggestions = ref([]);
const destinationSuggestions = ref([]);
let pickupDebounce = null;
let destinationDebounce = null;

let timer = null;
let map = null;
let pickupMarker = null;
let destinationMarker = null;

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
  const hours = String(Math.floor(elapsedSeconds.value / 3600)).padStart(
    2,
    "0",
  );
  const minutes = String(
    Math.floor((elapsedSeconds.value % 3600) / 60),
  ).padStart(2, "0");
  const seconds = String(elapsedSeconds.value % 60).padStart(2, "0");
  return `${hours}:${minutes}:${seconds}`;
});

const searchPlace = (query, type) => {
  if (type === "pickup") {
    clearTimeout(pickupDebounce);
    if (!query || query.length < 3) {
      pickupSuggestions.value = [];
      return;
    }
    pickupDebounce = setTimeout(async () => {
      try {
        const res = await fetch(
          `https://nominatim.openstreetmap.org/search?q=${encodeURIComponent(query)}&format=json&limit=5&countrycodes=ph`,
          { headers: { "Accept-Language": "en" } },
        );
        pickupSuggestions.value = await res.json();
      } catch {
        pickupSuggestions.value = [];
      }
    }, 400);
  } else {
    clearTimeout(destinationDebounce);
    if (!query || query.length < 3) {
      destinationSuggestions.value = [];
      return;
    }
    destinationDebounce = setTimeout(async () => {
      try {
        const res = await fetch(
          `https://nominatim.openstreetmap.org/search?q=${encodeURIComponent(query)}&format=json&limit=5&countrycodes=ph`,
          { headers: { "Accept-Language": "en" } },
        );
        destinationSuggestions.value = await res.json();
      } catch {
        destinationSuggestions.value = [];
      }
    }, 400);
  }
};

const selectPlace = (place, type) => {
  const lat = parseFloat(place.lat);
  const lng = parseFloat(place.lon);
  const name = place.display_name.split(",").slice(0, 2).join(",").trim();

  if (type === "pickup") {
    pickupLocation.value = name;
    pickupCoords.value = { lat, lng };
    pickupSuggestions.value = [];

    if (pickupMarker) map.removeLayer(pickupMarker);
    pickupMarker = L.marker([lat, lng])
      .addTo(map)
      .bindPopup("Pick-up: " + name);
  } else {
    destination.value = name;
    destinationCoords.value = { lat, lng };
    destinationSuggestions.value = [];

    if (destinationMarker) map.removeLayer(destinationMarker);
    destinationMarker = L.marker([lat, lng])
      .addTo(map)
      .bindPopup("Drop-off: " + name);
  }

  // Fit map to show both markers if both are set
  if (pickupCoords.value && destinationCoords.value) {
    const bounds = L.latLngBounds(
      [pickupCoords.value.lat, pickupCoords.value.lng],
      [destinationCoords.value.lat, destinationCoords.value.lng],
    );
    map.fitBounds(bounds, { padding: [40, 40] });
  } else {
    map.setView([lat, lng], 15);
  }
};

const startTrip = async () => {
  tripError.value = "";
  const user = auth.currentUser;

  if (!user) {
    tripError.value = "You must be logged in to start a trip.";
    return;
  }

  try {
    const tripRef = await addDoc(collection(firestore, "trips"), {
      userId: user.uid,
      origin: pickupLocation.value,
      destination: destination.value,
      pickupCoords: pickupCoords.value || null,
      destinationCoords: destinationCoords.value || null,
      status: "active",
      createdAt: serverTimestamp(),
      startedAt: serverTimestamp(),
    });

    tripId.value = tripRef.id;
    tripStarted.value = true;
    elapsedSeconds.value = 0;
    shareableLink.value = `${window.location.origin}/trip/${tripRef.id}`;

    const now = new Date().toLocaleTimeString("en-US", {
      hour: "numeric",
      minute: "2-digit",
      hour12: true,
    });

    events.value = [
      {
        id: Date.now(),
        title: "Trip started",
        time: now,
        type: "neutral",
        iconType: "clock",
      },
      ...events.value,
    ];

    timer = setInterval(() => {
      elapsedSeconds.value += 1;
    }, 1000);
  } catch (err) {
    console.error("Failed to start trip:", err);
    tripError.value = "Failed to start trip. Please try again.";
  }
};

const copyLink = async () => {
  await navigator.clipboard.writeText(shareableLink.value);
  copied.value = true;
  setTimeout(() => {
    copied.value = false;
  }, 1800);
};

const endTrip = async () => {
  tripStarted.value = false;
  latestAlert.value = "";
  clearInterval(timer);
  timer = null;

  const now = new Date().toLocaleTimeString("en-US", {
    hour: "numeric",
    minute: "2-digit",
    hour12: true,
  });

  events.value = [
    {
      id: Date.now(),
      title: "Trip ended",
      time: now,
      type: "neutral",
      iconType: "flag",
    },
    ...events.value,
  ];

  if (tripId.value) {
    try {
      await updateDoc(doc(firestore, "trips", tripId.value), {
        status: "ended",
        endedAt: serverTimestamp(),
      });
    } catch (err) {
      console.error("Failed to update trip status:", err);
    }
  }

  tripId.value = null;
};

const initMap = async () => {
  await nextTick();
  map = L.map("home-map", { zoomControl: false }).setView(
    [14.5869, 121.0568],
    13,
  );
  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution: "© OpenStreetMap",
  }).addTo(map);
};

onMounted(() => {
  initMap();
});

onBeforeUnmount(() => {
  if (timer) clearInterval(timer);
  if (map) map.remove();
});
</script>

<style scoped src="./HomeView.css"></style>
