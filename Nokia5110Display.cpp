#include "Nokia5110Display.h"


Nokia5110Display::Nokia5110Display()
{
  lcd = new Adafruit_PCD8544(SPI_SCLK, SPI_MOSI, SPI_DC, SPI_CS, SPI_RST);
  _width = 84;
  _height = 48;
  _capy = 14;
}

Nokia5110Display::~Nokia5110Display()
{
  delete lcd;
}

void Nokia5110Display::Display()
{
  lcd->display();
}

void Nokia5110Display::InitRender()
{ 
  lcd->begin();
  lcd->setTextColor(BLACK);
  lcd->setTextSize(1);
  lcd->setContrast(50);  
  lcd->clearDisplay();
}

void Nokia5110Display::BackgroundRender()
{
    lcd->setFont();
    CentreText(15, "Celsius");
    CentreText(40, "%Humidity");
}

void Nokia5110Display::Logo()
{
  CentreText(0, "Sensor v1.0");
}

void Nokia5110Display::HasCapability(char *capability)
{
  CentreText(_capy, capability);
  _capy += 10;
}

void Nokia5110Display::RenderTemperature(float temperature)
{
  char buffer[32];

  sprintf(buffer, "%.02f", temperature);

}

void Nokia5110Display::RenderHumidity(float humidity)
{
  char buffer[32];

  sprintf(buffer, "%.02f", humidity);
}

void Nokia5110Display::RenderMaxMinTemperature(float maxTemperature, float minTemperature)
{
  char buffer[32];

  sprintf(buffer, "%.02f", maxTemperature);

  sprintf(buffer, "%.02f", minTemperature);
}

void Nokia5110Display::RenderMaxMinHumidity(float maxHumidity, float minHumidity)
{

}

void Nokia5110Display::CentreText(int y, char *text)
{
  int x = (_width / 2) - ((strlen(text) * 6) / 2);
  lcd->setCursor(x,y);
  lcd->print(text);
}