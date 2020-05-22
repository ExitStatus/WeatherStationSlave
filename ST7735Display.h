#ifndef _ST7735DISPLAY_H
#define _ST7735DISPLAY_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h>
#include <math.h>

#include "font1.h"

#include <SPI.h>

#include "Pins.h"
#include "SensorDisplay.h"
#include "WifiIcons.h"
#include "LogoImage.h"

class ST7735Display : public SensorDisplay
{
  private:
    Adafruit_ST7735 *tft;
    void CentreText(int y, const char *text);
    float _currentTemp = 999;
    float _currentHumid = 999;    

  public:
    ST7735Display();
    ~ST7735Display();
    
    virtual void InitRender();
    virtual void BackgroundRender();
    virtual void Display();
    virtual void Logo();
    virtual void Setup(String ssid, String password, String address);
    virtual void HasCapability(char *capability);
    virtual void RenderTemperature(float temperature);
    virtual void RenderHumidity(float humidity);
    virtual void RenderMaxMinTemperature(float maxTemperature, float minTemperature);
    virtual void RenderMaxMinHumidity(float maxHumidity, float minHumidity);
    virtual void RenderWifiStatus(int level);
    virtual void RenderWifiSSID(String ssid);
    virtual void RenderActivity(int activity);
    virtual void Error(char *message);
    virtual void RenderDateTime(char *time, char *date);
};

#endif
