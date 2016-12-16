#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int sensorPin = A0;
const int sampleWindow = 50;
unsigned int sample;

void setup() {
  lcd.begin(16, 2);
  Wire.begin();
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
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

  if (peakToPeak > 400) {
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  } else {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
  }

  int transmitValue = 2048 - (int)(peakToPeak * 2);
  Wire.beginTransmission(0x60);
  Wire.write(64);
  Wire.write(transmitValue / 16);
  Wire.write((transmitValue % 16) << 4);
  Wire.endTransmission();

  lcd.setCursor(0, 0);
  lcd.print("        ");
  lcd.setCursor(0, 0);
  lcd.print(peakToPeak);
}

