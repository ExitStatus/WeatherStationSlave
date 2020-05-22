#ifndef _TFTESPIDISPLAY_H
#define _TFTESPIDISPLAY_H

#include <TFT_eSPI.h>
#include <math.h>

#include <SPI.h>

#include "Pins.h"
#include "SensorDisplay.h"
#include "WifiIcons.h"
#include "LogoImage.h"

class TFTeSPIDisplay : public SensorDisplay
{
  private:
    TFT_eSPI *tft;
    void CentreText(int y, char *text);
    float _currentTemp = 999;
    float _currentHumid = 999;   

    int _cx;
    int _cy;

    bool _clearOnNextTimeDisplay = false;

  public:
    TFTeSPIDisplay();
    ~TFTeSPIDisplay();
    
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
