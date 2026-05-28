<script setup>
import { useRouter } from "vue-router";
import { signInWithPopup, GoogleAuthProvider } from "firebase/auth";
import { auth } from "../firebase";
import logoImg from "../assets/logo.png";
import { ref } from "vue";

const router = useRouter();

const isGoogleLoading = ref(false);
const errorMessage = ref("");

const handleGoogleSignIn = async () => {
  if (isGoogleLoading.value) return;
  errorMessage.value = "";
  isGoogleLoading.value = true;
  try {
    const provider = new GoogleAuthProvider();
    await signInWithPopup(auth, provider);
    router.push("/app/home");
  } catch (error) {
    console.error(error);
    if (error.code !== "auth/popup-closed-by-user") {
      errorMessage.value = "Google sign-in failed. Please try again.";
    }
  } finally {
    isGoogleLoading.value = false;
  }
};
</script>

<template>
  <div class="page-wrapper">
    <!-- LEFT SECTION (desktop only) -->
    <div class="left-section">
      <img :src="logoImg" alt="HoldON" class="logo-left" />
    </div>

    <!-- RIGHT SECTION -->
    <div class="right-section">
      <div class="form-container">
        <!-- TOP LOGO (tablet & mobile only) -->
        <img :src="logoImg" alt="HoldON" class="logo-top" />

        <!-- WELCOME TEXT -->
        <p class="welcome-text">Hello! Welcome to</p>

        <!-- BRAND NAME -->
        <h1 class="brand-name">
          <span>Hold</span><span class="accent">ON</span>
        </h1>

        <!-- ERROR -->
        <p v-if="errorMessage" class="error-msg">{{ errorMessage }}</p>

        <!-- GOOGLE SIGN IN -->
        <button
          @click="handleGoogleSignIn"
          class="google-btn"
          :class="{ loading: isGoogleLoading }"
          :disabled="isGoogleLoading"
        >
          <svg
            v-if="!isGoogleLoading"
            width="18"
            height="18"
            viewBox="0 0 48 48"
          >
            <path
              fill="#EA4335"
              d="M24 9.5c3.54 0 6.71 1.22 9.21 3.6l6.85-6.85C35.9 2.38 30.47 0 24 0 14.62 0 6.51 5.38 2.56 13.22l7.98 6.19C12.43 13.72 17.74 9.5 24 9.5z"
            />
            <path
              fill="#4285F4"
              d="M46.98 24.55c0-1.57-.15-3.09-.38-4.55H24v9.02h12.94c-.58 2.96-2.26 5.48-4.78 7.18l7.73 6c4.51-4.18 7.09-10.36 7.09-17.65z"
            />
            <path
              fill="#FBBC05"
              d="M10.53 28.59c-.48-1.45-.76-2.99-.76-4.59s.27-3.14.76-4.59l-7.98-6.19C.92 16.46 0 20.12 0 24c0 3.88.92 7.54 2.56 10.78l7.97-6.19z"
            />
            <path
              fill="#34A853"
              d="M24 48c6.48 0 11.93-2.13 15.89-5.81l-7.73-6c-2.18 1.48-4.97 2.31-8.16 2.31-6.26 0-11.57-4.22-13.47-9.91l-7.98 6.19C6.51 42.62 14.62 48 24 48z"
            />
            <path fill="none" d="M0 0h48v48H0z" />
          </svg>
          <span class="google-spinner" v-else></span>
          {{ isGoogleLoading ? "Signing in..." : "Continue with Google" }}
        </button>
      </div>
    </div>
  </div>
</template>

<style>
@import url("https://fonts.googleapis.com/css2?family=Inter:wght@100..900&display=swap");

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

/* PAGE WRAPPER */
.page-wrapper {
  width: 100%;
  min-height: 100vh;
  display: flex;
  overflow: hidden;
  position: relative;
  font-family: "Inter", sans-serif;
  background: #1e1e1e;
}

/* LEFT SECTION — desktop only */
.left-section {
  flex: 0 0 45%;
  position: relative;
  overflow: hidden;
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
}
.logo-left {
  position: absolute;
  width: 750px;
  height: auto;
  left: -190px;
  top: 50%;
  transform: translateY(-50%);
  user-select: none;
  pointer-events: none;
}

/* RIGHT SECTION */
.right-section {
  flex: 0 0 55%;
  display: flex;
  align-items: center;
  justify-content: flex-start;
  padding: 40px 40px 40px 0;
}
.form-container {
  width: 100%;
  max-width: 420px;
  display: flex;
  flex-direction: column;
  gap: 0;
}

/* TOP LOGO — hidden on desktop */
.logo-top {
  display: none;
  width: 90px;
  height: auto;
  margin-bottom: 20px;
}

/* WELCOME TEXT */
.welcome-text {
  font-size: 24px;
  margin-bottom: 2px;
  font-weight: 500;
  letter-spacing: -0.3px;
  color: #ffffff;
}

/* BRAND NAME */
.brand-name {
  font-size: 100px;
  font-weight: 700;
  line-height: 0.95;
  margin-bottom: 28px;
  letter-spacing: -4px;
}
.brand-name span {
  color: #ffffff;
}
.brand-name .accent {
  color: #39ff5a;
}

/* ERROR */
.error-msg {
  color: #ff4d4f;
  font-size: 13px;
  margin-bottom: 10px;
}

/* GOOGLE BUTTON */
.google-btn {
  width: 85%;
  height: 46px;
  border: 1.5px solid #5c5c5c;
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 10px;
  font-family: "Inter", sans-serif;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  background: #2a2a2a;
  color: #ffffff;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.08);
  transition: all 0.25s ease;
}
.google-btn:hover:not(:disabled) {
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
  transform: translateY(-1px);
}
.google-btn.loading {
  cursor: not-allowed;
  opacity: 0.7;
}

/* GOOGLE SPINNER */
.google-spinner {
  width: 18px;
  height: 18px;
  border: 2px solid #cfcfcf;
  border-top-color: #39ff5a;
  border-radius: 50%;
  animation: spin 0.7s linear infinite;
  flex-shrink: 0;
}
@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

/* ─── TABLET & MOBILE (≤1000px) ─── */
@media (max-width: 1000px) {
  .page-wrapper {
    flex-direction: column;
    align-items: center;
    justify-content: center;
  }
  .left-section {
    display: none;
  }
  .right-section {
    flex: unset;
    width: 100%;
    max-width: 480px;
    padding: 80px 32px 48px;
    justify-content: center;
    align-items: center;
  }
  .form-container {
    width: 100%;
    max-width: 100%;
    align-items: center;
    text-align: center;
  }
  .logo-top {
    display: block;
  }
  .welcome-text {
    font-size: 20px;
  }
  .brand-name {
    font-size: 76px;
    letter-spacing: -3px;
    margin-bottom: 32px;
  }
  .google-btn {
    width: 100%;
  }
}

/* ─── MOBILE (≤480px) ─── */
@media (max-width: 480px) {
  .right-section {
    padding: 72px 24px 40px;
  }
  .brand-name {
    font-size: 60px;
    letter-spacing: -2px;
  }
  .welcome-text {
    font-size: 17px;
  }
  .logo-top {
    width: 72px;
  }
}
</style>
