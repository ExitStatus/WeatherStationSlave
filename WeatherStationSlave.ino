#include <Wire.h>
#include <Adafruit_AHT10.h>

#include "Interval.h"
#include "RingBuffer.h"
#include "TheWifi.h"
#include "Button.h"

// -----------------
// Define the client
// -----------------

#define ClientName  "Workshop"

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
Interval *ntpInterval = NULL;
Interval *clockInterval = NULL;
Interval *clearInterval = NULL;

// ------------------------
// Define the input buttons
// ------------------------
Button *button1; 

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

  while (!Serial) {  }

  wifi = new TheWifi(0, display);

  Wire.begin(I2C_SDA, I2C_SCL);

  display->InitRender();
  display->Logo();
  display->RenderWifiSSID(wifi->GetSSID());
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
  ntpInterval = new Interval(1000, true);   
  clockInterval = new Interval(1000, true);   
  clearInterval = new Interval(360000, false);

  button1 = new Button(BUTTON1);
}

// -------------------------------
// Handle the WIFI status changing
// -------------------------------
void HandleWifiStatus()
{
  wifi->Render();  
}

// ------------------------
// Handle fetching NTP data
// ------------------------
void HandleNtp()
{
  if (!ntpInterval->Ready() || !wifi->IsConnected())
    return;

  display->RenderActivity(2);

  if (wifi->GetNtpTime())
  {
    ntpInterval->Reset(360000);
    clearInterval->Reset(2000);
  }
  else
    clearInterval->Reset(500);
}

// ------------------------
// Handle the clock display
// ------------------------
void HandleClock()
{
  if (!clockInterval->Ready())
    return;

  char timebuffer[16];
  char datebuffer[64];

  wifi->GetTime(timebuffer, 16, datebuffer, 64);
  display->RenderDateTime(timebuffer, datebuffer);
}

// ---------------------------------
// Handle clearing the activity icon
// ---------------------------------
void HandleClearActivity()
{
  if (!clearInterval->Ready())
    return;

  display->RenderActivity(0);
}

// -------------------------
// Handle updating the stats
// -------------------------
void HandleStatsActivity()
{
  if (!displayInterval->Ready())
    return;

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

    if (!reportInterval->Ready())
      return;

    // -----------------------------
    // Send the stats to the IOT hub
    // -----------------------------
    sprintf(buffer, "\"temperature\" : %f, \"humidity\" : %f", temp, humid);
    wifi->PostReport(IOTHUB, IOTPORT, ClientName, buffer);

    clearInterval->Reset();
  }
}

// -------------
// THE MAIN LOOP
// -------------
void loop() 
{
  HandleWifiStatus();
  HandleNtp();
  HandleClock();
  HandleStatsActivity();
  HandleClearActivity();

  if (button1->State() == BUTTON_CLICKED)
  {
  }

  if (button1->State() == BUTTON_HELD)
  {
    MaxMinTemp.Initial = true;
    MaxMinHumid.Initial = true;

    displayInterval->Now();
  }
}

