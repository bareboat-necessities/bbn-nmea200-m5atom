# DIY NMEA 2000 to USB gateway in Actisense format on M5Atom

Effectively equivalent of NGT-1.

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for NMEA 2000)

## Setup in SignalK

To register gateway in SignalK

- find out /dev/tty* USB device of gateway (in your raspberry pi with BBN OS or other Linux)
- and add NMEA 2000 connection with Source Type (Actisense AGT-1 Canboat-js) baud rate: 115200
- Connect to USB 2.0 port on pi (I had drops of nmea 2000 datt stream when connected to USB 3.0 on external hub)

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
