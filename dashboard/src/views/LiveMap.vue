<script setup>
import { onMounted, onUnmounted, ref } from "vue";

// Leaflet is loaded via CDN in the head — we access it via window.L
const mapContainer = ref(null);
let map = null;
let markersLayer = null;

// ── Mock pickup/drop points (replace with real data later) ────────────────────
const stops = [
  { id: 1, type: "pickup", label: "Stop A",  lat: 14.5995, lng: 120.9842, passenger: "Juan dela Cruz" },
  { id: 2, type: "pickup", label: "Stop B",  lat: 14.6050, lng: 120.9900, passenger: "Maria Santos" },
  { id: 3, type: "drop",   label: "Stop C",  lat: 14.6120, lng: 120.9780, passenger: "Pedro Reyes" },
  { id: 4, type: "pickup", label: "Stop D",  lat: 14.5930, lng: 120.9760, passenger: "Ana Gomez" },
  { id: 5, type: "drop",   label: "Stop E",  lat: 14.6080, lng: 121.0000, passenger: "Carlos Lim" },
];

const selectedStop = ref(null);

const pickupIcon = (L) => L.divIcon({
  className: "",
  html: `<div style="
    width: 32px; height: 32px;
    background: #2ecc71;
    border: 3px solid #fff;
    border-radius: 50% 50% 50% 0;
    transform: rotate(-45deg);
    box-shadow: 0 2px 8px rgba(0,0,0,0.35);
  "></div>`,
  iconSize: [32, 32],
  iconAnchor: [16, 32],
  popupAnchor: [0, -34],
});

const dropIcon = (L) => L.divIcon({
  className: "",
  html: `<div style="
    width: 32px; height: 32px;
    background: #e74c3c;
    border: 3px solid #fff;
    border-radius: 50% 50% 50% 0;
    transform: rotate(-45deg);
    box-shadow: 0 2px 8px rgba(0,0,0,0.35);
  "></div>`,
  iconSize: [32, 32],
  iconAnchor: [16, 32],
  popupAnchor: [0, -34],
});

const initMap = () => {
  const L = window.L;
  if (!L || !mapContainer.value) return;

  // Center on Metro Manila
  map = L.map(mapContainer.value, {
    center: [14.6010, 120.9870],
    zoom: 14,
    zoomControl: true,
    attributionControl: true,
  });

  // Dark tile layer matching the app theme
  L.tileLayer("https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png", {
    attribution: '&copy; <a href="https://carto.com/">CARTO</a>',
    subdomains: "abcd",
    maxZoom: 19,
  }).addTo(map);

  markersLayer = L.layerGroup().addTo(map);

  stops.forEach((stop) => {
    const icon = stop.type === "pickup" ? pickupIcon(L) : dropIcon(L);
    const marker = L.marker([stop.lat, stop.lng], { icon })
      .addTo(markersLayer)
      .bindPopup(`
        <div style="font-family: 'Segoe UI', sans-serif; min-width: 140px;">
          <div style="font-weight: 700; font-size: 13px; color: ${stop.type === 'pickup' ? '#2ecc71' : '#e74c3c'}; margin-bottom: 4px;">
            ${stop.type === "pickup" ? "⬆ Pickup" : "⬇ Drop-off"}
          </div>
          <div style="font-size: 12px; color: #111; font-weight: 600;">${stop.label}</div>
          <div style="font-size: 11px; color: #555; margin-top: 2px;">${stop.passenger}</div>
        </div>
      `, { maxWidth: 200 });

    marker.on("click", () => {
      selectedStop.value = stop;
    });
  });
};

const loadLeaflet = () => {
  return new Promise((resolve) => {
    if (window.L) { resolve(); return; }

    const link = document.createElement("link");
    link.rel = "stylesheet";
    link.href = "https://unpkg.com/leaflet@1.9.4/dist/leaflet.css";
    document.head.appendChild(link);

    const script = document.createElement("script");
    script.src = "https://unpkg.com/leaflet@1.9.4/dist/leaflet.js";
    script.onload = resolve;
    document.head.appendChild(script);
  });
};

onMounted(async () => {
  await loadLeaflet();
  initMap();
});

onUnmounted(() => {
  if (map) { map.remove(); map = null; }
});
</script>

<template>
  <div class="livemap-wrapper">

    <!-- Legend -->
    <div class="legend">
      <span class="legend-item">
        <span class="dot dot--pickup"></span> Pickup
      </span>
      <span class="legend-item">
        <span class="dot dot--drop"></span> Drop-off
      </span>
      <span class="legend-count">{{ stops.length }} stops</span>
    </div>

    <!-- Map -->
    <div ref="mapContainer" class="map-container"></div>

    <!-- Selected stop info panel -->
    <transition name="slide-up">
      <div v-if="selectedStop" class="stop-panel">
        <button class="stop-panel-close" @click="selectedStop = null">✕</button>
        <div class="stop-panel-type" :class="selectedStop.type">
          {{ selectedStop.type === 'pickup' ? '⬆ Pickup' : '⬇ Drop-off' }}
        </div>
        <div class="stop-panel-label">{{ selectedStop.label }}</div>
        <div class="stop-panel-passenger">{{ selectedStop.passenger }}</div>
        <div class="stop-panel-coords">
          {{ selectedStop.lat.toFixed(4) }}, {{ selectedStop.lng.toFixed(4) }}
        </div>
      </div>
    </transition>

  </div>
</template>

<style scoped>
.livemap-wrapper {
  position: relative;
  width: 100%;
  height: calc(100vh - 48px - 52px); /* subtract header + footer */
  background: #0a0a0a;
  overflow: hidden;
}

.map-container {
  width: 100%;
  height: 100%;
}

/* ── Legend ────────────────────────────────────────────────────────────────── */
.legend {
  position: absolute;
  top: 16px;
  left: 16px;
  z-index: 1000;
  background: rgba(17, 17, 17, 0.9);
  border: 1px solid #2a2a2a;
  border-radius: 10px;
  padding: 8px 14px;
  display: flex;
  align-items: center;
  gap: 14px;
  backdrop-filter: blur(6px);
}

.legend-item {
  display: flex;
  align-items: center;
  gap: 6px;
  font-family: "Segoe UI", sans-serif;
  font-size: 11px;
  font-weight: 500;
  color: #ccc;
}

.dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  display: inline-block;
}

.dot--pickup { background: #2ecc71; }
.dot--drop   { background: #e74c3c; }

.legend-count {
  font-family: "Segoe UI", sans-serif;
  font-size: 11px;
  color: #555;
  border-left: 1px solid #2a2a2a;
  padding-left: 12px;
}

/* ── Stop info panel ───────────────────────────────────────────────────────── */
.stop-panel {
  position: absolute;
  bottom: 16px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 1000;
  background: rgba(17, 17, 17, 0.95);
  border: 1px solid #2a2a2a;
  border-radius: 14px;
  padding: 16px 20px;
  min-width: 220px;
  backdrop-filter: blur(8px);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.5);
}

.stop-panel-close {
  position: absolute;
  top: 10px;
  right: 12px;
  background: none;
  border: none;
  color: #555;
  font-size: 13px;
  cursor: pointer;
  padding: 0;
  line-height: 1;
}
.stop-panel-close:hover { color: #fff; }

.stop-panel-type {
  font-family: "Segoe UI", sans-serif;
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 0.5px;
  text-transform: uppercase;
  margin-bottom: 4px;
}
.stop-panel-type.pickup { color: #2ecc71; }
.stop-panel-type.drop   { color: #e74c3c; }

.stop-panel-label {
  font-family: "Segoe UI", sans-serif;
  font-size: 15px;
  font-weight: 700;
  color: #fff;
}

.stop-panel-passenger {
  font-family: "Segoe UI", sans-serif;
  font-size: 12px;
  color: #aaa;
  margin-top: 2px;
}

.stop-panel-coords {
  font-family: "Courier New", monospace;
  font-size: 10px;
  color: #444;
  margin-top: 8px;
}

/* ── Transition ────────────────────────────────────────────────────────────── */
.slide-up-enter-active,
.slide-up-leave-active {
  transition: all 0.25s ease;
}
.slide-up-enter-from,
.slide-up-leave-to {
  opacity: 0;
  transform: translateX(-50%) translateY(16px);
}
</style>