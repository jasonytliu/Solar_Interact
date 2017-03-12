/** Solar Interact - UC San Diego ESW 
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair
 * @version 1.0
 * 
 */

/******************Libraries*************************/


/******************Constants*************************/


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

/******************Helpers**************************/

/** Performs any pinMode() definitions normally done in setup()
* @param null
* @return null
*/
void initializePins(){
  pinMode(LED_PIN, OUTPUT);
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

}

