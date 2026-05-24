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
  <div class="page-wrapper" :class="{ dark: darkMode }">

    <!-- THEME TOGGLE -->
    <button @click="toggleTheme" class="theme-toggle" :class="{ dark: darkMode }">
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

    <!-- DESKTOP: two-column layout -->
    <template v-if="false"></template>

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
        <p class="welcome-text" :class="{ dark: darkMode }">Hello! Welcome to</p>

        <!-- BRAND NAME -->
        <h1 class="brand-name">
          <span :class="{ dark: darkMode }">Hold</span><span class="accent">ON</span>
        </h1>

        <!-- INPUTS -->
        <div class="inputs-wrapper">
          <input
            v-model="username"
            type="email"
            placeholder="Username"
            class="input-field"
            :class="{ dark: darkMode }"
          />
          <input
            v-model="password"
            type="password"
            placeholder="Password"
            class="input-field"
            :class="{ dark: darkMode }"
          />
          <p v-if="errorMessage" class="error-msg">{{ errorMessage }}</p>
          <button
            @click="handleLogin"
            class="login-btn"
            :class="{ dark: darkMode, loading: isLoading }"
            :disabled="isLoading"
          >
            {{ isLoading ? "Logging In..." : "Log In" }}
          </button>
        </div>

        <!-- DIVIDER -->
        <div class="divider" :class="{ dark: darkMode }"></div>

        <!-- SIGN UP -->
        <p class="signup-text" :class="{ dark: darkMode }">
          Don't have an account?
          <span class="signup-link" @click="router.push('/register')">Sign Up</span>
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

/* PAGE WRAPPER */
.page-wrapper {
  width: 100%;
  min-height: 100vh;
  display: flex;
  overflow: hidden;
  position: relative;
  font-family: 'Inter', sans-serif;
  background: #FFFFFF;
  transition: background 0.3s ease;
}
.page-wrapper.dark { background: #1E1E1E; }

/* THEME TOGGLE */
.theme-toggle {
  position: absolute;
  top: 25px;
  right: 25px;
  width: 42px;
  height: 42px;
  border-radius: 50%;
  border: 2px solid #39FF5A;
  background: #F0F0F0;
  cursor: pointer;
  z-index: 50;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: all 0.3s ease;
  box-shadow: 0 2px 8px rgba(0,0,0,0.15);
}
.theme-toggle.dark { background: #2A2A2A; }

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
  color: #000000;
  transition: color 0.3s ease;
}
.welcome-text.dark { color: #FFFFFF; }

/* BRAND NAME */
.brand-name {
  font-size: 100px;
  font-weight: 700;
  line-height: 0.95;
  margin-bottom: 20px;
  letter-spacing: -4px;
}
.brand-name span { color: #000000; transition: color 0.3s ease; }
.brand-name span.dark { color: #FFFFFF; }
.brand-name .accent { color: #39FF5A; }

/* INPUTS */
.inputs-wrapper {
  display: flex;
  flex-direction: column;
  gap: 16px;
}
.input-field {
  width: 85%;
  height: 46px;
  border: none;
  outline: none;
  border-radius: 10px;
  padding: 0 16px;
  font-size: 13px;
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  background: #DDF2D3;
  color: #000000;
  box-shadow: 0 4px 6px rgba(0,0,0,0.15);
  transition: background 0.3s ease;
}
.input-field.dark { background: #F5F5F5; }

.error-msg {
  color: #ff4d4f;
  font-size: 13px;
  margin-top: -5px;
}

.login-btn {
  width: 140px;
  height: 42px;
  border: none;
  border-radius: 10px;
  margin: 6px auto 0;
  font-family: 'Inter', sans-serif;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
  background: #000000;
  color: #39FF5A;
  box-shadow: 0 4px 6px rgba(0,0,0,0.2);
  transition: all 0.25s ease;
  display: block;
}
.login-btn.dark { background: #39FF5A; color: #000000; }
.login-btn.loading { cursor: not-allowed; opacity: 0.7; }

/* DIVIDER */
.divider {
  width: 100%;
  height: 1px;
  margin-top: 22px;
  margin-bottom: 14px;
  background: #CFCFCF;
  transition: background 0.3s ease;
}
.divider.dark { background: #5C5C5C; }

/* SIGNUP */
.signup-text {
  text-align: center;
  font-size: 13px;
  font-weight: 500;
  color: #777777;
  transition: color 0.3s ease;
}
.signup-text.dark { color: #FFFFFF; }
.signup-link {
  color: #39FF5A;
  font-weight: 700;
  cursor: pointer;
}

/* ─── TABLET & MOBILE (≤1000px): stacked, centered ─── */
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
    display: flex;
    flex-direction: column;
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
    margin-bottom: 28px;
  }
  .inputs-wrapper {
    width: 100%;
    align-items: center;
  }
  .input-field {
    width: 100%;
  }
  .login-btn {
    width: 100%;
    margin-top: 4px;
  }
  .divider {
    width: 100%;
  }
  .theme-toggle {
    top: 16px;
    right: 16px;
  }
}

/* ─── MOBILE (≤480px): tighter sizing ─── */
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