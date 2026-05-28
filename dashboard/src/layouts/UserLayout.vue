<script setup>
import { computed } from "vue";
import { useRoute } from "vue-router";
import Header from "../layouts/Header.vue";
import Footer from "../layouts/Footer.vue";

const route = useRoute();

// Check if the user is currently on the shared route
const isSharedView = computed(() => route.path === "/shared");
</script>

<template>
  <div>
    <Header />

    <main 
      :style="{
        paddingTop: '48px',
        paddingBottom: isSharedView ? '0' : '52px',
        minHeight: '100vh',
        overflowY: 'auto'
      }"
    >
      <router-view />
    </main>

    <Footer v-if="!isSharedView" />
  </div>
</template>