# N2K Attitude (pitch and heel) with M5Atom

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for N2K)
- MPU6886 module for M5Atom
- Optional: M5Stack I2C hub (if you plan to connect more i2c sensors, like illuminance, IMU, etc)

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
