#include "TimerOne.h"
#include "LightControl.h"
#include <elapsedMillis.h>
#include <SPI.h>
#include "RF24.h"

#define redLEDPin 5
#define greenLEDPin 6


////////////
int numBalls = 100;
const int numGoals = 2;
int interval = 3000;
////////////

RF24 radio(7,8);

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

LightControl lights(redLEDPin, greenLEDPin);


elapsedMillis timeElapsed;
int liveGoal;

int numHits;
int numMisses;
int timeTaken;

bool timeout;
int time_waiting;

//bool hit[numGoals];
//bool timeExpire[numGoals];
//bool wrongGoal[numGoals];
bool goalStatus = false;
bool sensing;


void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, n_pipes[0]);
  radio.openReadingPipe(2, n_pipes[1]);
  radio.startListening();

  delay(1000);

}

void loop()
{
  //reset stats
  numHits = 0;
  numMisses = 0;
  timeTaken = 0;

  //flash lights to show starting
//  for(int k = 0; k < 5; k++){
//    for(int i = 0; i < numGoals; i++){
//      lights[i].red();
//    }
//    delay(250);
//    for(int i = 0; i < numGoals; i++){
//      lights[i].green();
//    }
//    delay(250);
//  }
//  for(int i = 0; i < numGoals; i++){
//    lights[i].off();
//  }
//  delay(1500);
  Serial.println("Begin");

  //run the drill
  for(int i=1; i<=numBalls; i++){
    lights.red();
    delay(1000);
    lights.green();
    chooseGoal(numGoals);
    Serial.print("Chosen Goal ");
    Serial.print(liveGoal+1);
    Serial.print(": ");
    goalStatus = false;
    timeElapsed = 0;


    //tell goal it is the chosen one
//    radio.openWritingPipe(b_pipes[0]);
//    radio.stopListening();
//    radio.write( &liveGoal, sizeof(liveGoal) );
//    radio.startListening();

    
    //keep reading the goal to see if it is hit
    while (timeElapsed < interval){

      radio.openWritingPipe(b_pipes[liveGoal]);
      radio.stopListening();
      radio.write( &liveGoal, sizeof(liveGoal) );
      radio.startListening();

//      Serial.println("Checking");
      time_waiting = millis();
      timeout = false;
      while(!radio.available() && !timeout){
        if(millis() - time_waiting > 250){
          timeout = true;
//          Serial.println("Timeout");
        }
      }

      if(!timeout){
        radio.read( &goalStatus, sizeof(goalStatus) );
//        Serial.println(goalStatus);
      }
      
      if(goalStatus && sensing){
        Serial.println("HIT");
        sensing = false;
        timeTaken += timeElapsed;
        numHits += 1;
        liveGoal = 2;
        }

        delay(10);
    }

    if(!goalStatus && sensing){
      numMisses += 1;
      Serial.println("MISS (time)");
      sensing = false;
      liveGoal = 2;
    }
    
  }
  printStats();
  delay(5000); 
}

void chooseGoal(int _numGoals)
{
  liveGoal = random(_numGoals);
//  lights[liveGoal].red();
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
