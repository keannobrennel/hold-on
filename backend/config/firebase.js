const admin = require("firebase-admin");
require("dotenv").config();

const serviceAccountJson = process.env.FIREBASE_SERVICE_ACCOUNT;
const serviceAccount = JSON.parse(serviceAccountJson.replace(/\\n/g, "\n"));

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: process.env.FIREBASE_DATABASE_URL,
});

const db = admin.database();
const firestore = admin.firestore();

module.exports = { admin, db, firestore };
