# DIY N2K to USB gateway in Actisense format on M5Atom

Effectively equivalent of NGT-1.

Powered by USB. CanBus isolated. (Connect H and L only)

## Required Hardware:

- M5Atom Lite
- CAN CAIS3050G module for M5Atom (for N2K)

## Suggested enclosure and connectors

Waterproof ABS Plastic Enclosure Box Electronic Ip67 Flanged, Clear Cover, Size 100x68x50 (mm)

https://www.aliexpress.us/item/3256806147195874.html

Panel mount USB-C connector. USB-C 3.1 IP67 waterproof cable type c 90 degree male to female panel car ship dashboard installation connector

https://www.aliexpress.us/item/3256801869230308.html

Panel mount NMEA-2000 connector with pig tails

## Setup in SignalK

To register gateway in SignalK

- Connect to USB 2.0 port on pi (I had drops of N2K data stream when connected to USB 3.0 on external hub)
- find out /dev/tty* USB device of gateway (in your raspberry pi with BBN OS or other Linux)
- and add N2K connection with Source Type (Actisense AGT-1 Canboat-js) baud rate: 115200

## Flashing firmware on Bareboat Necessities OS

````
wget https://raw.githubusercontent.com/bareboat-necessities/my-bareboat/refs/heads/master/m5stack-tools/bbn-flash-m5-n2k-usb.sh
chmod +x bbn-flash-m5-n2k-usb.sh
./bbn-flash-m5-n2k-usb.sh -p /dev/ttyUSB0
````

## Troubleshooting

- Normal working behavior is fast flashing blue status light indicating traffic passed.

## Patches

NMEA2000_esp32 forked from https://github.com/ttlappalainen/NMEA2000_esp32
with some error handling additions
based on https://www.esp32.com/viewtopic.php?t=5010
