#include <Wire.h>
#include <Adafruit_AHT10.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "lcdfont.h"
#include "Interval.h"
#include "RingBuffer.h"

#define IOTHUB  "192.168.0.10"
#define IOTPORT "5555"

Interval *reportInterval = NULL;
Interval *displayInterval = NULL;

// SPI Serial clock out (SCLK)
// SPI Serial data out (DIN)
// SPI Data/Command select (D/C)
// SPI LCD chip select (CS)
// SPI LCD reset (RST)
#define SPI_SCLK  12
#define SPI_DIN   13
#define SPI_DC    15
#define SPI_CS    3
#define SPI_RST   1

// I2C Data
// I2C Clock
#define I2C_SDA   0
#define I2C_SCL   2

Adafruit_PCD8544 display = Adafruit_PCD8544(SPI_SCLK, SPI_DIN, SPI_DC, SPI_CS, SPI_RST);
Adafruit_AHT10 aht;

RingBuffer tempBuffer(12);
RingBuffer humidBuffer(12);

char* activity = "|/-\\";

bool hasAht10 = false;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.setTimeout(2000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }

  Wire.begin(I2C_SDA, I2C_SCL);

  display.begin();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setContrast(50);  
  display.clearDisplay();
  
  CentreText(0, "IOT Sensor");
  CentreText(10, "Monitor v1.0");
  display.display();

  if (aht.begin(&Wire, 0x32))
  {
    CentreText(24, "Temperature");
    CentreText(34, "Humidity");
    display.display();  

    hasAht10 = true;
  }
  
  delay(5000);
 
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
    sensors_event_t humidity, temperature;
    aht.getEvent(&humidity, &temperature);// populate temp and humidity objects with fresh data

    tempBuffer.Add(temperature.temperature);
    humidBuffer.Add(humidity.relative_humidity);
    
    float temp = tempBuffer.Average();
    float humid = humidBuffer.Average();

    // -----------------
    // Display the stats
    // -----------------
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

    if (!reportInterval->Ready())
      return;
      
    // -----------------------------
    // Send the stats to the IOT hub
    // -----------------------------
      
    sprintf(buffer, "\"temperature\" : %f, \"humidity\" : %f", temp, humid);
    WifiReport(buffer);
  }
}

void DisplayText(int x, int y, char *text)
{
  display.setCursor(x,y);
  display.println(text);
}

void CentreText(int y, char *text)
{
  int x = 42 - ((strlen(text) * 6) / 2);
  DisplayText(x,y,text);
}

void WifiStatus(char statusChar)
{
    display.fillRect(0,0,8,18, WHITE);
    display.setCursor(0,0);
    display.print(statusChar);
    display.display(); 
}

void WifiReport(char *sensorData)
{
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
}
