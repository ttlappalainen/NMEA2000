// Demo: NMEA2000 library. Send battery status to the bus.
//
//
//      In this example are shown ways to minimize the size and RAM usage using two techniques:
//        1) Moving data strictures to PROGMEM vs. using inline constantans when calling a function
//        2) Reducing the size of NMEA CAN buffer to the min needed.  Use caution with this, as some functions
//           (specifically fast packet Messages) require bigger buffer. 
//


#include <Arduino.h>
//#define N2k_CAN_INT_PIN 21
#include "NMEA2000_CAN.h"       // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kMessages.h"

// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM={127506L,127508L,127513L,0};

// ---  Example of using PROGMEM to hold Product ID.  However, doing this will prevent any updating of
//      these details outside of recompiling the program.
const tNMEA2000::tProductInformation BatteryMonitorProductInformation PROGMEM={
                                       1300,                        // N2kVersion
                                       100,                         // Manufacturer's product code
                                       "Simple battery monitor",    // Manufacturer's Model ID
                                       "1.1.0.14 (2017-06-11)",     // Manufacturer's Software version code
                                       "1.1.0.0 (2017-06-11)",      // Manufacturer's Model version
                                       "00000001",                  // Manufacturer's Model serial code
                                       0,                           // SertificationLevel
                                       1                            // LoadEquivalency
                                      };                                      

// ---  Example of using PROGMEM to hold Configuration information.  However, doing this will prevent any updating of
//      these details outside of recompiling the program.
const char BatteryMonitorManufacturerInformation [] PROGMEM = "John Doe, john.doe@unknown.com"; 
const char BatteryMonitorInstallationDescription1 [] PROGMEM = "Just for sample"; 
const char BatteryMonitorInstallationDescription2 [] PROGMEM = "No real information send to bus"; 

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation(&BatteryMonitorProductInformation );
  // Set Configuration information
  NMEA2000.SetProgmemConfigurationInformation(BatteryMonitorManufacturerInformation,BatteryMonitorInstallationDescription1,BatteryMonitorInstallationDescription2);
  // Set device information
  NMEA2000.SetDeviceInformation(1,      // Unique number. Use e.g. Serial number.
                                170,    // Device function=Battery. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                35,     // Device class=Electrical Generation. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046    // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
                               
                               
  // Uncomment 3 rows below to see, what device will send to bus                           
   Serial.begin(115200);
   NMEA2000.SetForwardStream(&Serial);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);     // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  // NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText);     // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  // NMEA2000.EnableForward(false);                      // Disable all msg forwarding to USB (=Serial)
  
//  NMEA2000.SetN2kCANMsgBufSize(2);                    // For this simple example, limit buffer size to 2, since we are only sending data
  NMEA2000.Open();
}


void loop() {
  SendN2kBattery();
  NMEA2000.ParseMessages();
}

#define BatUpdatePeriod 1000

void SendN2kBattery() {
  static unsigned long TempUpdated=millis();
  tN2kMsg N2kMsg;

  if ( TempUpdated+BatUpdatePeriod<millis() ) {
    TempUpdated=millis();
    SetN2kDCBatStatus(N2kMsg,1,13.87,5.12,35.12,1);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Battery,56,92,38500,0.012);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kBatConf(N2kMsg,1,N2kDCbt_Gel,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(420),53,1.251,75);
    NMEA2000.SendMsg(N2kMsg);
    // Serial.print(millis()); Serial.println(", Battery send ready");
  }
}

