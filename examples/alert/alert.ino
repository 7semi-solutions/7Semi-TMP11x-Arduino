/**
 * ALERT Pin Setup (Therm Mode + Active-High + Limits)
 *
 * - Sets ALERT polarity to active-high
 * - Selects Therm mode (thermostat-style behavior)
 * - Sets low and high thresholds
 *
 * Typical usage:
 * - Connect ALERT pin to a GPIO input on your MCU
 * - Read the GPIO level to know whether the alert is active
 *
 * Notes:
 * - Exact ALERT behavior depends on TMP11x alert logic settings
 * - This example focuses on:
 *   - polarity
 *   - therm/alert selection
 *   - limit registers
 */

#include <7Semi_TMP11x.h>

TMP11x_7Semi tmp(Wire);

/**
 * Change this to your board GPIO connected to TMP11x ALERT pin.
 * - Example for ESP32: 4, 5, 18, etc.
 * - Example for UNO: 2, 3, etc.
 */
static const int ALERT_GPIO = 2;

void setup() {
  Serial.begin(115200);

  if (!tmp.begin(0x49)) {
    Serial.println("TMP11x not found!");
    while (1) delay(100);
  }

  /**
   * Configure MCU input pin for ALERT.
   * - If your board needs pullups/pulldowns, set them here
   */
  pinMode(ALERT_GPIO, INPUT);

  /**
   * Configure ALERT behavior in TMP11x.
   *
   * - Active-high:
   *   - ALERT pin goes HIGH when active
   *
   * - Therm mode:
   *   - Thermostat-style assertion/deassertion around thresholds
   */
  tmp.setAlertPolarity(ALERT_ACTIVE_HIGH);
  tmp.setThermAlertMode(THERM_MODE);

  /**
   * Configure threshold window.
   * - Example window: 20°C to 35°C
   */
  tmp.setLowLimit(20.0f);
  tmp.setHighLimit(35.0f);

  Serial.println("ALERT configured: ActiveHigh + Therm mode, limits: 20C..35C");
}

void loop() {
  float tC = 0.0f;

  if (tmp.readTemperatureC(tC)) {
    Serial.print("Temp: ");
    Serial.print(tC, 4);
    Serial.print(" C  |  ");
  } else {
    Serial.print("Temp: (read fail)  |  ");
  }

  /**
   * Read ALERT pin status from MCU GPIO.
   * - HIGH means alert active (because we set active-high)
   */
  int alertState = digitalRead(ALERT_GPIO);
  if (alertState == HIGH) {
    Serial.println("ALERT: ACTIVE");
  } else {
    Serial.println("ALERT: inactive");
  }

  delay(500);
}
