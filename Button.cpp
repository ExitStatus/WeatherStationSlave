#include "Button.h"

//define DEBUG_BUTTON

Button::Button(uint8_t pin)
{
    _pin = pin;
    pinMode(_pin, INPUT_PULLUP);
    _currentState = HIGH;
}

uint8_t Button::State()
{
    int newState = digitalRead(_pin);
    uint64_t currentMS = millis();

    if (newState == HIGH && _currentState == HIGH) 
        return BUTTON_NONE;

    if (newState == LOW && _currentState == HIGH)
    {
        _downtime = currentMS;
        _currentState = LOW;

#ifdef DEBUG_BUTTON
        Serial.println("Was HIGH now LOW");
        Serial.printf("Downtime = %lu\n", _downtime);
        Serial.println("Current State = LOW");
#endif
        return BUTTON_NONE;
    }

    if (newState == HIGH && _currentState == LOW)
    {
#ifdef DEBUG_BUTTON
        Serial.println("Was LOW now HIGH");
#endif
        _currentState = HIGH;

        if (_downtime == 0)
        {
#ifdef DEBUG_BUTTON
            Serial.println("BUTTON_NONE");
#endif
            return BUTTON_NONE;
        }

        if (currentMS - _downtime < 50)
        {
#ifdef DEBUG_BUTTON
            Serial.printf("%lu - %lu = %d\n", currentMS, _downtime, currentMS - _downtime);
            Serial.println("BUTTON_BOUNCED");
#endif
            return BUTTON_BOUNCED;
        }

        if (currentMS - _downtime < 1000)
        {
#ifdef DEBUG_BUTTON
            Serial.printf("%lu - %lu = %d\n", currentMS, _downtime, currentMS - _downtime);
            Serial.println("BUTTON_CLICKED");
#endif
            _downtime = 0;
            return BUTTON_CLICKED;
        }
    }

    if (_downtime > 0 && (currentMS - _downtime > 3000))
    {
#ifdef DEBUG_BUTTON
            Serial.printf("%lu - %lu = %d\n", currentMS, _downtime, currentMS - _downtime);
            Serial.println("BUTTON_HELD");
#endif
        _downtime = 0;
        return BUTTON_HELD;
    }

    return BUTTON_NONE;
}