#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

#define pullUp 2
#define redLED 8
#define greenLED 7

void setup() {
  // put your setup code here, to run once:
//start serial connection

  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address
  pinMode(pullUp, INPUT_PULLUP);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}
static const uint8_t PROGMEM

 smile[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };

void writeName(String Name, uint16_t color, int wait){
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(color);
  int limit = Name.length()*(-7);
  for (int8_t x=7; x>= limit ; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(Name);
    matrix.writeDisplay();
    delay(wait);
  }
}



void loop() {
  int sensorVal = digitalRead(pullUp);
  Serial.println(sensorVal);
  if (sensorVal == HIGH) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    matrix.clear();
    matrix.setRotation(3);
    matrix.drawBitmap(0, 0, smile, 8, 8, LED_YELLOW);
    matrix.writeDisplay();
  } 
  else {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    writeName("THUNDER-DOME", LED_GREEN, 70);
       
  }
}

