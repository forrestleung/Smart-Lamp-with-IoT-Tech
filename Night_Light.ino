// Forrest - BlueStamp Main Project

// libraries 
#include <CayenneMQTTESP8266.h> 
#include <esp8266wifi.h> 
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#ifdef __AVR__ 
 #include <avr power.h=""> 
#endif
#define CAYENNE_PRINT Serial
#define DHTPIN 4 // setting dht11 pin 
#define DHTTYPE DHT11// setting type of the dht 
#define PIN D5 // neopixel pin conected to d5 
#define NUMPIXELS 12 // number of leds in neopixel ring 
#define buzzer D7// set buzzer pin 
DHT dht(DHTPIN, DHTTYPE);// object for dht11 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);// object for neopixel

// WiFi network info.
char ssid[] = "forrest";  
char wifiPassword[] = "testme21"; 

// Cayenne authentication info.
char username[] = "a0105d20-8d5e-11e9-b4eb-6bf2c2412b24";
char password[] = "da33a4def7d97b6a864a5501d42903a8dc2d511f";
char clientID[] = "b0505e10-8d5e-11e9-ace6-4345fcc6b81e";

const int LDRpin = A0;// ldr pin 
const int PIRpin = D0;// pir pin 
int LDRval = 0;// for reading ldr value 
int PIRval = 0;// for reading pir value  
int temp;// for reading temperature value from dht11 
int hum;// for reading humidity value from dht11 
int r,g,b;// variables for storing red green and blue led mix values 
int Cmode;// variable for storing security mode activate or deactivate 
int TURN;// variable for manuale mode 
int SAFE_MODE;// variable for false safe mode 

void setup() {  
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(LDRpin,INPUT);
  pinMode(PIRpin,INPUT); 
  pinMode(buzzer,OUTPUT);
  dht.begin();// starting dht11 sensor 
  pixels.begin(); // start neopixel ring 
  Serial.println("Hello World");
} 

void loop()  
{  
  Cayenne.loop();  
  // checking secuirity mode activated! 
  if(Cmode==1) 
 { 
   buglerMode();// if secuirity mode active will call bugler mode function 
 } 
 else if(Cmode<1){ 
   setLight();// if not active then call setlight function 
 }


hum = dht.readHumidity();// storing humidity value to hum variable 
temp = dht.readTemperature();//storing temperature value to temp variable 
LDRval = analogRead(LDRpin);// read and store ldr value 
PIRval = digitalRead(PIRpin); // read and store pir value 
Serial.println(PIRval); 
// send all sensor values to cayenne 
      Cayenne.virtualWrite(0,temp); 
      Cayenne.virtualWrite(1,hum); 
      Cayenne.virtualWrite(2,LDRval); 
      Cayenne.virtualWrite(3,PIRval); 
      
// checking temperature or humidity is in dangerous level        
if(hum>85&&hum<15){ 
 digitalWrite(buzzer,HIGH); 
} 
if(temp>40&&temp<10) 
 { 
   digitalWrite(buzzer,HIGH); 
 } 
// checking false safe mode       
if(SAFE_MODE==1) 
 { 
   digitalWrite(buzzer,LOW); 
 } 
} 
void setColor(int red,int green,int blue) 
{ 
 for(int i =0;i<=NUMPIXELS;i++)// set every led into the color 
 { 
   pixels.setPixelColor(i,pixels.Color(red,green,blue));// seting color neopixel 
   pixels.show();// activate neopixel 
 } 
} 
void setLight()// checking manual mode active or not 
{ 
if(TURN==1) 
{ 
 manual(); 
} 
 else 
 { 
    if(PIRval>0) 
{ 
 setColor(r,g,b); 
} 
else if(LDRval<30) 
{ 
 setColor(r,g,b); 
} 
else if(PIRval<1&&LDRval>10) 
{ 
 setColor(0,0,0); 
} 
} 
} 
void buglerMode()// bugler mode's function 
{ 
 if(SAFE_MODE==1) 
 { 
   digitalWrite(buzzer,LOW); 
 } 
 if(PIRval>0) 
{ 
 digitalWrite(buzzer,HIGH); 
 Serial.println("bugler on"); 
} 
} 
void manual()// manual mod's function 
{ 
  setColor(r,g,b); 
} 

//recieve values from cayenne 
   CAYENNE_IN(4) 
{ 
 r= getValue.asInt();// recieve red value for neopixel from cayenne 
} 
CAYENNE_IN(5) 
{ 
 g= getValue.asInt();// recieve green value for neopixel from cayenne 
} 
CAYENNE_IN(6) 
{ 
   b= getValue.asInt();// recieve blue value for neopixel from cayenne 
} 
CAYENNE_IN(7) 
{ 
 Cmode =getValue.asInt();// recieve commands for security mode 
Serial.println(Cmode); 
} 
CAYENNE_IN(8) 
{ 
 TURN = getValue.asInt();// recieve commands for manual mode 
} 
CAYENNE_IN(9) 
{ 
 SAFE_MODE = getValue.asInt();// recieve commands for false safe 
} 
