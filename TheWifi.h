#ifndef _THEWIFI_H
#define _THEWIFI_H

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include "time.h"

#include "Interval.h"
#include "SensorDisplay.h"

class TheWifi
{
  private:
    int _networkId;
    bool _wifiOn = false;
    bool _connected = false;
    Interval *_timer;
    SensorDisplay *_lcd;
    time_t ntpTime = 0;
    uint32_t ntpTimeStart = 0;
    bool DaylightSavings(struct tm *timeinfo, int year, int fromDay, int fromMonth, int toDay, int toMonth);
    
#ifdef ESP8266
    WiFiUDP *_ntpUDP;
    NTPClient *_timeClient;
#endif

  public: 
    TheWifi(int networkId, SensorDisplay *display);
    ~TheWifi();

    bool IsConnected();
    int GetStrength();
    const __FlashStringHelper *GetStatus();
    char *GetSSID();
    int GetRSSI();
    IPAddress GetIP();
    bool GetNtpTime();
    void GetTime(char *timeBuffer, int timeLen, char *dateBuffer, int dateLen);
    void Render();
    void PostReport(char *server, char *port, char *name, char *sensorData);
}; 
#endif