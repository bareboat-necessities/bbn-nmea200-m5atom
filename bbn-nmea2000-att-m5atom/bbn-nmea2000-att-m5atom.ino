
// NMEA2000 Attitude with MPU6886 (For example purposes only)

// For Can CAIS3050G module. Powered by USB
// Can isolated (so connect only H and L)

#include <M5Atom.h>
#include <Arduino.h>

#define ESP32_CAN_TX_PIN GPIO_NUM_22  // Set CAN TX port to 22 for M5ATOM CANBUS
#define ESP32_CAN_RX_PIN GPIO_NUM_19  // Set CAN RX port to 19 for M5ATOM CANBUS

#define CAN_TX_PIN ESP32_CAN_TX_PIN
#define CAN_RX_PIN ESP32_CAN_RX_PIN

#include <Preferences.h>
#include "NMEA2000_esp32.h"
#include <N2kMessages.h>
#include <Wire.h>

#define ENABLE_DEBUG_LOG 0  // Debug log

int NodeAddress;  // To store last Node Address

tNMEA2000* nmea2000;

Preferences preferences;  // Nonvolatile storage on ESP32 - To store LastDeviceAddress

// Set the information for other bus devices, which messages we support

const unsigned long TransmitMessages[] PROGMEM = { 127257L,  // Attitude parameters
                                                   0
                                                 };
// Send time offsets
#define sendOffset 0

#define dataUpdatePeriod 250  // Time between CAN Messages sent

static bool led_state = false;

float pitch, roll, yaw;

float att[3];          // attitude
float attOffset[3];

// MPU6886 Calibration for Z axis (gravity), X and Y are still factory defaults
void calibrate6886() {
  float attSum[3];
  int counter = 2500;
  for (int i = 0; i < counter; i++) {
    M5.IMU.getAhrsData(&att[0], &att[1], &att[2]);
    attSum[0] += att[0];
    attSum[1] += att[1];
    attSum[2] += att[2];
    delay(2);
  }
  attOffset[0] = attSum[0] / counter;
  attOffset[1] = attSum[1] / counter;
  attOffset[2] = attSum[2] / counter;

  preferences.begin("nvs", false);
  preferences.putFloat("pitchOff", attOffset[0]);
  preferences.putFloat("rollOff", attOffset[1]);
  preferences.end();
}

void ToggleLed() {
  if (led_state) {
    M5.dis.drawpix(0, 0x00ff00);
  } else {
    M5.dis.drawpix(0, 0x000000);
  }
  led_state = !led_state;
}

void debug_log(char* str) {
#if ENABLE_DEBUG_LOG == 1
  Serial.println(str);
#endif
}

bool IsTimeToUpdate(unsigned long NextUpdate) {
  return (NextUpdate < millis());
}

unsigned long InitNextUpdate(unsigned long Period, unsigned long Offset = 0) {
  return millis() + Period + Offset;
}

void SetNextUpdate(unsigned long& NextUpdate, unsigned long Period) {
  while (NextUpdate < millis()) NextUpdate += Period;
}

void SendN2kAttitide(void) {
  static unsigned long dataUpdate = InitNextUpdate(dataUpdatePeriod, sendOffset);
  tN2kMsg N2kMsg;

  if (IsTimeToUpdate(dataUpdate)) {
    SetNextUpdate(dataUpdate, dataUpdatePeriod);

    ToggleLed();

    M5.IMU.getAhrsData(&pitch, &roll, &yaw);
    Serial.printf("%.2f, %.2f\n", (pitch - attOffset[0]), (roll - attOffset[1]));

    SetN2kAttitude(N2kMsg, 0, N2kDoubleNA, (pitch - attOffset[0]) * PI / 180.0, (roll - attOffset[1]) * PI / 180.0);
    nmea2000->SendMsg(N2kMsg);
  }
}

void setup() {
  M5.begin(true, false, true);  // Init M5Atom.
  Wire1.begin(26, 32, 100000UL);           // Initialize pin 26,32. (i2c)
  M5.IMU.Init();   // Init IMU sensor.

  uint8_t chipid[6];
  uint32_t id = 0;
  int i = 0;

  // Init USB serial port
  Serial.begin(115200);
  delay(10);

  preferences.begin("nvs", true);
  attOffset[0] = preferences.getFloat("pitchOff", 0.0f);
  attOffset[1] = preferences.getFloat("rollOff", 0.0f);
  preferences.end();

  // instantiate the NMEA2000 object
  nmea2000 = new tNMEA2000_esp32(CAN_TX_PIN, CAN_RX_PIN);

  // Reserve enough buffer for sending all messages. This does not work on small memory devices like Uno or Mega

  nmea2000->SetN2kCANMsgBufSize(8);
  nmea2000->SetN2kCANReceiveFrameBufSize(250);
  nmea2000->SetN2kCANSendFrameBufSize(250);

  esp_efuse_mac_get_default(chipid);
  for (i = 0; i < 6; i++) id += (chipid[i] << (7 * i));

  // Set product information
  nmea2000->SetProductInformation("00301",                  // Manufacturer's Model serial code
                                  130,                      // Manufacturer's product code
                                  "BBN Attitude Module",    // Manufacturer's Model ID
                                  "1.0.2.25 (2023-05-31)",  // Manufacturer's Software version code
                                  "1.0.2.0 (2023-05-31)"    // Manufacturer's Model version
                                 );
  // Set device information
  nmea2000->SetDeviceInformation(id,   // Unique number. Use e.g. Serial number.
                                 180,  // Device function=Attitude. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                 40,   // Device class=Steering and Control Surfaces. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                 2046  // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                );

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below

  //nmea2000->SetForwardType(tNMEA2000::fwdt_Text);  // Show in clear text. Leave uncommented for default Actisense format.

  preferences.begin("nvs", false);                          // Open nonvolatile storage (nvs)
  NodeAddress = preferences.getInt("LastNodeAddress", 35);  // Read stored last NodeAddress, default 35
  preferences.end();
  //Serial.printf("NodeAddress=%d\n", NodeAddress);

  nmea2000->SetMode(tNMEA2000::N2km_NodeOnly, NodeAddress);
  nmea2000->ExtendTransmitMessages(TransmitMessages);

  nmea2000->Open();

  delay(200);
}

void loop() {

  M5.update();
  if (M5.Btn.isPressed()) {
    calibrate6886();
  } else {
    SendN2kAttitide();

    nmea2000->ParseMessages();
    int SourceAddress = nmea2000->GetN2kSource();
    if (SourceAddress != NodeAddress) {  // Save potentially changed Source Address to NVS memory
      NodeAddress = SourceAddress;       // Set new Node Address (to save only once)
      preferences.begin("nvs", false);
      preferences.putInt("LastNodeAddress", SourceAddress);
      preferences.end();
      //Serial.printf("Address Change: New Address=%d\n", SourceAddress);
    }

    // Dummy to empty input buffer to avoid board to stuck with e.g. NMEA Reader
    if (Serial.available()) {
      Serial.read();
    }
  }
}
