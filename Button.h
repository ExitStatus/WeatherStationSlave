#ifndef Button_h
#define Button_h

#include "Arduino.h"

#define BUTTON_NONE     0
#define BUTTON_CLICKED  1
#define BUTTON_HELD     2
#define BUTTON_BOUNCED  3

class Button
{
  private:
    uint8_t _pin;
    uint8_t _currentState;
    uint64_t _downtime;
    
  public: 
    Button(uint8_t pin);
    uint8_t State();
};

#endif