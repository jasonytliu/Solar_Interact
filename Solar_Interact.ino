/** Solar Interact - UC San Diego ESW 
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair
 * @version 1.0
 * 
 */

/******************Libraries*************************/


/******************Constants*************************/
#define R_PIN 10
#define G_PIN 9
#define B_PIN 11

#define MAX_POWER 20
#define MAX_ANALOG 255

#define pinIn A0

/******************Functions*************************/

float pollPower(int pinIn){
  int Vint;
  float Power;
  float Vflo;
  Vint = analogRead(pinIn);
  Vflo = 5.0*(Vint/1024.0);
  Power = 1000*(16.0*(Vflo*Vflo))/20000.0;
  return Power;
}

void ledBrightness(float power) {
  power /= MAX_POWER;
  int brightness = MAX_ANALOG*power;
  analogWrite(R_PIN, brightness);
  analogWrite(G_PIN, brightness);
  analogWrite(B_PIN, brightness);
}

/******************Helpers**************************/

/** Performs any pinMode() definitions normally done in setup()
* @param null
* @return null
*/
void initializePins(){
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
}

/** Performs any initializations normally done in setup()
* @param null
* @return null
*/
void performStartupSequence(){
  ;
}

/***************Main setup and loop******************/
void setup(){
  initializePins();
  performStartUpSequence();
}

void loop(){
  ledBrightness();
}

