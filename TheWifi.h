#ifndef _THEWIFI_H
#define _THEWIFI_H

#include "WiFi.h"
#include "time.h"

#include "Interval.h"
#include "SensorDisplay.h"

class TheWifi
{
  private:
    int _networkId;
    bool _connected = false;
    Interval *_timer;
    SensorDisplay *_lcd;
    
  public: 
    TheWifi(int networkId, SensorDisplay *display);
    ~TheWifi();

    bool IsConnected();
    int GetStrength();
    const __FlashStringHelper *GetStatus();
    String GetSSID();
    int GetRSSI();
    IPAddress GetIP();
    uint32_t GetNtpTime();
    void Render();
}; 
#endif