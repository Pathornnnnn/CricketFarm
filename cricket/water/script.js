const firebaseConfig = {
    apiKey: "AIzaSyC108mRatpnA0k_4GQhQQ1_RXNyn-obIu4",
    authDomain: "smartfarm-cricket.firebaseapp.com",
    databaseURL: "https://smartfarm-cricket-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "smartfarm-cricket",
    storageBucket: "smartfarm-cricket.appspot.com",
    messagingSenderId: "725266047089",
    appId: "1:725266047089:web:108b3cf9a25ca1abfcfbd0",
    measurementId: "G-ZV49FTC39B"
  };

firebase.initializeApp(firebaseConfig);
const database = firebase.database();

// Initialize Firebase with your Firebase configuration object (same as before)
// ...

const foodWaterElement = document.getElementById("foodWater");
const waterWaterElement = document.getElementById("waterWater");
const indicators = document.querySelectorAll(".indicator");
let foodWaterLevel = 0;
let waterWaterLevel = 0;

function updateWaterLevel(element, level) {
    element.style.height = `${level}%`;
    indicators.forEach((indicator) => {
        indicator.classList.remove("active");
    });
    const activeIndicator = document.querySelector(`.indicator[style="top: ${level}%;"]`);
    if (activeIndicator) {
        activeIndicator.classList.add("active");
    }
}

// Read water level data from Firebase and update the display for food and water bottles
database.ref("foodLevel").on("value", (snapshot) => {
    const level = snapshot.val();
    foodWaterLevel = level || 0; // Use 0 if data is missing
    updateWaterLevel(foodWaterElement, foodWaterLevel);
});

database.ref("waterLevel").on("value", (snapshot) => {
    const level = snapshot.val();
    waterWaterLevel = level || 0; // Use 0 if data is missing
    updateWaterLevel(waterWaterElement, waterWaterLevel);
});