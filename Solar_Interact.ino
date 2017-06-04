/** Solar Interact - UC San Diego ESW 
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair
 * @version 1.0
 * 
 */

/******************Libraries*************************/
#include <SoftwareSerial.h>
#include <Time.h>
#include <TimeLib.h>

/******************Constants*************************/
#define LVL_1a 2
#define LVL_2a 3
#define LVL_3a 4
#define LVL_4a 5
#define LVL_5a 6
#define LVL_1b 8
#define LVL_2b 9
#define LVL_3b 10
#define LVL_4b 11
#define LVL_5b 12

#define MAX_POWER 20
#define MAX_ANALOG 100

#define V_RES 1024.0 //resolution of analogRead function
#define V_MAX_READ 5.0 //max voltage Arduino can read with analogRead

#define pinIn A0

#define BTrefreshTime 10 // (ms)

/******************Global Variables*******************/

int power = 0;

SoftwareSerial BTserial(22, 24); // TX | RX (On BT module) 


/******************Functions*************************/

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

/******************Helpers**************************/

/** Controls lighting of LEDs
* @param power
* @return
*/
int ledBrightness(float power, int side) {
  if (side == 1) // controls whether the user-controlled or solar-controlled side is lit
    int a = LVL_1a;
  else int a = LVL_1b;
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
  return level;
}

/** Reads simulated solar data from tablet 
 * @param null
 * @return null
 */
void readFromTablet(){
    static unsigned long timeKeeper = millis();
//    power = BTserial.parseInt();
    if((unsigned long)(millis() - timeKeeper > 1000) || BTserial.available() > 0){ 
      power = BTserial.parseInt();
      Serial.print("Read from phone... Power: ");
      Serial.println(power);
      timeKeeper = millis();
    }


}

/** Prints power measurements to the tablet
 *  @param null
 *  @return null
 */
void printToTablet(){
  static unsigned long timeKeeper = 0;

  if((unsigned long)(millis() - timeKeeper > BTrefreshTime)){ 
      
//    BTserial.print("Solar Interact");
//    
//    BTserial.print(",");
//    
//    BTserial.print("Sending Power");
//    
//    BTserial.print(",");
    
    BTserial.print("Time elapsed: ");
    
    BTserial.print(",");
    
    BTserial.print(millis());
    
    BTserial.print(";");
  
    timeKeeper = millis();
  }
}


/** Ensures that all LED's are working
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

/******************Helpers**************************/

/** Performs any pinMode() definitions normally done in setup()
* @param null
* @return null
*/
void initializePins(){
  pinMode(LVL_1, OUTPUT);
  pinMode(LVL_2, OUTPUT);
  pinMode(LVL_3, OUTPUT);
  pinMode(LVL_4, OUTPUT);
  pinMode(LVL_5, OUTPUT);
}

/** Performs any initializations normally done in setup()
* @param null
* @return null
*/
void performStartupSequence(){
  BTserial.begin(9600); 
  Serial.begin(115200);
  Serial.println("Initialized");
}

/** Checks that a portion of code is not stuck in loop
 *  @param null
 *  @return null
 */
void checkInfiniteLoop(){
  int checkWorking=1000;
  static unsigned long lastCheck = 0;

   if((unsigned long)(millis() - lastCheck > checkWorking)){
    Serial.println("Not stuck in loop.. "); 
   }
}


/***************Main setup and loop******************/
void setup(){
  initializePins();
  performStartupSequence();
  lightShow();
}

void loop(){
//  ledBrightness(power, side);
  readFromTablet();
  printToTablet();
//  checkInfiniteLoop();
}
