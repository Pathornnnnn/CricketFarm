// Firebase Configuration
const firebaseConfig = {
  // Your Firebase config here
};

// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Firebase Realtime Database Reference
const database = firebase.database();

// DOM Elements
const foodIntervalInput = document.getElementById('foodInterval');
const waterLevelInput = document.getElementById('waterLevel');
const fanTemperatureInput = document.getElementById('fanTemperature');
const setButton = document.getElementById('setButton');
const foodNextTime = document.getElementById('foodNextTime');
const waterLevelValue = document.getElementById('waterLevelValue');
const fanStatus = document.getElementById('fanStatus');

// Set Button Click Event
setButton.addEventListener('click', () => {
  const foodInterval = parseInt(foodIntervalInput.value);
  const waterThreshold = parseInt(waterLevelInput.value);
  const temperatureThreshold = parseInt(fanTemperatureInput.value);

  // Write user-set values to the database
  database.ref('settings').set({
      foodInterval,
      waterThreshold,
      temperatureThreshold
  });
});

// Firebase Listener for Realtime Updates
database.ref('settings').on('value', (snapshot) => {
  const settings = snapshot.val();

  if (settings) {
      // Update UI with the retrieved settings
      foodIntervalInput.value = settings.foodInterval;
      waterLevelInput.value = settings.waterThreshold;
      fanTemperatureInput.value = settings.temperatureThreshold;
  }
});

// Function to calculate and update the next food feeding time
function updateFoodNextTime() {
  const now = new Date();
  const foodInterval = parseInt(foodIntervalInput.value);
  const nextTime = new Date(now.getTime() + foodInterval * 60 * 60 * 1000);
  foodNextTime.textContent = nextTime.toLocaleTimeString();
}

// Function to simulate water level and fan status updates (replace with real data)
function updateWaterLevelAndFanStatus() {
  // Simulate water level (0-100%)
  const waterLevel = Math.floor(Math.random() * 101);
  waterLevelValue.textContent = waterLevel + '%';

  // Simulate fan status based on temperature threshold (replace with real data)
  const temperatureThreshold = parseInt(fanTemperatureInput.value);
  const currentTemperature = Math.floor(Math.random() * 41); // Simulate temperature between 0°C and 40°C
  if (currentTemperature > temperatureThreshold) {
      fanStatus.textContent = 'ON';
  } else {
      fanStatus.textContent = 'OFF';
  }
}

// Update the UI when the page loads
updateFoodNextTime();
updateWaterLevelAndFanStatus();

// Set intervals for updating UI elements
setInterval(updateFoodNextTime, 1000); // Update food next time every second
setInterval(updateWaterLevelAndFanStatus, 5000); // Update water level and fan status every 5 seconds
