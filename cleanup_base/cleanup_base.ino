#include "TimerOne.h"
#include <elapsedMillis.h>
#include <SPI.h>
#include "RF24.h"
#include <Wire.h>

////////////////////////
// System Definitions //
////////////////////////
int numBalls = 15;          //total number of balls
const int numGoals = 2;     //total number of targets
const int interval = 3000;  //max time to hit target

///////////////////////
// Radio Definitions //
///////////////////////
RF24 radio(7,8);
const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};
const int max_wait = 3500;  //max time to wait for radio comm

//////////////////////
// Data Definitions //
/////////////////////
int sessionNumber = 0;
int hit[numGoals] = {0};
int target[numGoals] = {0};
unsigned long timeTaken[numGoals] = {0.0};

//////////////////////
// Temp Definitions //
/////////////////////
int liveGoal;
unsigned long start_wait;
unsigned long goalTime;
bool timeout;
bool start = false;
bool goalStatus = false;
bool sensing;


void setup() {
  Serial.begin(115200);

  // initialize radio comm with target nodes
  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, n_pipes[0]);
  radio.openReadingPipe(2, n_pipes[1]);
  radio.startListening();

  
//  randomSeed(analogRead(0));

  // setup for wire comm. to esp8266
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  delay(1000);

}

void loop()
{

  while(!start){//wait for cue from app
  }

  resetStats();
  
  Serial.println("Begin Base");

  //run the drill
  for(int i=0; i<numBalls; i++){
    delay(2500);
    chooseGoal(numGoals);
    Serial.print("Chosen Goal ");
    Serial.print(liveGoal+1);
    Serial.print(": ");

    // store data for target chosen
    target[i] = liveGoal+1;

    while (sensing){

      goalStatus = false;

      //inform chosen node to start listening
      radio.openWritingPipe(b_pipes[liveGoal]);
      radio.stopListening();
      radio.write(&sensing, sizeof(sensing));
      radio.startListening();

      //start timeout timer
      start_wait = millis();
      timeout = false;

      //if no reply from node and not yet timout
      while(!radio.available() && !timeout){
        
        //check for timeout
        if(millis() - start_wait > max_wait){
          timeout = true;
        }
      }

      radio.read( &goalTime, sizeof(goalTime));
      sensing = false;

      delay(10);
    }

    //inform chosen node to stop listening
    radio.openWritingPipe(b_pipes[liveGoal]);
    radio.stopListening();
    radio.write( &sensing, sizeof(sensing) );
    radio.startListening();

    delay(10);

    if(timeout){
      Serial.println("No Communication");
    }
    
    else{
      Serial.println(goalTime);
      timeTaken[i] = goalTime;

      //if time taken is less then interval, record as hit
      if(goalTime <= interval){hit[i] = 1;}
      //else record as miss
      else{hit[i]=0;}
    }

    timeout = false;
  }
  
  start = false;
  delay(1000); 
}


void chooseGoal(int _numGoals){
  liveGoal = random(_numGoals);
  sensing = true;
}

void resetStats(){
  numHits = 0;
  numMisses = 0;
  timeTaken = 0;
  hit[numGoals] = {0};
  target[numGoals] = {0};
  timeTaken[numGoals] = {0.0};
}

// function called when receive wire event from esp8266
void receiveEvent(byte command){
  if (command = 1){
    // begin session
    start = true;
  }
}

// function called when receive a request event from esp8266
void requestEvent(){
  sessionNumber += 1;
  Wire.write(sessionNumber);
  
  for(int i=0; i<numBalls; i++){
    Wire.write(target[i]);
  }
  for(int i=0; i<numBalls; i++){
    Wire.write(hit[i]);
  }
  for(int i=0; i<numBalls; i++){
    Wire.write(int(timeTaken[i]);
  }
}

