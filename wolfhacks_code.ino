#include <Servo.h>
#include <Wire.h>
#include <ds3231.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11

Servo water;
DHT dht(2, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27,16,2);

struct ts t; 
float sensorValue = 0; 
int watered = 0;

void setup() {
  water.attach(3);
  DS3231_init(DS3231_CONTROL_INTCN);  
  waterPlants();
  //applyAluminumSulfate();
  Serial.begin(9600);
  Wire.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
  DS3231_get(&t);

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

  
  int val = map(analogRead(A0),550,0,0,100) + 100;

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
  delay(1000);

  if(val < 18 && watered % 5 == 0){
    applyAluminumSulfate();
    watered++;
  } else if(val < 18){
    waterPlants();
    watered++;
  }
}


void waterPlants(){
  for(int i = 90; i < 180; i++){
    water.write(i);
    delay(50);
  }
  water.write(180);
  delay(1000);
  water.write(90);
}

void applyAluminumSulfate(){
  water.write(90);
  delay(1000);
  water.write(0);
  delay(1000);
  water.write(90);
}

String printDateTime(){
  return String(t.mon) + "/" + String(t.mday) + "/" + String(t.year) + " " + String(t.hour) + ":" + String(t.min);
}
