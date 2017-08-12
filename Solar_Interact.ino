/** Solar Interact - UC San Diego ESW
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair, JJ Kadifa
 * @version 1.0
 *
 */

/******************Libraries*************************/
#include <Arduino.h>
#include <SoftwareSerial.h>
//#include <Time.h>
//#include <TimeLib.h>

/******************Constants*************************/
#define LVL_1PLAY 2
#define LVL_2PLAY 3
#define LVL_3PLAY 4
#define LVL_4PLAY 22
#define LVL_5PLAY 22
#define LVL_6PLAY 22

#define LVL_1SIM 8
#define LVL_2SIM 9
#define LVL_3SIM 10
#define LVL_4SIM 22
#define LVL_5SIM 22

#define MAX_POWER 15 //Originally 20
#define MAX_ANALOG 20

#define V_RES 1024.0 //resolution of analogRead function
#define V_MAX_READ 5.0 //max voltage Arduino can read with analogRead

#define VOLTAGE_DIVIDER A0 //Power measurement

#define BTrefreshTime 2000 // (ms)

#define TX 50 //On BT Module //Warning: Some pins can't be used for TX/RX.
#define RX 51 //On BT Module // See SoftSerial lib for details.


//Below are bluetooth communication codes
//TODO: Ensure power measurements do not conflict with below
#define IDLE 99
#define MODE_1 1
#define MODE_2 2
#define ACK_MODE_1 1 //Acknowledges game mode received
#define ACK_MODE_2 2 //             "
#define ACK_DATA 3 //Acknowledges data received
#define STORE_ARRAY 3
#define START 11
#define END 22

const char *PLAYER = "PLAYER";
const char *SIMDATA = "SIMDATA";

#define POLL_POWER_RATE 500//in millis. Also controls LED update rate

//BT
String incomingData = ""; // For BT
SoftwareSerial BTserial(TX, RX); // TX | RX (On BT module)

//Game
#define TOTAL_GAME_TIME 60 //seconds // Sync w/ Tablet
#define SIZE_SIM_DATA 120            //Sync w/ Tablet
unsigned long SIM_LIGHTING_STEP = 500;
 // Formula: (TOTAL_GAME_TIME/SIZE_SIM_DATA)*1000; //in millis
float simData[SIZE_SIM_DATA]; //-1 Represents empty
int STATUS = 99; //Start Arduino on idle
boolean GAME_START = false; //Controls when SimData lighting begins

/******************Power*************************/

float pollPower(int pinIn){
  int Vint;
  float Power;
  float Vflo;
  float powerScale = 1000.0; // scalar to make power unit more readable for the user (1000 corresponds to an output in mW)
  float Rload = 20000.0; // load resistance being driven by the generator
  Vint = analogRead(pinIn); //returns integer between 0 (0V) and 1024 (5V) corresponding to voltage at pinIn
  Vflo = V_MAX_READ*(Vint/V_RES); //converts Vint into a float equal to the actual voltage at the node
  Power = powerScale*(16.0*(Vflo*Vflo))/Rload; //calculates power using P = V^2/R.  Some scaling needed in case of resistive divider to prevent overloading of the analog read function (that's why the 16.0 is there, will likely change later)
  return Power;
}

/******************LED Control**************************/

bool reachTop(int pinIn) {
  int a = analogRead(pinIn);
  if (a == 1023) { //we will need to monitor this for errors and adjust
    return true;
  }
  else return false;
}

int endGameWin() {
  if (reachTop(LVL_5PLAY) == true) {
    return 1;
  }
//  if (reachTop(LVL_5SIM) == true) {
//    return 2;
//  }
  else return 0;
}

/** Controls lighting of LEDs
 * @param power
 * @return
 */
 void ledBrightness(float power, int side) { //TODO: Works in increasing power, but not in decreasing
     int a = LVL_1SIM;
     if (side == 1) // controls whether the user-controlled or solar-controlled side is lit
       a = LVL_1PLAY;
  //   else a = LVL_1SIM;
     int level = 0; //how many extra levels will be lit
     if (power > MAX_POWER) {
       for (int i = 0; i < 5; i++)
         analogWrite(a+i, MAX_ANALOG);
       return 6; //TODO: Why 6?
     }
     power /= (MAX_POWER/5); //scales power to a proportion of the maximum power needed to advance each level of Geisel
     while (power > 1) { //controls how many levels will be lit up
       level++;
       power -= 1;
     }
     int brightness = MAX_ANALOG*power; // 200 is basically completely on and 70 is low power, 200-70 is the range of brightness, we will change the numbers into variables
     if (level > 0) //sets number of levels - 1 at maximum brightness
     int i = 0;
       for (int i = 0; i < level; i++) {
         analogWrite(a+i, MAX_ANALOG);
       }
      int j = 0;
       for (int j = level+a; j < (a+6); j++) {
        analogWrite(j, 0);
       }
     analogWrite(a+level, brightness); //last level will be at proportional brightness
 }

void lightItUp(const char *side){
  static unsigned long lastCallPlayer = 0;
  static unsigned long lastCallSimData = 0;
  static int dataPtCounter = 0; //To cycle through SimData

  if(strcmp(side,"PLAYER") == 0){ //Chooses which data to use to light up Geisel

    if((unsigned long)(millis() - lastCallPlayer) > POLL_POWER_RATE){
      ledBrightness(pollPower(VOLTAGE_DIVIDER), 1);
      Serial.print(pollPower(VOLTAGE_DIVIDER));
      Serial.print("    ");
      Serial.print(millis());
      Serial.print("    ");
      Serial.print(POLL_POWER_RATE);
      Serial.print("    ");
      lastCallPlayer = millis();
      Serial.println(lastCallPlayer);
    }

  }else if(strcmp(side,"SIMDATA") == 0){
    if(GAME_START){ //Won't start lighting until flag GAME_START set to true
        if((unsigned long)(millis() - lastCallSimData) > SIM_LIGHTING_STEP){
          ledBrightness(simData[dataPtCounter], 2);
          lastCallSimData = millis();
          ++dataPtCounter;

          if(dataPtCounter == SIZE_SIM_DATA){ //Auto-resets after game ends
            dataPtCounter = 0;
          }
        }
      }

  }
}
//  int competitivePlay() {
//    static float powerUser = 0;
//    powerUser += pollPower(userPinIn);
// //   static float powerSolar += pollPower(solarPinIn);
//    ledBrightness(powerUser, 1);
// //   ledBrightness(powerSolar, 2);
//    return endGameWin();
//  }


/******************Bluetooth*******************/

/** Store the next set of data into array (simData)
  *   via while() (won't break until array completed)
  * @param null
  * @return null
  */
void storeArray(){
  int counter = 0;
  while(counter < SIZE_SIM_DATA){
    if(BTserial.available()){
        Serial.println("---- storeArray()");
        Serial.println("Receving:");
        incomingData = ""; //Good habit
        while(BTserial.available()){ // While there is more to be read, keep reading.
          incomingData += (char)BTserial.read();
        }
        Serial.println(incomingData);
        simData[counter] = incomingData.toInt();
        counter++;

        incomingData = ""; //No repeats
    }
  }

  for(int a = 0; a < SIZE_SIM_DATA; a++){
      Serial.print("|");
      Serial.print(simData[a]);
      if(a == SIZE_SIM_DATA-1){
        Serial.print("|");
      }
  }
}
/** To tablet
 *  @param data 0 for STATUS; Otherwise, send int data
 *  @return null
 */
void send(int data){
  static unsigned long timeKeeper = 0;

  if(data == 0){
    if((unsigned long)(millis() - timeKeeper) > BTrefreshTime){
      if(STATUS == IDLE){
        BTserial.print(IDLE); //99 for Idle
        BTserial.print("\n");
      }else if(STATUS == MODE_1){
        BTserial.print(MODE_1);
        BTserial.print("\n");
      }else if(STATUS == MODE_2){
        BTserial.print(MODE_2);
        BTserial.print("\n");
      }
      timeKeeper = millis();
      Serial.println("---- send(" + String(data) + ")");
    }
  }else{
    BTserial.print(data);
    BTserial.print("\n");
  }
}
/** Assigns meaning to incoming data
  * @param data Received #
  * @return null
  */
void decipher(int data){
  Serial.print("---- decipher() "); Serial.println(millis());
  switch(data){
    case MODE_1:
      STATUS = MODE_1;
      Serial.println("Mode 1 entered");
      send(ACK_MODE_1);
      break;
    case MODE_2:
      STATUS = MODE_2;
      Serial.println("Mode 2 entered");
      send(ACK_MODE_2);
      break;
    case STORE_ARRAY:
      storeArray();
      send(ACK_DATA);
      break;
    case START:
      send(START);
      GAME_START = true;
      break;
    case END:
      send(END);
      GAME_START = false;
      break;
    default:
      Serial.print(data); Serial.println(" is not valid.");
  }
}

/** From tablet
 * @param null
 * @return null
 */
void receiveData(){
  if(BTserial.available()){
      Serial.print("---- receiveData() "); Serial.println(millis());
      Serial.println("Receving:");
      while(BTserial.available()){ // While there is more to be read, keep reading.
        incomingData += (char)BTserial.read();
      }
      Serial.println(incomingData);

      decipher(incomingData.toInt());
  }
  incomingData = ""; //No repeats
}

/** Runs @ startup to check connections
 * @param null
 * @return null
 */
void lightShow(){

  analogWrite(LVL_1PLAY, MAX_ANALOG);
  delay(100);
    analogWrite(LVL_2PLAY, MAX_ANALOG);
    delay(100);
      analogWrite(LVL_3PLAY, MAX_ANALOG);
      delay(100);
        analogWrite(LVL_4PLAY, MAX_ANALOG);
        delay(100);
          analogWrite(LVL_5PLAY, MAX_ANALOG);
          delay(100);
            analogWrite(LVL_1SIM, MAX_ANALOG);
            delay(100);
              analogWrite(LVL_2SIM, MAX_ANALOG);
              delay(100);
                analogWrite(LVL_3SIM, MAX_ANALOG);
                delay(100);
                  analogWrite(LVL_4SIM, MAX_ANALOG);
                  delay(100);
                    analogWrite(LVL_5SIM, MAX_ANALOG);
                    delay(100);

  analogWrite(LVL_1PLAY, 0);
  delay(100);
    analogWrite(LVL_2PLAY,0);
    delay(100);
      analogWrite(LVL_3PLAY, 0);
      delay(100);
        analogWrite(LVL_4PLAY, 0);
        delay(100);
          analogWrite(LVL_5PLAY, 0);
          delay(100);
            analogWrite(LVL_1SIM, 0);
            delay(100);
              analogWrite(LVL_2SIM, 0);
              delay(100);
                analogWrite(LVL_3SIM, 0);
                delay(100);
                  analogWrite(LVL_4SIM, 0);
                  delay(100);
                    analogWrite(LVL_5SIM, 0);
                    delay(100);
  Serial.println("lightShow()");
}

/* Debug use: Fills simData set with random numbers.
*
*/
void initializeSimData(){
  int maxPowerRead = 20; //Depends on max power read from pollPower()
  //Create random data set between 0 and MAX_ANALOG
  // for(int a = 0 ;  a < SIZE_SIM_DATA ; a++){
  // 	simData[a] = random(0,MAX_ANALOG);
  // }
  //Creates data set increasing then decreasing
  for(int a = 0 ;  a < SIZE_SIM_DATA ; a++){
    simData[a] = a*0.166;
    // if(a >= (int)(SIZE_SIM_DATA/2)){
    //   simData[a] = SIZE_SIM_DATA - a;
    // }
  }

  Serial.print("simData:");
  for(int a = 0; a < SIZE_SIM_DATA ; a++){
    Serial.print(" ");
    Serial.print(simData[a]);
    if(a == (SIZE_SIM_DATA -1)){
      Serial.println(".");
    }
  }



}
/***************Main setup and loop******************/

/** Performs any initializations normally done in setup(). Include pinMode() here
* @param null
* @return null
*/
void performStartupSequence(){
  Serial.begin(115200);
  Serial.println("System running.");

  BTserial.begin(9600);
  pinMode(TX, INPUT_PULLUP); // only needed for JY-MCUY v1.06x
  BTserial.listen();

  BTserial.print("BT GO.."); BTserial.println(millis());

  initializeSimData();
  Serial.println(SIM_LIGHTING_STEP);
}

void setup(){
  performStartupSequence();
  lightShow();
}

void loop(){
  receiveData();
  send(0);
  lightItUp(PLAYER);
  lightItUp(SIMDATA);
  // Serial.println(pollPower(VOLTAGE_DIVIDER));
}
