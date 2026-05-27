<script setup>
import { useRoute, useRouter } from 'vue-router'

const route  = useRoute()
const router = useRouter()

const tabs = [
  {
    name: 'LiveMap',
    path: '/home',
    label: 'Live Map',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M12 21s-6-5.33-6-11a6 6 0 0 1 12 0c0 5.67-6 11-6 11z"/>
            <circle cx="12" cy="10" r="2.5"/>
          </svg>`,
  },
  {
    name: 'TripHistory',
    path: '/trip-history',
    label: 'Trip History',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M16 3h5v5"/>
            <path d="M21 3l-7 7"/>
            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"/>
            <path d="M8 12h4"/>
            <path d="M8 16h8"/>
          </svg>`,
  },
  {
    name: 'Notifications',
    path: '/notifications',
    label: 'Notifications',
    icon: `<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 24 24" fill="none"
            stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M18 8a6 6 0 0 0-12 0c0 7-3 9-3 9h18s-3-2-3-9"/>
            <path d="M13.73 21a2 2 0 0 1-3.46 0"/>
          </svg>`,
  },
]

const isActive = (path) => route.path === path
const navigate = (path) => router.push(path)
</script>

<template>
  <footer
    style="
      position: fixed;
      bottom: 0;
      left: 0;
      width: 100%;
      height: 45px;
      display: flex;
      background: #f3f3f3;
      border-top: 1px solid #dcdcdc;
      z-index: 999;
      font-family: Inter, sans-serif;
    "
  >
    <button
      v-for="tab in tabs"
      :key="tab.name"
      @click="navigate(tab.path)"
      :style="{
        flex: 1,
        border: 'none',
        borderRadius: '0',
        background: isActive(tab.path) ? '#1E1E1E' : '#f3f3f3',
        color: isActive(tab.path) ? '#3BFF65' : '#1a1a1a',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        gap: '10px',
        fontSize: '15px',
        fontWeight: '700',
        cursor: 'pointer',
        transition: 'all 0.2s ease',
        borderRight: '1px solid #dcdcdc',
      }"
    >
      <span
        v-html="tab.icon"
        :style="{
          display: 'flex',
          alignItems: 'center',
          color: isActive(tab.path) ? '#3BFF65' : '#1a1a1a',
        }"
      ></span>

      <span>{{ tab.label }}</span>
    </button>
  </footer>
</template>