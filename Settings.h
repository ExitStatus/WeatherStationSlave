#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <EEPROM.h>
#include <Arduino.h>

struct WifiSettings {
    char Location[64];
    bool SendReadings = true;
    char ServerAddress[24] = "192.168.0.10:5555";
};

class Settings
{
    private:
        WifiSettings _settings;

    public: 
        Settings();
        ~Settings();

        String GetLocation();
        String GetServerAddress();
        bool GetSendReadings();
};

#endif