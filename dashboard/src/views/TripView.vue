<template>
  <div class="page-wrapper">
    <div class="topbar">
      <div class="brand">
        <i class="ti ti-shield-lock" aria-hidden="true"></i>
        <span>HoldON</span>
      </div>
      <div class="trip-status" v-if="trip">
        <span class="status-dot" :class="trip.status"></span>
        {{ trip.status }}
      </div>
    </div>

    <div class="alerts">
      <div class="alert danger" v-if="panicEvent">
        <i class="ti ti-alert-triangle"></i>
        Panic alert at {{ panicEvent.lat }}, {{ panicEvent.lng }}
      </div>
      <div class="alert warning" v-if="notMoving">
        <i class="ti ti-player-pause"></i>
        Passenger not moving — please check immediately
      </div>
    </div>

    <div class="trip-info" v-if="trip">
      <div class="info-item">
        <i class="ti ti-map-pin"></i>
        <div>
          <span class="info-label">From</span>
          <span class="info-value">{{ trip.origin }}</span>
        </div>
      </div>
      <div class="info-divider"></div>
      <div class="info-item">
        <i class="ti ti-flag"></i>
        <div>
          <span class="info-label">To</span>
          <span class="info-value">{{ trip.destination }}</span>
        </div>
      </div>
    </div>

    <div id="map"></div>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import { useRoute } from "vue-router";
import { firestore, db } from "../firebase";
import { doc, getDoc } from "firebase/firestore";
import { ref as dbRef, onValue } from "firebase/database";
import L from "leaflet";
import "leaflet/dist/leaflet.css";

const route = useRoute();
const trip = ref(null);
const panicEvent = ref(null);
const notMoving = ref(false);
let map = null;
let marker = null;

onMounted(async () => {
  const tripDoc = await getDoc(doc(firestore, "trips", route.params.tripId));
  if (tripDoc.exists()) {
    trip.value = tripDoc.data();
  }

  map = L.map("map").setView([14.5995, 120.9842], 13);
  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    attribution: "© OpenStreetMap contributors",
  }).addTo(map);

  const locationRef = dbRef(db, "liveLocation");
  onValue(locationRef, (snapshot) => {
    const data = snapshot.val();
    if (data) {
      const latlng = [data.lat, data.lng];
      if (marker) {
        marker.setLatLng(latlng);
      } else {
        marker = L.marker(latlng).addTo(map);
      }
      map.setView(latlng);
    }
  });
});
</script>

<style scoped>
.page-wrapper {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}

.topbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 1.5rem;
  border-bottom: 0.5px solid var(--border);
  background: var(--bg-secondary);
}

.brand {
  display: flex;
  align-items: center;
  gap: 8px;
  font-weight: 500;
  font-size: 17px;
}

.brand i {
  font-size: 20px;
  color: var(--accent);
}

.trip-status {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  color: var(--text-secondary);
  text-transform: capitalize;
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--text-secondary);
}

.status-dot.waiting {
  background: var(--accent);
}
.status-dot.active {
  background: var(--success);
}
.status-dot.ended {
  background: var(--text-secondary);
}

.alerts {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
  padding: 0.75rem 1rem;
}

.alert {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 10px 14px;
  border-radius: var(--radius-md);
  font-size: 14px;
  font-weight: 500;
}

.alert.danger {
  background: rgba(226, 75, 74, 0.12);
  border: 0.5px solid var(--danger);
  color: var(--danger);
}

.alert.warning {
  background: rgba(224, 92, 42, 0.12);
  border: 0.5px solid var(--accent);
  color: var(--accent);
}

.trip-info {
  display: flex;
  align-items: center;
  gap: 1rem;
  padding: 1rem 1.5rem;
  background: var(--bg-card);
  border-bottom: 0.5px solid var(--border);
}

.info-item {
  display: flex;
  align-items: center;
  gap: 8px;
  flex: 1;
}

.info-item i {
  font-size: 18px;
  color: var(--accent);
  flex-shrink: 0;
}

.info-item div {
  display: flex;
  flex-direction: column;
}

.info-label {
  font-size: 11px;
  color: var(--text-secondary);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.info-value {
  font-size: 14px;
  color: var(--text-primary);
}

.info-divider {
  width: 0.5px;
  height: 36px;
  background: var(--border);
}

#map {
  flex: 1;
  min-height: 400px;
}
</style>
