# Enviromental NMEA 2000 sensors (pressure and temperature outside) with M5Atom

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- Can CAIS3050G module for M5Atom (for NMEA 2000)
- Env III Sensor with SHT30 and QMP6988 module for M5Atom
- Optional: M5Stack I2C hub (if you plan to connect more i2c sensors, like illuminance, IMU, etc)

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
