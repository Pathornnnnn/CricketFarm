// Initialize Firebase with your Firebase configuration object
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

  let btn = document.querySelector("button");
  let div = document.querySelector(".graph");
  
  btn.addEventListener("click", () => {
    if (div.style.display === "none") {
      div.style.display = "block";
    } else {
      div.style.display = "none";
    }
  });

firebase.initializeApp(firebaseConfig);
const database = firebase.database();

// Add event listeners for all switch buttons
const switches = document.querySelectorAll('.device-switch');
switches.forEach((deviceSwitch) => {
    deviceSwitch.addEventListener('change', (event) => {
        const deviceName = event.target.dataset.device;
        const newState = event.target.checked ? 0 : 1;
        database.ref(`devices/${deviceName}`).set(newState);
    });

    // Listen for changes in Firebase and update the switch state
    database.ref(`devices/${deviceSwitch.dataset.device}`).on('value', (snapshot) => {
        const state = snapshot.val();
        deviceSwitch.checked = state === 0;
    });
});



var temperatureRef = database.ref('temperature');
var humidityRef = database.ref('humidity');
var waterLevelRef = database.ref('waterLevel');
var foodLevelRef = database.ref('foodLevel');
var AutomodeRef = database.ref('Auto');
var timeintervalRef = database.ref('timeinterval');
var waterpipeRef = database.ref('waterpipe');
var maxtemRef = database.ref('maxtem');


timeintervalRef.on('value', function(snapshot){
  var interval = snapshot.val();
  document.getElementById('timeinterval').textContent = interval;
})

waterpipeRef.on('value', function(snapshot){
  var level = snapshot.val();
  document.getElementById('waterlevel').textContent = level ? "HIGH" : "LOW";
})

maxtemRef.on('value', function(snapshot){
  var maxtemperature = snapshot.val();
  document.getElementById('maxtem').textContent = maxtemperature;
})

// Listen for changes in the values
temperatureRef.on('value', function(snapshot) {
  var temperature = snapshot.val();
  document.getElementById('temperatureValue').textContent = temperature;
});

humidityRef.on('value', function(snapshot) {
  var humidity = snapshot.val();
  document.getElementById('humidityValue').textContent = humidity;
});

waterLevelRef.on('value', function(snapshot) {
    var waterLevel = snapshot.val();
    document.getElementById('waterLevelValue').textContent = waterLevel;
  });
  
foodLevelRef.on('value', function(snapshot) {
    var foodLevel = snapshot.val();
    document.getElementById('foodLevelValue').textContent = foodLevel;
});

//level script
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



// Line Notify access token
const lineNotifyAccessToken = "k45dNLsfcDvnYFRcqNn2zpZJPd5h3XvvwinvA5YhJRd";

// Threshold temperature value for notification
const temperatureThreshold = 80; // Change this to your desired threshold

// Function to read temperature data from Firebase
function readTemperatureData() {
  const temperatureRef = database.ref("temperature");

  temperatureRef.on("value", (snapshot) => {
    const temperature = snapshot.val();

    if (temperature > temperatureThreshold) {
      sendLineNotification(`Temperature is ${temperature}°C. It's too hot!`);
    }
  });
}

// Function to send Line notification
function sendLineNotification(message) {
  const xhr = new XMLHttpRequest();
  xhr.open("POST", "https://notify-api.line.me/api/notify", true);
  xhr.setRequestHeader("Authorization", `Bearer ${lineNotifyAccessToken}`);
  xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

  const data = `message=${encodeURIComponent(message)}`;

  xhr.send(data);
}

// Start reading temperature data
readTemperatureData();
