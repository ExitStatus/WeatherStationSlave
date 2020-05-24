#include "Settings.h"

Settings::Settings()
{
    EEPROM.begin(sizeof(WifiSettings) + 2);

    if ((EEPROM.read(0) != 0x98) || (EEPROM.read(1) != 0x76))
    {
        EEPROM.write(0, 0x98);
        EEPROM.write(1, 0x76);
        EEPROM.put(2, _settings);
        EEPROM.commit();
    }
    else
        EEPROM.get(2, _settings);
}

Settings::~Settings()
{

}

String Settings::GetLocation()
{
    return _settings.Location;
}

String Settings::GetServerAddress()
{
    return _settings.ServerAddress;
}

bool Settings::GetSendReadings()
{
    return _settings.SendReadings;
}

Settings GlobalSettings;
