<script setup>
import Header from "../layouts/Header.vue";
import Footer from "../layouts/Footer.vue";
</script>

<template>
  <div>
    <Header />

    <!-- pushes content below the fixed header (48px) and above the fixed footer (52px) -->
    <main style="padding-top: 48px; padding-bottom: 52px; min-height: 100vh; overflow-y: auto;">
      <router-view />
    </main>

    <Footer />
  </div>
</template>