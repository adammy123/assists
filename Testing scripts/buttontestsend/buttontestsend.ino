#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

byte addresses[][6] = {"1Node"};
bool isGoal = false;
int buttonPin = 2;

void setup() {

  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  delay(1000);

  Serial.println("Sender");

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

//  radio.openReadingPipe(1, addresses[0]);
//  radio.startListening();
  radio.stopListening();
  radio.openWritingPipe(addresses[0]);
  delay(1000);

}

void loop() {
//  radio.stopListening();
//  radio.openWritingPipe(addresses[0]);
  radio.write( &isGoal, sizeof(isGoal) );
//  radio.startListening();
  Serial.println(isGoal);
  isGoal = digitalRead(buttonPin);

}
