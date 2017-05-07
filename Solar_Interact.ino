/** Solar Interact - UC San Diego ESW 
 * @authors: Jason Liu, Kelly Levick, Jon Bradshaw, Mukund Nair
 * @version 1.0
 * 
 */

/******************Libraries*************************/


/******************Constants*************************/
#define LVL_1 2
#define LVL_2 3
#define LVL_3 4
#define LVL_4 5
#define LVL_5 6

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
/******************Helpers**************************/

/** Controls lighting of LEDs
* @param power
* @return
*/
void ledBrightness(float power) {
  int a = LVL_1;
  int level = 0; //how many extra levels will be lit
  power /= (MAX_POWER/5); //scales power to a proportion of the maximum power needed to advance each level of Geisel
  while (power > 1) { //controls how many levels will be lit up
    level++;
    power -= 1;
  }
  int brightness = MAX_ANALOG*power;
  if (level > 0) //sets number of levels - 1 at maximum brightness
    for (i = 0; i < level; i++)
      analogWrite(a+i, MAX_ANALOG);
  analogWrite(a+level, brightness); //last level will be at proportional brightness
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

