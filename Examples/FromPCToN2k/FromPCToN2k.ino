// Demo: NMEA2000 library. Read data from SerialUSB and forwards it to the N2kBus
// With this you can e.g. send data with NMEA_Simulator to the N2kBus. 
// See also http://www.kave.fi/Apps/index.html

#include <Arduino.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <ActisenseReader.h>

// Uncomment these for using Arduino Due internal CAN
#include <due_can.h>  // https://github.com/collin80/due_can
#include <NMEA2000_due.h>
tNMEA2000_due NMEA2000;
// Uncomment these for using Arduino Due internal CAN

// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip
//#include <SPI.h>
//#include <mcp_can.h>  // https://github.com/Seeed-Studio/CAN_BUS_Shield
//#include <NMEA2000_mcp.h>
//#define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
//tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN);
// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip

tActisenseReader ActisenseReader;

void setup() {
  // Set Product information
  Serial.begin(115200);
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 666, // Manufacturer's product code
                                 "PC -> N2k",  // Manufacturer's Model ID
                                 "1.0.0.1 (2015-08-14)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-08-14)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20%26%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  // NMEA2000.SetForwardOwnMessages();

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  NMEA2000.Open();

  // Note that we use SerialUSB here. I had problems of reading data from programming port (Serial).
  // It worked for a while, then it stopped completely and I had to unplug the card to get it work again.
  SerialUSB.begin(115200);
  ActisenseReader.SetReadStream(&SerialUSB);
  ActisenseReader.SetMsgHandler(HandleStreamN2kMsg);
}


void loop() {
  ActisenseReader.ParseMessages();
  NMEA2000.ParseMessages();
}

void HandleStreamN2kMsg(const tN2kMsg &N2kMsg) {
  // N2kMsg.Print(&Serial);
  NMEA2000.SendMsg(N2kMsg);
}

