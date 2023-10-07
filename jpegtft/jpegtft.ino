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
#include "RTClib.h"
RTC_DS1307 rtc;
#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#include <NTPClient.h>
#include <WiFiUdp.h>
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

const long offsetTime = 25200; 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", offsetTime);   
int hourNow, minuteNow, secondNow;

bool down;
uint16_t x, y;
int i =0;
int hrwork = 1;
int fanwork = 35;
int humiwork = 50;
int buttonState = 0;
int statuswater;
int statusfan;
int statuspump;
int timeautowork1 =0;
int timeautowork2 =0;
bool autosettimework1 = false;
bool autosettimework2 = false;
int intValue;
float floatValue;
bool signupOK = false;
int load1=1, load2=1, load3=1, load4=1, Power;
#define Relay1  1//GPIO 16
#define Relay2  33 //GPIO 5
#define Relay3  26 //GPIO 4
#define Relay4  27 //GPIO 0
#define waterLavel 35
#define waterLavel2 34
int relay1stat=0;
int relay2stat=0;
int relay3stat=0;
int relay4stat=0;
int selectedItem = 0;
int Screen=1;
bool autostat = false;
bool autostatoff = false;
unsigned long last1;
unsigned long last2;
unsigned long last3;
unsigned long last4;

void autocontrol();
void relay1info();
void relay2info();
void relay3info();
void relay4info();
void updateValue();
void Relays();
void sendValue();
void writetime();
void relay1control();
void relay2control();
void relay3control();
void relay4control();
void checkonoff();
void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(115200); // Used for messages and the C array generator
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(waterLavel, INPUT);
  //pinMode(waterLavel2, INPUT);
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
  timeClient.begin();
  dht.begin();
  homemenu();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  checkmenu();
  while(down==true)
  down = tft.getTouch(&x, &y);
  delay(100);
  while(down==false)
  {
    down = tft.getTouch(&x, &y);
    delay(1);
    if(Screen == 1){
         if(millis() - last1 >= 1000){
         last1 = millis();
         sensorupdate(); //write LCD
         
       }
    }
    if(millis() - last2 >= 1000){    
      last2 = millis();
      sendValue();
      updateValue();
      relay1info(); 
      relay2info(); 
      relay3info(); 
      relay4info();  
      autowork();
      checkonoff();
     }
   }
  handleMainMenuTouch();
  down=false;
  tft.fillScreen(TFT_BLACK);
  
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
    case 3:
      autocontrol();
      Screen=3;
      break;
    case 4:
      timesetmenu();
      Screen=4;
      break;
    case 5:
      relay1control();
      Screen=5;
      break;
    case 6:
      relay2control();
      Screen=6;
      break;
    case 7:
      relay3control();
      Screen=7;
      break;
    case 8:
      relay4control();
      Screen=8;
      break;
  }
}
void handleMainMenuTouch(){
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
   Serial.printf("Touched at (%d, %d)\n", x, y);
   if ((x > 0 && x < 240) && (y >0 && y<200)&&(selectedItem==3)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
      autostatoff = true;
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/AutoMode", 0);
      }
      
    } 
    else if ((x > 240 && x < 480) && (y >0 && y<200)&&(selectedItem==3)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
      autostatoff = false;
      autosettimework2 = false;
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/AutoMode", 1);
      }
   }
   if ((x > 0 && x < 240) && (y >0 && y<200)&&(selectedItem==5)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
      relay1stat = 1;
      relay1info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/food", 0);
      }
      
      
    } 
    else if ((x > 240 && x < 480) && (y >0 && y<200)&&(selectedItem==5)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
      relay1stat = 0;
      relay1info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/food", 1);
      }
   }

   if ((x > 0 && x < 240) && (y >0 && y<200)&&(selectedItem==6)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
      relay2stat = 1;
      relay2info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/water", 0);
      }
      
    } 
    else if ((x > 240 && x < 480) && (y >0 && y<200)&&(selectedItem==6)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
      relay2stat = 0;
      relay2info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/water", 1);
      }
   }

   if ((x > 0 && x < 240) && (y >0 && y<200)&&(selectedItem==7)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
      relay3stat = 1;
      relay3info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/fanl", 0);
      }
      
    } 
    else if ((x > 240 && x < 480) && (y >0 && y<200)&&(selectedItem==7)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
      relay3stat = 0;
      relay3info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/fanl", 1);
      }
   }

   if ((x > 0 && x < 240) && (y >0 && y<200)&&(selectedItem==8)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
      relay4stat = 1;
      relay4info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/fanr", 0);
      }
      
    } 
    else if ((x > 240 && x < 480) && (y >0 && y<200)&&(selectedItem==8)) {
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
      relay4stat = 0;
      relay4info();
      if(WiFi.status() == WL_CONNECTED){
        Firebase.RTDB.setInt(&fbdo, "/devices/fanr", 1);
      }
   }
   
   if((x>0 && x< 240) && (y > 270 && y < 320)){
      selectedItem--;
      if(selectedItem<1){
        selectedItem = 8;
      }
    }
    if((x>240 && x< 480) && (y > 270 && y < 320)){
     selectedItem++;
      if(selectedItem>8){
        selectedItem = 1;
      }
    }
    delay(100);
  }
}

void homemenu(){
  drawJpeg("/home.jpg", 0 , 0);     // 240 x 320 image
}

void statusmenu(){
  drawJpeg("/status.jpg", 0, 0);  
}

void autocontrol(){
  tft.loadFont("THNiramitAS-36"); 
  tft.fillScreen(tft.color24to16(0xFFFFFF));
  if(autostatoff){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
  }
  else{
    tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
  }
  
  tft.fillRect(0, 200, 240, 120, tft.color24to16(0x1ABC9C));
  tft.fillRect(240, 200, 240, 120, tft.color24to16(0xE74C3C));
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("AutoMode", 200, 100);
  tft.setTextColor(TFT_BLACK,tft.color24to16(0x1ABC9C));
  tft.drawString("ON", 100, 260);
  tft.setTextColor(TFT_BLACK, tft.color24to16(0xE74C3C));
  tft.drawString("OFF", 360, 260);
}

void relay1control(){
  tft.loadFont("THNiramitAS-36"); 
  tft.fillScreen(tft.color24to16(0xFFFFFF));
  if(!relay1stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
  }
  else{
    tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
  }
  
  tft.fillRect(0, 200, 240, 120, tft.color24to16(0x1ABC9C));
  tft.fillRect(240, 200, 240, 120, tft.color24to16(0xE74C3C));
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Relay1", 200, 100);
  tft.setTextColor(TFT_BLACK,tft.color24to16(0x1ABC9C));
  tft.drawString("ON", 100, 260);
  tft.setTextColor(TFT_BLACK, tft.color24to16(0xE74C3C));
  tft.drawString("OFF", 360, 260);
}

void relay2control(){
  tft.loadFont("THNiramitAS-36"); 
  tft.fillScreen(tft.color24to16(0xFFFFFF));
  if(!relay2stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
  }
  else{
    tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
  }
  
  tft.fillRect(0, 200, 240, 120, tft.color24to16(0x1ABC9C));
  tft.fillRect(240, 200, 240, 120, tft.color24to16(0xE74C3C));
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Relay2", 200, 100);
  tft.setTextColor(TFT_BLACK,tft.color24to16(0x1ABC9C));
  tft.drawString("ON", 100, 260);
  tft.setTextColor(TFT_BLACK, tft.color24to16(0xE74C3C));
  tft.drawString("OFF", 360, 260);
}

void relay3control(){
  tft.loadFont("THNiramitAS-36"); 
  tft.fillScreen(tft.color24to16(0xFFFFFF));
  if(!relay3stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
  }
  else{
    tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
  }
  
  tft.fillRect(0, 200, 240, 120, tft.color24to16(0x1ABC9C));
  tft.fillRect(240, 200, 240, 120, tft.color24to16(0xE74C3C));
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Relay3", 200, 100);
  tft.setTextColor(TFT_BLACK,tft.color24to16(0x1ABC9C));
  tft.drawString("ON", 100, 260);
  tft.setTextColor(TFT_BLACK, tft.color24to16(0xE74C3C));
  tft.drawString("OFF", 360, 260);
}

void relay4control(){
  tft.loadFont("THNiramitAS-36"); 
  tft.fillScreen(tft.color24to16(0xFFFFFF));
  if(!relay4stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
  }
  else{
    tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
  }
  
  tft.fillRect(0, 200, 240, 120, tft.color24to16(0x1ABC9C));
  tft.fillRect(240, 200, 240, 120, tft.color24to16(0xE74C3C));
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("Relay4", 200, 100);
  tft.setTextColor(TFT_BLACK,tft.color24to16(0x1ABC9C));
  tft.drawString("ON", 100, 260);
  tft.setTextColor(TFT_BLACK, tft.color24to16(0xE74C3C));
  tft.drawString("OFF", 360, 260);
}

void timesetmenu(){
  drawJpeg("/timesetting.jpg", 0, 0);
}

void controlmenu(){
  drawJpeg("/control.jpg", 0, 0);
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
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextPadding(138);
  String humi = String(h)+String(" %");
  String tem = String(t)+String(" °C");
  tft.drawString(tem, 342, 135); 
  tft.drawString(humi, 342, 216); 
  
  if(WiFi.status() == WL_CONNECTED){
    tft.drawString("Connect", 245, 265);
  }
  else{
    tft.drawString("Not Connect", 245, 265);
  }
  
}



void updateValue(){
  if (Firebase.RTDB.getInt(&fbdo, "/devices/food")) {
      if (fbdo.dataType() == "int") {
        relay1stat = fbdo.intData();
        Serial.println("Food : "+String(relay1stat));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getInt(&fbdo, "/devices/water")) {
      if (fbdo.dataType() == "int") {
        relay2stat = fbdo.intData();
        Serial.println("water : "+String(relay2stat));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    if (Firebase.RTDB.getInt(&fbdo, "/devices/fanl")) {
      if (fbdo.dataType() == "int") {
        relay3stat = fbdo.intData();
        Serial.println("fanl : "+String(relay3stat));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getInt(&fbdo, "/devices/fanr")) {
      if (fbdo.dataType() == "int") {
        relay4stat = fbdo.intData();
        Serial.println("fanr : "+String(relay4stat));
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getInt(&fbdo, "/devices/AutoMode")) {
      if (fbdo.dataType() == "int") {
        if(fbdo.intData() == 1){
          autostat = false;
          autosettimework1 = false;
        }
        else{
          autostat = true;
        }
        Serial.println("Auto : "+String(autostat));
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

void relay1info()
 {
   if(relay1stat==1)
  {
    digitalWrite(Relay1,1);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("OFF", 320, 120); 
    }
    
    
  }
  else
  {
    digitalWrite(Relay1,0);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("ON", 320, 120); 
    }
    
  }
 }

 void relay2info()
 {
   if(relay2stat==1)
  {
    digitalWrite(Relay2,1);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("OFF", 320, 170); 
    }
    
  }
  else
  {
    digitalWrite(Relay2,0);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("ON", 320, 170); 
    }
  }
 }

void relay3info()
{
   if(relay3stat==1)
  {
    digitalWrite(Relay3,1);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("OFF", 320, 208); 
    }
  }
  else
  {
    digitalWrite(Relay3,0);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("ON", 320, 208); 
    }
  }
}

 void relay4info()
{
   if(relay4stat==1)
  {
    digitalWrite(Relay4,1);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("OFF", 320, 255); 
    }
    
  }
  else
  {
    digitalWrite(Relay4,0);
    if(Screen == 2){
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      tft.setTextPadding(138);
      tft.drawString("ON", 320, 255); 
    }
  }
 }

void checkonoff(){
  if(Screen == 3){
    if(autostat || autostatoff){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
    }
    else{
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
    }
  }
  
  if(Screen == 5){
    if(!relay1stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
    }
    else{
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
    }
    
  }
  if(Screen == 6){
    if(!relay2stat){
    tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
    }
    else{
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
    }
  }
  
  if(Screen == 7){
    
    if(!relay3stat){
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
    }
    else{
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
    }
  }

  if(Screen == 8){
    if(!relay4stat){
      tft.fillCircle(100, 100, 60, tft.color24to16(0x27AE60));
    }
    else{
      tft.fillCircle(100, 100, 60, tft.color24to16(0xC3C3C3));
    }
  }
}

 void autowork(){
  if(autostat == true || autostatoff ==true){
    Serial.println("AutoMode : Work");
    float Humidity = dht.readHumidity();
    float Temperature = dht.readTemperature();
    //buttonState = digitalRead(waterLavel);
  
    //food
    if(WiFi.status() == WL_CONNECTED){
      timeClient.update();
      secondNow = timeClient.getSeconds();
      minuteNow = timeClient.getMinutes();
      hourNow = timeClient.getHours();
      if(!autosettimework1){
        timeautowork1 = hourNow + hrwork;
        Serial.println("time work Set :"+String(timeautowork1));
        autosettimework1 = true;
      }
      Serial.println("time now :"+String(hourNow));
      Serial.println("time work:"+String(timeautowork1));
      if(hourNow == timeautowork1){
         Firebase.RTDB.setInt(&fbdo, "/devices/food", 0);
         relay1stat = 1;
         relay1info();
         delay(10000);
         timeautowork1 += hrwork;
      }
    }
    else{
      if(!autosettimework2){
        timeautowork2 = millis()+(hrwork*60*60*1000);
        autosettimework2 = true;
      }
      if(millis() == timeautowork2){
        digitalWrite(Relay1,0);
        delay(1000);
        timeautowork2 = millis()+(hrwork*60*60*1000);
      }
    }
    /*
     if((buttonState == 1)&(statuswater == 0)){
        statuswater = 1;
      }
  
      if((buttonState != 1)&(statuswater == 1)){
        statuswater = 0;
      }
  
      if(statuswater == 1){
        digitalWrite(Relay2,0);      
      }
      else{
        digitalWrite(Relay2,1);
      }
  
      
  
      //fan on
      if((Temperature > fanwork)&&(statusfan == 0)){
        digitalWrite(Relay3,0);
        statusfan = 1;
  
      }
      if((Temperature < fanwork)&&(statusfan ==1)){
        digitalWrite(Relay3,1);
        statusfan = 0;
      }
  
      //pump on
      if((Humidity < humiwork)&&(statuspump ==0){
        digitalWrite(Relay4,0);
        statuspump = 1;
      }
      if((Temperature < humiwork)&&(statuspump ==1)){
        digitalWrite(Relay4,1);
        statuspump = 0;
      }*/
   }
   else{
    Serial.println("AutoMode : not Work");
   }
}
