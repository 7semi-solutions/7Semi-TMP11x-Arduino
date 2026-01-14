/**
 * 7Semi TMP11x Library (TMP117)
 *
 * - Platform: Arduino / ESP32 / ESP8266 compatible (Wire / TwoWire)
 * - Device: Texas Instruments TMP117 high-accuracy digital temperature sensor
 * - Interface: I2C (7-bit address), 16-bit registers (MSB first)
 * - Units:
 *   - Temperature register LSB = 0.0078125 °C (7.8125 m°C)
 * - Notes:
 *   - ESP32/ESP8266 can optionally use custom SDA/SCL pins
 *   - EEPROM writes require a short internal write time (~10 ms)
 */

#include "7Semi_TMP11x.h"

TMP11x_7Semi::TMP11x_7Semi(TwoWire &wirePort) {
    i2c = &wirePort;
}

/* ================= Initialization ================= */

/**
 * Initialize the TMP117 device on the given I2C bus.
 *
 * - Sets I2C address and initializes Wire
 * - Optionally configures SDA/SCL on supported platforms (ESP32/ESP8266)
 * - Sets I2C clock
 * - Verifies device ID (expects 0x0117)
 */
bool TMP11x_7Semi::begin(uint8_t i2cAddress, uint8_t sda, uint8_t scl, uint32_t i2cClockSpeed) {
    address = i2cAddress;

#if defined(ESP32) || defined(ESP8266)
    /**
     * Platforms that support custom SDA/SCL pins
     *
     * - If valid pins are provided, initialize Wire on those pins
     * - Otherwise use default pins
     */
    if (sda != 0xFF && scl != 0xFF) {
        i2c->begin(sda, scl);
    } else {
        i2c->begin();
    }
#else
    /**
     * Platforms with fixed SDA/SCL pins
     */
    i2c->begin();
#endif

    (void)scl;
    (void)sda;
    /**
     * Configure I2C clock speed for the bus
     */
    i2c->setClock(i2cClockSpeed);

    /**
     * Confirm correct device is connected by checking DEVICE_ID
     */
    uint16_t deviceID;
    if (!getDeviceID(deviceID))
        return false;

    if ((deviceID == 0x0117) || (deviceID == 0x1116))
        return true;

    return false;
}

/* ================= Device ================= */

/**
 * Read the TMP117 device ID register.
 *
 * - Expected TMP117 ID: 0x0117
 */
uint8_t TMP11x_7Semi::getDeviceID(uint16_t &deviceID) {
    return readReg(REG_DEVICE_ID, deviceID);
}

/* ================= Temperature ================= */

/**
 * Read raw temperature register value.
 *
 * - Returns signed 16-bit raw temperature (two's complement)
 * - Convert using rawToCelsius() for °C
 */
uint8_t TMP11x_7Semi::readRawTemperature(int16_t &rawTemperature) {
    uint16_t raw;
    if (!readReg(REG_TEMP, raw))
        return false;
    rawTemperature = (int16_t)raw;
    return true;
}

/**
 * Read temperature in Celsius.
 *
 * - Reads raw temperature and converts to °C
 */
uint8_t TMP11x_7Semi::readTemperatureC(float &temperatureC) {
    int16_t raw;
    if (!readRawTemperature(raw))
        return false;
    temperatureC = rawToCelsius(raw);
    return true;
}

/**
 * Read temperature in Fahrenheit.
 *
 * - Reads temperature in °C and converts to °F
 */
uint8_t TMP11x_7Semi::readTemperatureF(float &temperatureF) {
    float tempC;
    if (!readTemperatureC(tempC))
        return false;
    temperatureF = (tempC * 1.8f) + 32.0f;
    return true;
}

/* ================= Configuration ================= */

/**
 * Read the TMP117 configuration register.
 */
uint8_t TMP11x_7Semi::readConfig(uint16_t &config) {
    return readReg(REG_CONFIG, config);
}

/**
 * Write the TMP117 configuration register.
 */
uint8_t TMP11x_7Semi::writeConfig(uint16_t config) {
    return writeReg(REG_CONFIG, config);
}

/**
 * Soft reset the device.
 *
 * - Writes the reset bit in CONFIG register
 */
uint8_t TMP11x_7Semi::reset() {
    return writeConfig(0x8000);  // Soft reset
}

/* ================= Conversion Rate ================= */

/**
 * Set conversion rate (CONFIG[9:7]).
 *
 * - Uses 3-bit field (0..7)
 * - Exact mapping depends on TMP117 datasheet table
 */
uint8_t TMP11x_7Semi::setConversionRate(TMP11x_CONV conversionRate) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    cfg &= ~(0x07 << 7);
    cfg |= ((conversionRate & 0x07) << 7);
    return writeConfig(cfg);
}

/**
 * Get conversion rate (CONFIG[9:7]).
 */
uint8_t TMP11x_7Semi::getConversionRate(uint8_t &conversionRate) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    conversionRate = (cfg >> 7) & 0x07;
    return true;
}

/* ================= Averaging ================= */

/**
 * Set averaging (CONFIG[6:5]).
 *
 * - Uses 2-bit field (0..3)
 * - Exact sample count depends on TMP117 datasheet table
 */
uint8_t TMP11x_7Semi::setAveraging(TMP11x_AVG avg) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    cfg &= ~(0x03 << 5);
    cfg |= ((avg & 0x03) << 5);
    return writeConfig(cfg);
}

/**
 * Get averaging (CONFIG[6:5]).
 */
uint8_t TMP11x_7Semi::getAveraging(uint8_t &avg) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    avg = (cfg >> 5) & 0x03;
    return true;
}

/* ================= Power / Mode ================= */

/**
 * Set device operating mode (CONFIG[11:10]).
 *
 * - Uses 2-bit field (0..3)
 * - Exact meaning depends on TMP11x datasheet table (continuous / shutdown / one-shot)
 */
uint8_t TMP11x_7Semi::setMode(TMP11x_MODE  mode) {
    uint16_t cfg;
     /* Read current configuration */
    if (!readConfig(cfg))
        return false;

    /* Clear mode bits CONFIG[11:10] */
    cfg &= ~(0x03 << 10);

    /* Set new mode */
    cfg |= ((uint16_t)mode & 0x03) << 10;

    /* Write configuration */
    return writeConfig(cfg);
}

/**
 * Get device operating mode (CONFIG[11:10]).
 */
uint8_t TMP11x_7Semi::getMode(uint8_t &mode) {
    uint16_t cfg;

    /* Read configuration */
    if (!readConfig(cfg))
        return false;

    /* Extract mode bits */
    mode = (uint8_t)((cfg >> 10) & 0x03);
    return true;
}

/* ================= Alert Limits ================= */

/**
 * Set high temperature limit threshold.
 *
 * - Value is stored in TMP117 format (same LSB scaling as temperature)
 */
uint8_t TMP11x_7Semi::setHighLimit(float tempC) {
    return writeReg(REG_T_HIGH, celsiusToRaw(tempC));
}

/**
 * Set low temperature limit threshold.
 */
uint8_t TMP11x_7Semi::setLowLimit(float tempC) {
    return writeReg(REG_T_LOW, celsiusToRaw(tempC));
}

/**
 * Read high temperature limit threshold.
 */
uint8_t TMP11x_7Semi::getHighLimit(float &tempC) {
    uint16_t raw;
    if (!readReg(REG_T_HIGH, raw))
        return false;
    tempC = rawToCelsius((int16_t)raw);
    return true;
}

/**
 * Read low temperature limit threshold.
 */
uint8_t TMP11x_7Semi::getLowLimit(float &tempC) {
    uint16_t raw;
    if (!readReg(REG_T_LOW, raw))
        return false;
    tempC = rawToCelsius((int16_t)raw);
    return true;
}

/* ================= Offset ================= */

/**
 * Set temperature offset (TEMP_OFFSET register).
 *
 * - Used for simple calibration adjustment
 */
uint8_t TMP11x_7Semi::setOffset(float offsetC) {
    return writeReg(REG_TEMP_OFFSET, celsiusToRaw(offsetC));
}

/**
 * Read temperature offset (TEMP_OFFSET register).
 */
uint8_t TMP11x_7Semi::getOffset(float &offsetC) {
    uint16_t raw;
    if (!readReg(REG_TEMP_OFFSET, raw))
        return false;
    offsetC = rawToCelsius((int16_t)raw);
    return true;
}

/* ================= Alert Polarity ================= */

/**
 * Set ALERT pin polarity (CONFIG[3]).
 *
 * - 0 = Active Low
 * - 1 = Active High
 */
uint8_t TMP11x_7Semi::setAlertPolarity(TMP11x_ALERT_POLARITY active_high) {
    uint16_t config;

    /**
     * Read current configuration
     */
    if (!readConfig(config))
        return false;

    /**
     * Set or clear polarity bit
     */
    if (active_high)
        config |= (1 << 3);
    else
        config &= ~(1 << 3);

    /**
     * Write updated configuration
     */
    return writeConfig(config);
}

/**
 * Get ALERT pin polarity (CONFIG[3]).
 *
 * - 0 = Active Low
 * - 1 = Active High
 */
uint8_t TMP11x_7Semi::getAlertPolarity(uint8_t &active_high) {
    uint16_t config;

    /**
     * Read configuration register
     */
    if (!readConfig(config))
        return false;

    /**
     * Extract polarity bit
     */
    active_high = (config >> 3) & 0x01;
    return true;
}

/* ================= Therm / Alert Mode ================= */

/**
 * Set alert behavior mode (CONFIG[4] TM bit).
 *
 * - 0 = Alert mode
 * - 1 = Therm mode
 */
uint8_t TMP11x_7Semi::setThermAlertMode(TMP11x_THERM_ALERT mode) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    cfg &= ~(1 << 4);
    cfg |= ((mode & 0x01) << 4);

    return writeConfig(cfg);
}

/**
 * Get alert behavior mode (CONFIG[4] TM bit).
 *
 * - 0 = Alert mode
 * - 1 = Therm mode
 */
uint8_t TMP11x_7Semi::getThermAlertMode(uint8_t &mode) {
    uint16_t cfg;
    if (!readConfig(cfg))
        return false;

    mode = (cfg >> 4) & 0x01;
    return true;
}

/* ================= EEPROM Lock / Unlock ================= */

/**
 * Unlock EEPROM for write operations
 */
uint8_t TMP11x_7Semi::unlockEEPROM() {
    uint16_t ul;

    /* Set unlock bit (BIT15 = 1) */
    ul = 0x8000;

    /* Write back */
    if (!writeReg(REG_EEPROM_UL, ul))
        return false;

    delay(2);  // Small settle delay
    return true;
}

/**
 * Lock EEPROM to prevent writes
 */
uint8_t TMP11x_7Semi::lockEEPROM() {
    uint16_t ul;

    /* Clear unlock bit (BIT15 = 0) */
    ul = 0x0000;

    if (!writeReg(REG_EEPROM_UL, ul))
        return false;

    delay(2);
    return true;
}

/* ================= EEPROM ================= */

/**
 * Read EEPROM register value.
 *
 * - From EEPROM1/EEPROM2/EEPROM3 
 */
uint8_t TMP11x_7Semi::readEEPROM(uint8_t reg, uint16_t &value) {

    if (reg != REG_EEPROM1 && reg != REG_EEPROM2 && reg != REG_EEPROM3)
        return false;

    return readReg(reg, value);
}

/**
 * Write EEPROM register value.
 *
 * - TO EEPROM1/EEPROM2/EEPROM3
 * - Device requires an internal write time after write
 */
uint8_t TMP11x_7Semi::writeEEPROM(uint8_t reg, uint16_t value) {

    if (reg != REG_EEPROM1 && reg != REG_EEPROM2 && reg != REG_EEPROM3)
        return false;

    /* Unlock EEPROM */
    if (!unlockEEPROM())
        return false;

    /**
     * Write to EEPROM
     */
    if (!writeReg(reg, value))
        return false;

    /**
     * EEPROM internal write delay
     */
    delay(10);

    /* Lock EEPROM */
    lockEEPROM();

    return true;
}

/* ================= Low-Level I2C ================= */

/**
 * Read a 16-bit register (MSB first).
 *
 * - Uses a repeated start (endTransmission(false)) for proper register reads
 */
uint8_t TMP11x_7Semi::readReg(uint8_t reg, uint16_t &value) {
    i2c->beginTransmission(address);
    i2c->write(reg);
    if (i2c->endTransmission(false) != 0)
        return false;

    if (i2c->requestFrom(address, (uint8_t)2) != 2)
        return false;

    value = ((uint16_t)i2c->read() << 8) | i2c->read();
    return true;
}

/**
 * Write a 16-bit register (MSB first).
 */
uint8_t TMP11x_7Semi::writeReg(uint8_t reg, uint16_t value) {
    i2c->beginTransmission(address);
    i2c->write(reg);
    i2c->write(value >> 8);
    i2c->write(value & 0xFF);
    return (i2c->endTransmission() == 0);
}

/* ================= Helpers ================= */

/**
 * Convert raw TMP117 temperature to Celsius.
 *
 * - 1 LSB = 0.0078125 °C (7.8125 m°C)
 */
float TMP11x_7Semi::rawToCelsius(int16_t raw) {
    return raw * 0.0078125f;
}

/**
 * Convert Celsius to raw TMP117 temperature format.
 *
 * - Inverse of rawToCelsius()
 */
int16_t TMP11x_7Semi::celsiusToRaw(float temp) {
    return (int16_t)(temp / 0.0078125f);
}
