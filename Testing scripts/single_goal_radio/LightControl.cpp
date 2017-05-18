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
