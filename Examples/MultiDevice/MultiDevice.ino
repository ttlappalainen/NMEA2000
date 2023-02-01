// Demo: NMEA2000 library. MultiDevice example. 
// This demo creates device, which you see as two different device on the bus.
// Please do not event try this with systems (like Arduino Uno) less that 8 kB RAM!

#include <Arduino.h>
#define N2k_CAN_INT_PIN 21
#include "NMEA2000_CAN.h"  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kMessages.h"

// List here messages your device will transmit.
const unsigned long TemperatureMonitorTransmitMessages[] PROGMEM={130310L,130311L,130312L,0};
const unsigned long BatteryMonitorTransmitMessages[] PROGMEM={127506L,127508L,127513L,0};

#define DEV_TEMP 0
#define DEV_BAT  1

void setup() {
  Serial.begin(115200);
  NMEA2000.SetDeviceCount(2); // Enable multi device support for 2 devices
  // Set Product information for temperature monitor
  NMEA2000.SetProductInformation("112233", // Manufacturer's Model serial code. 
                                 100, // Manufacturer's product code
                                 "Simple temperature monitor",  // Manufacturer's Model ID
                                 "1.0.0.2 (2017-06-13)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2017-06-13)", // Manufacturer's Model version
                                 0xff, // load equivalency - use default
                                 0xffff, // NMEA 2000 version - use default
                                 0xff, // Sertification level - use default
                                 DEV_TEMP
                                 );
  // Set Product information for battery monitor
  NMEA2000.SetProductInformation("112234", // Manufacturer's Model serial code. 
                                 100, // Manufacturer's product code
                                 "Simple battery monitor",  // Manufacturer's Model ID
                                 "1.0.0.2 (2017-06-13)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2017-06-13)", // Manufacturer's Model version
                                 0xff, // load equivalency - use default
                                 0xffff, // NMEA 2000 version - use default
                                 0xff, // Sertification level - use default
                                 DEV_BAT
                                 );
// Set device information for temperature monitor
  NMEA2000.SetDeviceInformation(112233, // Unique number. Use e.g. Serial number.
                                130, // Device function=Temperature. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                75, // Device class=Sensor Communication Interface. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2040, // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                4, // Marine
                                DEV_TEMP
                               );
  // Set device information for battery monitor
  NMEA2000.SetDeviceInformation(112234,  // Unique number. Use e.g. Serial number.
                                170,    // Device function=Battery. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                35,     // Device class=Electrical Generation. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2040,    // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                4, // Marine
                                DEV_BAT
                               );
  // Uncomment 2 rows below to see, what device will send to bus. Use e.g. OpenSkipper or Actisense NMEA Reader                           
  NMEA2000.SetForwardStream(&Serial);
  // If you want to use simple ascii monitor like Arduino Serial Monitor, uncomment next line
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  //NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  
  // Here we tell, which PGNs we transmit from temperature monitor
  NMEA2000.ExtendTransmitMessages(TemperatureMonitorTransmitMessages,DEV_TEMP);
  // Here we tell, which PGNs we transmit from battery monitor
  NMEA2000.ExtendTransmitMessages(BatteryMonitorTransmitMessages,DEV_BAT);
  NMEA2000.Open();
}


void loop() {
  SendN2kTemperature();
  SendN2kBattery();
  NMEA2000.ParseMessages();
}

double ReadCabinTemp() {
  return CToKelvin(22.5); // Read here the true temperature e.g. from analog input
}

double ReadWaterTemp() {
  return CToKelvin(15.5); // Read here the true temperature e.g. from analog input
}

#define TempUpdatePeriod 2000

void SendN2kTemperature() {
  static unsigned long Updated=millis();
  tN2kMsg N2kMsg;

  if ( Updated+TempUpdatePeriod<millis() ) {
    Updated=millis();
    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
    NMEA2000.SendMsg(N2kMsg,DEV_TEMP);
    SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
//    NMEA2000.SendMsg(N2kMsg,DEV_TEMP);
    SetN2kOutsideEnvironmentalParameters(N2kMsg, 1, ReadWaterTemp());
//    NMEA2000.SendMsg(N2kMsg,DEV_TEMP);
    //Serial.print(millis()); Serial.println(", Temperature send ready");
  }
}

#define BatUpdatePeriod 1000

void SendN2kBattery() {
  static unsigned long Updated=millis()+100;
  tN2kMsg N2kMsg;

  if ( Updated+BatUpdatePeriod<millis() ) {
    Updated=millis();
    SetN2kDCBatStatus(N2kMsg,1,13.87,5.12,35.12,1);
    NMEA2000.SendMsg(N2kMsg,DEV_BAT);
    SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Battery,56,92,38500,0.012, AhToCoulomb(420));
//    NMEA2000.SendMsg(N2kMsg,DEV_BAT);
    SetN2kBatConf(N2kMsg,1,N2kDCbt_Gel,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(420),53,1.251,75);
//    NMEA2000.SendMsg(N2kMsg,DEV_BAT);
    // Serial.print(millis()); Serial.println(", Battery send ready");
  }
}
