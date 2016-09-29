// Demo: NMEA2000 library. NMEA 2000 bus Actisense format listener and sender. 
//   Sends all bus data to serial1 in Actisense format.
//   Send all data received from serial (USB) in Actisense format to the N2kBus.
//   Use this e.g. with NMEA Simulator (see. http://www.kave.fi/Apps/index.html) to send simulated data to the bus.

#include <Arduino.h>
#include <NMEA2000_CAN.h>
#include <ActisenseReader.h>

tActisenseReader ActisenseReader;

void setup() {
  Serial.begin(115200); // Data from PC
  Serial1.begin(115200); // Data to PC
  NMEA2000.SetForwardStream(&Serial1); 
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndSend);
  //NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  NMEA2000.Open();

  // I had problem to use same Serial stream for reading and sending.
  // It worked for a while, but then stopped.
  ActisenseReader.SetReadStream(&Serial);
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

