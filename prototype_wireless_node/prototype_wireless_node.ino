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

int count;

int goalNumber = 1;

bool listening;


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
  listening = true;
}

void loop()
{
  if(!chosen){
    time_wait = millis();
    timeout = false;
    while(!radio.available() && !timeout){
      if(millis() - time_wait > 250){
        timeout = true;
//        Serial.println("Timeout");
      }
    }

  if(!timeout){
    while(radio.available()){
      radio.read( &chosen, sizeof(chosen) );
      Serial.println("reading");
    }
    }
  }
    

    if(chosen){

//      Serial.println("Goal chosen");
//      lights.red();

      if(goalStatus){
        radio.stopListening();
        radio.openWritingPipe(n_pipes[goalNumber-1]);
        while( !radio.write( &goalStatus, sizeof(goalStatus) ))
        {
          Serial.println("bad send");
        }
        Serial.print("good send ");
        Serial.println(goalStatus);
        radio.startListening();
        lights.green();
        delay(2000);
        lights.off();
        goalStatus = false;
      }

      time_wait = millis();
      timeout = false;
      while(!radio.available() && !timeout){
        if(millis() - time_wait > 250){
          timeout = true;
//          Serial.println("Time");
        }
      }
 
      if(!timeout){
        while(radio.available()){
          radio.read( &chosen, sizeof(chosen) );
          Serial.print("not chosen: ");
          Serial.println(chosen);
        } 
      }
  }
}

void goalState(){        //probably best to create goal objects
  goalStatus = true;
}

