// Replace with your Firebase project's configuration
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

// Initialize Firebase
firebase.initializeApp(firebaseConfig);

// Reference to your Firebase Realtime Database
const database = firebase.database();

// Add click event listeners for the buttons
document.getElementById('relay1').addEventListener('click', () => toggleRelay(5));
document.getElementById('relay2').addEventListener('click', () => toggleSwitch(water));
document.getElementById('relay3').addEventListener('click', () => toggleRelay(3));
document.getElementById('relay4').addEventListener('click', () => toggleRelay(4));
// Function to toggle the state of a relay

function toggleRelay(relayNumber) {
    const relayRef = database.ref(`relays/relay${relayNumber}`);
    
    // Read the current state of the relay
    relayRef.once('value', (snapshot) => {
        const currentState = snapshot.val();
        
        // Toggle the state (assuming it's boolean)
        const newState = !currentState;
        if(typeof(currentState) == "number" ){
            newState = Number(newState);
        };
        
        // Update the relay state in Firebase
        relayRef.set(newState);
    });
}

function toggleSwitch(relayNumber){
    const relayRef = database.ref(`/${relayNumber}`);
    
    // Read the current state of the relay
    relayRef.once('value', (snapshot) => {
        const currentState = snapshot.val();
        
        // Toggle the state (assuming it's boolean)
        const newState = !currentState;
        if(typeof(currentState) == "number" ){
            newState = Number(newState);
        };
        
        // Update the relay state in Firebase
        relayRef.set(newState);
    });
}
