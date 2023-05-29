// NMEA2000 library. Bus listener and sender for m5atom with canbus.
//   Sends all bus data to serial in Actisense format.
//   Send all data received from serial in Actisense format to the N2kBus.
// Based on: https://github.com/AK-Homberger/NMEA2000-SignalK-Gateway/tree/main/ActisenseListenerSender-ESP32
// GPL 3.0 Licensed

// Acts as Actisense USB nmea 2000 gateway. Powered by USB. NMEA 2000 is isolated (so connect only H and L)
// To register in SignalK find out /dev/tty* USB device
// and add NMEA 2000 connection with Source Type (Actisense AGT-1 Canboat-js)
// baud rate: 115200
//
// Plugin into USB 2.0 directly on pi. I've seen issues with USB 3.0 when can data stream would stop.

#include <Arduino.h>

//#define N2k_CAN_INT_PIN 2
//#define USE_N2K_CAN 1
//#define N2k_SPI_CS_PIN 10

#define ESP32_CAN_TX_PIN GPIO_NUM_22  // Set CAN TX port to 22 for M5ATOM CANBUS
#define ESP32_CAN_RX_PIN GPIO_NUM_19  // Set CAN RX port to 19 for M5ATOM CANBUS

#include <NMEA2000_CAN.h>
#include <N2kMsg.h>
#include <NMEA2000.h>

#include <ActisenseReader.h>

tActisenseReader ActisenseReader;

// Define READ_STREAM to port, where you write data from PC e.g. with NMEA Simulator.
#define READ_STREAM Serial
// Define ForwardStream to port, what you listen on PC side. On Arduino Due you can use e.g. SerialUSB
#define FORWARD_STREAM Serial

Stream *ReadStream = &READ_STREAM;
Stream *ForwardStream = &FORWARD_STREAM;

void setup() {
  // Define buffers big enough
  NMEA2000.SetN2kCANSendFrameBufSize(150);
  NMEA2000.SetN2kCANReceiveFrameBufSize(150);

  if (ReadStream != ForwardStream) READ_STREAM.begin(115200);
  FORWARD_STREAM.begin(115200);
  NMEA2000.SetForwardStream(ForwardStream);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndSend);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show bus data in clear text
  if (ReadStream == ForwardStream) NMEA2000.SetForwardOwnMessages(false);  // If streams are same, do not echo own messages.
  // NMEA2000.EnableForward(false);
  NMEA2000.Open();

  // I originally had problem to use same Serial stream for reading and sending.
  // It worked for a while, but then stopped. Later it started to work.
  ActisenseReader.SetReadStream(ReadStream);
  ActisenseReader.SetDefaultSource(75);
  ActisenseReader.SetMsgHandler(HandleStreamN2kMsg);
}

void HandleStreamN2kMsg(const tN2kMsg &N2kMsg) {
  //N2kMsg.Print(&Serial);
  NMEA2000.SendMsg(N2kMsg, -1);
}

void loop() {
  NMEA2000.ParseMessages();
  ActisenseReader.ParseMessages();
}
