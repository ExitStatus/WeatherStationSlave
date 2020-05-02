

#include "TheWifi.h"

char *ssids[1] = { "SKYDCFAB" }; 
char *passwords[1] = { "XXX" };

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

TheWifi::TheWifi(int networkId, SensorDisplay *display)
{
    _lcd = display;

    _networkId = networkId;

    Serial.println("Connecting to WIFI");

    Serial.setDebugOutput(true);

    WiFi.disconnect();
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setAutoReconnect(true);
    WiFi.setOutputPower(17); 
    WiFi.begin(ssids[_networkId], passwords[_networkId]);

#ifdef ESP8266
    _ntpUDP = new WiFiUDP();
    _timeClient = new NTPClient(*_ntpUDP, ntpServer);
#endif

    _timer = new Interval();
    _timer->Start(1000, true);
}

TheWifi::~TheWifi()
{
    delete _timer;
    delete _timeClient;
    delete _ntpUDP;
}

void TheWifi::Render()
{
    if (!_timer->Ready())
      return;

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

char *TheWifi::GetSSID()
{
    return ssids[_networkId];
}

int TheWifi::GetRSSI()
{
    return WiFi.RSSI();
}

IPAddress TheWifi::GetIP()
{
    return WiFi.localIP();
}

bool TheWifi::DaylightSavings(struct tm *timeinfo, int year, int fromDay, int fromMonth, int toDay, int toMonth)
{
    int day = timeinfo->tm_mday;
    int month = timeinfo->tm_mon + 1;
    int hour = timeinfo->tm_hour;

    if (timeinfo->tm_year + 1900 != year)
        return false;

    if (month < fromMonth || (month == fromMonth && day < fromDay) || (month == fromMonth && day == fromDay && hour < 1))
        return false;

    if (month > toMonth || (month == toMonth && day > toDay) || (month == toMonth && day == toDay && hour > 1))
        return false;

    return true;
}

bool TheWifi::GetNtpTime()
{
    struct tm timeinfo;

#ifdef ESP32
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo))
        return false;

    ntpTime = mktime(&timeinfo);

    if (DaylightSavings(&timeinfo, 2020, 29, 3, 25, 10) ||
        DaylightSavings(&timeinfo, 2021, 28, 3, 31, 10) ||
        DaylightSavings(&timeinfo, 2022, 27, 3, 30, 10) ||
        DaylightSavings(&timeinfo, 2023, 26, 3, 29, 10) ||
        DaylightSavings(&timeinfo, 2024, 31, 3, 27, 10) ||
        DaylightSavings(&timeinfo, 2025, 30, 3, 26, 10) ||
        DaylightSavings(&timeinfo, 2026, 29, 3, 25, 10) ||
        DaylightSavings(&timeinfo, 2027, 28, 3, 31, 10) ||
        DaylightSavings(&timeinfo, 2028, 26, 3, 29, 10) ||
        DaylightSavings(&timeinfo, 2029, 25, 3, 28, 10))
            ntpTime += 3600;
#endif

#ifdef ESP8266

    if (ntpTime == 0)
    {
        _timeClient->begin();
        _timeClient->setTimeOffset(0);
    }

    _timeClient->update();
    ntpTime = _timeClient->getEpochTime();

    struct tm *p = localtime(&ntpTime);
    
    if (DaylightSavings(p, 2020, 29, 3, 25, 10) ||
        DaylightSavings(p, 2021, 28, 3, 31, 10) ||
        DaylightSavings(p, 2022, 27, 3, 30, 10) ||
        DaylightSavings(p, 2023, 26, 3, 29, 10) ||
        DaylightSavings(p, 2024, 31, 3, 27, 10) ||
        DaylightSavings(p, 2025, 30, 3, 26, 10) ||
        DaylightSavings(p, 2026, 29, 3, 25, 10) ||
        DaylightSavings(p, 2027, 28, 3, 31, 10) ||
        DaylightSavings(p, 2028, 26, 3, 29, 10) ||
        DaylightSavings(p, 2029, 25, 3, 28, 10))
            ntpTime += 3600;
#endif

    ntpTimeStart = millis();

    return true;
}

void TheWifi::GetTime(char *timeBuffer, int timeLen, char *dateBuffer, int dateLen)
{
    if (ntpTime == 0)
    {
        *timeBuffer = '\0';
        *dateBuffer = '\0';
        return;
    }

    uint64_t ms = millis();
    uint64_t elapsed = 0;

    if (ms < ntpTimeStart)
    {
        if (GetNtpTime())
        {
            ms = millis();
            elapsed = ms - ntpTimeStart;
        }
        else
            elapsed = (18446744073709551615 - ntpTimeStart) + ms;

    }
    else
        elapsed = ms - ntpTimeStart;

    time_t current = ntpTime + (elapsed / 1000);

    struct tm *timeinfo = localtime(&current);

    strftime(timeBuffer, timeLen, "%I:%M:%S %p", timeinfo);
    strftime(dateBuffer, dateLen, "%a %e %b %Y", timeinfo);
}

void TheWifi::PostReport(char *server, char *port, char *name, char *sensorData)
{
    char buffer[512];

    if (WiFi.status() != WL_CONNECTED)
        return;
  
    _lcd->RenderActivity(1);

    WiFiClient client;
    HTTPClient http;
  
    sprintf(buffer, "http://%s:%s/api/record", server, port);
    
    http.begin(client, buffer);
    http.addHeader("Content-Type", "application/json");
  
    sprintf(buffer, "{ \"client_name\": \"%s\",\"power_type\": \"AC\",\"power_level\": \"100\", \"sensors\" : { %s }}", name, sensorData);

    int httpCode = http.POST(buffer);
    if (httpCode != HTTP_CODE_OK)
    {
        sprintf(buffer, "Error %d", httpCode);
        _lcd->Error(buffer);
        delay(10000);
    }
}

  /*
  char buffer[512];
  WifiStatus('?');
  
  WiFi.mode(WIFI_STA);
  WiFi.begin("SKYDCFAB", "TPPDMCMDRD");
    
  int activityIdx = 0;
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    WifiStatus(activity[activityIdx]);
      
    if (activityIdx++ > 3)
      activityIdx = 0;
    
    delay(250);

    if (++retries > 120 || WiFi.status() == WL_NO_SSID_AVAIL)
    {
      display.clearDisplay();
      display.setFont();
      display.printf("An error occurred connecting to Wifi access point SKYDCFAB");
      display.display();
      delay(10000);
    
      return;  
    }
  }
    
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  
  sprintf(buffer, "{ \"client_name\": \"Office\",\"power_type\": \"AC\",\"power_level\": \"100\", \"sensors\" : { %s }}", sensorData);
  
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, "http://" IOTHUB ":" IOTPORT "/api/record");
  http.addHeader("Content-Type", "application/json");
  
  Serial.printf("Sending packet: %s\n", buffer);
  int httpCode = http.POST(buffer);
  if (httpCode == HTTP_CODE_OK)
  {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
  }
  else
  {
    display.clearDisplay();
    display.setFont();
    display.printf("An error occurred sending data to the IOT hub - Code %d", httpCode);
    display.display();
    delay(10000);
  }
  
  WiFi.disconnect();
  
  display.fillRect(0,0,8,8, WHITE);
  display.display(); 
  */
