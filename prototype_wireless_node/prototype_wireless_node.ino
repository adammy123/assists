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

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

LightControl lights(redLEDPin, greenLEDPin);

bool goalStatus = false;
bool chosen = false;

int time_wait;
bool timeout;
int liveGoal;


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

  radio.openReadingPipe(1, b_pipes[0]);
  radio.startListening();

  delay(500);
  Serial.println("Begin");
  lights.off();
}

void loop()
{
//  //flash lights to show starting
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

  
  time_wait = millis();
  timeout = false;
//  Serial.println("loop");
  while(!radio.available() && !timeout){
//    Serial.println("wait");
    if(millis() - time_wait > 250){
      timeout = true;
//      Serial.println("timout");
    }
  }

  if(!timeout){
    while(radio.available()){
      radio.read( &liveGoal, sizeof(liveGoal) );
      Serial.println(liveGoal);
    }
    
    chosen = true;
    radio.stopListening();
    radio.openWritingPipe(n_pipes[0]);
  }

    if(chosen){
      lights.red();
      radio.write( &goalStatus, sizeof(goalStatus) );
      Serial.println(goalStatus);

      if(goalStatus){
        lights.green();
        delay(1000);
        lights.off();
      }

      chosen = false;
      goalStatus = false;
    }

  radio.startListening();
}

void goalState(){        //probably best to create goal objects
  goalStatus = true;
}

