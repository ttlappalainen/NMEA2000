// Demo: NMEA2000 library. Send main cabin temperature to the bus.

#include <Arduino.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <N2kMessages.h>

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

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple temp monitor",  // Manufacturer's Model ID
                                 "1.0.0.10 (2015-08-03)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-08-03)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                75, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
  // Serial.begin(115200);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  // NMEA2000.SetForwardOwnMessages();

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  NMEA2000.Open();
}


void loop() {
  SendN2kTemperature();
  NMEA2000.ParseMessages();
}

double ReadCabinTemp() {
  return CToKelvin(21.0); // Read here the true temperature e.g. from analog input
}

#define TempUpdatePeriod 1000

void SendN2kTemperature() {
  static unsigned long TempUpdated=millis();
  tN2kMsg N2kMsg;

  if ( TempUpdated+TempUpdatePeriod<millis() ) {
    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
    TempUpdated=millis();
    NMEA2000.SendMsg(N2kMsg);
  }
}

