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
  try {
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
      const tripId = payload.tripId || "default";
      firestore
        .collection("trips")
        .doc(tripId)
        .collection("events")
        .add({
          type: payload.type,
          lat: payload.lat || 0,
          lng: payload.lng || 0,
          timestamp: payload.timestamp,
          createdAt: new Date(),
        });
    }
  } catch (err) {
    console.error("[MQTT] Failed to process message:", err.message);
  }
});

// Test protected route
app.get("/api/me", authenticate, (req, res) => {
  res.json({ uid: req.user.uid, email: req.user.email });
});

// Send trip config to device via MQTT
// Called by dashboard after startTrip() creates the Firestore document
app.post("/api/config", authenticate, async (req, res) => {
  const { tripId, destLat, destLng } = req.body;

  if (!tripId) {
    return res.status(400).json({ error: "tripId is required" });
  }

  const configPayload = JSON.stringify({
    tripId,
    destLat: destLat || 0,
    destLng: destLng || 0,
  });

  mqttClient.publish("holdon/config", configPayload, (err) => {
    if (err) {
      console.error("[MQTT] Failed to publish config:", err.message);
      return res
        .status(500)
        .json({ error: "Failed to publish config to device" });
    }
    console.log("[MQTT] Config sent to device:", configPayload);
    res.json({ success: true, payload: configPayload });
  });
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`HoldON backend running on port ${PORT}`);
});
