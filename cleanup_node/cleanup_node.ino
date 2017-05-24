#define irLEDPin 9
#define irSensorPin 2

#define redLEDPin 5
#define greenLEDPin 6


#include "TimerOne.h"
#include "LightControl.h"
#include <elapsedMillis.h>
#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

int goalNumber = 2;

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

LightControl lights(redLEDPin, greenLEDPin);

bool goalStatus = false;
bool chosen = false;
bool goal = false;
bool signalLights = false;

unsigned long time_waiting;
unsigned long start_wait;
unsigned long goalTime;

unsigned long time_wait;
bool timeout;
int liveGoal;

int count;
int interval = 3000;


void setup() {
  pinMode(irLEDPin, OUTPUT);
  pinMode(irSensorPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(irSensorPin), goalState, FALLING);
  
  Timer1.initialize(26); //26 us is 38 kHz
  Timer1.pwm(irLEDPin, 512);

  Serial.begin(115200);
  delay(1000);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, b_pipes[goalNumber-1]);
  radio.startListening();

  delay(500);
  Serial.print("Begin ");
  Serial.println(goalNumber);
  lights.off();
}

void loop(){
  if(!chosen){
//    Serial.println("not chosen");
    lights.off();
//    time_wait = millis();
//    timeout = false;
    while(!radio.available()){        //might not need timeout checker
//      if(millis() - time_wait > 250){
//        timeout = true;
//        Serial.println("timeout");
//      }
    }

//    if(!timeout){
//      while(radio.available()){
        radio.read( &chosen, sizeof(chosen) );
//      }
//    }
  }
    

  if(chosen){
    start_wait = millis();
    goalTime = 0;
    lights.red();

    while(!goalStatus && (millis() - start_wait < interval)){
      //
    }
    goalTime = millis() - start_wait;

    
      radio.stopListening();
      radio.openWritingPipe(n_pipes[goalNumber-1]);
      while( !radio.write( &goalTime, sizeof(goalTime) )){
          //makes sure that it gets sent
      }
      
      signalLights = true;
      radio.startListening();
//      goalStatus = false;
   }

     
  while(!radio.available()){}
 
  radio.read( &chosen, sizeof(chosen) );

  if(signalLights){
    if(goalStatus){flashHit();}
    else{flashMiss();}
  }

  goalStatus = false;
  signalLights = false;    
}

void goalState(){        //probably best to create goal objects
  if(chosen){
    goalStatus = true;
  }
}

void flashHit(){
  lights.green();
  delay(2000);
  lights.off();
}

void flashMiss(){
  for(int i=0; i<3; i++){
    lights.red();
    delay(500);
    lights.off();
    delay(500);
  }
}

