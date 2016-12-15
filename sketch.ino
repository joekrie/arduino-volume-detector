#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int sensorPin = A0;
const int sampleWindow = 50;
unsigned int sample;

void setup() {
  lcd.begin(16, 2);
  Wire.begin();
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

  int transmitValue = (int)(peakToPeak * 20);  
  Wire.beginTransmission(0x60);
  Wire.write(64);
  Wire.write(transmitValue >> 4);
  Wire.write((transmitValue & 15) << 4);
  Wire.endTransmission();
  
  lcd.setCursor(0, 0);  
  lcd.print("        ");  
  lcd.setCursor(0, 0);  
  lcd.print(peakToPeak);
}
