// Demo: NMEA2000 library. Bus listener. Sends all bus data to serial in Actisense format.
// Note! If you use Arduino Mega, it can not handle lot of messages without using interrupt.
// enable interrupt by defining pin you have used for MCP2515 interrupt pin. See below
// definition #define N2k_CAN_INT_PIN 20

//#define N2k_CAN_INT_PIN 21
#include <Arduino.h>
#include <NMEA2000_CAN.h>

void setup() {
  Serial.begin(115200);
  NMEA2000.SetForwardStream(&Serial);  // PC output on due native port
//  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.Open();
}

void loop() {
  NMEA2000.ParseMessages();
}

