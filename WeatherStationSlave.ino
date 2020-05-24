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

#define Display_TFT_eSPI
// #define Display_ST7735
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

#ifdef Display_TFT_eSPI
#include "TFTeSPIDisplay.h"
SensorDisplay *display = new TFTeSPIDisplay();
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
void setup() 
{

  Serial.begin(57600);
  Serial.setTimeout(2000);
  while (!Serial) {  }

  
  //----------------------------------------------
  // Set the backlight full for the initial screen
  //----------------------------------------------
#ifdef ESP32
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BACKLIGHT_OUTPUT_PIN, 0);
  ledcWrite(0, 255);
#endif

#ifdef ESP8266
  analogWrite(BACKLIGHT_OUTPUT_PIN, 1024);
#endif

  display->InitRender();

  // If the reset button is pressed then start up as 
  // a soft AP mode rather than station mode
  pinMode(D6, INPUT_PULLUP);
  if (digitalRead(D6) == 0)
  {
    Serial.println("RESET Requested");
    wifi = new TheWifi(Server, display);
  }
  else
    wifi = new TheWifi(Client, display);

  // Check if are a client (station mode) and run normally. Even if the reset button 
  // has not been pressed and we have requested being a client, if there are no credentials
  // stored then we have to default to server (softAP)
  if (wifi->GetMode() == Client)
  {
    display->Logo();

    Wire.begin(I2C_SDA, I2C_SCL);

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
  }
  else
  {
    display->Setup(wifi->GetSSID(), wifi->GetSoftApPassword(), wifi->GetIP());
  }
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
    ntpInterval->Reset(3600000);
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

  if (clockInterval->GetInterval() != 1000)
    clockInterval->Reset(1000);
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
    if (wifi->PostReport(IOTHUB, IOTPORT, ClientName, buffer) == 0)
      clockInterval->Reset(10000);

    clearInterval->Reset();

  }
}

// -------------
// THE MAIN LOOP
// -------------
void loop() 
{
  if (wifi->GetMode() == Client)
  {
    int val = analogRead(LIGHT_INPUT_PIN);

    #ifdef ESP32
    ledcWrite(0, map(val, 0, 4096, 1, 255));
    #endif

    #ifdef ESP8266
    int lightVal = map(val, 100, 800, 10, 1023);
    if (lightVal < 10)
      lightVal = 10;
    else 
    if (lightVal > 1023)
      lightVal = 1023;
      
    analogWrite(BACKLIGHT_OUTPUT_PIN, lightVal);
    #endif

    HandleWifiStatus();
    HandleNtp();
    HandleClock();
    HandleStatsActivity();
    HandleClearActivity();

  /*
    switch (button1->State())
    {
      case BUTTON_CLICKED:
        break;

      case BUTTON_HELD:
        MaxMinTemp.Initial = true;
        MaxMinHumid.Initial = true;
        displayInterval->Now();
        break;
    }
    */
  }
  else
  {
    wifi->HandleSoftApClient();
  }

  delay(100);
}
