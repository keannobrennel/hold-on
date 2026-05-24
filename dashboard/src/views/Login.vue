<script setup>
import { ref } from "vue";
import { useRouter } from "vue-router";
import { signInWithEmailAndPassword } from "firebase/auth";
import { auth } from "../firebase";
import logoImg from "../assets/logo.png";

const router = useRouter();

const username = ref("");
const password = ref("");
const darkMode = ref(false);
const isLoading = ref(false);
const errorMessage = ref("");

const toggleTheme = () => {
  darkMode.value = !darkMode.value;
};

const handleLogin = async () => {
  if (isLoading.value) return;
  errorMessage.value = "";
  isLoading.value = true;
  try {
    await signInWithEmailAndPassword(auth, username.value, password.value);
    router.push("/home");
  } catch (error) {
    console.error(error);
    switch (error.code) {
      case "auth/user-not-found":
        errorMessage.value = "User not found";
        break;
      case "auth/wrong-password":
        errorMessage.value = "Incorrect password";
        break;
      case "auth/invalid-email":
        errorMessage.value = "Invalid email";
        break;
      case "auth/invalid-credential":
        errorMessage.value = "Invalid email or password";
        break;
      default:
        errorMessage.value = "Login failed";
    }
  } finally {
    isLoading.value = false;
  }
};
</script>

<template>
  <div
    :style="{
      width: '100%',
      minHeight: '100vh',
      display: 'flex',
      overflow: 'hidden',
      position: 'relative',
      fontFamily: '\'Inter\', sans-serif',
      background: darkMode ? '#1E1E1E' : '#FFFFFF',
      transition: 'background 0.3s ease',
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

    <!-- LEFT SECTION -->
    <div
      class="left-section-hide"
      :style="{
        flex: '0 0 45%',
        position: 'relative',
        overflow: 'hidden',
        minHeight: '100vh',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
      }"
    >

      <img
        :src="logoImg"
        alt="HoldON"
        :style="{
          position: 'absolute',
          width: '750px',
          height: 'auto',
          left: '-190px',
          top: '50%',
          transform: 'translateY(-50%)',
          userSelect: 'none',
          pointerEvents: 'none',
        }"
      />
    </div>

    <!-- RIGHT SECTION -->
    <div
      :style="{
        flex: '0 0 55%',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-start',
        padding: '40px 40px 40px 0px',
      }"
    >
      <div :style="{ width: '100%', maxWidth: '420px' }">

        <!-- WELCOME TEXT -->
        <p
          :style="{
            fontSize: '24px',
            marginBottom: '2px',
            fontWeight: 500,
            letterSpacing: '-0.3px',
            color: darkMode ? '#FFFFFF' : '#000000',
            transition: 'color 0.3s ease',
          }"
        >
          Hello! Welcome to
        </p>

        <!-- BRAND NAME -->
        <h1 :style="{ fontSize: '100px', fontWeight: 700, lineHeight: 0.95, marginBottom: '20px', letterSpacing: '-4px' }">
          <span :style="{ color: darkMode ? '#FFFFFF' : '#000000', transition: 'color 0.3s ease' }">Hold</span>
          <span :style="{ color: '#39FF5A' }">ON</span>
        </h1>

        <!-- INPUTS -->
        <div :style="{ display: 'flex', flexDirection: 'column', gap: '16px' }">

          <input
            v-model="username"
            type="email"
            placeholder="Username"
            :style="{
              width: '85%', height: '46px', border: 'none', outline: 'none',
              borderRadius: '10px', padding: '0 16px', fontSize: '13px',
              fontFamily: '\'Inter\', sans-serif', fontWeight: 500,
              background: darkMode ? '#F5F5F5' : '#DDF2D3', color: '#000000',
              boxShadow: '0 4px 6px rgba(0,0,0,0.15)', transition: 'background 0.3s ease',
            }"
          />

          <input
            v-model="password"
            type="password"
            placeholder="Password"
            :style="{
              width: '85%', height: '46px', border: 'none', outline: 'none',
              borderRadius: '10px', padding: '0 16px', fontSize: '13px',
              fontFamily: '\'Inter\', sans-serif', fontWeight: 500,
              background: darkMode ? '#F5F5F5' : '#DDF2D3', color: '#000000',
              boxShadow: '0 4px 6px rgba(0,0,0,0.15)', transition: 'background 0.3s ease',
            }"
          />

          <p v-if="errorMessage" :style="{ color: '#ff4d4f', fontSize: '13px', marginTop: '-5px' }">
            {{ errorMessage }}
          </p>

          <button
            @click="handleLogin"
            :style="{
              width: '140px', height: '42px', border: 'none', borderRadius: '10px',
              margin: '6px auto 0', fontFamily: '\'Inter\', sans-serif', fontSize: '13px',
              fontWeight: 700, cursor: isLoading ? 'not-allowed' : 'pointer',
              background: darkMode ? '#39FF5A' : '#000000',
              color: darkMode ? '#000000' : '#39FF5A',
              boxShadow: '0 4px 6px rgba(0,0,0,0.2)',
              transition: 'all 0.25s ease', opacity: isLoading ? 0.7 : 1,
            }"
          >
            {{ isLoading ? "Logging In..." : "Log In" }}
          </button>

        </div>

        <!-- DIVIDER -->
        <div
          :style="{
            width: '100%', height: '1px', marginTop: '22px', marginBottom: '14px',
            background: darkMode ? '#5C5C5C' : '#CFCFCF', transition: 'background 0.3s ease',
          }"
        ></div>

        <!-- CREATE ACCOUNT -->
        <p :style="{ textAlign: 'center', fontSize: '13px', fontWeight: 500, color: darkMode ? '#FFFFFF' : '#777777', transition: 'color 0.3s ease' }">
          Don't have an account?
          <span
            :style="{ color: '#39FF5A', fontWeight: 700, cursor: 'pointer' }"
            @click="router.push('/register')"
          >
            Create Account
          </span>
        </p>

      </div>
    </div>

  </div>
</template>

<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@100..900&display=swap');

* { box-sizing: border-box; margin: 0; padding: 0; }

input::placeholder {
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  color: #8b8b8b;
}

@media (max-width: 1000px) {
  .left-section-hide { display: none !important; }
}
</style>