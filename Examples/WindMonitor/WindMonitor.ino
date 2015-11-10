// Demo: NMEA2000 library. Send main cabin temperature to the bus.

#include <Arduino.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <N2kMessages.h>

// Uncomment these for using Arduino Due internal CAN
//#include <due_can.h>  // https://github.com/collin80/due_can
//#include <NMEA2000_due.h>
//tNMEA2000_due NMEA2000;
// Uncomment these for using Arduino Due internal CAN

// CAN_BUS_shield libraries will be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
// There is improved library, which branch can be found on https://github.com/peppeve/CAN_BUS_Shield
// That works also with Maple mini and 8 MHz clock. Hopefully these improvements will be applied to
// original library

// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip
#include <SPI.h>
#include <mcp_can.h> 
#include <NMEA2000_mcp.h>
#define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN);
// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple wind monitor",  // Manufacturer's Model ID
                                 "1.0.0.11 (2015-11-10)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-11-10)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                55, // Device class=External Environment. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
  Serial.begin(115200);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  NMEA2000.SetForwardOwnMessages();

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,23);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.Open();
}


void loop() {
  SendN2kWind();
  NMEA2000.ParseMessages();
}

double ReadWindAngle() {
  return DegToRad(50); // Read here the measured wind angle e.g. from analog input
}

double ReadWindSpeed() {
  return 10.3; // Read here the wind speed e.g. from analog input
}

#define WindUpdatePeriod 1000

void SendN2kWind() {
  static unsigned long WindUpdated=millis();
  tN2kMsg N2kMsg;

  if ( WindUpdated+WindUpdatePeriod<millis() ) {
    SetN2kWindSpeed(N2kMsg, 1, ReadWindSpeed(), ReadWindAngle(),N2kWind_Apprent);
    WindUpdated=millis();
    NMEA2000.SendMsg(N2kMsg);
  }
}

