#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

int buttonPin = 2;
int ledPin = 9;
int choice;
bool isGoal = false;
int time_wait;
bool timeout;

void setup() {

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(115200);
  delay(1000);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, b_pipes[1]);
  radio.startListening();
  Serial.println("Ready to go");

}

void loop() {

//  Serial.println("loop");

  time_wait = millis();
  timeout = false;
  while(!radio.available() && !timeout){
    if(millis() - time_wait > 250){
      timeout = true;
    }
  }
  
  isGoal = digitalRead(buttonPin);
   
  if(!timeout){
    while(radio.available()){
      radio.read( &choice, sizeof(choice) );
    }
  
    radio.stopListening();
    radio.openWritingPipe(n_pipes[1]);
  
    radio.write( &isGoal, sizeof(isGoal) );
  
    radio.startListening();
  }

  digitalWrite(ledPin, isGoal);

}
