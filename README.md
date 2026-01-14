# 7Semi TMP11x Arduino Library

Arduino library for Texas Instruments **TMP116** and **TMP117**
high-accuracy digital temperature sensors.

---

## Overview

- Device family: TMP116 / TMP117
- Manufacturer: Texas Instruments
- Interface: I2C
- Register width: 16-bit (MSB first)
- Default I2C address: 0x48

---

## Features

- Supports TMP116 and TMP117
- Compatible with:
  - Arduino
  - ESP32
- Temperature output:
  - Celsius (°C)
  - Fahrenheit (°F)
- Configurable:
  - Conversion rate
  - Averaging
  - Power modes (continuous / shutdown / one-shot)
  - Therm / Alert mode
  - ALERT pin polarity
  - High / Low temperature limits
  - Temperature offset
- EEPROM read/write support

---

## Temperature Resolution

- 1 LSB = **0.0078125 °C**
- Resolution = **7.8125 m°C**

---

## Installation

### Arduino Library Manager

- Open Arduino IDE
- Sketch → Include Library → Manage Libraries
- Search for **7Semi TMP11x**
- Install

### Manual Installation

- Download repository as ZIP
- Extract to:
Documents/Arduino/libraries/

arduino
Copy code
- Restart Arduino IDE

---



# Configuration Enums
## Power / Mode
CONTINUOUS

SHUTDOWN

CONTINUOUS

ONE_SHOT

## Averaging
AVG_NONE

AVG_8

AVG_32

AVG_64

##  Conversion Rate
CONV_15P5MS

CONV_125MS

CONV_250MS

CONV_500MS

CONV_1S

CONV_4S

CONV_8S

CONV_16S

##  Therm / Alert
ALERT_MODE

THERM_MODE

##  ALERT Polarity
ALERT_ACTIVE_LOW

ALERT_ACTIVE_HIGH

## License
MIT License.
