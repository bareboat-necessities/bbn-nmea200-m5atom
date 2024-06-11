// Acts as Actisense USB nmea 2000 gateway. Powered by USB. NMEA 2000 is isolated (so connect only H and L)
// To register in SignalK find out /dev/tty* USB device
// and add NMEA 2000 connection with Source Type (Actisense AGT-1 Canboat-js)
// baud rate: 115200
// Connect to USB 2.0 port on pi (I had drops of nmea 2000 data stream when connected to USB 3.0 on external hub)

// Based on: https://github.com/hatlabs/SH-ESP32-nmea2000-gateway/

/*
                                Apache License
                           Version 2.0, January 2004
                        http://www.apache.org/licenses/
*/

#include <M5Atom.h>
#include <Arduino.h>

#define ESP32_CAN_TX_PIN GPIO_NUM_22  // Set CAN TX port to 22 for M5ATOM CANBUS
#define ESP32_CAN_RX_PIN GPIO_NUM_19  // Set CAN RX port to 19 for M5ATOM CANBUS

#define CAN_TX_PIN ESP32_CAN_TX_PIN
#define CAN_RX_PIN ESP32_CAN_RX_PIN

#include <ActisenseReader.h>
#include "NMEA2000_esp32.h"
#include <ReactESP.h>
#include <Wire.h>
#include <esp_private/esp_int_wdt.h>
#include <esp_task_wdt.h>

using namespace reactesp;

ReactESP app;

Stream *read_stream = &Serial;
Stream *forward_stream = &Serial;

tActisenseReader actisense_reader;
tNMEA2000 *nmea2000;

static bool led_state = false;

void ToggleLed() {
  if (led_state) {
    M5.dis.drawpix(0, 0x0000ff);
  } else {
    M5.dis.drawpix(0, 0x000000);
  }
  led_state = !led_state;
}

int num_n2k_messages = 0;
void HandleStreamN2kMsg(const tN2kMsg &message) {
  // N2kMsg.Print(&Serial);
  num_n2k_messages++;
  ToggleLed();
}

int num_actisense_messages = 0;
void HandleStreamActisenseMsg(const tN2kMsg &message) {
  // N2kMsg.Print(&Serial);
  num_actisense_messages++;
  ToggleLed();
  nmea2000->SendMsg(message);
}

String can_state;

void PollCANStatus() {
  // CAN controller registers are SJA1000 compatible.
  // Bus status value 0 indicates bus-on; value 1 indicates bus-off.
  unsigned int bus_status = MODULE_CAN->SR.B.BS;

  switch (bus_status) {
    case 0:
      can_state = "RUNNING";
      break;
    case 1:
      can_state = "BUS-OFF";
      // try to automatically recover by rebooting
      app.onDelay(2000, []() {
        esp_task_wdt_init(1, true);
        esp_task_wdt_add(NULL);
        while (true) {}
      });
      break;
  }
}

void setup() {
  M5.begin(true, false, true);
  // setup serial output
  Serial.begin(115200);
  delay(100);

  // toggle the LED pin at rate of 1 Hz
  app.onRepeatMicros(1e6 / 1, []() {
    ToggleLed();
  });

  // instantiate the NMEA2000 object
  nmea2000 = new tNMEA2000_esp32(CAN_TX_PIN, CAN_RX_PIN);

  // input the NMEA 2000 messages

  // Reserve enough buffer for sending all messages. This does not work on small
  // memory devices like Uno or Mega
  nmea2000->SetN2kCANSendFrameBufSize(250);
  nmea2000->SetN2kCANReceiveFrameBufSize(250);

  // Set Product information
  nmea2000->SetProductInformation(
    "20230529",                 // Manufacturer's Model serial code (max 32 chars)
    103,                        // Manufacturer's product code
    "M5Atom NMEA 2000 USB GW",  // Manufacturer's Model ID (max 33 chars)
    "0.1.0.0 (2021-03-31)",     // Manufacturer's Software version code (max 40 chars)
    "0.0.3.1 (2021-03-07)"      // Manufacturer's Model version (max 24 chars)
  );
  // Set device information
  nmea2000->SetDeviceInformation(
    230529,  // Unique number. Use e.g. Serial number.
    130,     // Device function=Analog to NMEA 2000 Gateway. See codes on
             // http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
    25,      // Device class=Inter/Intranetwork Device. See codes on
             // http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
    2046     // Just choosen free from code list on
             // http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  );

  nmea2000->SetForwardStream(forward_stream);
  nmea2000->SetMode(tNMEA2000::N2km_ListenAndNode);

  nmea2000->SetForwardOwnMessages(false);  // do not echo own messages.
  nmea2000->SetMsgHandler(HandleStreamN2kMsg);
  nmea2000->Open();

  actisense_reader.SetReadStream(read_stream);
  actisense_reader.SetDefaultSource(75);
  actisense_reader.SetMsgHandler(HandleStreamActisenseMsg);

  // No need to parse the messages at every single loop iteration; 1 ms will do
  app.onRepeat(1, []() {
    nmea2000->ParseMessages();
    actisense_reader.ParseMessages();
  });

  // enable CAN status polling
  app.onRepeat(100, []() {
    PollCANStatus();
  });

  delay(100);

  app.onRepeat(1000, []() {
    num_n2k_messages = 0;
    num_actisense_messages = 0;
  });
}

void loop() {
  app.tick();
}
