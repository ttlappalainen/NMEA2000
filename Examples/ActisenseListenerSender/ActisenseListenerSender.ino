// Demo: NMEA2000 library. Bus listener and sender. 
//   Sends all bus data to serial in Actisense format.
//   Send all data received from serial in Actisense format to the N2kBus.
//   Use this e.g. with NMEA Simulator (see. http://www.kave.fi/Apps/index.html) to send simulated data to the bus.

#include <Arduino.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <ActisenseReader.h>

// Sample for due only. Not tested with Mega
#include <due_can.h>
#include <NMEA2000_due.h>

tNMEA2000_due NMEA2000;
tActisenseReader ActisenseReader;

void setup() {
  SerialUSB.begin(115200);
  Serial.begin(115200);
  NMEA2000.SetForwardStream(&Serial);  // PC output on due programming port
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndSend);
//  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.Open();

  // I had problem to use same Serial stream for reading and sending.
  // It worked for a while, but then stopped.
  ActisenseReader.SetReadStream(&SerialUSB);
  ActisenseReader.SetMsgHandler(HandleStreamN2kMsg); 
}

void HandleStreamN2kMsg(const tN2kMsg &N2kMsg) {
  // N2kMsg.Print(&Serial);
  NMEA2000.SendMsg(N2kMsg,-1);
}

void loop() {
  NMEA2000.ParseMessages();
  ActisenseReader.ParseMessages();
}

