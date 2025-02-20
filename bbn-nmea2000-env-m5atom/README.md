# Enviromental N2K sensors (pressure and temperature outside) with M5Atom

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for N2K)
- Env III Sensor with SHT30 and QMP6988 module for M5Atom
- Optional: M5Stack I2C hub (if you plan to connect more i2c sensors, like illuminance, IMU, etc)

## Flashing firmware on Bareboat Necessities OS

````
# shutdown signalk
sudo systemctl stop signalk

if [ -f bbn-flash-m5-n2k-env3.sh ]; then rm bbn-flash-m5-n2k-env3.sh; fi
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-m5-n2k-env3.sh
chmod +x bbn-flash-m5-n2k-env3.sh
./bbn-flash-m5-n2k-env3.sh -p /dev/ttyUSB0
````

## Power from NMEA 2000

You can step down power provided by
NMEA 2000 bus and direct it to USB power. 

The 12v can be reduced to 5 Volt with a DC Step-Down_Converter (D24V10F5, https://www.pololu.com/product/2831)

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010

## Alternatives

For m5stack atomS3-lite version look at: https://github.com/bareboat-necessities/bbn-m5-s3-n2k-i2c
