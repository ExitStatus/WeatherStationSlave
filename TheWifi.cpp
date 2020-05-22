#include "TheWifi.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;
const char* softApSSID = "SensorSetup";

TheWifi::TheWifi(WifiMode wifiMode, SensorDisplay *display)
{
    _lcd = display;
    _wifiMode = wifiMode;

    if (WiFi.SSID().length() == 0)
    {
        Serial.println("No setup SSID");
        _wifiMode = Server;
    }

    if (_wifiMode == Server)
    {
        Serial.println("WIFI Starting in Soft AP mode");

        WiFi.disconnect();
        WiFi.mode(WIFI_AP);

        // srand ( time(NULL) );
        softApPassword = new char[20];
        sprintf(softApPassword, "%lu", rand () % 100000000);

        Serial.printf("Soft AP SSID=%s PASSWORD=%s\n", softApSSID, softApPassword);

        bool result = WiFi.softAP(softApSSID, softApPassword);

        if (result == true)
        {
            Serial.println("Access Point Ready");

            _webServer = new ESP8266WebServer(80);
            _webServer->on("/", std::bind(&TheWifi::HandleServerRoot, this));
            _webServer->begin();
        }
        else
            Serial.println("Access Point Failed");
    }
    else
    {
        Serial.println("WIFI Starting in Station mode");

        _wifiOn = true;

#ifdef ESP8266
        _ntpUDP = new WiFiUDP();
        _timeClient = new NTPClient(*_ntpUDP, ntpServer);
#endif

        _timer = new Interval();
        _timer->Start(1000, true);

    }

/*
    if (reset)
    {
        if (ssids[_networkId] != WiFi.SSID())
        {
            WiFi.disconnect();
            WiFi.persistent(false);
            WiFi.mode(WIFI_STA);
            WiFi.setSleepMode(WIFI_NONE_SLEEP);
            WiFi.setAutoReconnect(true);
            WiFi.setOutputPower(17); 
            WiFi.begin(ssids[_networkId], passwords[_networkId]);
        }
        else
        {
            if (WiFi.waitForConnectResult() != WL_CONNECTED) 
            {
                Serial.println(F("Connection Failed!"));
                //timeout_cb();
            }
        }
        // Serial.setDebugOutput(true);

    }
    */
}

TheWifi::~TheWifi()
{
    if (softApPassword != NULL)
        delete[] softApPassword;

    if (_wifiOn)
    {
        delete _timer;
#ifdef ESP8266
        delete _timeClient;
        delete _ntpUDP;

        if (_webServer != NULL)
            delete _webServer;
#endif
    }
}

void TheWifi::HandleServerRoot()
{
    _webServer->send(200, "text/html", "<h1>Hello from ESP8266 AP!</h1>");
}

void TheWifi::HandleSoftApClient()
{
    _webServer->handleClient();
}
WifiMode TheWifi::GetMode()
{
    return _wifiMode;
}

void TheWifi::Render()
{
    if (!_wifiOn || !_timer->Ready())
      return;

    if (WiFi.status() != WL_CONNECTED)
        _lcd->RenderWifiStatus(0);
    else
        _lcd->RenderWifiStatus(GetStrength());
}

bool TheWifi::IsConnected()
{
    if (_wifiOn && WiFi.status() == WL_CONNECTED)
        return true;
    else
        return false;
}

int TheWifi::GetStrength()
{
    if (_wifiOn)
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
    }
    return 6;
}

const __FlashStringHelper *TheWifi::GetStatus()
{

    if (!_wifiOn)
        return F("Off");

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
    if (_wifiMode == Server)
    {
        return softApSSID;
    }
    else
    {
        if (!_wifiOn)
            return "WIFI OFF";

        return WiFi.SSID();
    }
}

int TheWifi::GetRSSI()
{
    if (_wifiOn)
        return 0;   

    return WiFi.RSSI();
}

String TheWifi::GetSoftApPassword()
{
    return softApPassword;
}

String TheWifi::GetIP()
{
    IPAddress addr;
    
    if (_wifiMode == Server)
        addr = WiFi.softAPIP();
    else
    {
        if (!_wifiOn)
            return "None"; 

        addr = WiFi.localIP();
    }

    String s="";
    for (int i=0; i<4; i++)
        s += i  ? "." + String(addr[i]) : String(addr[i]);
    return s;
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
    if (!_wifiOn)
        return false;

    struct tm timeinfo;

#ifdef ESP32
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(&timeinfo))
        return false;

    ntpTime = mktime(&timeinfo);
    if (ntpTime == 0)
        return false;

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

    if (ntpTime == 0)
        return false;

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
    if (!_wifiOn || ntpTime == 0)
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
    strftime(dateBuffer, dateLen, "%a %d %b %Y", timeinfo);
}

int TheWifi::PostReport(char *server, char *port, char *name, char *sensorData)
{
    if (!_wifiOn)
        return -1;

    char buffer[512];

    if (WiFi.status() != WL_CONNECTED)
        return -1;
  
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
        Serial.printf("POST FAILED to %s\n", buffer);
        Serial.println(http.errorToString(httpCode).c_str());

        _lcd->Error("IOT Fail");
        return 0;
    }

    return 1;
}

  /*
  char buffer[512];
  WifiStatus('?');
  
  WiFi.mode(WIFI_STA);
  WiFi.begin("xxx", "XXXXw");
    
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
