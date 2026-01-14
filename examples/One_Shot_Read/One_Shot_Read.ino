/**
 * One Shot Read (Shutdown + One-Shot Read)
 *
 * - Keeps TMP11x in shutdown most of the time
 * - Triggers one-shot conversion only when needed
 * - Useful for battery-powered applications
 *
 * Notes:
 * - One-shot conversion time depends on:
 *   - Conversion rate setting
 *   - Averaging setting
 * - Use a small delay after one-shot before reading temperature
 */

#include <7Semi_TMP11x.h>

TMP11x_7Semi tmp(Wire);

void setup() {
  Serial.begin(115200);

  if (!tmp.begin(0x49)) {
    Serial.println("TMP11x not found!");
    while (1) delay(100);
  }

  /**
   * Optional: Choose a faster conversion for quicker one-shot.
   * - CONV_125MS is a common fast-but-not-minimum option
   */
  tmp.setConversionRate(CONV_125MS);

  /**
   * Optional: No averaging for fastest one-shot response.
   */
  tmp.setAveraging(AVG_NONE);

  /**
   * Put device in shutdown for low power idle.
   */
  tmp.setMode(SHUTDOWN);

  Serial.println("One-shot mode demo ready");
}

void loop() {
  float tC = 0.0f;

  /**
   * Trigger a one-shot conversion.
   * - Device performs one conversion cycle
   */
  tmp.setMode(ONE_SHOT);

  /**
   * Wait for conversion to complete.
   * - Tune this delay based on your conversion/averaging settings
   */
  delay(30);

  /**
   * Read temperature result.
   */
  if (tmp.readTemperatureC(tC)) {
    Serial.print("One-shot Temp: ");
    Serial.print(tC, 4);
    Serial.println(" C");
  } else {
    Serial.println("One-shot read failed");
  }

  /**
   * Return to shutdown to save power.
   */
  tmp.setMode(SHUTDOWN);

  /**
   * Sample every 2 seconds.
   */
  delay(2000);
}
