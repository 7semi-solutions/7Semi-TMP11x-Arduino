/**
 *  Basic Temperature Read (°C and °F)
 *
 * - Initializes TMP117 on default I2C (Wire)
 * - Reads temperature in Celsius and Fahrenheit
 * - Prints values to Serial
 *
 * Wiring (typical):
 * - SDA -> MCU SDA
 * - SCL -> MCU SCL
 * - VCC -> 3.3V / 5V (as per breakout)
 * - GND -> GND
 *
 * * Default I2C address:
 * - 0x48
 *
 * Possible I2C addresses (based on ADDR pin):
 * - 0x48 : ADDR = GND
 * - 0x49 : ADDR = VDD
 * - 0x4A : ADDR = SDA
 * - 0x4B : ADDR = SCL
 *
 * Supported devices:
 * - TMP117 : DEVICE_ID = 0x0117
 * - TMP116 : DEVICE_ID = 0x1116
 */

#include <7Semi_TMP11x.h>

//For Arduino
uint8_t i2CAddress = 0x49;
uint8_t sda = -1;  //Arduino A4
uint8_t scl = -1;  //Arduino A5
uint32_t clockSpeed = 400000;

//For ESP32
// uint8_t i2CAddress = 0x49;
// uint8_t sda = -1;  //Arduino 21
// uint8_t scl = -1;  //Arduino 22
// uint32_t clockSpeed = 400000;


TMP11x_7Semi tmp(Wire);

void setup() {

  Serial.begin(115200);

  /**
   * Start I2C and verify device ID.
   * - Default address for TMP11x is commonly 0x48 
   Possible address 
   */
  if (!tmp.begin(i2CAddress, sda, scl, clockSpeed)) {
    Serial.println("TMP11x not found or wrong device ID!");
    while (1) delay(100);
  }

  Serial.println("TMP11x initialized");
}

void loop() {
  float tC = 0.0f;

  if (tmp.readTemperatureC(tC)) {
    Serial.print("Temp: ");
    Serial.print(tC, 4);
    Serial.println(" C");
  } else {
    Serial.println("Temperature read failed");
  }

  delay(500);
}
