const mqtt = require("mqtt");
const { db, firestore } = require("./firebase");
require("dotenv").config();

const client = mqtt.connect(
  `mqtts://${process.env.MQTT_HOST}:${process.env.MQTT_PORT}`,
  {
    username: process.env.MQTT_USERNAME,
    password: process.env.MQTT_PASSWORD,
  },
);

client.on("connect", () => {
  console.log("Connected to HiveMQ broker");
  client.subscribe("holdon/location");
  client.subscribe("holdon/grip");
  client.subscribe("holdon/motion");
  client.subscribe("holdon/event");
});

client.on("message", async (topic, message) => {
  let data;
  try {
    data = JSON.parse(message.toString());
  } catch (e) {
    console.error("Failed to parse MQTT message:", message.toString());
    return;
  }

  const tripId = data.tripId;
  if (!tripId || tripId === "default") {
    console.warn(`[MQTT] Skipping message — no valid tripId on ${topic}`);
    return;
  }

  try {
    if (topic === "holdon/location") {
      await db.ref(`liveLocation/${tripId}`).set({
        lat: data.lat,
        lng: data.lng,
        timestamp: data.timestamp,
      });
      console.log(`[location] Updated liveLocation/${tripId}`);
    } else if (topic === "holdon/event") {
      await firestore
        .collection("trips")
        .doc(tripId)
        .collection("events")
        .add({
          type: data.type,
          lat: data.lat ?? 0,
          lng: data.lng ?? 0,
          accelMag: data.accelMag ?? 0,
          gripValue: data.gripValue ?? 0,
          timestamp: data.timestamp,
          createdAt: new Date(),
        });
      console.log(`[event] Wrote ${data.type} to trips/${tripId}/events`);
    } else if (topic === "holdon/grip") {
      await db.ref(`gripData/${tripId}`).set({
        value: data.value,
        timestamp: data.timestamp,
      });
    } else if (topic === "holdon/motion") {
      await db.ref(`motionData/${tripId}`).set({
        accelMag: data.accelMag,
        angularVel: data.angularVel,
        timestamp: data.timestamp,
      });
    }
  } catch (err) {
    console.error(`[MQTT] Firebase write error on ${topic}:`, err.message);
  }
});

client.on("error", (err) => {
  console.error("MQTT connection error:", err.message);
});

module.exports = client;
