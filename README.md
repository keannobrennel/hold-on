# HoldON

A live trip sharing and panic tracking system for motorcycle ride-hailing passengers.

Passengers share a real-time tracking link with family or friends before boarding. The NodeMCU device mounted on the motorcycle publishes GPS, grip, motion, and event data over MQTT. The backend receives and stores it. The dashboard displays it live — no app install required for recipients.

---

## Tech Stack

| Layer     | Technology                                                       |
| --------- | ---------------------------------------------------------------- |
| Firmware  | NodeMCU (ESP8266), Arduino                                       |
| Broker    | HiveMQ Cloud (MQTT over TLS)                                     |
| Backend   | Node.js, Express, Firebase Admin SDK                             |
| Database  | Firebase Realtime Database (live GPS), Firestore (trips, events) |
| Auth      | Firebase Authentication (Google Sign-In)                         |
| Dashboard | Vue 3, Vite, Leaflet.js                                          |

---

## Repository Structure

```
hold-on/
├── backend/          # Node.js server — MQTT subscriber, Firebase writes, REST API
│   ├── config/
│   │   ├── firebase.js
│   │   └── mqtt.js
│   └── index.js
├── dashboard/        # Vue 3 frontend — passenger login, trip creation, live map
│   └── src/
│       ├── views/
│       │   ├── LoginView.vue
│       │   ├── HomeView.vue
│       │   └── TripView.vue
│       ├── components/
│       ├── composables/
│       ├── router/index.js
│       └── firebase.js
├── firmware/         # NodeMCU Arduino sketches
└── docs/             # Diagrams, proposal, references
```

---

## Branches

| Branch             | Purpose                                       |
| ------------------ | --------------------------------------------- |
| `main`             | Stable releases only                          |
| `dev`              | Integration branch — all PRs merge here first |
| `yourname/feature` | Personal feature branches                     |

Always branch off `dev` and PR back into `dev`. Never push directly to `main`.

---

## Local Setup

### 1. Clone the repo

```bash
git clone https://github.com/your-username/hold-on.git
cd hold-on
git checkout dev
```

### 2. Create environment files

These are NOT on GitHub. Get the values from Keanno privately.

**Dashboard** — create `dashboard/.env`:

```
VITE_FIREBASE_API_KEY=
VITE_FIREBASE_AUTH_DOMAIN=
VITE_FIREBASE_DATABASE_URL=
VITE_FIREBASE_PROJECT_ID=
VITE_FIREBASE_STORAGE_BUCKET=
VITE_FIREBASE_MESSAGING_SENDER_ID=
VITE_FIREBASE_APP_ID=
```

**Backend** — create `backend/.env`:

```
FIREBASE_API_KEY=
FIREBASE_AUTH_DOMAIN=
FIREBASE_DATABASE_URL=
FIREBASE_PROJECT_ID=
FIREBASE_STORAGE_BUCKET=
FIREBASE_MESSAGING_SENDER_ID=
FIREBASE_APP_ID=
MQTT_HOST=
MQTT_PORT=8883
MQTT_USERNAME=
MQTT_PASSWORD=
```

### 3. Add the Firebase service account key

Place `serviceAccountKey.json` inside `backend/config/`. Get this file from Keanno directly.

### 4. Install dependencies

```bash
# Dashboard
cd dashboard
npm install

# Backend
cd ../backend
npm install
```

---

## Running Locally

**Dashboard (dev server):**

```bash
cd dashboard
npm run dev
# Opens at http://localhost:5173
```

**Backend:**

```bash
cd backend
node index.js
# Runs on http://localhost:3000
```

---

## MQTT Topics

| Topic             | Publisher | Data                            |
| ----------------- | --------- | ------------------------------- |
| `holdon/location` | NodeMCU   | `{ lat, lng, timestamp }`       |
| `holdon/grip`     | NodeMCU   | `{ value, timestamp }`          |
| `holdon/motion`   | NodeMCU   | `{ moving, timestamp }`         |
| `holdon/event`    | NodeMCU   | `{ type, lat, lng, timestamp }` |

---

## How It Works

1. Passenger signs in with Google and enters origin + destination
2. A trip document is created in Firestore and a shareable link is generated
3. Passenger shares the link with family or friends before boarding
4. The NodeMCU publishes live data to HiveMQ over MQTT
5. The backend subscribes, receives the data, and writes it to Firebase
6. Anyone with the link opens the trip page and sees the live map update in real time
7. Panic or motion events trigger visible alerts on the trip page
