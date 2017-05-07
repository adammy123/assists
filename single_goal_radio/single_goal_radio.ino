#define irLED_1_Pin 9
#define irSensor_1_Pin 2

#define redLED_1_Pin 5
#define greenLED_1_Pin 4

#include "TimerOne.h"
#include "LightControl.h"
#include <elapsedMillis.h>

////////////
int numBalls = 5;
const int numGoals = 1;
int interval = 5000;
////////////

elapsedMillis timeElapsed;
int liveGoal;

int numHits;
int numMisses;
int timeTaken;

LightControl light1(redLED_1_Pin, greenLED_1_Pin);
LightControl lights[numGoals] = {light1};


bool hit[numGoals];
bool timeExpire[numGoals];
bool wrongGoal[numGoals];
bool sensing;


void setup() {
  pinMode(irLED_1_Pin, OUTPUT);
  pinMode(irSensor_1_Pin, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(irSensor_1_Pin), goalState1, FALLING);
  
  Timer1.initialize(26); //26 us is 38 kHz
  Timer1.pwm(irLED_1_Pin, 512);

  Serial.begin(9600);
}

void loop()
{
  //reset stats
  numHits = 0;
  numMisses = 0;
  timeTaken = 0;

  //flash lights to show starting
  for(int k = 0; k < 5; k++){
    for(int i = 0; i < numGoals; i++){
      lights[i].red();
    }
    delay(250);
    for(int i = 0; i < numGoals; i++){
      lights[i].green();
    }
    delay(250);
  }
  for(int i = 0; i < numGoals; i++){
    lights[i].off();
  }
  delay(1500);
  Serial.println("Begin");

  //run the drill
  for(int i=1; i<=numBalls; i++){
    delay(1000);
    chooseGoal(numGoals);
    Serial.print("Goal ");
    Serial.print(liveGoal+1);
    Serial.print(": ");
    hit[liveGoal] = false;
    timeExpire[liveGoal] = false;
    wrongGoal[liveGoal] = false;
    timeElapsed = 0;
    
    while (timeElapsed < interval){
      if(hit[liveGoal] && sensing){
        Serial.println("HIT");
        sensing = false;
        lights[liveGoal].green();
        delay(1000);
        lights[liveGoal].off();
        timeTaken += timeElapsed;
        numHits += 1;
        }else if(wrongGoal[liveGoal] && sensing){
          numMisses += 1;
          Serial.println("MISS (wrong goal)");
          sensing = false;
          for(int j = 0; j < 3; j++){     //flash lights to show miss
            for(int i = 0; i < numGoals; i++){
              lights[i].red();
            }  
            delay(250);
            for(int i = 0; i < numGoals; i++){
              lights[i].off();
            } 
            delay(250);
          }
        }
      }
    if(!hit[liveGoal] && !wrongGoal[liveGoal] && sensing){
      numMisses += 1;
      Serial.println("MISS (time)");
      sensing = false;
      for(int j = 0; j < 3; j++){     //flash lights to show miss
        for(int i = 0; i < numGoals; i++){
          lights[i].red();
        } 
        delay(250);
        for(int i = 0; i < numGoals; i++){
          lights[i].off();
        } 
        delay(250);
      }
    }
  }
  printStats();
  delay(5000); 
}

void goalState1(){        //probably best to create goal objects
  if(liveGoal == 0){
    hit[liveGoal] = true;
  }else{
    wrongGoal[liveGoal] = true;
  }
}

void chooseGoal(int _numGoals)
{
  liveGoal = random(_numGoals);
  lights[liveGoal].red();
  sensing = true;
}

void printStats(){
  Serial.println("\n-----Drill Stats-----");
  
  Serial.print("Hits: ");
  Serial.println(numHits);
  
  Serial.print("Misses: ");
  Serial.println(numMisses);
  
  Serial.print("Average time for completed passes: ");
  Serial.print((float)timeTaken/numHits/1000);
  Serial.println(" seconds");

  Serial.println();
}

