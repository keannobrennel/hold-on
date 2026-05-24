<script setup>
import { ref } from "vue";
import { useRouter } from "vue-router";
import {
  GoogleAuthProvider,
  FacebookAuthProvider,
  GithubAuthProvider,
  signInWithPopup,
} from "firebase/auth";
import { auth } from "../firebase";

const router = useRouter();
const darkMode = ref(false);
const isLoading = ref("");
const errorMessage = ref("");

const toggleTheme = () => {
  darkMode.value = !darkMode.value;
};

const signInWith = async (providerName) => {
  if (isLoading.value) return;
  isLoading.value = providerName;
  errorMessage.value = "";

  let provider;
  switch (providerName) {
    case "google":
      provider = new GoogleAuthProvider();
      break;
    case "facebook":
      provider = new FacebookAuthProvider();
      break;
    case "github":
      provider = new GithubAuthProvider();
      break;
    default:
      return;
  }

  try {
    await signInWithPopup(auth, provider);
    router.push("/home");
  } catch (error) {
    console.error(error);
    switch (error.code) {
      case "auth/popup-closed-by-user":
        errorMessage.value = "Popup closed. Please try again.";
        break;
      case "auth/account-exists-with-different-credential":
        errorMessage.value = "Account already exists with a different provider.";
        break;
      case "auth/cancelled-popup-request":
        errorMessage.value = "";
        break;
      default:
        errorMessage.value = "Authentication failed. Please try again.";
    }
  } finally {
    isLoading.value = "";
  }
};
</script>

<template>
  <div
    :style="{
      width: '100%',
      minHeight: '100vh',
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'center',
      fontFamily: '\'Inter\', sans-serif',
      background: darkMode ? '#1E1E1E' : '#DEFFD2',
      transition: 'background 0.3s ease',
      position: 'relative',
    }"
  >

    <!-- THEME TOGGLE -->
    <button
      @click="toggleTheme"
      :style="{
        position: 'absolute',
        top: '25px',
        right: '25px',
        width: '42px',
        height: '42px',
        borderRadius: '50%',
        border: '2px solid #39FF5A',
        background: darkMode ? '#2A2A2A' : '#F0F0F0',
        cursor: 'pointer',
        zIndex: 50,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        transition: 'all 0.3s ease',
        boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
      }"
    >
      <svg v-if="!darkMode" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#39FF5A" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="5"/>
        <line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/>
        <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/>
        <line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/>
        <line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/>
      </svg>
      <svg v-else width="18" height="18" viewBox="0 0 24 24" fill="#39FF5A" stroke="#39FF5A" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
        <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/>
      </svg>
    </button>

    <!-- CARD -->
    <div
      :style="{
        width: '100%',
        maxWidth: '420px',
        padding: '48px 40px',
        borderRadius: '20px',
        background: darkMode ? '#2A2A2A' : '#F9F9F9',
        boxShadow: darkMode
          ? '0 8px 32px rgba(0,0,0,0.4)'
          : '0 8px 32px rgba(0,0,0,0.08)',
        transition: 'all 0.3s ease',
      }"
    >

      <!-- TITLE -->
      <h1
        :style="{
          fontSize: '45px',
          fontWeight: 800,
          letterSpacing: '-1.5px',
        }"
      >
        <span :style="{ color: darkMode ? '#FFFFFF' : '#000000' }">Create </span>
        <span :style="{ color: '#3BFF65' }">Account</span>
      </h1>

      <p
        :style="{
          fontSize: '13px',
          fontWeight: 500,
          color: darkMode ? '#AAAAAA' : '#777777',
          marginBottom: '32px',
        }"
      >
        Choose a provider to get started with HoldON.
      </p>

      <!-- PROVIDER BUTTONS -->
      <div :style="{ display: 'flex', flexDirection: 'column', gap: '14px' }">

        <!-- GOOGLE -->
        <button
          @click="signInWith('google')"
          :style="{
            width: '100%',
            height: '50px',
            border: darkMode ? '1px solid #3A3A3A' : '1px solid #E0E0E0',
            borderRadius: '12px',
            background: darkMode ? '#333333' : '#FFFFFF',
            color: darkMode ? '#FFFFFF' : '#000000',
            fontFamily: '\'Inter\', sans-serif',
            fontSize: '14px',
            fontWeight: 600,
            cursor: isLoading === 'google' ? 'not-allowed' : 'pointer',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            gap: '12px',
            opacity: isLoading === 'google' ? 0.7 : 1,
            transition: 'all 0.2s ease',
            boxShadow: '0 2px 6px rgba(0,0,0,0.06)',
          }"
        >
          <!-- Google SVG -->
          <svg width="20" height="20" viewBox="0 0 48 48">
            <path fill="#FFC107" d="M43.6 20H24v8h11.3C33.6 33.1 29.3 36 24 36c-6.6 0-12-5.4-12-12s5.4-12 12-12c3 0 5.8 1.1 7.9 3l5.7-5.7C34.1 6.5 29.3 4 24 4 12.9 4 4 12.9 4 24s8.9 20 20 20c11 0 20-8 20-20 0-1.3-.2-2.7-.4-4z"/>
            <path fill="#FF3D00" d="M6.3 14.7l6.6 4.8C14.5 15.1 18.9 12 24 12c3 0 5.8 1.1 7.9 3l5.7-5.7C34.1 6.5 29.3 4 24 4 16.3 4 9.7 8.4 6.3 14.7z"/>
            <path fill="#4CAF50" d="M24 44c5.2 0 9.9-1.9 13.5-5.1l-6.2-5.2C29.4 35.5 26.8 36 24 36c-5.2 0-9.6-2.9-11.3-7.1l-6.5 5C9.6 39.6 16.3 44 24 44z"/>
            <path fill="#1976D2" d="M43.6 20H24v8h11.3c-.9 2.5-2.6 4.6-4.8 6l6.2 5.2C40.5 35.7 44 30.3 44 24c0-1.3-.2-2.7-.4-4z"/>
          </svg>
          {{ isLoading === 'google' ? 'Connecting...' : 'Continue with Google' }}
        </button>

        <!-- FACEBOOK -->
        <button
          @click="signInWith('facebook')"
          :style="{
            width: '100%',
            height: '50px',
            border: 'none',
            borderRadius: '12px',
            background: '#1877F2',
            color: '#FFFFFF',
            fontFamily: '\'Inter\', sans-serif',
            fontSize: '14px',
            fontWeight: 600,
            cursor: isLoading === 'facebook' ? 'not-allowed' : 'pointer',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            gap: '12px',
            opacity: isLoading === 'facebook' ? 0.7 : 1,
            transition: 'all 0.2s ease',
            boxShadow: '0 2px 6px rgba(24,119,242,0.3)',
          }"
        >
          <!-- Facebook SVG -->
          <svg width="20" height="20" viewBox="0 0 24 24" fill="#FFFFFF">
            <path d="M22 12c0-5.52-4.48-10-10-10S2 6.48 2 12c0 4.84 3.44 8.87 8 9.8V15H8v-3h2V9.5C10 7.57 11.57 6 13.5 6H16v3h-2c-.55 0-1 .45-1 1v2h3v3h-3v6.95C18.05 21.45 22 17.19 22 12z"/>
          </svg>
          {{ isLoading === 'facebook' ? 'Connecting...' : 'Continue with Facebook' }}
        </button>

        <!-- GITHUB -->
        <button
          @click="signInWith('github')"
          :style="{
            width: '100%',
            height: '50px',
            border: 'none',
            borderRadius: '12px',
            background: darkMode ? '#3BFF65' : '#24292F',
            color: darkMode ? '#000000' : '#FFFFFF',
            fontFamily: '\'Inter\', sans-serif',
            fontSize: '14px',
            fontWeight: 600,
            cursor: isLoading === 'github' ? 'not-allowed' : 'pointer',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            gap: '12px',
            opacity: isLoading === 'github' ? 0.7 : 1,
            transition: 'all 0.2s ease',
            boxShadow: '0 2px 6px rgba(0,0,0,0.15)',
          }"
        >
          <!-- GitHub SVG -->
          <svg width="20" height="20" viewBox="0 0 24 24" :fill="darkMode ? '#24292F' : '#FFFFFF'">
            <path d="M12 2C6.48 2 2 6.48 2 12c0 4.42 2.87 8.17 6.84 9.5.5.09.68-.22.68-.48v-1.7c-2.78.6-3.37-1.34-3.37-1.34-.45-1.16-1.11-1.47-1.11-1.47-.91-.62.07-.61.07-.61 1 .07 1.53 1.03 1.53 1.03.89 1.52 2.34 1.08 2.91.83.09-.65.35-1.08.63-1.33-2.22-.25-4.56-1.11-4.56-4.95 0-1.09.39-1.98 1.03-2.68-.1-.26-.45-1.27.1-2.64 0 0 .84-.27 2.75 1.02A9.56 9.56 0 0 1 12 6.8c.85 0 1.71.11 2.51.33 1.91-1.29 2.75-1.02 2.75-1.02.55 1.37.2 2.38.1 2.64.64.7 1.03 1.59 1.03 2.68 0 3.85-2.34 4.7-4.57 4.95.36.31.68.92.68 1.85v2.74c0 .27.18.58.69.48A10.01 10.01 0 0 0 22 12c0-5.52-4.48-10-10-10z"/>
          </svg>
          {{ isLoading === 'github' ? 'Connecting...' : 'Continue with GitHub' }}
        </button>

      </div>

      <!-- ERROR -->
      <p
        v-if="errorMessage"
        :style="{
          color: '#ff4d4f',
          fontSize: '13px',
          marginTop: '16px',
          textAlign: 'center',
        }"
      >
        {{ errorMessage }}
      </p>

      <!-- DIVIDER -->
      <div
        :style="{
          width: '100%',
          height: '1px',
          margin: '28px 0 20px',
          background: darkMode ? '#3A3A3A' : '#E8E8E8',
        }"
      ></div>

      <!-- BACK TO LOGIN -->
      <p
        :style="{
          textAlign: 'center',
          fontSize: '13px',
          fontWeight: 500,
          color: darkMode ? '#AAAAAA' : '#777777',
        }"
      >
        Already have an account?
        <span
          :style="{ color: '#39FF5A', fontWeight: 700, cursor: 'pointer' }"
          @click="router.push('/login')"
        >
          Log In
        </span>
      </p>

    </div>
  </div>
</template>

<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@100..900&display=swap');

* { box-sizing: border-box; margin: 0; padding: 0; }
</style>