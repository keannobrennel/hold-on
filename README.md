# HoldON 🛵

Live Trip Sharing, Dual-Sensor Panic Tracker & Non-Movement Escalation for Motorcycle Ride-Hailing Passengers

## About

HoldON is a clip-on IoT device and web dashboard that lets motorcycle ride-hailing passengers share their live trip with family or friends, and silently trigger an emergency alert if something goes wrong mid-ride.

## Features

- 📍 Live GPS tracking via shareable link — no app install needed
- ✊ Dual-sensor panic detection (grip + jolt)
- 🆘 Manual panic via long-press button with countdown cancel
- 🔴 Non-movement escalation if passenger stops moving after a panic
- ✅ Auto trip close on arrival

## Project Structure

hold-on/
├── firmware/ # Arduino/C++ code for NodeMCU ESP8266
├── backend/ # Node.js + Express + Firebase + MQTT
├── dashboard/ # Vue.js 3 frontend dashboard
└── docs/ # Wiring diagrams, proposal, documentation

## Tech Stack

- **Device:** NodeMCU ESP8266, FSR406, MPU6050, NEO-7M GPS
- **Backend:** Node.js, Express, MQTT, HiveMQ, Firebase
- **Frontend:** Vue.js 3, Leaflet.js, Firebase RTDB

## Timeline

Target completion: June 1, 2026
