<script setup>
import { computed } from "vue";
import { useRoute, useRouter } from "vue-router";
import { signOut } from "firebase/auth";
import { auth } from "../firebase";
import Header from "../layouts/Header.vue";
import Footer from "../layouts/Footer.vue";

const route = useRoute();
const router = useRouter();

const isSharedView = computed(() => route.path.startsWith("/trip/"));

const handleExit = async () => {
  await signOut(auth);
  router.push("/login");
};
</script>

<template>
  <div>
    <Header @exit="handleExit" />

    <main
      :style="{
        paddingTop: '48px',
        paddingBottom: isSharedView ? '0' : '52px',
        minHeight: '100vh',
        overflowY: 'auto',
      }"
    >
      <router-view />
    </main>

    <Footer v-if="!isSharedView" />
  </div>
</template>
