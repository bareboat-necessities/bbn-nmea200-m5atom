# Enviromental N2K sensors (pressure and temperature outside) with M5Atom

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for N2K)
- Env III Sensor with SHT30 and QMP6988 module for M5Atom
- Optional: M5Stack I2C hub (if you plan to connect more i2c sensors, like illuminance, IMU, etc)

## Flashing firmware on Bareboat Necessities OS

````
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-m5-n2k-env3.sh
chmod +x bbn-flash-m5-n2k-env3.sh
./bbn-flash-m5-n2k-env3.sh -p /dev/ttyUSB0
````

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
