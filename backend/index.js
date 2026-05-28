const express = require("express");
const cors = require("cors");
require("dotenv").config();

const app = express();
app.use(cors());
app.use(express.json());

// Firebase and MQTT connections
const { admin, db, firestore } = require("./config/firebase");
const mqttClient = require("./config/mqtt");

// Authentication middleware
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

// MQTT message handler
mqttClient.on("message", (topic, message) => {
  const payload = JSON.parse(message.toString());

  console.log(`[MQTT] Topic: ${topic}`, payload);

  if (topic === "holdon/location") {
    const tripId = payload.tripId || "default";
    db.ref(`liveLocation/${tripId}`).set({
      lat: payload.lat,
      lng: payload.lng,
      timestamp: payload.timestamp,
    });
  }

  if (topic === "holdon/event") {
    // Write events to Firestore
    firestore.collection("events").add({
      type: payload.type,
      lat: payload.lat,
      lng: payload.lng,
      timestamp: payload.timestamp,
      createdAt: new Date(),
    });
  }
});

// Protected routes (to be added)
// app.use("/api/trips", authenticate, require("./routes/trips"));

// Test protected route
app.get("/api/me", authenticate, (req, res) => {
  res.json({ uid: req.user.uid, email: req.user.email });
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`HoldON backend running on port ${PORT}`);
});
