#include "Arduino.h"
#include "LightControl.h"


LightControl::LightControl(int redPin, int greenPin)
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  _redPin = redPin;
  _greenPin = greenPin;
}

void LightControl::red()
{
  digitalWrite(_redPin, HIGH);
  digitalWrite(_greenPin, LOW);
}

void LightControl::green(){
  digitalWrite(_redPin, LOW);
  digitalWrite(_greenPin, HIGH);
}

void LightControl::off(){
  digitalWrite(_redPin, LOW);
  digitalWrite(_greenPin, LOW);
}

void LightControl::flash(int _delay, int numFlashes)
{
  for(int i=0; i<numFlashes; i++){
    digitalWrite(_redPin, HIGH);
    digitalWrite(_greenPin, HIGH);
    delay(_delay);
    digitalWrite(_redPin, LOW);
    digitalWrite(_greenPin, LOW);
    delay(_delay);
  }
}

