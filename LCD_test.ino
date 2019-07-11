#include <LiquidCrystal_I2C.h>

#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

Serial.begin(115200);
Wire.begin(D2, D1);   //Use predefined PINS consts
lcd.begin(20,4);      // The begin call takes the width and height. This
                      // Should match the number provided to the constructor.

lcd.backlight();      // Turn on the backlight.

lcd.home();

lcd.setCursor(3, 0);  // Move the cursor at origin
lcd.print("HELLO WORLD!,");
lcd.setCursor(0, 1);
lcd.print("Just testing");
lcd.setCursor(0, 2);
lcd.print("Thank you.");
lcd.setCursor(5, 3);
lcd.print("GOOD LUCK!");

}

void loop() {         

}
