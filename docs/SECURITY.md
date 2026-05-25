# HoldON — Security Documentation

This document outlines the security measures implemented in the HoldON system covering encryption and authentication across all layers of the stack.

---

## 1. Encryption

### 1.1 MQTT over TLS (Transport Layer Security)

All communication between the NodeMCU device and the HiveMQ cloud broker is encrypted using TLS on port **8883** — the standard secure MQTT port.

This means:
- GPS coordinates, grip sensor data, motion data, and panic events are all encrypted in transit
- Data cannot be intercepted or read by a third party while traveling from the device to the broker
- Plain MQTT on port 1883 (unencrypted) is not used anywhere in this system

**Implementation:** `firmware/holdon-device/mqtt.h`
```cpp
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
// Connects on port 8883 (TLS)
mqttClient.setServer(MQTT_HOST, MQTT_PORT); // MQTT_PORT = 8883
```

**Backend MQTT connection:** `backend/config/mqtt.js`
```javascript
const client = mqtt.connect(`mqtts://${process.env.MQTT_HOST}:${process.env.MQTT_PORT}`, {
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
});
// mqtts:// protocol enforces TLS encryption
```

---

### 1.2 HTTPS on Firebase

All communication between the backend and Firebase (Realtime Database and Firestore) and between the dashboard and Firebase uses **HTTPS** — enforced by the Firebase SDK by default.

This means:
- Trip data, event data, and live GPS written to Firebase are encrypted in transit
- Dashboard reads from Firebase are encrypted in transit
- No configuration required — Firebase SDK enforces HTTPS on all connections

---

### 1.3 Credentials Management

Sensitive credentials (API keys, MQTT passwords, Firebase service account) are stored in environment files that are excluded from version control via `.gitignore`:

| File | Contains | Tracked by Git |
|---|---|---|
| `backend/.env` | Firebase keys, MQTT credentials | ❌ No |
| `dashboard/.env` | Firebase public config | ❌ No |
| `backend/config/serviceAccountKey.json` | Firebase Admin private key | ❌ No |
| `firmware/holdon-device/config.h` | WiFi credentials, MQTT password | ❌ No |

---

## 2. Authentication

### 2.1 Passenger Authentication (Frontend)

Passengers authenticate via **Google Sign-In** through Firebase Authentication before accessing the dashboard.

- Only authenticated users can access the `/home` route (trip creation)
- Unauthenticated users are automatically redirected to `/login`
- The `/trip/:tripId` route is intentionally public — recipients of a shared link do not need to log in to view the live map

**Implementation:** `dashboard/src/router/index.js`
```javascript
router.beforeEach((to, from, next) => {
  const user = auth.currentUser;
  if (to.meta.requiresAuth && !user) {
    next("/login");
  } else {
    next();
  }
});
```

---

### 2.2 Backend API Authentication (Server)

All protected API routes on the backend require a valid **Firebase ID token** passed in the request header. The backend verifies the token using the Firebase Admin SDK before processing any request.

**How it works:**
1. The dashboard obtains a Firebase ID token after the user logs in
2. The token is sent in the `Authorization` header of every API request
3. The backend middleware verifies the token with Firebase
4. If valid, the request proceeds and `req.user` contains the decoded user info
5. If invalid or missing, the server returns `401 Unauthorized`

**Implementation:** `backend/index.js`
```javascript
const authenticate = async (req, res, next) => {
  const authHeader = req.headers.authorization;
  if (!authHeader || !authHeader.startsWith("Bearer ")) {
    return res.status(401).json({ error: "Unauthorized — no token provided" });
  }

  const idToken = authHeader.split("Bearer ")[1];
  try {
    const decodedToken = await admin.auth().verifyIdToken(idToken);
    req.user = decodedToken;
    next();
  } catch (error) {
    return res.status(401).json({ error: "Unauthorized — invalid token" });
  }
};
```

**Usage on protected routes:**
```javascript
app.get("/api/me", authenticate, (req, res) => {
  res.json({ uid: req.user.uid, email: req.user.email });
});
```

---

## 3. Summary

| Security Measure | Layer | Status |
|---|---|---|
| TLS encryption on MQTT (port 8883) | Hardware → Broker | ✅ Implemented |
| HTTPS on all Firebase connections | Backend + Dashboard → Firebase | ✅ Implemented |
| Credentials excluded from version control | All layers | ✅ Implemented |
| Google Sign-In for passenger authentication | Dashboard | ✅ Implemented |
| Route guards for protected pages | Dashboard | ✅ Implemented |
| Firebase ID token verification on API routes | Backend | ✅ Implemented |
