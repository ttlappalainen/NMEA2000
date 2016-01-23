// Demo: NMEA2000 library. Bus listener. Sends all bus data to serial in Actisense format.

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

