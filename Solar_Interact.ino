/** Solar Interact - UC San Diego ESW 
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair
 * @version 1.0
 * 
 */

/******************Libraries*************************/


/******************Constants*************************/


/******************Functions*************************/

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

