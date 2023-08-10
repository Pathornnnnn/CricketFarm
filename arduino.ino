#include <SimpleTimer.h>
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_TEMPLATE_ID "TMPL6KG_Z3pew"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "MA4nJ-3Co94RnP4bNoFXwGCNNWEGt0Sc"
char ssid[] = "Wokwi-GUEST"; // ใส่ชื่อ WiFi ที่ต้องการเชื่อมต่อ
char pass[] = ""; // ใส่รหัสผ่าน WiFi ที่ต้องการเชื่อมต่อ
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>

#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

/* Hello Wokwi! */

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {14,27, 26, 25};
byte colPins[COLS] = {33, 16, 17, 32};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int i = 0;
SimpleTimer timer;
#include "RTClib.h"

RTC_DS1307 rtc;

#include <DHT.h>
 // ใส่หมายเลขของพิน GPIO ที่เชื่อมกับ LED
 //input
#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int waterlevel = 23;

//output
#define motor 2
#define solinoid 4
#define fan1 12
#define fan2 13


int hr = 0;
int mn = 0;
int s = 0;
int hrwork = 4;
int fanwork = 35;

void Blynkfunc(){
  float Humidity = dht.readHumidity();
  float Temperature = dht.readTemperature();
  Blynk.run();
  Serial.print("Humidity: " + String(Humidity) + " %");
  Serial.print("\t");
  Serial.println("Temperature: " + String(Temperature) + " C");

  // Update Blynk data
  Blynk.virtualWrite(V10, Temperature);
  Blynk.virtualWrite(V11, Humidity);
  
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000, Blynkfunc);
  pinMode(motor, OUTPUT);
  pinMode(solinoid, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(waterlevel, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Cricket Farm!");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  delay(1000);
  lcd.clear();
  if(EEPROM.read(0)!=1){
    settime();
    EEPROM.write(0,1);
  }
  else{
    EEPROM.get(1,hr);
    EEPROM.get(2,mn);
  }
  rtc.adjust(DateTime(0,0 ,0 , hr, mn, s));
  EEPROM.put(1,hr);
  EEPROM.put(2,mn);
}



void loop() {
  timer.run();
  char key = keypad.getKey();
  home();
  if (key) {
      switch(key)
      {  
      case '1': // SELECCION PARA LEER SENSORE 1 Y 2 
        lcd.clear();
        lcd.setCursor (0, 0);
        lcd.print("Auto work");
        delay(1000); 
        work();
        break;  


      case '2': // SELECCION PARA LEER SENSORES 3 Y 4
        
        lcd.clear();
        lcd.setCursor (0, 0);
        lcd.print("Manual");
        delay(1000); 
        Manual();
        break; 

      case '3':
        lcd.clear();
        lcd.setCursor (0, 0);
        lcd.print("Setting");
        delay(1000); 
        lcd.clear();
        setting();
        break;

      case '4':
        float Humidity = dht.readHumidity();
        float Temperature = dht.readTemperature();
        String humid = String(Humidity);
        String Tem = String(Temperature);
        lcd.clear();
        lcd.setCursor (0, 0);
        lcd.print("Temperature : "+Tem);
        lcd.setCursor (0, 1);
        lcd.print("Humidity : "+humid);
        delay(5000); 
        lcd.clear();
        break;
    }
  }
  delay(500);
}

void updatetime(){
  DateTime now = rtc.now();
  EEPROM.put(1,now.hour());
  EEPROM.put(2,now.minute());
}

void home(){
  DateTime now = rtc.now();
  lcd.clear(); 
  lcd.setCursor (3,0);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  lcd.setCursor (0, 1);
  lcd.print("1.Auto food");
  lcd.setCursor (0, 2);
  lcd.print("2.Manual Control");
  lcd.setCursor (0, 3);
  lcd.print("3.Setting time");
  delay(100);

}


void settime(){
  //set hour
  int x = 0;
  lcd.setCursor (0, 0);
  lcd.print("Enter Hour");
  lcd.setCursor (x, 1);
  lcd.blink();
  String hrstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      hrstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  hr = hrstring.toInt();

  //set minute
  x = 0;
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("Enter Minute");
  lcd.setCursor (x, 1);
  lcd.blink();
  String mnstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      mnstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  mn = mnstring.toInt();

  //set second
  x = 0;
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("Enter Second");
  lcd.setCursor (x, 1);
  lcd.blink();
  String sstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      sstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  s = sstring.toInt();

  
}


void work(){
  DateTime now = rtc.now();
  int water = digitalRead(waterlevel);
  int statuswater = 0;
  int statusfan = 0;
  int statusfood = 0;
  int nexttime = now.hour()+hrwork;
  if(nexttime > 23){
      nexttime -= 24;
      if(nexttime < 0){
        nexttime *= nexttime;
      }
    }
  while(1){
    water = digitalRead(waterlevel);
    DateTime now = rtc.now();
    float Humidity = dht.readHumidity();
    float Temperature = dht.readTemperature();
    String a;
    a = String(nexttime);
    char key = keypad.getKey();
    lcd.setCursor (3,2);
    lcd.print("Now :");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second());
    lcd.setCursor (0, 1);
    lcd.print("Next time feed: "+a+"hr");
    lcd.setCursor(0,3);
    lcd.print("Press # to exit");
    if(key == '#'){
      break;
    }

      //food
    if(nexttime == now.hour()){
      digitalWrite(motor,1);
      delay(10000);
      digitalWrite(motor,0);
      nexttime += hrwork;
      if(nexttime > 23){
        nexttime -= 24;
        if(nexttime < 0){
          nexttime *= nexttime;
        }
      }
    }

    //value on
    if((water != 1)&&(statuswater == 0)){
      digitalWrite(solinoid,1);
      statuswater = 1;
    }
    if((water != 0 )&&(statuswater == 1)){
      digitalWrite(solinoid,0);
      statuswater = 1;
    }

    //fan on
    if((Temperature > fanwork)&&(statusfan == 0)){
      digitalWrite(fan1,1);
      digitalWrite(fan2,1);
      statusfan = 1;

    }
    if((Temperature < fanwork)&&(statusfan ==1)){
      digitalWrite(fan1,0);
      digitalWrite(fan2,0);
      statusfan = 0;
    }
    delay(1000);
  }

}


void Manual(){
  int z = 1;
  lcd.clear();
  lcd.setCursor (4, 0);
  lcd.print("Manual");
  lcd.setCursor (0, 1);
  lcd.print("1. Food");
  lcd.setCursor (0, 2);
  lcd.print("2. Fan");
  lcd.setCursor (0, 3);
  lcd.print("3. Water");
  while(z > 0){
    char key = keypad.getKey();
    if(key){
      switch(key){
        case '1':
          lcd.clear();
          lcd.setCursor (4, 0);
          lcd.print("Settime");
          lcd.setCursor (0, 1);
          lcd.print("1. ON");
          lcd.setCursor (0, 2);
          lcd.print("2. OFF");
          lcd.setCursor (0, 3);
          foodman();
          break;
        case '2':
          lcd.clear();
          lcd.setCursor (4, 0);
          lcd.print("Fannnn");
          lcd.setCursor (0, 1);
          lcd.print("1. ON");
          lcd.setCursor (0, 2);
          lcd.print("2. OFF");
          lcd.setCursor (0, 3);
          fanman();
          break;
        case '#':
          z=0;
          break;
        default:
          Serial.println("default");
      }
    }
  }
}


void foodman(){
  int food = 0;
  while(1){
      char key = keypad.getKey();
      if(key == '#'){
        lcd.clear();
        lcd.setCursor (4, 0);
        lcd.print("Manual");
        lcd.setCursor (0, 1);
        lcd.print("1. Food");
        lcd.setCursor (0, 2);
        lcd.print("2. Fan");
        lcd.setCursor (0, 3);
        lcd.print("3. Water");
        break;
      }
      if(key == '1'){
        food = 1;
        delay(1000);
      }
      if((key == '2')&&(food == 1)){
        food = 0;
      }
      if(food == 1){
        digitalWrite(motor,1);       
      }
      else{
        digitalWrite(motor,0);
      }   
    delay(100);
  }
}

void fanman(){
  int fan = 0;
  while(1){
    char key = keypad.getKey();
    if(key == '#'){
      lcd.clear();
       lcd.setCursor (4, 0);
        lcd.print("Manual");
        lcd.setCursor (0, 1);
        lcd.print("1. Food");
        lcd.setCursor (0, 2);
        lcd.print("2. Fan");
        lcd.setCursor (0, 3);
        lcd.print("3. Water");
      break;
    }
    if(key == '1'){
      fan = 1;
      delay(1000);
    }
    if((key == '2')&&(fan == 1)){
      fan = 0;
    }
    if(fan == 1){
      digitalWrite(fan1,1);
      digitalWrite(fan2,1);      
    }
    else{
      digitalWrite(fan1,0);
      digitalWrite(fan2,0);
    }
  }
}


void settimeedit(){
  //set hour
  int x = 0;
  lcd.setCursor (0, 0);
  lcd.print("Enter Hour");
  lcd.setCursor (x, 1);
  lcd.blink();
  String hrstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      hrstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  hr = hrstring.toInt();

  //set minute
  x = 0;
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("Enter Minute");
  lcd.setCursor (x, 1);
  lcd.blink();
  String mnstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      mnstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  mn = mnstring.toInt();
  //set second
  x = 0;
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("Enter Second");
  lcd.setCursor (x, 1);
  lcd.blink();
  String sstring = "";
  while(1){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>1)){
        break;
      }
      lcd.print(key);
      sstring += key;
      lcd.setCursor(x+=1,1);
    }
  }
  s = sstring.toInt();
  EEPROM.put(1,hr);
  EEPROM.put(2,mn);
  EEPROM.put(3,s);
}

void Foodhour(){
  int y = 1;
  int x = 11;
  String a = String(hrwork);
  String hrworknew = "";
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Food Hour");
  lcd.setCursor(0,1);
  lcd.print("Food Hour Work :"+a+" hr");
  lcd.setCursor(0,2);
  lcd.print("Change hr : ");
  lcd.setCursor(x,2);
  delay(1000);
  lcd.blink();
  while(y >0){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>12)){
        break;
      }
      lcd.print(key);
      hrworknew += key;
      lcd.setCursor(x+=1,2);
    }
  }
  hrwork = hrworknew.toInt();
  a = String(hrwork);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Food Hour");
  lcd.setCursor(0,1);
  lcd.print("Food Hour Work :"+a+" hr");
  lcd.setCursor(0,2);
  lcd.print("Press # to exit");
  delay(2000);
}
void Fantemperature(){
  int y = 1;
  int x = 13;
  String a = String(fanwork);
  String fanhrworknew = "";
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Fan temperature");
  lcd.setCursor(0,1);
  lcd.print("Fan work at tem : "+a);
  lcd.setCursor(0,2);
  lcd.print("Change tem : ");
  lcd.setCursor(x,2);
  delay(1000);
  lcd.blink();
  while(y >0){
    char key = keypad.getKey();
    if(key){
      if((key == '#')||(x>14)){
        break;
      }
      lcd.print(key);
      fanhrworknew += key;
      lcd.setCursor(x+=1,2);
    }
  }
  fanwork = fanhrworknew.toInt();
  a = String(fanwork);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Fan temperature");
  lcd.setCursor(0,1);
  lcd.print("Fan work at tem : "+a);
  lcd.setCursor(0,2);
  lcd.print("Press # to exit");
  delay(2000);
}



void setting(){
  int z = 1;
  lcd.clear();
  lcd.setCursor (4, 0);
  lcd.print("Setting");
  lcd.setCursor (0, 1);
  lcd.print("1. TIME");
  lcd.setCursor (0, 2);
  lcd.print("2. Food hour");
  lcd.setCursor (0, 3);
  lcd.print("3. Fan temperature");
  while(z > 0){
    char key = keypad.getKey();
    if(key){
      switch(key){
        case '1':
          lcd.clear();
          lcd.setCursor (4, 0);
          lcd.print("Settime");
          delay(1000);
          settimeedit();
          rtc.adjust(DateTime(0,0 ,0 , hr, mn, 0));
          break;
        case '2':
          lcd.clear();
          lcd.setCursor (4, 0);
          lcd.print("Set");
          lcd.setCursor (4, 1);
          lcd.print("Food hour");
          delay(1000);
          Foodhour();
          break;
        case '3':
          lcd.clear();
          lcd.setCursor (4, 0);
          lcd.print("Set");
          lcd.setCursor (4, 1);
          lcd.print("Fan temperature");
          delay(1000);
          Fantemperature();
          break;
        case '#':
          z=0;
          break;
        default:
          Serial.println("default");
      }
    }
  }
}
