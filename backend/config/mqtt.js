const mqtt = require("mqtt");
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

client.on("error", (err) => {
  console.error("MQTT connection error:", err.message);
});

module.exports = client;
