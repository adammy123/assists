#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

byte addresses[][6] = {"1Node"};
bool isGoal = false;
int ledPin = 2;

void setup() {

  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  delay(1000);

  Serial.println("Receiver");

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

  Serial.println("Ready to go");

}

void loop() {
  if(radio.available())
  {
    //Serial.println("Receiving");
    while(radio.available())
    {
      radio.read( &isGoal, sizeof(isGoal));
    }
  }
  Serial.println(isGoal);
  digitalWrite(ledPin, isGoal);

}
