// Forrest - BlueStamp Main Project

// libraries
#include <CayenneMQTTESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#ifdef __AVR__
#include <avr power.h="">
#endif
#define CAYENNE_PRINT Serial
#define DHT22_PIN D2 // setting dht11 pin 
#define DHTTYPE DHT22 // setting type of the dht 
#define PIN D5 // neopixel pin conected to d5 
#define NUMPIXELS 12 // number of leds in neopixel ring 
#define buzzer D7 // set buzzer pin 
DHT dht(DHT22_PIN, DHTTYPE); // object for dht22
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);// object for neopixel
LiquidCrystal_I2C lcd(0x27, 16, 2);// declaration for LCD display

// WiFi network info.
const char *ssid = "BlueStamp Wifi V1";
const char *wifiPassword = "blue$tamp";

// Cayenne authentication info.
char username[] = "a0105d20-8d5e-11e9-b4eb-6bf2c2412b24";
char password[] = "da33a4def7d97b6a864a5501d42903a8dc2d511f";
char clientID[] = "b0505e10-8d5e-11e9-ace6-4345fcc6b81e";

const long utcOffsetInSeconds = -25200; //NTP time zone offset
const int LDRpin = A0;// ldr pin
const int PIRpin = D0;// pir pin
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int LDRval = 0; // for reading ldr value
int PIRval = 0; // for reading pir value
int temp; // for reading temperature value from dht22
int hum; // for reading humidity value from dht22
int r, g, b; // variables for storing red green and blue led mix values
int Cmode; // variable for storing security mode activate or deactivate
int TURN; // variable for manuale mode
int SAFE_MODE; // variable for false safe mode
int currentday; // check if day of week changes

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(LDRpin, INPUT);
  pinMode(PIRpin, INPUT);
  pinMode(buzzer, OUTPUT);
  dht.begin(); // starting dht22 sensor
  pixels.begin(); // start neopixel ring
  Wire.begin(D2, D1); //PINS consts
  timeClient.begin();

  currentday = timeClient.getDay();

  lcd.begin(20, 4);
  lcd.backlight(); // Turns on backlight.
  lcd.home();

  Serial.println("Hello World");
}

void loop() {

  Cayenne.loop();

  if (Cmode == 1) {
    buglerMode(); // if secuirity mode active will call bugler mode function
  }
  else if (Cmode < 1) {
    setLight(); // if not active then call setlight function

    timeClient.update();

    lcd.setCursor(0, 0);
    lcd.print("Goodmorning Forrest!");


    if (timeClient.getDay()!=currentday){
      lcd.clear();
    }

    
    if (timeClient.getDay()==0||timeClient.getDay()==1||timeClient.getDay()==2||timeClient.getDay()==5){
          lcd.setCursor(2, 1);
    }
    if (timeClient.getDay()==3||timeClient.getDay()==4||timeClient.getDay()==6){
          lcd.setCursor(1, 1);
    }

    lcd.print(daysOfTheWeek[timeClient.getDay()]);
    lcd.print(", ");
    if (timeClient.getHours() < 10) {
      lcd.print("0");
      lcd.print(timeClient.getHours());
    }
    else {
      lcd.print(timeClient.getHours());
    }
    lcd.print(":");
    if (timeClient.getMinutes() < 10) {
      lcd.print("0");
      lcd.print(timeClient.getMinutes());
    }
    else {
      lcd.print(timeClient.getMinutes());
    }
    lcd.print(":");
    if (timeClient.getSeconds() < 10) {
      lcd.print("0");
      lcd.print(timeClient.getSeconds());
    }
    else {
      lcd.print(timeClient.getSeconds());
    }
    lcd.print("  ");

    lcd.setCursor(0, 2);
    lcd.print("   Weather: Sunny");
    lcd.setCursor(0, 3);
    lcd.print("   Enjoy your day   ");


     currentday = timeClient.getDay();

    delay(250);
  }


  hum = dht.readHumidity();// storing humidity value to hum variable
  temp = dht.readTemperature();//storing temperature value to temp variable
  LDRval = analogRead(LDRpin);// read and store ldr value
  PIRval = digitalRead(PIRpin); // read and store pir value
  Serial.println(PIRval);
  // send all sensor values to cayenne
  Cayenne.virtualWrite(0, temp);
  Cayenne.virtualWrite(1, hum);
  Cayenne.virtualWrite(2, LDRval);
  Cayenne.virtualWrite(3, PIRval);

  // checking temperature or humidity is in dangerous level
  if (hum > 85 && hum < 15) {
    digitalWrite(buzzer, HIGH);
  }
  if (temp > 40 && temp < 10)
  {
    digitalWrite(buzzer, HIGH);
  }
  // checking false safe mode
  if (SAFE_MODE == 1)
  {
    digitalWrite(buzzer, LOW);
  }
}
void setColor(int red, int green, int blue)
{
  for (int i = 0; i <= NUMPIXELS; i++) // set every led into the color
  {
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); // seting color neopixel
    pixels.show();// activate neopixel
  }
}
void setLight()// checking manual mode active or not
{
  if (TURN == 1)
  {
    manual();
  }
  else
  {
    if (PIRval > 0)
    {
      setColor(r, g, b);
    }
    else if (LDRval < 30)
    {
      setColor(r, g, b);
    }
    else if (PIRval < 1 && LDRval > 10)
    {
      setColor(0, 0, 0);
    }
  }
}
void buglerMode()// bugler mode's function
{
  if (SAFE_MODE == 1)
  {
    digitalWrite(buzzer, LOW);
  }
  if (PIRval > 0)
  {
    digitalWrite(buzzer, HIGH);
    Serial.println("bugler on");
  }
}
void manual()// manual mod's function
{
  setColor(r, g, b);
}

//recieve values from cayenne
CAYENNE_IN(4)
{
  r = getValue.asInt(); // recieve red value for neopixel from cayenne
}
CAYENNE_IN(5)
{
  g = getValue.asInt(); // recieve green value for neopixel from cayenne
}
CAYENNE_IN(6)
{
  b = getValue.asInt(); // recieve blue value for neopixel from cayenne
}
CAYENNE_IN(7)
{
  Cmode = getValue.asInt(); // recieve commands for security mode
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
