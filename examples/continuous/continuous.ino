/**
 *  Continuous Temperature read (Averaging + Conversion Rate)
 *
 * - Sets conversion cycle to 1s
 * - Enables averaging (32 samples)
 * - Reads temperature continuously
 *
 * Why this helps:
 * - Averaging reduces noise/jitter in readings
 * - Slower conversion reduces update rate but improves stability
 */

#include <7Semi_TMP11x.h>

TMP11x_7Semi tmp;

void setup() {
  Serial.begin(115200);

  if (!tmp.begin(0x49)) {
    Serial.println("TMP11x not found!");
    while (1) delay(100);
  }

  /**
   * Configure for stable readings.
   *
   * - Conversion rate option:
   *   - CONV_1S: conversion cycle about 1 second
   *
   * - Averaging option:
   *   - AVG_32: averages 32 conversions internally
   */
  tmp.setConversionRate(CONV_1S);
  tmp.setAveraging(AVG_32);

  /**
   * Keep device in continuous mode.
   */
  tmp.setMode(CONTINUOUS_0);

  Serial.println("Configured: CONV=1s, AVG=32, MODE=continuous");
}

void loop() {
  float tC = 0.0f;

  /**
   * Read temperature.
   * - Even if loop runs faster, sensor updates based on conversion settings
   */
  if (tmp.readTemperatureC(tC)) {
    Serial.print("Temp: ");
    Serial.print(tC, 4);
    Serial.println(" C");
  } else {
    Serial.println("Read failed");
  }

  delay(250);
}
