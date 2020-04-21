#include <Wire.h>
#include <Adafruit_AHT10.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#include "Interval.h"
#include "RingBuffer.h"
#include "TheWifi.h"

// ----------------------------------------
// Define what kind of display we are using
// ----------------------------------------

#define Display_ST7735
// #define Display_Nokia5110
// #define Display_OLED
// #define Display_LCD

#ifdef Display_ST7735
#include "ST7735Display.h"
SensorDisplay *display = new ST7735Display();
#endif

#ifdef Display_Nokia5110
#include "Nokia5110Display.h"
SensorDisplay *display = new Nokia5110Display();
#endif

// ----------------------------------------
// Define where we will post sensor data to
// ----------------------------------------
#define IOTHUB  "192.168.0.10"
#define IOTPORT "5555"

// ---------------------------
// Define the wifi connection
// ---------------------------
TheWifi *wifi;

// ---------------------------
// Define the action intervals
// ---------------------------
Interval *reportInterval = NULL;
Interval *displayInterval = NULL;

// ----------------------------------------------------
// Define for the AHT10 temperature and humidity sensor
// ----------------------------------------------------
Adafruit_AHT10 aht;
RingBuffer tempBuffer(12);
RingBuffer humidBuffer(12);

bool hasAht10 = false;

struct MaxMin_t
{
  bool Initial = true;
  float MaxValue;
  float MinValue;
} MaxMinTemp, MaxMinHumid;

// -------------------------
// Setup method called Once
// -------------------------
void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) {
  }

  wifi = new TheWifi(0, display);

  Wire.begin(I2C_SDA, I2C_SCL);

  display->InitRender();
  display->Logo();
  display->Display();
   
  if (aht.begin(&Wire, 0x32))
  {
    display->HasCapability("Temperature");
    display->HasCapability("Humidity");
    display->Display();  

    hasAht10 = true;
  }
  
  delay(5000);
 
  display->BackgroundRender();
  
  reportInterval = new Interval(60000, false);   
  displayInterval = new Interval(5000, true);   
}

void loop() 
{
  if (displayInterval->Ready())
  {
    char buffer[64];

    // -------------
    // Get the stats
    // -------------
    if (hasAht10)
    {
      sensors_event_t humidity, temperature;
      aht.getEvent(&humidity, &temperature);// populate temp and humidity objects with fresh data

      tempBuffer.Add(temperature.temperature);
      humidBuffer.Add(humidity.relative_humidity);
      
      float temp = tempBuffer.Average();
      float humid = humidBuffer.Average();

      if (MaxMinTemp.Initial)
      {
        MaxMinTemp.MaxValue = MaxMinTemp.MinValue = temp;
        MaxMinTemp.Initial = false;
      }
      else
      {
        if (temp > MaxMinTemp.MaxValue)
          MaxMinTemp.MaxValue = temp;

        if (temp < MaxMinTemp.MinValue)
          MaxMinTemp.MinValue = temp;
      }

      if (MaxMinHumid.Initial)
      {
        MaxMinHumid.MaxValue = MaxMinHumid.MinValue = humid;
        MaxMinHumid.Initial = false;
      }
      else
      {
        if (humid > MaxMinHumid.MaxValue)
          MaxMinHumid.MaxValue = humid;

        if (humid < MaxMinHumid.MinValue)
          MaxMinHumid.MinValue = humid;
      }

      // -----------------
      // Display the stats
      // -----------------

      display->RenderTemperature(temp);
      display->RenderHumidity(humid);

      display->RenderMaxMinTemperature(MaxMinTemp.MaxValue, MaxMinTemp.MinValue);
      display->RenderMaxMinHumidity(MaxMinHumid.MaxValue, MaxMinHumid.MinValue);

      display->Display();

#ifdef ESP8266
      if (!reportInterval->Ready())
        return;

      // -----------------------------
      // Send the stats to the IOT hub
      // -----------------------------
      sprintf(buffer, "\"temperature\" : %f, \"humidity\" : %f", temp, humid);
      WifiReport(buffer);
#endif
    }
    /*
    display.clearDisplay();
    display.setFont(&Open_Sans_ExtraBold_20);
    
    sprintf(buffer, "%0.2f", temp);
    display.setCursor((42-(strlen(buffer)/2)*15),14);
    display.print(buffer);

    display.setFont();
    CentreText(15, "Celsius");

    display.setFont(&Open_Sans_ExtraBold_20);
    sprintf(buffer, "%0.2f", humid);
    display.setCursor((42-(strlen(buffer)/2)*15),39);
    display.print(buffer);

    display.setFont();
    CentreText(40, "%Humidity");
    
    display.display();
    */

  }

  wifi->Render();
}

void WifiStatus(char statusChar)
{
  /*
    display.fillRect(0,0,8,18, WHITE);
    display.setCursor(0,0);
    display.print(statusChar);
    display.display(); 
    */
}

void WifiReport(char *sensorData)
{
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
}
