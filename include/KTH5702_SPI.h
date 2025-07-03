/**
 * KTH5702_SPI.h - Arduino Library for KTH5702 2D Hall Angle Sensor (SPI Mode)
 * Author: Kavin-x
 */

#ifndef KTH5702_SPI_H
#define KTH5702_SPI_H

#include <Arduino.h>
#include <SPI.h>

class KTH5702_SPI {
  public:
    KTH5702_SPI(SPIClass &spi, uint8_t csPin);
    void begin();

    bool reset();
    bool startSingleConversion();
    bool readAngle(float &angleDeg);
    bool isDataReady();

  private:
    SPIClass *_spi;
    uint8_t _csPin;
    SPISettings _spiSettings;

    void writeCommand(uint8_t cmd);
    uint16_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint16_t value);
};

#endif

