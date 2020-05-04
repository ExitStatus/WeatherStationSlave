#include "TFTeSPIDisplay.h"

TFTeSPIDisplay::TFTeSPIDisplay()
{
  tft = new TFT_eSPI();
  tft->setSwapBytes(true);

  _width = 128;
  _height = 160;
  _capy = 104;

  _cx = _width / 2;
  _cy = _height / 2;

 // tempArea.x = tempArea.y = tempArea.w = tempArea.h = 0;
}

TFTeSPIDisplay::~TFTeSPIDisplay()
{
  delete tft;
}

void TFTeSPIDisplay::Display()
{
  // Not needed for ST7735
}

void TFTeSPIDisplay::Logo()
{
    tft->setTextDatum(TC_DATUM);
    tft->drawString("Sensor v1.0", _cx, 5, 2);

    tft->pushImage(18, 22, 92, 77, logoImage);
}

void TFTeSPIDisplay::RenderWifiSSID(char *ssid)
{
    tft->setTextDatum(TC_DATUM);
    tft->drawString(ssid, _cx, 150, 1);
}

void TFTeSPIDisplay::HasCapability(char *capability)
{
    tft->setTextDatum(TC_DATUM);
    tft->drawString(capability, _cx, _capy, 1);

    _capy += 10;
}

void TFTeSPIDisplay::InitRender()
{
    Serial.print("Init Display");
    tft->init();
    tft->setRotation(0);
    tft->fillScreen(TFT_BLACK);
}

void TFTeSPIDisplay::BackgroundRender()
{
    tft->fillScreen(TFT_BLACK);

    tft->drawRoundRect(0, 0, 128, 60, 10, TFT_WHITE);
    tft->drawRoundRect(0, 65, 128, 60, 10, TFT_WHITE);
    tft->drawRoundRect(0, 130, 128, 30, 10, TFT_WHITE);

    tft->setTextColor(TFT_GREEN, TFT_BLACK);
    
    tft->setTextDatum(TC_DATUM);

    tft->drawString("Temperature", _cx, 8, 1);
    tft->drawString("Humidity", _cx, 73, 1);

    tft->setTextDatum(TL_DATUM);
    tft->setCursor(14, 45);
    tft->print("Min");
    tft->setCursor(64, 45);
    tft->print("Max");

    tft->setCursor(14, 110);
    tft->print("Min");
    tft->setCursor(64, 110);
    tft->print("Max");
}

void TFTeSPIDisplay::RenderTemperature(float temperature)
{
  char buffer[32];
  int16_t x1, y1;
  uint16_t w, h;

  temperature = floorf(temperature * 10) / 10;

  if (_currentTemp == temperature)
    return;

  sprintf(buffer, "%.01f  C", temperature);
  
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->drawString(buffer, _cx - (tft->textWidth(buffer, 4)/2), 18, 4);

  //tft->drawCircle(x1 + w + 5, y1 + 4, 3, TFT_WHITE);
    
  _currentTemp = temperature;
}

void TFTeSPIDisplay::RenderHumidity(float humidity)
{
  char buffer[32];
  int16_t x1, y1;
  uint16_t w, h;

  humidity = floorf(humidity * 10) / 10;

  if (_currentHumid == humidity)
    return;

  sprintf(buffer, "%.01f%%", humidity);

  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->drawString(buffer, _cx - (tft->textWidth(buffer, 4)/2), 83, 4);

  _currentHumid = humidity;
}

void TFTeSPIDisplay::RenderMaxMinTemperature(float maxTemperature, float minTemperature)
{
    char buffer[32];

    tft->setTextColor(TFT_WHITE, TFT_BLACK);  

    sprintf(buffer, "%.01f", floorf(minTemperature * 10) / 10);
    tft->drawString(buffer, 34, 45, 1);

    sprintf(buffer, "%.01f", floorf(maxTemperature * 10) / 10);
    tft->drawString(buffer, 84, 45, 1);
}

void TFTeSPIDisplay::RenderMaxMinHumidity(float maxHumidity, float minHumidity)
{
    char buffer[32];

    tft->setTextColor(TFT_WHITE, TFT_BLACK);  

    sprintf(buffer, "%.01f", floorf(minHumidity * 10) / 10);
    tft->drawString(buffer, 34, 110, 1);

    sprintf(buffer, "%.01f", floorf(maxHumidity * 10) / 10);
    tft->drawString(buffer, 84, 110, 1);
}

void TFTeSPIDisplay::RenderWifiStatus(int level)
{
  switch (level)
  {
      case 0: tft->drawBitmap(115, 140, wifiOff, 8, 8, TFT_RED, TFT_BLACK); break;
      case 1: tft->drawBitmap(115, 140, wifiPower1, 8, 8, TFT_RED, TFT_BLACK); break;
      case 2: tft->drawBitmap(115, 140, wifiPower2, 8, 8, TFT_ORANGE, TFT_BLACK); break;
      case 3: tft->drawBitmap(115, 140, wifiPower3, 8, 8, TFT_ORANGE, TFT_BLACK); break;
      case 4: tft->drawBitmap(115, 140, wifiPower4, 8, 8, TFT_ORANGE, TFT_BLACK); break;
      case 5: tft->drawBitmap(115, 140, wifiPower5, 8, 8, TFT_GREEN, TFT_BLACK); break;
      default: tft->drawBitmap(115, 140, wifiPower6, 8, 8, TFT_GREEN, TFT_BLACK); break;
  }
}

void TFTeSPIDisplay::Error(char *message)
{
  tft->setTextColor(TFT_RED, TFT_BLACK);
  tft->setCursor(((_width - 20) / 2) - (strlen(message) * 6) / 27, 141);
  tft->print(message);
}

void TFTeSPIDisplay::RenderActivity(int activity)
{
  if (activity > 0)
  {
    switch (activity)
    {
      case 1: tft->pushImage(5, 137, 16, 16, wifiPost); break;
      case 2: tft->pushImage(5, 137, 16, 16, wifiClock); break;
    }
  }
  else
    tft->fillRect(5, 137, 16, 16, TFT_BLACK);
}

void TFTeSPIDisplay::RenderDateTime(char *time, char *date)
{
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->drawString(time, 24, 137, 1);
    tft->drawString(date, 24, 147, 1);
}