#include "Interval.h"

Interval::Interval()
{
    _every = 1000;
    _last = millis();
}

Interval::Interval(uint32_t e, bool now)
{
    _every = e;
    _last = millis();
    _now = now;
}

void Interval::Start(uint32_t e, bool now)
{
    _every = e;
    _last = millis();      
    _now = now;
}

void Interval::Now()
{
    _now = true;
}

bool Interval::Ready()
{
    uint32_t current = millis();
    
    if (current < _last)
        _last = 0;

    if (current - _last >= _every || _now)
    {
        _last = current;
        _now = false;

        return true;
    }
    else
        return false;
}