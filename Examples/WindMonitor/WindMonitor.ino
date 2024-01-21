/***********************************************************************//**
  \file   WindMonitor.ino
  \brief  NMEA2000 library example. Send main wind data to the bus.
  \ingroup group_examples

  This simple example sends hardcoded wind angle and speed information
  to the NMEA2000 bus. To make it working device you need add functionality, which
  reads real values.

  Example does not fullfill all NMEA2000 requirements.
*/

#include <Arduino.h>
//#define N2k_SPI_CS_PIN 53    // If you use mcp_can and CS pin is not 53, uncomment this and modify definition to match your CS pin.
//#define N2k_CAN_INT_PIN 21   // If you use mcp_can and interrupt pin is not 21, uncomment this and modify definition to match your interrupt pin.
//#define USE_MCP_CAN_CLOCK_SET 8  // If you use mcp_can and your mcp_can shield has 8MHz chrystal, uncomment this.
//#define ESP32_CAN_TX_PIN GPIO_NUM_16 // If you use ESP32 and do not have TX on default IO 16, uncomment this and and modify definition to match your CAN TX pin.
//#define ESP32_CAN_RX_PIN GPIO_NUM_17 // If you use ESP32 and do not have RX on default IO 4, uncomment this and and modify definition to match your CAN TX pin.
//#define NMEA2000_ARDUINO_DUE_CAN_BUS tNMEA2000_due::CANDevice1  // If you use Arduino DUE and want to use CAN bus 1 instead of 0, uncomment this.
//#define NMEA2000_TEENSY_CAN_BUS 1 // If you use Teensy 3.5 or 3.6 and want to use second CAN bus, uncomment this.
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>

// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM={130306L,0};

// Define schedulers for messages. Define schedulers here disabled. Schedulers will be enabled
// on OnN2kOpen so they will be synchronized with system.
// We use own scheduler for each message so that each can have different offset and period.
// Setup periods according PGN definition (see comments on IsDefaultSingleFrameMessage and
// IsDefaultFastPacketMessage) and message first start offsets. Use a bit different offset for
// each message so they will not be sent at same time.
tN2kSyncScheduler WindScheduler(false,100,500);

// *****************************************************************************
// Call back for NMEA2000 open. This will be called, when library starts bus communication.
// See NMEA2000.SetOnOpen(OnN2kOpen); on setup()
void OnN2kOpen() {
  // Start schedulers now.
  WindScheduler.UpdateNextTime();
}

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple wind monitor",  // Manufacturer's Model ID
                                 "1.2.0.24 (2022-10-01)",  // Manufacturer's Software version code
                                 "1.2.0.0 (2022-10-01)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                85, // Device class=External Environment. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader                           
  //Serial.begin(115200);
  //NMEA2000.SetForwardStream(&Serial);
  // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  //NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,23);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.EnableForward(false);
  // Here we tell library, which PGNs we transmit
  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  // Define OnOpen call back. This will be called, when CAN is open and system starts address claiming.
  NMEA2000.SetOnOpen(OnN2kOpen);
  NMEA2000.Open();
}


// *****************************************************************************
void loop() {
  SendN2kWind();
  NMEA2000.ParseMessages();
}

// *****************************************************************************
double ReadWindAngle() {
  return DegToRad(50); // Read here the measured wind angle e.g. from analog input
}

// *****************************************************************************
double ReadWindSpeed() {
  return 10.3; // Read here the wind speed e.g. from analog input
}

// *****************************************************************************
void SendN2kWind() {
  tN2kMsg N2kMsg;

  if ( WindScheduler.IsTime() ) {
    WindScheduler.UpdateNextTime();
    SetN2kWindSpeed(N2kMsg, 1, ReadWindSpeed(), ReadWindAngle(),N2kWind_Apprent);
    NMEA2000.SendMsg(N2kMsg);
  }
}

