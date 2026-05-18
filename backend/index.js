const express = require("express");
const cors = require("cors");
require("dotenv").config();

const app = express();
app.use(cors());
app.use(express.json());

// Firebase and MQTT connections
const { db, firestore } = require("./config/firebase");
const mqttClient = require("./config/mqtt");

// MQTT message handler
mqttClient.on("message", (topic, message) => {
  const payload = JSON.parse(message.toString());

  console.log(`[MQTT] Topic: ${topic}`, payload);

  if (topic === "holdon/location") {
    // Write live GPS to Firebase RTDB
    db.ref("liveLocation").set({
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

// Routes (to be added)
// app.use("/api/trips", require("./routes/trips"));

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`HoldON backend running on port ${PORT}`);
});
