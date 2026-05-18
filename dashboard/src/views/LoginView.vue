<template>
  <div class="auth-wrapper">
    <div class="auth-card">
      <div class="brand">
        <i class="ti ti-shield-lock" aria-hidden="true"></i>
        <h1>HoldON</h1>
      </div>
      <p class="subtitle">
        Live trip sharing & panic tracker for motorcycle ride-hailing passengers
      </p>
      <button @click="signInWithGoogle" class="google-btn">
        <i class="ti ti-brand-google" aria-hidden="true"></i>
        Sign in with Google
      </button>
    </div>
  </div>
</template>

<script setup>
import { GoogleAuthProvider, signInWithPopup } from "firebase/auth";
import { auth } from "../firebase";
import { useRouter } from "vue-router";

const router = useRouter();

const signInWithGoogle = async () => {
  const provider = new GoogleAuthProvider();
  try {
    await signInWithPopup(auth, provider);
    router.push("/home");
  } catch (error) {
    console.error("Login error:", error.message);
  }
};
</script>

<style scoped>
.auth-wrapper {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 2rem;
}

.auth-card {
  background: var(--bg-card);
  border: 0.5px solid var(--border);
  border-radius: var(--radius-lg);
  padding: 2.5rem 2rem;
  width: 100%;
  max-width: 400px;
  display: flex;
  flex-direction: column;
  gap: 1.25rem;
}

.brand {
  display: flex;
  align-items: center;
  gap: 10px;
}

.brand i {
  font-size: 28px;
  color: var(--accent);
}

.brand h1 {
  font-size: 26px;
  font-weight: 600;
  letter-spacing: -0.5px;
}

.subtitle {
  font-size: 14px;
  color: var(--text-secondary);
  line-height: 1.5;
}

.google-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  margin-top: 0.5rem;
}

.google-btn i {
  font-size: 18px;
}
</style>
