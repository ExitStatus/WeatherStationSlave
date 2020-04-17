#ifndef Interval_h
#define Interval_h

#include "Arduino.h"

class Interval
{
  private:
    uint32_t _every = 1000;
    uint32_t _last = 0;
    bool _now = false;
  
  public: 
    Interval();
    Interval(uint32_t e, bool now = false);
    void Start(uint32_t e, bool now);
    void Now();
    bool Ready();
};

#endif
