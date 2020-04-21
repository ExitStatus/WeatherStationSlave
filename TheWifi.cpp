#include "TheWifi.h"

char *ssids[1] = { "SKYDCFAB" }; 
char *passwords[1] = { "XXXX" };

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

TheWifi::TheWifi(int networkId, SensorDisplay *display)
{
    _lcd = display;

    _networkId = networkId;

    Serial.println("Connecting to WIFI");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(ssids[_networkId], passwords[_networkId]);

    Serial.print(ssids[_networkId]);

    _timer = new Interval();
    _timer->Start(1000, true);
}

TheWifi::~TheWifi()
{
    delete _timer;
}

void TheWifi::Render()
{
    if (!_timer->Ready())
      return;

    Serial.println(GetStatus());
    if (WiFi.status() != WL_CONNECTED)
        _lcd->RenderWifiStatus(0);
    else
        _lcd->RenderWifiStatus(GetStrength());
}

bool TheWifi::IsConnected()
{
    if (WiFi.status() == WL_CONNECTED)
        return true;
    else
        return false;
}

int TheWifi::GetStrength()
{
    int rssi = WiFi.RSSI();
    if (rssi < -80)
        return 1;

    if (rssi< -75)
        return 2;

    if (rssi < -70)
        return 3;

    if (rssi < -65)
        return 4;

    if (rssi < -60)
        return 5;

    return 6;
}

const __FlashStringHelper *TheWifi::GetStatus()
{
    switch (WiFi.status())
    {
        case WL_CONNECTED: return F("Connected");
        case WL_NO_SHIELD: return F("No Shield");
        case WL_IDLE_STATUS: return F("Idle");
        case WL_NO_SSID_AVAIL: return F("No SSID");
        case WL_SCAN_COMPLETED: return F("Scan Complete");
        case WL_CONNECT_FAILED: return F("Failed");
        case WL_CONNECTION_LOST: return F("Lost");
        case WL_DISCONNECTED: return F("Disconnected");
    }
}

String TheWifi::GetSSID()
{
    return WiFi.SSID();
}

int TheWifi::GetRSSI()
{
    return WiFi.RSSI();
}

IPAddress TheWifi::GetIP()
{
    return WiFi.localIP();
}

uint32_t TheWifi::GetNtpTime()
{
    struct tm timeinfo;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return 0;
    }

    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");    

    time_t rawtime = mktime(&timeinfo);
    Serial.println(rawtime);

    return 0;
}