#ifndef _NOKIA5110DISPLAY_H
#define _NOKIA5110DISPLAY_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include "Pins.h"
#include "lcdfont.h"
#include "SensorDisplay.h"
#include "WifiIcons.h"

class Nokia5110Display : public SensorDisplay
{
  private:
    Adafruit_PCD8544 *lcd;
    char* activity = "|/-\\";
    void CentreText(int y, char *text);

  public:
    Nokia5110Display();
    ~Nokia5110Display();
    
    virtual void InitRender();
    virtual void BackgroundRender();
    virtual void Display();
    virtual void Logo();
    virtual void HasCapability(char *capability);
    virtual void RenderTemperature(float temperature);
    virtual void RenderHumidity(float humidity);
    virtual void RenderMaxMinTemperature(float maxTemperature, float minTemperature);
    virtual void RenderMaxMinHumidity(float maxHumidity, float minHumidity);
    virtual void RenderWifiStatus(int level);
    virtual void RenderWifiSSID(char *ssid);
    virtual void RenderActivity(int activity);
    virtual void Error(char *message);
    virtual void RenderDateTime(char *time, char *date);
};

#endif
