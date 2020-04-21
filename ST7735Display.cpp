#include "ST7735Display.h"

ST7735Display::ST7735Display()
{
  tft = new Adafruit_ST7735(SPI_CS, SPI_DC, SPI_MOSI, SPI_SCLK, SPI_RST);
  tft->setSPISpeed(4000000000);

  _width = 128;
  _height = 160;
  _capy = 14;

 // tempArea.x = tempArea.y = tempArea.w = tempArea.h = 0;
}

ST7735Display::~ST7735Display()
{
  delete tft;
}

void ST7735Display::Display()
{
  // Not needed for ST7735
}

void ST7735Display::Logo()
{
  CentreText(0, "Sensor v1.0");
}

void ST7735Display::HasCapability(char *capability)
{
  CentreText(_capy, capability);
  _capy += 10;
}

void ST7735Display::InitRender()
{
  tft->initR(INITR_BLACKTAB);
  tft->fillScreen(ST77XX_BLACK);
}

void ST7735Display::BackgroundRender()
{
  tft->fillScreen(ST77XX_BLACK);

  tft->drawRoundRect(0, 0, 128, 60, 10, ST77XX_WHITE);
  tft->drawRoundRect(0, 65, 128, 60, 10, ST77XX_WHITE);
  tft->drawRoundRect(0, 130, 128, 30, 10, ST77XX_WHITE);

  tft->setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  tft->setFont();
  
  CentreText(8, "Temperature");
  CentreText(73, "Humidity");

  tft->setCursor(14, 45);
  tft->print("Min");
  tft->setCursor(64, 45);
  tft->print("Max");

  tft->setCursor(14, 110);
  tft->print("Min");
  tft->setCursor(64, 110);
  tft->print("Max");
}

void ST7735Display::RenderTemperature(float temperature)
{
  char buffer[32];
  int16_t x1, y1;
  uint16_t w, h;

  temperature = floorf(temperature * 10) / 10;

  if (_currentTemp == temperature)
    return;

  sprintf(buffer, "%.01f", temperature);
  
  GFXcanvas16 *rh_canvas = new GFXcanvas16(100, 22);
  rh_canvas->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  rh_canvas->setFont(&Dialog_bold_24);
  rh_canvas->setCursor(0, 22);
  rh_canvas->print(buffer);
  rh_canvas->print(" C");
  rh_canvas->getTextBounds(buffer, 0, 22, &x1, &y1, &w, &h);
  rh_canvas->drawCircle(x1 + w + 5, y1 + 4, 4, ST77XX_WHITE);
    
  tft->drawRGBBitmap(16, 17, rh_canvas->getBuffer(), 100, 22);

  delete rh_canvas;

  _currentTemp = temperature;
}

void ST7735Display::RenderHumidity(float humidity)
{
  char buffer[32];
  int16_t x1, y1;
  uint16_t w, h;

  humidity = floorf(humidity * 10) / 10;

  if (_currentHumid == humidity)
    return;

  sprintf(buffer, "%.01f%%", humidity);
  
  GFXcanvas16 *rh_canvas = new GFXcanvas16(100, 22);
  rh_canvas->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  rh_canvas->setFont(&Dialog_bold_24);
  rh_canvas->setCursor(0, 22);
  rh_canvas->print(buffer);
  tft->drawRGBBitmap(20, 82, rh_canvas->getBuffer(), 100, 22);

  delete rh_canvas;

  _currentHumid = humidity;
}

void ST7735Display::RenderMaxMinTemperature(float maxTemperature, float minTemperature)
{
  char buffer[32];

  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);  
  tft->setFont();

  sprintf(buffer, "%.01f", floorf(minTemperature * 10) / 10);
  tft->setCursor(34, 45);
  tft->print(buffer);

  sprintf(buffer, "%.01f", floorf(maxTemperature * 10) / 10);
  tft->setCursor(84, 45);
  tft->print(buffer);
}

void ST7735Display::RenderMaxMinHumidity(float maxHumidity, float minHumidity)
{
  char buffer[32];

  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);  
  tft->setFont();

  sprintf(buffer, "%.01f", floorf(minHumidity * 10) / 10);
  tft->setCursor(34, 110);
  tft->print(buffer);

  sprintf(buffer, "%.01f", floorf(maxHumidity * 10) / 10);
  tft->setCursor(84, 110);
  tft->print(buffer);
}

void ST7735Display::CentreText(int y, char *text)
{
  int x = (_width / 2) - ((strlen(text) * 6) / 2);
  tft->setCursor(x,y);
  tft->print(text);
}

void ST7735Display::RenderWifiStatus(int level)
{
  GFXcanvas16 *rh_canvas = new GFXcanvas16(8, 8);
  switch (level)
  {
      case 0: rh_canvas->drawBitmap(0, 0, wifiOff, 8, 8, ST77XX_RED); break;
      case 1: rh_canvas->drawBitmap(0, 0, wifiPower1, 8, 8, ST77XX_RED); break;
      case 2: rh_canvas->drawBitmap(0, 0, wifiPower2, 8, 8, ST77XX_ORANGE); break;
      case 3: rh_canvas->drawBitmap(0, 0, wifiPower3, 8, 8, ST77XX_ORANGE); break;
      case 4: rh_canvas->drawBitmap(0, 0, wifiPower4, 8, 8, ST77XX_ORANGE); break;
      case 5: rh_canvas->drawBitmap(0, 0, wifiPower5, 8, 8, ST77XX_GREEN); break;
      default: rh_canvas->drawBitmap(0, 0, wifiPower6, 8, 8, ST77XX_GREEN); break;
  }

  tft->drawRGBBitmap(115, 140, rh_canvas->getBuffer(), 8, 8);
  delete rh_canvas;
}