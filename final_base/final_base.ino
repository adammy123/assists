#include "TimerOne.h"
#include <elapsedMillis.h>
#include <SPI.h>
#include "RF24.h"
#include <Wire.h>

bool serialMode = false;  // switch to true to enable debugging mode using a computer
                         // and without the app

////////////////////////
// System Definitions //
////////////////////////
const int numBalls = 10;     //total number of balls
const int numGoals = 4;     //total number of targets
const int interval = 3000;  //max time to hit target
const unsigned long ballFeedRate = 3500;

///////////////////////
// Radio Definitions //
///////////////////////
RF24 radio(7,8);
const uint64_t b_pipes[4] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL, 0x0F0F0F0F33LL, 0x0F0F0F0F44LL};  
const uint64_t n_pipes[4] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL, 0x1F1F1F1F33LL, 0x1F1F1F1F44LL};
const int max_wait = 3500;  //max time to wait for radio comm
const bool targetResponseCheck = true;
unsigned long targetResponseCheckDump;
byte targetCheck[numGoals];

//////////////////////
// Data Definitions //
/////////////////////
byte sessionNumber;
byte hit[numBalls];
byte target[numBalls];
byte timeTakenByte[2];
unsigned long timeTaken[numBalls];
bool dataToSend = true;

//////////////////////
// Temp Definitions //
//////////////////////
int liveGoal;
unsigned long start_wait;
unsigned long goalTime;
bool timeout;
bool start = false;
bool _ready = false;
bool goalStatus = false;
bool sensing = false;
bool commandToReceive = true;

///////////////////////
// Error Definitions //
///////////////////////
bool isError = false;
bool fatalError = false;
int targetWithError = -1;
byte errorByte = 0;

void setup() {
  Serial.begin(9600);

  // initialize radio comm with target nodes
  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  for(int i=0; i<numGoals; i++){
    radio.openReadingPipe((i+1), n_pipes[i]);
  }

  
  randomSeed(analogRead(0));

  // setup for wire comm. to esp8266
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  delay(100);

}

void loop()
{
  // for debugging purposes: use Serial Moniter input
  //                         to start session instead of app
  if(serialMode){
    Serial.println ("Hit a key to ready");
    while(Serial.available() == 0){}
    byte temp = Serial.read();
  }

  // wait for cue from app
  else{
    Serial.println("waiting for ready signal from app");
    while(!_ready){delay(100);}
  }

  checkTargetComm();

  if(serialMode){
    Serial.println ("Hit a key to start");
    while(Serial.available() == 0){}
    byte temp = Serial.read();
  }
  //wait for cue from app
  else{
    Serial.println("waiting for start signal from app");
    while(!start){
    delay(100);
    }
  }
  
  resetStats();
  if(serialMode) Serial.println("Begin Base");

  //run the drill
  goalTime = 0;
  for(int i=0; i<numBalls; i++){
    
    delay(abs(ballFeedRate - goalTime));
    
    liveGoal = chooseGoal(numGoals);
    if(isError && liveGoal == targetWithError){
      liveGoal = (liveGoal%(numGoals-1)) + 1;
    }

    Serial.print("#");
    Serial.print(i+1);
    Serial.print("     Chosen Goal ");
    Serial.print(liveGoal+1);
    Serial.print(": ");

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

      // check if this is first error
      if(!isError){
        targetWithError = liveGoal;
        errorByte = (byte)(liveGoal+1);
        isError = true;
        goalTime = ballFeedRate;
      }

      // if there is more than one error
      // stop the session
      else{
        fatalError = true;
        errorByte = 100;
//        i = 10;
      }
    }
    
    else{
      Serial.println(goalTime);
      timeTaken[i] = (int)goalTime;

      //if time taken is less then interval, record as hit
      if(goalTime < interval){hit[i] = 1;}
      //else record as miss
      else{hit[i]=0;}
    }
    target[i] = liveGoal+1;
    timeout = false;
  }
  
  start = false;
  _ready = false;
  delay(1000); 
  
}


int chooseGoal(int _numGoals){
  sensing = true;
  return random(_numGoals);
}

void checkTargetComm(){
  
  for(int i=0; i<numGoals; i++){
    radio.openWritingPipe(b_pipes[i]);
    radio.stopListening();
    radio.write(&targetResponseCheck, sizeof(targetResponseCheck));
    radio.startListening();
    delay(10);
    start_wait = millis();
    timeout = false;
    
    while(!radio.available() && !timeout){  
      //check for timeout
      if(millis() - start_wait > max_wait){
        timeout = true;
      }
    }
    
    radio.read( &targetResponseCheckDump, sizeof(targetResponseCheckDump));
    
    if(timeout){
      targetCheck[i] = 0;
      isError = true;
      targetWithError = i;
      errorByte = (byte)(i+1);
    }
    
    else{
      targetCheck[i] = 1;
    }
    
    //inform chosen node to stop listening
    radio.openWritingPipe(b_pipes[i]);
    radio.stopListening();
    radio.write( &sensing, sizeof(sensing) );
    Serial.print(targetCheck[i]);
    delay(200);
  }
  
}


// function called when receive wire event from esp8266
void receiveEvent(byte command){

  if(commandToReceive){
    _ready = true;
    isError = false;
    targetWithError = -1;
    errorByte = 0;
  }
  
  else{
    start = true;
  }
  commandToReceive = !commandToReceive;
}

// function called when receive a request event from esp8266
void requestEvent(){

  // send target status results:
  if(_ready){
    Wire.write(targetCheck, numGoals);
  }

  // send session results:
  else{
    // because I2C buffer only hold 32 bytes, we send the data in two parts
    // send first half of data
    if(dataToSend){
      sessionNumber += 1;
      Wire.write(sessionNumber);
      Wire.write(errorByte);
      
      for(int j=0; j<numBalls; j++){
        Wire.write(target[j]);
      }
      for(int j=0; j<numBalls; j++){
        Wire.write(hit[j]);
      }
    }
  
    // send second half of data
    else{
      for(int j=0; j<numBalls; j++){
        timeTakenByte[0] = (timeTaken[j] >> 8) & 0xFF;
        timeTakenByte[1] = timeTaken[j] & 0xFF;
        Wire.write(timeTakenByte, 2);
      }
    }
  
    dataToSend = !dataToSend;
  }
  
  while(Wire.available()){
    byte dump = Wire.read();
  }
}

void resetStats(){
  for(int i=0; i<numBalls; i++){
    hit[i] = 0;
    target[i] = 0;
    timeTaken[i] = 0;
    _ready = false;
  }
  Serial.println("Stats have been reset.");
}

