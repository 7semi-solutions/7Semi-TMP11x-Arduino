/**
 * 7Semi TMP11x Library
 *
 * - Platform: Arduino / ESP32 / ESP8266 compatible
 * - Device: Texas Instruments TMP116/TMP117 high-accuracy digital temperature sensor
 * - Interface: I2C (7-bit address), 16-bit registers (MSB first)
 * - Units:
 *   - Temperature LSB = 0.0078125 °C (7.8125 m°C)
 *
 * - Default settings:
 *   - I2C address: 0x48
 *   - I2C clock: 400 kHz
 *
 * Notes:
 * - ESP32/ESP8266: SDA/SCL can be set in begin(), or use default pins with 0xFF
 */

#ifndef _7SEMI_TMP11X_H_
#define _7SEMI_TMP11X_H_

#include <Arduino.h>
#include <Wire.h>

/* ================= TMP116/TMP117 Register Map ================= */

/**
 * TMP116/TMP117 register map (16-bit registers, MSB first).
 *
 * - 0x00: Temperature result
 * - 0x01: Configuration
 * - 0x02: High limit
 * - 0x03: Low limit
 * - 0x04: EEPROM unlock
 * - 0x05: EEPROM1
 * - 0x06: EEPROM2
 * - 0x07: Temperature offset
 * - 0x08: EEPROM3
 * - 0x0F: Device ID
 */
#define REG_TEMP           0x00
#define REG_CONFIG         0x01
#define REG_T_HIGH         0x02
#define REG_T_LOW          0x03
#define REG_EEPROM_UL      0x04
#define REG_EEPROM1        0x05
#define REG_EEPROM2        0x06
#define REG_TEMP_OFFSET    0x07
#define REG_EEPROM3        0x08
#define REG_DEVICE_ID      0x0F

/* ================= Configuration Options ================= */

/**
 * Operating mode (CONFIG MOD[1:0], bits 11:10).
 *
 * - 0: Continuous conversion
 * - 1: Shutdown
 * - 2: Continuous conversion (same as 0, reads back as 0)
 * - 3: One-shot conversion
 */
typedef enum {
    CONTINUOUS_0 = 0,
    SHUTDOWN     = 1,
    CONTINUOUS_2 = 2,
    ONE_SHOT     = 3
} TMP11x_MODE;

/**
 * Averaging (CONFIG AVG[1:0], bits 6:5).
 *
 * - 0: No averaging
 * - 1: 8 averaged conversions
 * - 2: 32 averaged conversions
 * - 3: 64 averaged conversions
 */
typedef enum {
    AVG_NONE = 0,
    AVG_8    = 1,
    AVG_32   = 2,
    AVG_64   = 3
} TMP11x_AVG;

/**
 * Conversion cycle selector (CONFIG CONV[2:0], bits 9:7).
 *
 * - Selects the conversion cycle time in continuous conversion mode
 * - Cycle time also depends on AVG selection
 *
 * Common mapping:
 * - 0: 15.5 ms (AVG=0), 125 ms (AVG=1), 500 ms (AVG=2), 1 s (AVG=3)
 * - 1: 125 ms
 * - 2: 250 ms
 * - 3: 500 ms
 * - 4: 1 s
 * - 5: 4 s
 * - 6: 8 s
 * - 7: 16 s
 */
typedef enum {
    CONV_15P5MS  = 0,
    CONV_125MS   = 1,
    CONV_250MS   = 2,
    CONV_500MS   = 3,
    CONV_1S      = 4,
    CONV_4S      = 5,
    CONV_8S      = 6,
    CONV_16S     = 7
} TMP11x_CONV;

/**
 * Therm / Alert selection (CONFIG TM, bit 4).
 *
 * - 0: Alert mode
 * - 1: Therm mode
 */
typedef enum {
    ALERT_MODE = 0,
    THERM_MODE = 1
} TMP11x_THERM_ALERT;

/**
 * ALERT polarity (CONFIG POL, bit 3).
 *
 * - 0: Active low
 * - 1: Active high
 */
typedef enum {
    ALERT_ACTIVE_LOW  = 0,
    ALERT_ACTIVE_HIGH = 1
} TMP11x_ALERT_POLARITY;

/* ================= TMP11x Class ================= */

class TMP11x_7Semi {
public:
    /**
     * Constructor.
     *
     * - wirePort: I2C port instance (Wire / Wire1 / etc.)
     */
    TMP11x_7Semi(TwoWire &wirePort = Wire);

    /**
     * Initialize TMP116/TMP117 sensor.
     *
     * - i2cAddress: 7-bit I2C address (default 0x48)
     * - sda/scl:
     *   - ESP32/ESP8266 only: specify custom pins
     *   - Use 0xFF to keep default pins
     * - i2cClockSpeed: I2C bus speed in Hz (default 400000)
     *
     * - Returns:
     *   - true if device responds and DEVICE_ID is valid
     *   - false if device not found or wrong device
     */
    bool begin(uint8_t i2cAddress = 0x48,
               uint8_t sda = 0xFF,
               uint8_t scl = 0xFF,
               uint32_t i2cClockSpeed = 400000);

    /* ================= Temperature ================= */

    /**
     * Read raw temperature register.
     *
     * - rawTemperature is signed (two's complement)
     * - Convert to °C using:
     *   - °C = raw * 0.0078125
     */
    uint8_t readRawTemperature(int16_t &rawTemperature);

    /**
     * Read temperature in Celsius.
     */
    uint8_t readTemperatureC(float &temperatureC);

    /**
     * Read temperature in Fahrenheit.
     */
    uint8_t readTemperatureF(float &temperatureF);

    /* ================= Configuration ================= */

    /**
     * Read CONFIG register (raw 16-bit value).
     */
    uint8_t readConfig(uint16_t &config);

    /**
     * Write CONFIG register (raw 16-bit value).
     */
    uint8_t writeConfig(uint16_t config);

    /**
     * Soft reset the device.
     *
     * - Returns device to default configuration
     */
    uint8_t reset();

    /* ================= Conversion Rate ================= */

    /**
     * Set conversion cycle selector (CONFIG CONV[2:0]).
     *
     * - Use TMP11x_CONV for readable configuration
     */
    uint8_t setConversionRate(TMP11x_CONV conversionRate);

    /**
     * Get conversion cycle selector (CONFIG CONV[2:0]).
     *
     * - Returns raw value 0..7
     * - Cast to TMP11x_CONV if needed
     */
    uint8_t getConversionRate(uint8_t &conversionRate);

    /* ================= Averaging ================= */

    /**
     * Set averaging (CONFIG AVG[1:0]).
     *
     * - Use TMP11x_AVG for readable configuration
     */
    uint8_t setAveraging(TMP11x_AVG avg);

    /**
     * Get averaging (CONFIG AVG[1:0]).
     *
     * - Returns raw value 0..3
     * - Cast to TMP11x_AVG if needed
     */
    uint8_t getAveraging(uint8_t &avg);

    /* ================= Power / Mode ================= */

    /**
     * Set operating mode (CONFIG MOD[1:0]).
     *
     * - Use TMP11x_MODE:
     *   - CONTINUOUS_0
     *   - SHUTDOWN
     *   - CONTINUOUS_2
     *   - ONE_SHOT
     */
    uint8_t setMode(TMP11x_MODE mode);

    /**
     * Get operating mode (CONFIG MOD[1:0]).
     *
     * - Returns raw value 0..3
     * - Cast to TMP11x_MODE if needed
     */
    uint8_t getMode(uint8_t &mode);

    /* ================= Alert Limits ================= */

    /**
     * Set high temperature limit (T_HIGH).
     */
    uint8_t setHighLimit(float tempC);

    /**
     * Set low temperature limit (T_LOW).
     */
    uint8_t setLowLimit(float tempC);

    /**
     * Read high temperature limit (T_HIGH).
     */
    uint8_t getHighLimit(float &tempC);

    /**
     * Read low temperature limit (T_LOW).
     */
    uint8_t getLowLimit(float &tempC);

    /* ================= Offset ================= */

    /**
     * Set temperature offset (TEMP_OFFSET).
     *
     * - Use this to calibrate out a fixed error
     * - Example: if sensor reads +0.3 °C too high, set offset to -0.3 °C
     */
    uint8_t setOffset(float offsetC);

    /**
     * Read temperature offset (TEMP_OFFSET).
     */
    uint8_t getOffset(float &offsetC);

    /* ================= Therm / Alert Mode ================= */

    /**
     * Set Therm / Alert selection (CONFIG TM).
     *
     * - Use TMP11x_THERM_ALERT:
     *   - ALERT_MODE
     *   - THERM_MODE
     */
    uint8_t setThermAlertMode(TMP11x_THERM_ALERT mode);

    /**
     * Get Therm / Alert selection (CONFIG TM).
     *
     * - Returns raw value 0..1
     * - Cast to TMP11x_THERM_ALERT if needed
     */
    uint8_t getThermAlertMode(uint8_t &mode);

    /* ================= Alert Polarity ================= */

    /**
     * Set ALERT polarity (CONFIG POL).
     *
     * - Use TMP11x_ALERT_POLARITY:
     *   - ALERT_ACTIVE_LOW
     *   - ALERT_ACTIVE_HIGH
     */
    uint8_t setAlertPolarity(TMP11x_ALERT_POLARITY active_high);

    /**
     * Get ALERT polarity (CONFIG POL).
     *
     * - Returns raw value 0..1
     * - Cast to TMP11x_ALERT_POLARITY if needed
     */
    uint8_t getAlertPolarity(uint8_t &active_high);

    /* ================= EEPROM ================= */

    /**
     * Read EEPROM scratch register.
     *
     * - Allowed:
     *   - REG_EEPROM1
     *   - REG_EEPROM2
     *   - REG_EEPROM3
     */
    uint8_t readEEPROM(uint8_t reg, uint16_t &value);

    /**
     * Write EEPROM scratch register.
     *
     * - Allowed:
     *   - REG_EEPROM1
     *   - REG_EEPROM2
     *   - REG_EEPROM3
     * - EEPROM programming takes time; library applies a short delay after write
     */
    uint8_t writeEEPROM(uint8_t reg, uint16_t value);

    /* ================= Device ================= */

    /**
     * Read DEVICE_ID register.
     *
     * - TMP117 expected value: 0x0117
     * - TMP116 expected value differs by variant
     */
    uint8_t getDeviceID(uint16_t &deviceID);

private:
    TwoWire *i2c;
    uint8_t address;

    /* ================= Low-Level I2C ================= */

    /**
     * Read 16-bit register (MSB first).
     */
    uint8_t readReg(uint8_t reg, uint16_t &value);

    /**
     * Write 16-bit register (MSB first).
     */
    uint8_t writeReg(uint8_t reg, uint16_t value);

    /* ================= EEPROM Lock Control ================= */

    /**
     * Unlock EEPROM for write access.
     *
     * - Writes the unlock sequence to EEPROM_UL register
     * - Call before writing EEPROM registers (if your implementation requires it)
     */
    uint8_t unlockEEPROM();

    /**
     * Lock EEPROM to prevent writes.
     *
     * - Restores EEPROM to locked state (if supported/required)
     */
    uint8_t lockEEPROM();

    /* ================= Helpers ================= */

    /**
     * Convert raw temperature code to Celsius.
     *
     * - 1 LSB = 0.0078125 °C
     */
    float rawToCelsius(int16_t raw);

    /**
     * Convert Celsius to raw temperature code.
     */
    int16_t celsiusToRaw(float temp);
};

#endif
