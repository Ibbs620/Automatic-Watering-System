#include <Servo.h>
#include <Wire.h>
#include <ds3231.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
//Import Libraries

#define DHTTYPE DHT11 //Define type of temperature sensor being used

Servo water;
DHT dht(2, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27,16,2);
//Declare LCD, servo, DHT sensor

struct ts t; 

void setup() {
  water.attach(3); //Set servo as pin 3
  DS3231_init(DS3231_CONTROL_INTCN); //Start up temperature sensor
  Wire.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();
  //start up components
}

void loop() {
  DS3231_get(&t); //get time variable from clock
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  int val = map(analogRead(A0),550,0,0,100) + 100; //Read soil moisture sensor and fix readings

  lcd.setCursor(0,0);
  lcd.print(printDateTime());
  lcd.setCursor(0,1);
  lcd.print(t);
  lcd.setCursor(4,1);
  lcd.print("C");
  lcd.setCursor(9,1);
  lcd.print("Mstr:");
  lcd.setCursor(14,1);
  lcd.print(val);
  //print information to LCD

 if(val < 18){ //water plants when soil is dry
    waterPlants();
 }
 delay(1000); //delay
}
void waterPlants(){ //Waters the plants with the servo
  for(int i = 90; i < 180; i++){ //slowly push servo. If pushed fast, the water will shoot out too fast
    water.write(i);
    delay(50);
  }
  water.write(180); //return servo to original position
  delay(1000);
  water.write(90);
}

String printDateTime(){ //format date and time for LCD display
  return String(t.mon) + "/" + String(t.mday) + "/" + String(t.year) + " " + String(t.hour) + ":" + String(t.min);
}
