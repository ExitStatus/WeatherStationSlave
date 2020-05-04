#ifndef _PINS_H
#define _PINS_H

#ifdef ESP32

#define SPI_SCLK    18
#define SPI_MOSI    23
#define SPI_DC      21
#define SPI_CS      5
#define SPI_RST     17

#define I2C_SDA     25
#define I2C_SCL     26

#define BUTTON1     27

#define LIGHT_INPUT_PIN 33
#define BACKLIGHT_OUTPUT_PIN    16

#endif

#ifdef ESP8266

#define SPI_SCLK  14
#define SPI_MOSI  13
#define SPI_DC    15
#define SPI_CS    3
#define SPI_RST   1

#define I2C_SDA   5
#define I2C_SCL   4

#define BUTTON1   3

#define DIP1      10
#define DIP2      9

#define LIGHT_INPUT_PIN A0
#define BACKLIGHT_OUTPUT_PIN    16

#endif

#endif