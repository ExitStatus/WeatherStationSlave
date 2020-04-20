#ifndef _PINS_H
#define _PINS_H

#ifdef ESP32

#define SPI_SCLK    18
#define SPI_MOSI    23
#define SPI_DC      21
#define SPI_CS      5
#define SPI_RST     17

#define I2C_SDA   25
#define I2C_SCL   26

#endif

#ifdef ESP8266

#define SPI_SCLK  12
#define SPI_MOSI  13
#define SPI_DC    15
#define SPI_CS    3
#define SPI_RST   1

#define I2C_SDA   0
#define I2C_SCL   2

#endif

#endif