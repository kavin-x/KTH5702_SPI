#include <SPI.h>
#include "KTH5702_SPI.h"

SPIClass SPI_2(PA7, PA6, PA5); // SCLK, MISO, MOSI
#define CS_PIN PB2             // Chip Select pin 

KTH5702_SPI sensor(SPI_2, CS_PIN);  // Use SPI_3

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Serial Monitor

  sensor.begin();

  if (sensor.reset()) {
    Serial.println("Sensor reset successful.");
  } else {
    Serial.println("Sensor reset failed.");
  }
}

void loop() {
  sensor.startSingleConversion();
  delay(10);  // Wait for measurement

  if (sensor.isDataReady()) {
    float angle;
    if (sensor.readAngle(angle)) {
      Serial.print("Angle: ");
      Serial.print(angle, 2);  // 2 decimal places
      Serial.println(" °");
    } else {
      Serial.println("Failed to read angle.");
    }
  } else {
    Serial.println("Data not ready.");
  }

  delay(200);  // Update every 200 ms
}
