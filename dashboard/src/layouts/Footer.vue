<script setup>
import { useRoute, useRouter } from 'vue-router'

const route  = useRoute()
const router = useRouter()

const tabs = [
  {
    name: 'LiveMap',
    path: '/live-map',
    label: 'Live Map',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <circle cx="12" cy="12" r="10"/>
            <circle cx="12" cy="12" r="3"/>
            <line x1="12" y1="2" x2="12" y2="5"/>
            <line x1="12" y1="19" x2="12" y2="22"/>
            <line x1="2" y1="12" x2="5" y2="12"/>
            <line x1="19" y1="12" x2="22" y2="12"/>
          </svg>`,
  },
  {
    name: 'TripHistory',
    path: '/trip-history',
    label: 'Trip History',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <polyline points="23 6 13.5 15.5 8.5 10.5 1 18"/>
            <polyline points="17 6 23 6 23 12"/>
          </svg>`,
  },
  {
    name: 'Notifications',
    path: '/notifications',
    label: 'Notifications',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"/>
            <path d="M13.73 21a2 2 0 0 1-3.46 0"/>
          </svg>`,
  },
  {
    name: 'AccountSettings',
    path: '/account-settings',
    label: 'Account Settings',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/>
            <circle cx="12" cy="7" r="4"/>
          </svg>`,
  },
]

const isActive = (path) => route.path === path

const navigate = (path) => router.push(path)
</script>

<template>
  <footer style="
    position: fixed;
    bottom: 0;
    left: 0;
    width: 100%;
    z-index: 50;
    display: flex;
    align-items: stretch;
    height: 52px;
    background: #111111;
    border-top: 1px solid #222;
  ">
    <button
      v-for="tab in tabs"
      :key="tab.name"
      @click="navigate(tab.path)"
      :style="{
        flex: 1,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        gap: '8px',
        border: 'none',
        cursor: 'pointer',
        fontFamily: '\'Segoe UI\', Helvetica, Arial, sans-serif',
        fontSize: '12px',
        fontWeight: isActive(tab.path) ? '600' : '400',
        letterSpacing: '0.2px',
        transition: 'background 0.15s, color 0.15s',
        background: isActive(tab.path) ? '#1a1a1a' : 'transparent',
        color: isActive(tab.path) ? '#2ecc71' : '#888',
        borderTop: isActive(tab.path) ? '2px solid #2ecc71' : '2px solid transparent',
        paddingBottom: '2px',
      }"
    >
      <!-- Icon -->
      <span
        v-html="tab.icon"
        :style="{ display: 'flex', alignItems: 'center', color: isActive(tab.path) ? '#2ecc71' : '#888' }"
      ></span>
      <!-- Label -->
      <span>{{ tab.label }}</span>
    </button>
  </footer>
</template>