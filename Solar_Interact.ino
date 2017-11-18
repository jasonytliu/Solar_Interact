#include <Arduino.h>
#include <SoftwareSerial.h>

#define TX 50
#define RX 51

#define START   100
#define ENDGAME 10000

#define MAX_POWER 5000
#define BTrefreshTime 2000 // (ms)

#define LED1 2
#define LED2 3
#define LED3 4
#define LED4 5
#define LED5 6
#define LED6 7
#define LED7 8
#define LED8 9
#define LED9 10
#define LED10 11

#define GENERATOR_PIN A5
#define SIM 0
#define GEN 1

const int LEDPins_Sim[5] = {LED1, LED2, LED3, LED4, LED5}; //Do not need to call pinMode()
const int LEDPins_Gen[5] = {LED6, LED7, LED8, LED9, LED10}; //Do not need to call pinMode()
const int maxPower = 1000; //mW
const int numberFloors = 5;

String incomingData = ""; // For BT
int receivedData = 0;
SoftwareSerial BTserial(TX, RX); // TX | RX (On BT module)

float pollPower(){
  float Vint;
  float Power;
  float Vflo;
  float powerScale = 1000.0; // scalar to make power unit more readable for the user (1000 corresponds to an output in mW)
  float Rload = 20000.0; // load resistance being driven by the generator
  Vint = analogRead(GENERATOR_PIN); //returns integer between 0 (0V) and 1024 (5V) corresponding to voltage at pinIn
  Vflo = 5.0*(Vint/1023); //converts Vint into a float equal to the actual voltage at the node
  Power = powerScale*(16*(Vflo*Vflo))/Rload; //calculates power using P = V^2/R.  Some scaling needed in case of resistive divider to prevent overloading of the analog read function (that's why the 16.0 is there, will likely change later)
  Serial.println(Power);
  return Power;
}

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

void gameMode() {
  receivedData = 0;
  while (true) {
    receiveData();
    if (receivedData == ENDGAME) {
      Serial.println("Game end");
      break;
    }
    else {
      Serial.print("Power read: ");
      Serial.println(receivedData);
      lightGeisel(receivedData, SIM);
    }
    lightGeisel(pollPower(), GEN);
    delay(100);
  }
}

void lightGeisel(int M, int side){ //M is measured power in mW
  // int minPower = 0; //mW
  int maxFloorThreshold = maxPower/numberFloors;
  int index = int(M/(maxFloorThreshold+1));
  int brightness = ((M-(index*201))*1.25);
  if (side == SIM) {
    for(int i = 0; i<index;i++){
      analogWrite(LEDPins_Sim[i], 1023);
    }
    analogWrite(LEDPins_Sim[index], brightness);
    for(int i = index+1; i<numberFloors; i++){
      analogWrite(LEDPins_Sim[i], 0);
    }  
  }
  if (side == GEN) {
    for(int i = 0; i<index;i++){
      analogWrite(LEDPins_Gen[i], 1023);
    }
    analogWrite(LEDPins_Gen[index], brightness);
    for(int i = index+1; i<numberFloors; i++){
      analogWrite(LEDPins_Gen[i], 0);
    }
  }
  Serial.print("Brightness is set at " );
  Serial.println(brightness);
}

void setup() {
  Serial.begin(115200);
  Serial.println("System running.");
  receivedData = START; //delete later
  BTserial.begin(9600);
  pinMode(TX, INPUT_PULLUP); // only needed for JY-MCUY v1.06x
  BTserial.listen();

  BTserial.print("BT GO.."); BTserial.println(millis());

//  initializeSimData();
//  Serial.println(SIM_LIGHTING_STEP);
}

void loop() {
    receiveData();
    if (receivedData == START) {
        Serial.println("Game start");
        gameMode(); 
    }
    delay(100);
}
