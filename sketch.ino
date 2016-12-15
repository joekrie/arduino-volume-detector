/*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */

#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int sensorPin = A0;
const int sampleWindow = 50;
unsigned int sample;

void setup() {
  lcd.begin(16, 2);
  Wire.begin();
  
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;
  
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
   
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(0);
    
    if (sample < 1024)
    {
       if (sample > signalMax)
       {
          signalMax = sample;
       }
       else if (sample < signalMin)
       {
          signalMin = sample;
       }
    }
  }
  
  peakToPeak = signalMax - signalMin;
  double volts = (peakToPeak * 5.0) / 1024;
  
  lcd.setCursor(0, 0);  
  lcd.print("        ");  
  lcd.setCursor(0, 0);  
  lcd.print(peakToPeak);
}
