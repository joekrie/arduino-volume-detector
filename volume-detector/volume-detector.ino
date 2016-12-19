#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // tell Arduino which pins connect to text display

const int sensorPin = A0;  // mic is hooked up to A0 pin
const int sampleWindow = 50;  // # of milliseconds per sample
unsigned int sample;  // holds sample every iteration

void setup() {
  lcd.begin(16, 2);  // initialize text display
  Wire.begin();  // initialize DAC connection
  pinMode(6, OUTPUT);  // pin 6 turns LED red
  pinMode(7, OUTPUT);  // pin 7 turns LED green
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow) {  // sample window
    sample = analogRead(0);

    if (sample < 1024)  // reject invalid readings
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

  peakToPeak = signalMax - signalMin;  // what we care about is difference 

  if (peakToPeak > 400) {  // turn LED red if volume above threshold
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  } else {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
  }

  int transmitValue = 2048 - (int)(peakToPeak * 2);  // value to send to DAC
  Wire.beginTransmission(0x60);  // begin transmission to DAC
  Wire.write(64);  // command to update DAC
  Wire.write(transmitValue / 16);  // 8 most significant bits
  Wire.write((transmitValue % 16) << 4);  // 4 least significant bits
  Wire.endTransmission();

  lcd.setCursor(0, 0);
  lcd.print("        ");  // clear previous reading
  lcd.setCursor(0, 0);
  lcd.print(peakToPeak);  // set text display to show volume
}

