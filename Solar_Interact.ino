#include <Arduino.h>
#include <SoftwareSerial.h>

#define TX 50
#define RX 51

#define START   100
#define ENDGAME 10000

#define MAX_POWER 5000
#define BTrefreshTime 2000 // (ms)

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
    }
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("System running.");

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
