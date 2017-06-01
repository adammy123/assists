#define irLEDPin 9
#define irSensorPin 2
#define redLEDPin 5
#define greenLEDPin 6

#include "TimerOne.h"
#include "LightControl.h"
#include <elapsedMillis.h>
#include <SPI.h>
#include "RF24.h"

////////////////////////
// System Definitions //
////////////////////////
const int goalNumber = 3;
const int interval = 3000;

///////////////////////
// Radio Definitions //
///////////////////////
RF24 radio(7,8);
const uint64_t b_pipes[4] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL, 0x0F0F0F0F33LL, 0x0F0F0F0F44LL};  
const uint64_t n_pipes[4] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL, 0x1F1F1F1F33LL, 0x1F1F1F1F44LL};
<<<<<<< Updated upstream

=======
                                                                                                                                                                                                                
>>>>>>> Stashed changes
//////////////////////
// Temp Definitions //
//////////////////////
bool goalStatus = false;
bool chosen = false;
bool signalLights = false;
unsigned long start_wait;
unsigned long goalTime;

//////////////////////////
// Lighting Definitions //
//////////////////////////
LightControl lights(redLEDPin, greenLEDPin);

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

  lights.flash(1000, 2);
}

void loop(){
  
  if(!chosen){
    lights.off();
    while(!radio.available()){delay(10);}
    radio.read( &chosen, sizeof(chosen) );
  }

  
  if(chosen){
    start_wait = millis();
    goalTime = 0;
    lights.red();

    while(!goalStatus && (millis() - start_wait < interval)){delay(10);}
    goalTime = millis() - start_wait;
    
    radio.stopListening();
    radio.openWritingPipe(n_pipes[goalNumber-1]);
    
    while( !radio.write( &goalTime, sizeof(goalTime) )){} //makes sure that it gets sent
      
    signalLights = true;
    radio.startListening();

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
  delay(500);
  lights.off();
}

void flashMiss(){
  lights.off();
  delay(500);
}

