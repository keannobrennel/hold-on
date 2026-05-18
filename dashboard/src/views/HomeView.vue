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
.page-wrapper {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
}

.topbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 1.5rem;
  border-bottom: 0.5px solid var(--border);
  background: var(--bg-secondary);
}

.brand {
  display: flex;
  align-items: center;
  gap: 8px;
  font-weight: 500;
  font-size: 17px;
}

.brand i {
  font-size: 20px;
  color: var(--accent);
}

.signout-btn {
  background: transparent;
  color: var(--text-secondary);
  border: 0.5px solid var(--border);
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 13px;
  padding: 7px 14px;
}

.signout-btn:hover {
  color: var(--text-primary);
  opacity: 1;
  border-color: var(--text-secondary);
}

.content {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 2rem 1rem;
  gap: 1.25rem;
}

.card {
  background: var(--bg-card);
  border: 0.5px solid var(--border);
  border-radius: var(--radius-lg);
  padding: 2rem;
  width: 100%;
  max-width: 480px;
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.card-sub {
  font-size: 14px;
  color: var(--text-secondary);
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.form-group label {
  font-size: 13px;
  color: var(--text-secondary);
}

button:disabled {
  opacity: 0.4;
  cursor: not-allowed;
}

button i {
  font-size: 16px;
  vertical-align: -2px;
  margin-right: 4px;
}

.link-card {
  border-color: var(--success);
}

.link-header {
  display: flex;
  align-items: center;
  gap: 8px;
  color: var(--success);
  font-weight: 500;
  font-size: 15px;
}

.link-header i {
  font-size: 20px;
}

.link-label {
  font-size: 13px;
  color: var(--text-secondary);
}

.link-row {
  display: flex;
  align-items: center;
  gap: 10px;
  background: var(--bg-secondary);
  border: 0.5px solid var(--border);
  border-radius: var(--radius-md);
  padding: 10px 14px;
}

.link-row a {
  flex: 1;
  font-size: 13px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.copy-btn {
  background: transparent;
  border: 0.5px solid var(--border);
  color: var(--text-primary);
  font-size: 13px;
  padding: 6px 12px;
  white-space: nowrap;
  display: flex;
  align-items: center;
  gap: 4px;
}

.copy-btn i {
  font-size: 14px;
  margin: 0;
}
</style>
