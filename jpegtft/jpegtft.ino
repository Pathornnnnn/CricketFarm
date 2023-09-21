#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only  #include "Esp32_Setup_9341.h"
  #include <WiFi.h>
#endif
#define TFT_GRAY 0x7BEF

// JPEG decoder library
#include <JPEGDecoder.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert your network credentials
#define WIFI_SSID "true_home2G_8hc"
#define WIFI_PASSWORD "n68q34aK"

// Insert Firebase project API Key
#define API_KEY "AIzaSyC108mRatpnA0k_4GQhQQ1_RXNyn-obIu4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://smartfarm-cricket-default-rtdb.asia-southeast1.firebasedatabase.app/" 
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int x =0;
int i =0;
int intValue;
float floatValue;
bool signupOK = false;
int load1=1, load2=1, load3=1, load4=1, Power;
#define Relay1  14 //GPIO 16
#define Relay2  33 //GPIO 5
#define Relay3  26 //GPIO 4
#define Relay4  27 //GPIO 0

int selectedItem = 0;
int Screen=1;
unsigned long last1;

void updateValue();
void Relays();
void sendValue();

void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(115200); // Used for messages and the C array generator
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  tft.drawString("Waiting for Wifi",200,160);
  Serial.print("Connecting to Wi-Fi");
  while ((i<50)&&(WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(300);
    i+=1;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    // Assign the api key (required) 
    config.api_key = API_KEY;
  
    // Assign the RTDB URL (required) 
    config.database_url = DATABASE_URL;
  
    // Sign up 
    if (Firebase.signUp(&config, &auth, "", "")) {  
      Serial.println("ok");
      signupOK = true;
    }
    else {
      Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
  
    // Assign the callback function for the long running token generation task 
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    tft.begin();
  }
  else{
    Serial.println();
    Serial.print("time out");
    Serial.println("Offline Mode");
    Serial.println();
  }
  delay(10);
  Serial.println("ESP32 decoder test!");

  tft.begin();
  tft.setRotation(1);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_RED);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  listFiles(); // Lists the files so you can see what is in the SPIFFS
  tft.loadFont("THNiramitAS-36"); 
  dht.begin();
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t x, y;
  if (millis() - last1 >= 1000) {
    last1 = millis();
    checkmenu();
    while(!tft.getTouch(&x, &y)){
      unsigned long last2;
      handleMainMenuTouch();
      if(Screen == 1){
        if(millis() - last2 >= 3000){
        last2 = millis();
        sensorupdate();
      }
      }
    }
  }
 
}
void checkmenu(){
  switch(selectedItem){
    case 1:
      homemenu();
      Screen=1;
      break;
    case 2:
      statusmenu();
      Screen=2;
      break;
  }
}
void handleMainMenuTouch(){
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
   Serial.printf("Touched at (%d, %d)\n", x, y);
   if(Screen == 1){
     Serial.printf("Touched at (%d, %d)\n", x, y);
      selectedItem = 2;
    }
    if(Screen == 2){
      Serial.printf("Touched at (%d, %d)\n", x, y);
      selectedItem = 1;
    }
  }
}
void homemenu(){
  drawJpeg("/home.jpg", 0 , 0);     // 240 x 320 image
}

void statusmenu(){
  drawJpeg("/status.jpg", 0, 0);  
}

void sensorupdate(){
  uint16_t bg_color = tft.readPixel(230, 80);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  tft.setTextColor(TFT_BLACK, bg_color);
  tft.setTextPadding(138);
  String humi = String(h)+String(" %");
  String tem = String(t)+String(" °C");
  tft.drawString(tem, 342, 135); 
  tft.drawString(humi, 342, 216); 
  if(WiFi.status() == WL_CONNECTED){
    tft.drawString("Connect", 245, 290);
  }else{
    tft.drawString("Not Connect", 245, 290);
  }
}



void updateValue(){
  if (Firebase.RTDB.getInt(&fbdo, "/devices/food")) {
      if (fbdo.dataType() == "int") {
        load1 = fbdo.intData();
        Serial.println("Food : "+String(load1));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getInt(&fbdo, "/devices/water")) {
      if (fbdo.dataType() == "int") {
        load2 = fbdo.intData();
        Serial.println("water : "+String(load2));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getInt(&fbdo, "/devices/fanl")) {
      if (fbdo.dataType() == "int") {
        load3 = fbdo.intData();
        Serial.println("fanl : "+String(load3));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getInt(&fbdo, "/devices/fanr")) {
      if (fbdo.dataType() == "int") {
        load4 = fbdo.intData();
        Serial.println("fanr : "+String(load4));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
}
void Relays(){  
  digitalWrite(Relay1, load1);  
  digitalWrite(Relay2, load2);
  digitalWrite(Relay3, load3);
  digitalWrite(Relay4, load4);
}

void sendValue(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  if (Firebase.RTDB.setInt(&fbdo, "foodLevel", random(1,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setInt(&fbdo, "waterLevel", random(1,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setInt(&fbdo, "humidity", h)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setInt(&fbdo, "temperature", t)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}
