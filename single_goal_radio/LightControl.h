#ifndef LightControl_h
#define LightControl_h

#include "Arduino.h"

class LightControl
{
  public:
    LightControl(int redPin, int greenPin);
    void red();
    void green();
    void off();

  private:
    int _redPin;
    int _greenPin;
};

#endif
