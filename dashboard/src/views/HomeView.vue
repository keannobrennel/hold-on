<template>
  <div class="page-wrapper">
    <div class="topbar">
      <div class="brand">
        <i class="ti ti-shield-lock" aria-hidden="true"></i>
        <span>HoldON</span>
      </div>
      <button class="signout-btn" @click="signOut">
        <i class="ti ti-logout" aria-hidden="true"></i>
        Sign out
      </button>
    </div>

    <div class="content">
      <div class="card">
        <h2>Start a trip</h2>
        <p class="card-sub">
          Enter your route and generate a shareable link for your contacts.
        </p>

        <div class="form-group">
          <label>Origin</label>
          <input
            v-model="origin"
            type="text"
            placeholder="e.g. Taft Avenue, Manila"
          />
        </div>

        <div class="form-group">
          <label>Destination</label>
          <input
            v-model="destination"
            type="text"
            placeholder="e.g. Amadeo, Cavite"
          />
        </div>

        <button @click="createTrip" :disabled="!origin || !destination">
          <i class="ti ti-link" aria-hidden="true"></i>
          Generate shareable link
        </button>
      </div>

      <div class="card link-card" v-if="shareableLink">
        <div class="link-header">
          <i class="ti ti-circle-check" aria-hidden="true"></i>
          <span>Trip created</span>
        </div>
        <p class="link-label">Share this link with family or friends:</p>
        <div class="link-row">
          <a :href="shareableLink" target="_blank">{{ shareableLink }}</a>
          <button class="copy-btn" @click="copyLink">
            <i class="ti ti-copy" aria-hidden="true"></i>
            {{ copied ? "Copied!" : "Copy" }}
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from "vue";
import { auth, firestore } from "../firebase";
import { collection, addDoc } from "firebase/firestore";
import { signOut as firebaseSignOut } from "firebase/auth";
import { useRouter } from "vue-router";

const router = useRouter();
const origin = ref("");
const destination = ref("");
const shareableLink = ref("");
const copied = ref(false);

const createTrip = async () => {
  const user = auth.currentUser;
  if (!user) return;
  try {
    const tripRef = await addDoc(collection(firestore, "trips"), {
      userId: user.uid,
      origin: origin.value,
      destination: destination.value,
      status: "waiting",
      createdAt: new Date(),
    });
    shareableLink.value = `${window.location.origin}/trip/${tripRef.id}`;
  } catch (error) {
    console.error("Error creating trip:", error.message);
  }
};

const copyLink = () => {
  navigator.clipboard.writeText(shareableLink.value);
  copied.value = true;
  setTimeout(() => (copied.value = false), 2000);
};

const signOut = async () => {
  await firebaseSignOut(auth);
  router.push("/login");
};
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@100..900&display=swap');

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

.login-container {
  width: 100%;
  min-height: 100vh;
  display: flex;
  overflow: hidden;
  position: relative;
  font-family: 'Inter', sans-serif;
}

/* TOGGLE */
.theme-toggle {
  position: absolute;
  top: 25px;
  right: 25px;
  padding: 10px 18px;
  border: none;
  border-radius: 10px;
  background: #39ff5a;
  color: #000;
  font-family: 'Inter', sans-serif;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
  z-index: 50;
  transition: 0.3s ease;
}

.theme-toggle:hover {
  transform: translateY(-2px);
}

/* LEFT */
.left-section {
  flex: 1;
  position: relative;
  overflow: hidden;
}

.location-wrapper {
  width: 100%;
  height: 100%;
  position: relative;
}

/* BIG LOCATION ICON */
.location-icon {
  width: 520px;
  height: 520px;
  background: #000;
  border: 20px solid #39ff5a;
  border-radius: 50%;
  position: absolute;
  top: 20px;
  left: -160px;
  display: flex;
  justify-content: center;
  align-items: center;
}

/* INNER CIRCLE */
.location-center {
  width: 120px;
  height: 120px;
  border-radius: 50%;
  z-index: 2;
}

/* POINTER */
.location-pointer {
  position: absolute;
  bottom: -200px;
  width: 0;
  height: 0;
  border-left: 155px solid transparent;
  border-right: 155px solid transparent;
  border-top: 260px solid #000;
}

/* CURVE */
.location-curve {
  position: absolute;
  bottom: 10px;
  left: -20px;
  width: 320px;
  height: 160px;
  border-bottom: 20px solid #39ff5a;
  border-radius: 0 0 220px 220px;
  transform: rotate(10deg);
}

/* RIGHT */
.right-section {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 40px;
}

/* FORM */
.form-wrapper {
  width: 100%;
  max-width: 420px;
}

.welcome-text {
  font-size: 22px;
  margin-bottom: 6px;
  font-weight: 500;
  letter-spacing: -0.5px;
}

.logo-text {
  font-size: 90px;
  font-weight: 900;
  line-height: 0.95;
  margin-bottom: 28px;
  letter-spacing: -4px;
}

.green-text {
  color: #39ff5a;
}

/* INPUTS */
.input-container {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.login-input {
  width: 100%;
  height: 46px;
  border: none;
  outline: none;
  border-radius: 10px;
  padding: 0 16px;
  font-size: 13px;
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.15);
}

.login-input::placeholder {
  font-family: 'Inter', sans-serif;
  font-weight: 500;
  color: #8b8b8b;
}

/* BUTTON */
.login-button {
  width: 140px;
  height: 42px;
  border: none;
  border-radius: 10px;
  margin: 6px auto 0;
  font-family: 'Inter', sans-serif;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
  transition: 0.25s ease;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);
}

.login-button:hover {
  transform: translateY(-2px);
}

/* ERROR */
.error-message {
  color: #ff4d4f;
  font-size: 13px;
  margin-top: -5px;
}

/* DIVIDER */
.divider {
  width: 100%;
  height: 1px;
  margin-top: 22px;
  margin-bottom: 14px;
}

/* CREATE */
.create-account-text {
  text-align: center;
  font-size: 13px;
  font-weight: 500;
}

.create-link {
  font-weight: 700;
  cursor: pointer;
}

/* RESPONSIVE */
@media (max-width: 1000px) {
  .login-container {
    flex-direction: column;
  }

  .left-section {
    display: none;
  }

  .right-section {
    width: 100%;
    min-height: 100vh;
    padding: 40px 24px;
  }

  .logo-text {
    font-size: 72px;
  }

  .form-wrapper {
    max-width: 450px;
  }
}

/* MOBILE */
@media (max-width: 600px) {
  .theme-toggle {
    top: 15px;
    right: 15px;
    font-size: 12px;
    padding: 8px 14px;
  }

  .right-section {
    padding: 70px 20px 40px;
  }

  .welcome-text {
    font-size: 18px;
  }

  .logo-text {
    font-size: 58px;
    letter-spacing: -2px;
  }

  .login-button {
    width: 100%;
  }
}
</style>