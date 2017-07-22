/** Solar Interact - UC San Diego ESW
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair, JJ Kadifa
 * @version 1.0
 *
 */

/******************Libraries*************************/
#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>

/******************Constants*************************/
#define LVL_1a 3
#define LVL_2a 4
#define LVL_3a 5
#define LVL_4a 6
#define LVL_5a 7
#define LVL_6a 8
#define LVL_1b 9
#define LVL_2b 10
#define LVL_3b 11
#define LVL_4b 12
#define LVL_5b 13
#define LVL_6b 14

#define MAX_POWER 20
#define MAX_ANALOG 100

#define V_RES 1024.0 //resolution of analogRead function
#define V_MAX_READ 5.0 //max voltage Arduino can read with analogRead

#define pinIn A0

#define BTrefreshTime 2000 // (ms)

#define TX 10 //On BT Module //Warning: Some pins can't be used for TX/RX.
#define RX 11 //On BT Module // See SoftSerial lib for details.
/******************Global Variables*******************/

String command = ""; // For BT
SoftwareSerial BTserial(TX, RX); // TX | RX (On BT module)


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
  Serial.println(Power);
  delay(1000);
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
  if (reachTop(LVL_5a) == true) {
    return 1;
  }
//  if (reachTop(LVL_5b) == true) {
//    return 2;
//  }
  else return 0;
}

 /** Controls lighting of LEDs
 * @param power
 * @return
 */
 void ledBrightness(float power, int side) {
   int a = 0;
   if (side == 1) // controls whether the user-controlled or solar-controlled side is lit
     a = LVL_1a;
//   else a = LVL_1b;
   int level = 0; //how many extra levels will be lit
   if (power > MAX_POWER) {
     for (int i = 0; i < 5; i++)
       analogWrite(a+i, MAX_ANALOG);
     return 6;
   }
   power /= (MAX_POWER/5); //scales power to a proportion of the maximum power needed to advance each level of Geisel
   while (power > 1) { //controls how many levels will be lit up
     level++;
     power -= 1;
   }
   int brightness = MAX_ANALOG*power;
   if (level > 0) //sets number of levels - 1 at maximum brightness
     for (int i = 0; i < level; i++)
       analogWrite(a+i, MAX_ANALOG);
   analogWrite(a+level, brightness); //last level will be at proportional brightness 
 }

 int competitivePlay() {
   static float powerUser = 0;
   powerUser += pollPower(userPinIn);
//   static float powerSolar += pollPower(solarPinIn);
   ledBrightness(powerUser, 1);
//   ledBrightness(powerSolar, 2);
   return endGameWin();
 }

/******************Bluetooth*******************/


/** Reads simulated solar data from tablet
 * @param null
 * @return null
 */
void receiveData(){
  if (BTserial.available()) {
    Serial.println("---");
    Serial.println("Fn open..");
    while(BTserial.available()) { // While there is more to be read, keep reading.
      command += (char)BTserial.read();
    }
    Serial.println(command);
    command = ""; // No repeats
    Serial.println("Fn closed.");
    Serial.println("---");
  }
}

/** Prints power measurements to the tablet
 *  @param null
 *  @return null
 */
void sendData(){
  static unsigned long timeKeeper = 0;

  if((unsigned long)(millis() - timeKeeper > BTrefreshTime)){
    BTserial.print("Time elapsed: ");
    BTserial.print(",");
    BTserial.print(millis());
    BTserial.print(";");
    timeKeeper = millis();
  }
}

/** Runs @ startup to check connections
 * @param null
 * @return null
 */
void lightShow(){
  //LED's attached to pins 3-7, 9-13
  analogWrite(3, 5);
  delay(100);
    analogWrite(4,5);
    delay(100);
      analogWrite(5, 5);
      delay(100);
        analogWrite(6, 5);
        delay(100);
          analogWrite(7, 5);
          delay(100);
            analogWrite(9, 5);
            delay(100);
              analogWrite(10, 5);
              delay(100);
                analogWrite(11, 5);
                delay(100);
                  analogWrite(12, 5);
                  delay(100);
                    analogWrite(13, 5);
                    delay(100);

  analogWrite(3, 0);
  delay(100);
    analogWrite(4,0);
    delay(100);
      analogWrite(5, 0);
      delay(100);
        analogWrite(6, 0);
        delay(100);
          analogWrite(7, 0);
          delay(100);
            analogWrite(9, 0);
            delay(100);
              analogWrite(10, 0);
              delay(100);
                analogWrite(11, 0);
                delay(100);
                  analogWrite(12, 0);
                  delay(100);
                    analogWrite(13, 0);
                    delay(100);
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
  pinMode(TX, INPUT_PULLUP); // only needed for JY-MCUY v1.06?
  BTserial.listen();
}

void setup(){
  performStartupSequence();
  // lightShow();
}

void loop(){
//  ledBrightness(power, side);
  receiveData();
  sendData();

}
