<template>
  <main class="live-trip-shell">
    <div class="layout-header-slot"></div>

    <section class="trip-layout">
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

        <div
          v-if="tripStarted"
          class="location-card"
          style="
            flex-direction: column;
            align-items: stretch;
            gap: 12px;
            margin-bottom: 22px;
            background: #2a2a2a;
            border: 1px dashed #444;
          "
        >
          <h3
            style="margin: 0; font-size: 11px; color: #888; letter-spacing: 1px"
          >
            HARDWARE SIMULATOR
          </h3>

          <div>
            <div
              style="
                display: flex;
                justify-content: space-between;
                margin-bottom: 6px;
              "
            >
              <span style="font-size: 12px; color: white; font-weight: 600"
                >Grip Pressure (FSR)</span
              >
              <span
                :style="{ color: fsrValue > 100 ? '#31ff62' : '#ff4444' }"
                style="font-size: 12px; font-weight: bold"
              >
                {{ fsrValue }}
              </span>
            </div>
            <input
              type="range"
              min="0"
              max="1024"
              v-model="fsrValue"
              @change="simulateFSR"
              style="width: 100%; cursor: pointer"
            />
            <span style="font-size: 9px; color: #888"
              >Drops below 100 trigger "Not Moving"</span
            >
          </div>

          <button
            @click="simulatePanic"
            style="
              background: #ff4a4a;
              color: white;
              border: none;
              border-radius: 6px;
              padding: 10px;
              font-weight: bold;
              cursor: pointer;
              display: flex;
              align-items: center;
              justify-content: center;
              gap: 8px;
            "
          >
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
            Trigger Hardware Panic
          </button>
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
import { auth, firestore, db } from "../../firebase"; // Added db back!
import { ref as dbRef, onValue, off } from "firebase/database"; // Added RTDB imports back!
import {
  collection,
  addDoc,
  doc,
  updateDoc,
  serverTimestamp,
  onSnapshot,
  query,
  where,
  getDocs,
} from "firebase/firestore";

// --- CUSTOM ICONS ---
const pickupIcon = L.divIcon({
  className: "custom-icon",
  html: `<svg viewBox="0 0 24 24" width="32" height="32" style="filter: drop-shadow(0px 3px 3px rgba(0,0,0,0.3));"><path fill="#2A81CB" stroke="#fff" stroke-width="1.5" d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7zm0 9.5c-1.38 0-2.5-1.12-2.5-2.5s1.12-2.5 2.5-2.5 2.5 1.12 2.5 2.5-1.12 2.5-2.5 2.5z"/></svg>`,
  iconSize: [32, 32],
  iconAnchor: [16, 32],
});

const dropoffIcon = L.divIcon({
  className: "custom-icon",
  html: `<svg viewBox="0 0 24 24" width="32" height="32" style="filter: drop-shadow(0px 3px 3px rgba(0,0,0,0.3));"><path fill="#CB2B3E" stroke="#fff" stroke-width="1.5" d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7zm0 9.5c-1.38 0-2.5-1.12-2.5-2.5s1.12-2.5 2.5-2.5 2.5 1.12 2.5 2.5-1.12 2.5-2.5 2.5z"/></svg>`,
  iconSize: [32, 32],
  iconAnchor: [16, 32],
});

const passengerIcon = L.divIcon({
  className: "custom-icon",
  html: `
    <div class="passenger-marker-wrapper">
      <div class="pulse-ring"></div>
      <svg viewBox="0 0 24 24" width="28" height="28" style="position:relative; z-index:2; filter: drop-shadow(0px 2px 4px rgba(0,0,0,0.5));">
        <path fill="#FFD700" stroke="#111" stroke-width="1.5" d="M12 12c2.21 0 4-1.79 4-4s-1.79-4-4-4-4 1.79-4 4 1.79 4 4 4zm0 2c-2.67 0-8 1.34-8 4v2h16v-2c0-2.66-5.33-4-8-4z"/>
      </svg>
    </div>
  `,
  iconSize: [28, 28],
  iconAnchor: [14, 28],
});

// --- STATE ---
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

// --- HARDWARE SIMULATOR STATE ---
const fsrValue = ref(500); // Defaults to a strong grip (500/1024)
let isHoldingOn = true;

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
let eventsUnsub = null;
let locationRef = null;
let passengerMarker = null;

const panicEvents = ref([]);
const events = ref([]);

// --- COMPUTED ---
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

// --- HELPERS ---
const formatTime = (timestamp) => {
  if (!timestamp) return "";
  const date = timestamp.toDate ? timestamp.toDate() : new Date(timestamp);
  return date.toLocaleTimeString("en-US", {
    hour: "numeric",
    minute: "2-digit",
    hour12: true,
  });
};

const getEventMeta = (type) => {
  switch (type) {
    case "panic":
    case "manual_panic":
      return { title: "Panic detected", iconType: "alert", cardType: "danger" };
    case "not_moving":
      return { title: "Not moving", iconType: "alert", cardType: "danger" };
    case "resumed":
      return {
        title: "Movement resumed",
        iconType: "check",
        cardType: "success",
      };
    case "arrived":
      return {
        title: "Passenger arrived",
        iconType: "arrival",
        cardType: "success",
      };
    case "trip_started":
      return { title: "Trip started", iconType: "clock", cardType: "neutral" };
    default:
      return { title: type, iconType: "clock", cardType: "neutral" };
  }
};

// --- CORE FUNCTIONS ---

const simulateFSR = async () => {
  if (!tripId.value) return;

  // The C++ firmware checks if gripValue > 100
  const currentlyHolding = fsrValue.value > 100;

  // Only push an event if the state actually changed
  if (currentlyHolding !== isHoldingOn) {
    isHoldingOn = currentlyHolding;

    try {
      await addDoc(collection(firestore, "trips", tripId.value, "events"), {
        type: isHoldingOn ? "resumed" : "not_moving",
        timestamp: serverTimestamp(),
      });
    } catch (err) {
      console.error("Failed to simulate FSR event:", err);
    }
  }
};

const simulatePanic = async () => {
  if (!tripId.value) return;

  try {
    await addDoc(collection(firestore, "trips", tripId.value, "events"), {
      type: "panic",
      timestamp: serverTimestamp(),
    });
  } catch (err) {
    console.error("Failed to simulate Panic event:", err);
  }
};

const listenToLocation = (id) => {
  locationRef = dbRef(db, `liveLocation/${id}`);
  onValue(locationRef, (snapshot) => {
    const data = snapshot.val();
    if (!data) return;
    const latlng = [data.lat, data.lng];
    if (passengerMarker) {
      passengerMarker.setLatLng(latlng);
    } else {
      passengerMarker = L.marker(latlng, { icon: passengerIcon })
        .addTo(map)
        .bindPopup("Passenger");
    }
    map.setView(latlng); // This makes the map follow the marker!
  });
};

const listenToEvents = (id) => {
  const eventsRef = collection(firestore, "trips", id, "events");
  eventsUnsub = onSnapshot(eventsRef, (snapshot) => {
    const loaded = [];
    snapshot.forEach((d) => {
      const e = d.data();
      const meta = getEventMeta(e.type);
      loaded.push({
        id: d.id,
        title: meta.title,
        time: formatTime(e.timestamp),
        type: meta.cardType,
        iconType: meta.iconType,
        _ts: e.timestamp?.seconds || 0,
      });

      if (e.type === "panic" || e.type === "manual_panic") {
        panicEvents.value.push(e);
        latestAlert.value = `Panic detected at ${formatTime(e.timestamp)}`;
      }
    });

    events.value = loaded.sort((a, b) => b._ts - a._ts);
  });
};

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
    pickupMarker = L.marker([lat, lng], { icon: pickupIcon })
      .addTo(map)
      .bindPopup("Pick-up: " + name);
  } else {
    destination.value = name;
    destinationCoords.value = { lat, lng };
    destinationSuggestions.value = [];
    if (destinationMarker) map.removeLayer(destinationMarker);
    destinationMarker = L.marker([lat, lng], { icon: dropoffIcon })
      .addTo(map)
      .bindPopup("Drop-off: " + name);
  }

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

    await addDoc(collection(firestore, "trips", tripRef.id, "events"), {
      type: "trip_started",
      timestamp: serverTimestamp(),
    });

    tripId.value = tripRef.id;
    tripStarted.value = true;
    elapsedSeconds.value = 0;
    shareableLink.value = `${window.location.origin}/trip/${tripRef.id}`;

    timer = setInterval(() => {
      elapsedSeconds.value += 1;
    }, 1000);

    listenToEvents(tripRef.id);
    listenToLocation(tripRef.id); // Starts map tracking
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

  if (eventsUnsub) {
    eventsUnsub();
    eventsUnsub = null;
  }

  if (locationRef) {
    off(locationRef);
    locationRef = null;
  }

  if (passengerMarker) {
    map.removeLayer(passengerMarker);
    passengerMarker = null;
  }

  if (tripId.value) {
    try {
      await addDoc(collection(firestore, "trips", tripId.value, "events"), {
        type: "arrived",
        timestamp: serverTimestamp(),
      });

      await updateDoc(doc(firestore, "trips", tripId.value), {
        status: "ended",
        endedAt: serverTimestamp(),
      });
    } catch (err) {
      console.error("Failed to end trip:", err);
    }
  }

  tripId.value = null;
};

const restoreActiveTrip = async () => {
  const user = auth.currentUser;
  if (!user) return;

  try {
    const q = query(
      collection(firestore, "trips"),
      where("userId", "==", user.uid),
      where("status", "==", "active"),
    );

    const snapshot = await getDocs(q);
    if (snapshot.empty) return;

    const tripDoc = snapshot.docs[0];
    const data = tripDoc.data();

    tripId.value = tripDoc.id;
    tripStarted.value = true;
    pickupLocation.value = data.origin || "";
    destination.value = data.destination || "";
    shareableLink.value = `${window.location.origin}/trip/${tripDoc.id}`;

    if (data.startedAt) {
      const startMs = data.startedAt.toDate().getTime();
      elapsedSeconds.value = Math.floor((Date.now() - startMs) / 1000);
    }

    if (data.pickupCoords) {
      pickupCoords.value = data.pickupCoords;
      pickupMarker = L.marker([data.pickupCoords.lat, data.pickupCoords.lng], {
        icon: pickupIcon,
      })
        .addTo(map)
        .bindPopup("Pick-up: " + data.origin);
    }
    if (data.destinationCoords) {
      destinationCoords.value = data.destinationCoords;
      destinationMarker = L.marker(
        [data.destinationCoords.lat, data.destinationCoords.lng],
        { icon: dropoffIcon },
      )
        .addTo(map)
        .bindPopup("Drop-off: " + data.destination);
    }
    if (data.pickupCoords && data.destinationCoords) {
      map.fitBounds(
        L.latLngBounds(
          [data.pickupCoords.lat, data.pickupCoords.lng],
          [data.destinationCoords.lat, data.destinationCoords.lng],
        ),
        { padding: [40, 40] },
      );
    }

    timer = setInterval(() => {
      elapsedSeconds.value += 1;
    }, 1000);

    listenToEvents(tripDoc.id);
    listenToLocation(tripDoc.id); // Restores map tracking
  } catch (err) {
    console.error("Failed to restore active trip:", err);
  }
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

onMounted(async () => {
  await initMap();
  await restoreActiveTrip();
});

onBeforeUnmount(() => {
  if (timer) clearInterval(timer);
  if (map) map.remove();
  if (eventsUnsub) eventsUnsub();
  if (locationRef) off(locationRef);
});
</script>

<style scoped src="./HomeView.css"></style>

<style>
.passenger-marker-wrapper {
  position: relative;
  display: flex;
  justify-content: center;
  align-items: center;
  width: 28px;
  height: 28px;
}

.pulse-ring {
  position: absolute;
  /* Make the base ring the same size as the icon wrapper */
  width: 100%;
  height: 100%;
  top: 0;
  left: 0;
  background-color: rgba(255, 215, 0, 0.35); /* Semi-transparent gold */
  border: 3px solid #ffb300; /* Solid darker-gold border for high visibility */
  border-radius: 50%;
  z-index: 1;
  box-sizing: border-box;
  animation: gps-pulse 1.4s ease-out infinite; /* Slightly faster, smooth fade */
}

@keyframes gps-pulse {
  0% {
    transform: scale(0.8);
    opacity: 1;
  }
  80% {
    transform: scale(4.5); /* Expands much further out */
    opacity: 0;
  }
  100% {
    transform: scale(4.5);
    opacity: 0;
  }
}
</style>
