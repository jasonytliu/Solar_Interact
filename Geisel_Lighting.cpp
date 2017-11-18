#include <Arduino.h>

#define LED1 2
#define LED2 3
#define LED3 4
#define LED4 5
#define LED5 6

const int LEDPins[5] = {LED1, LED2, LED3, LED4, LED5}; //Do not need to call pinMode()
const int maxPower = 1000; //mW
const int numberFloors = 5;

void lightGeisel(int M){ //M is measured power in mW
  // int minPower = 0; //mW
  int maxFloorThreshold = maxPower/numberFloors;
  int index = int(M/(maxFloorThreshold+1));
  int brightness = ((M-(index*201))*1.25);
  for(int i = 0; i<index;i++){
    analogWrite(LEDPins[i], 1023);
  }
  analogWrite(LEDPins[index], brightness);
  for(int i = index+1; i<numberFloors; i++){
    analogWrite(LEDPins[i], 0);
  }
  Serial.print("Brightness is set at " );
  Serial.println(brightness);
}

void setup(){
  Serial.begin(9600);
  Serial.println("System started.");
  for(int i = 0; i < 5;i++){
    analogWrite(LEDPins[i], 10);
    delay(100);
  }
  for(int i = 0; i < 5;i++){
    analogWrite(LEDPins[i], 0);
    delay(100);
  }
}

void loop(){
  int power = 0;
  while(power < maxPower){
    Serial.print("Power at ");
    Serial.println(power);
    lightGeisel(power);
    delay(20);
    power+=1;

  }
}
