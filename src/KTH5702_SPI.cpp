#include "KTH5702_SPI.h"

// SPI commands
#define CMD_IDLE            0x10
#define CMD_SINGLE_CONV     0x30
#define CMD_READ_DATA       0x50
#define CMD_RESET           0x70

// Register addresses
#define REG_STATUS           0x06
#define STATUS_DRDY_BIT      0

KTH5702_SPI::KTH5702_SPI(SPIClass &spi, uint8_t csPin)
  : _spi(&spi), _csPin(csPin), _spiSettings(5000000, MSBFIRST, SPI_MODE3) {}

void KTH5702_SPI::begin() {
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  _spi->begin();
  delay(5); // Ensure device startup > 4ms
}

void KTH5702_SPI::writeCommand(uint8_t cmd) {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(cmd);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

uint16_t KTH5702_SPI::readRegister(uint8_t reg) {
  uint16_t result = 0;
  uint8_t address = (reg << 2) | 0x01;
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(address);
  result = _spi->transfer(0x00) << 8;
  result |= _spi->transfer(0x00);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
  return result;
}

void KTH5702_SPI::writeRegister(uint8_t reg, uint16_t value) {
  uint8_t address = (reg << 2) & 0xFC;
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(address);
  _spi->transfer((value >> 8) & 0xFF);
  _spi->transfer(value & 0xFF);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

bool KTH5702_SPI::reset() {
  writeCommand(CMD_IDLE);
  delayMicroseconds(200);
  writeCommand(CMD_RESET);
  delay(10);
  uint16_t status = readRegister(REG_STATUS);
  return (status >> 1) & 0x01;  // softRst bit
}

bool KTH5702_SPI::startSingleConversion() {
  writeCommand(CMD_IDLE);
  delayMicroseconds(200);
  writeCommand(CMD_SINGLE_CONV);
  delay(10);
  return true;
}

bool KTH5702_SPI::isDataReady() {
  uint16_t status = readRegister(REG_STATUS);
  return status & (1 << STATUS_DRDY_BIT);
}

bool KTH5702_SPI::readAngle(float &angleDeg) {
  writeCommand(CMD_READ_DATA);
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(CMD_READ_DATA);
  _spi->transfer(0x00); // status, discard
  uint16_t angleRaw = _spi->transfer(0x00) << 8;
  angleRaw |= _spi->transfer(0x00);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();

  angleDeg = ((float)angleRaw * 360.0) / 65536.0;
  return true;
}