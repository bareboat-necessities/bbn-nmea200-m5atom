# N2K Attitude (pitch and heel) with M5Atom

Powered by USB. CanBus isolated. (Connect H and L only)

This one is just for illutration purposes because you need compass (magnetometer)
to know what is pitch and what is heel.

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for N2K)
- MPU6886 module for M5Atom
- Optional: M5Stack I2C hub (if you plan to connect more i2c sensors, like illuminance, IMU, etc)

## Flashing firmware on Bareboat Necessities OS

````
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-m5-n2k-att.sh
chmod +x bbn-flash-m5-n2k-att.sh
./bbn-flash-m5-n2k-att.sh -p /dev/ttyUSB0
````

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
