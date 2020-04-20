#include "ST7735Display.h"

struct eraseArea_t
{
  int16_t x = 0;
  int16_t y = 0;
  uint16_t w = 0;
  uint16_t h = 0;
} tempArea, humidArea;

ST7735Display::ST7735Display()
{
  tft = new Adafruit_ST7735(SPI_CS, SPI_DC, SPI_MOSI, SPI_SCLK, SPI_RST);
  tft->setSPISpeed(400000000);

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
  tft->print("Max");
  tft->setCursor(64, 45);
  tft->print("Min");

  tft->setCursor(14, 110);
  tft->print("Max");
  tft->setCursor(64, 110);
  tft->print("Min");
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
  
  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft->setFont(&Dialog_bold_24);
  tft->setCursor(16, 39);
  tft->fillRect(tempArea.x, tempArea.y-1, tempArea.w, tempArea.h, ST77XX_BLACK);

  tft->print(buffer);
  tft->print(" C");

  tft->getTextBounds(buffer, 16, 40, &x1, &y1, &w, &h);
  tft->drawCircle(x1 + w + 5, y1 + 4, 4, ST77XX_WHITE);

  sprintf(buffer, "%.01f C", temperature);
  tft->getTextBounds(buffer, 16, 40, &tempArea.x, &tempArea.y, &tempArea.w, &tempArea.h);

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
  
  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft->setFont(&Dialog_bold_24);
  tft->setCursor(20, 104);
  tft->fillRect(humidArea.x, humidArea.y, humidArea.w, humidArea.h, ST77XX_BLACK);

  tft->print(buffer);
  tft->getTextBounds(buffer, 20, 104, &humidArea.x, &humidArea.y, &humidArea.w, &humidArea.h);

  _currentHumid = humidity;
}

void ST7735Display::RenderMaxMinTemperature(float maxTemperature, float minTemperature)
{
  char buffer[32];

  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);  
  tft->setFont();

  sprintf(buffer, "%.01f", floorf(maxTemperature * 10) / 10);
  tft->setCursor(34, 45);
  tft->print(buffer);

  sprintf(buffer, "%.01f", floorf(minTemperature * 10) / 10);
  tft->setCursor(84, 45);
  tft->print(buffer);
}

void ST7735Display::RenderMaxMinHumidity(float maxHumidity, float minHumidity)
{
  char buffer[32];

  tft->setTextColor(ST77XX_WHITE, ST77XX_BLACK);  
  tft->setFont();

  sprintf(buffer, "%.01f", floorf(maxHumidity * 10) / 10);
  tft->setCursor(34, 110);
  tft->print(buffer);

  sprintf(buffer, "%.01f", floorf(minHumidity * 10) / 10);
  tft->setCursor(84, 110);
  tft->print(buffer);
}

void ST7735Display::CentreText(int y, char *text)
{
  int x = (_width / 2) - ((strlen(text) * 6) / 2);
  tft->setCursor(x,y);
  tft->print(text);
}