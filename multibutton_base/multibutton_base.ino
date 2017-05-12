#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

bool isGoal[2] = {false, false};

int choice;
int buttonPin = 2;
int ledPin1 = 9;
int ledPin2 = 10;
bool timeout;
int time_wait;

void setup() {

  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  Serial.begin(115200);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, n_pipes[0]);
  radio.openReadingPipe(2, n_pipes[1]);
  radio.startListening();

  delay(1000);

  Serial.println("Ready to go");

}

void loop() {
    choice = digitalRead(buttonPin);
    isGoal[0] = false;
    isGoal[1] = false;

    radio.openWritingPipe(b_pipes[choice]);
    radio.stopListening();
    radio.write( &choice, sizeof(choice) );
    radio.startListening();
    
  
    time_wait = millis();
    timeout = false;
    while(!radio.available() && !timeout){
      if(millis() - time_wait > 250){
        timeout = true;
      }  
    }

    if(!timeout){
      radio.read( &isGoal[choice], sizeof(isGoal[choice]) );
      Serial.print("isGoal: ");
      Serial.println(isGoal[choice]);
    }

    digitalWrite(ledPin1, isGoal[0]);
    digitalWrite(ledPin2, isGoal[1]);

    delay(10);

}
      


