#include <Arduino.h>
#include <SoftwareSerial.h>

//New
#include <Encoder.h>

Encoder knobLeft(2,3);
//New ^

#define TX 52
#define RX 53

#define START   100
#define ENDGAME 255

#define EASY 10
#define MEDIUM 20
#define HARD 30

#define MAX_POWER 5000
#define BTrefreshTime 2000 // (ms)

#define LED1USER 2
#define LED2USER 3
#define LED3USER 4
#define LED4USER 5
#define LED5USER 6

#define LED1SOLAR 8
#define LED2SOLAR 9
#define LED3SOLAR 10
#define LED4SOLAR 11
#define LED5SOLAR 12

const int LEDUserPins[5] = {LED1USER, LED2USER, LED3USER, LED4USER, LED5USER}; 
const int LEDSolarPins[5] = {LED1SOLAR, LED2SOLAR, LED3SOLAR, LED4SOLAR, LED5SOLAR}; 
const int LEDAllPins[10] = {LED1USER, LED2USER, LED3USER, LED4USER, LED5USER, LED1SOLAR, LED2SOLAR, LED3SOLAR, LED4SOLAR, LED5SOLAR};
const int maxPower = 25; // W
const int numberFloors = 5;

String difficulty; // stores difficulty level
int solarPower;

String incomingData = ""; // For BT
int receivedData = 0;
SoftwareSerial BTserial(TX, RX); // TX | RX (On BT module)

void send(int data){
  static unsigned long timeKeeper = 0;
  BTserial.print(data);
  BTserial.print("\n");
}

void receiveData(){
  if(BTserial.available()){
        incomingData = ""; //No repeats
      BTserial.print("Data received");
      Serial.print("---- receiveData() "); Serial.println(millis());
      Serial.println("Receving:");
      while(BTserial.available()){ // While there is more to be read, keep reading.
        incomingData += (char)BTserial.read();
      }

//      decipher(incomingData.toInt());
    receivedData = incomingData.toInt();
    Serial.println(receivedData);
  }
}

void difficultyLevel() {
  receivedData = 0;
  while (true) {
    receiveData();
    if (receivedData == EASY) {
      difficulty = "EASY";
      solarPower = 11; //11 Watts
      break; }
    if (receivedData == MEDIUM) {
      difficulty = "MEDIUM";
      solarPower = 15; //15 Watts
      break; }
    if (receivedData == HARD) {
      difficulty = "HARD";
      solarPower = 22; // 22 Watts
      break; }
  }
  
 
}

void gameMode() {
  int encoderReading, encoderPower;
  difficultyLevel();
  receivedData = 0;
  
  while (true) {
    receiveData();
    if (receivedData == ENDGAME) {
      Serial.println("Game end");
      break;
    }
    encoderReading = readEncoder();
    encoderPower = convertEncoderToPower(encoderReading);
    send(encoderPower);
    lightGeisel(encoderPower, 0); // lights right side of Geisel
    lightGeisel(solarPower, 1);
  }
}

//New
int readEncoder(){
  static long encoderPosition  = -999;

  static unsigned long dt = 0;
  static unsigned long lastTimeStamp = 0;
  static unsigned int rotSpeed = 0;

  long newPosition;
  newPosition = knobLeft.read();

  dt = (unsigned long)(millis() - lastTimeStamp);
  lastTimeStamp = millis(); 
  rotSpeed = abs(newPosition - encoderPosition)/dt; 
  rotSpeed = rotSpeed == 65535 ? 0 : rotSpeed; //Remove outliers 
  
  if (newPosition != encoderPosition) {
    Serial.print("Left = ");
    Serial.print(newPosition);
    Serial.print(", Speed = ");
    Serial.print(rotSpeed);
    Serial.println();
    
    encoderPosition = newPosition;
  }

  return 10; //for now, this is just a value
}

/* Should map from 0 to 25 Watts (or 30 Watts), Can only be an Int! */
int convertEncoderToPower(int encoderValue) {
  return encoderValue;
}

void lightGeisel(int geiselPower, int lightLeft) {
  int LEDPins[5];
  int maxFloorThreshold = maxPower/numberFloors;
  int index = int(geiselPower/(maxFloorThreshold+1));
  int brightness = ((geiselPower-(index*201))*1.25);

  if(lightLeft == 1) {
    for(int i = 0; i < numberFloors; i++) {
      LEDPins[i] = LEDSolarPins[i];
    }
  }

  if(lightLeft == 0) {
    for(int i = 0; i < numberFloors; i++) {
      LEDPins[i] = LEDUserPins[i];
    }
  }
  
  resetLights();
  
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

void resetLights() {
  for(int i = 0; i < 2*numberFloors;i++){
    analogWrite(LEDAllPins[i], 0);
  }
  delay(50);
}

//New ^

void setup() {
  Serial.begin(9600);
  Serial.println("System running.");
 
  BTserial.begin(9600);
  pinMode(TX, INPUT_PULLUP); // only needed for JY-MCUY v1.06x
  BTserial.listen();

  BTserial.print("BT GO.."); BTserial.println(millis());
  resetLights();
//  initializeSimData();
//  Serial.println(SIM_LIGHTING_STEP);
}


void loop() {
    receiveData();
    if (receivedData == START) {
        Serial.println("Game start");
        gameMode(); 
    }
//    delay(100);
  readEncoder();
  resetLights();
}
