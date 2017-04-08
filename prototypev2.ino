#define irLEDPin      9
#define irSensorPin   2
#define redLEDPin     5
#define greenLEDPin   4
#define vibrationPin  3

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
int numAlmost;
int timeTaken;

LightControl light1(redLEDPin, greenLEDPin);
LightControl lights[numGoals] = light1;

bool hit[numGoals];
bool post[numGoals];
bool sensing[numGoals];


void setup() {
  pinMode(irLEDPin, OUTPUT);
  pinMode(irSensorPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(irSensorPin), goalState, FALLING);
  attachInterrupt(digitalPinToInterrupt(vibrationPin), hitPost, RISING);
  
  Timer1.initialize(26); //26 us is 38 kHz
  Timer1.pwm(irLEDPin, 512);

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
    lights[0].red();
    delay(250);
    lights[0].green();
    delay(250);
  }
  lights[0].off();
  delay(1500);
  Serial.println("Begin");

  //run the drill
  for(int i=1; i<=numBalls; i++){
    delay(1000);
    chooseGoal(numGoals);
    hit[liveGoal] = false;
    post[liveGoal] = false;
    timeElapsed = 0;
    
    while (timeElapsed < interval){
      if(hit[liveGoal] && sensing[liveGoal]){
        Serial.println("HIT");       //only works if it prints something in this loop, not sure why
        sensing[liveGoal] = false;
        lights[liveGoal].green();
        delay(1000);
        lights[liveGoal].off();
        timeTaken += timeElapsed;
        numHits += 1;
        }
      }

    //if miss but hit post
    if(!hit[liveGoal] && post[liveGoal]){
      numAlmost += 1;
      Serial.println("ALMOST!");
      for(int j = 0; j < 6; j++){     //flash lights to show miss
        lights[liveGoal].red();
        delay(125);
        lights[liveGoal].off();
        delay(125);
      }
    }
    
    // if completely misses
    else{
      numMisses += 1;
      Serial.println("MISS");
      for(int j = 0; j < 3; j++){     //flash lights to show miss
        lights[liveGoal].red();
        delay(250);
        lights[liveGoal].off();
        delay(250);
      }
    }
  }
  printStats();
  delay(5000); 
}

void goalState(){
  hit[liveGoal] = true;
}

void hitPost(){
  post[liveGoal] = true;
}

void chooseGoal(int _numGoals)
{
  liveGoal = random(_numGoals);
  lights[liveGoal].red();
  sensing[liveGoal] = true;
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


