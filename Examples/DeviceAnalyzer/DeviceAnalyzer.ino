/***********************************************************************//**
  \file   DeviceAnalyzer.ino
  \brief  NMEA2000 library example. Show devices on NMEA2000 bus by using tN2kDeviceList.
  \ingroup group_examples

  tN2kDeviceList is helper class, which automatically keeps track of devices on the bus.
  This example simplyt lists all devices on the bus and prints their information by using
  data collected by tN2kDeviceList.

*/

#include <Arduino.h>
#define N2k_CAN_INT_PIN 21
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kDeviceList.h"

tN2kDeviceList *pN2kDeviceList;
Stream *OutputStream;

//*****************************************************************************
template<typename T> void PrintLabelValWithConversionCheckUnDef(const char* label, T val, double (*ConvFunc)(double val)=0, bool AddLf=false ) {
  OutputStream->print(label);
  if (!N2kIsNA(val)) {
    if (ConvFunc) { OutputStream->print(ConvFunc(val)); } else { OutputStream->print(val); }
  } else OutputStream->print("not available");
  if (AddLf) OutputStream->println();
}

//*****************************************************************************
void setup() {
  Serial.begin(115200);
  OutputStream=&Serial;
  Serial.println("Device analyzer is starting up...");
  delay(3000);
  NMEA2000.SetN2kCANReceiveFrameBufSize(150);
  NMEA2000.SetN2kCANMsgBufSize(8);
  // Set Product information
  NMEA2000.SetProductInformation("00000003", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "N2k bus device analyzer",  // Manufacturer's Model ID
                                 "1.0.0.10 (2017-07-29)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2017-07-12)" // Manufacturer's Model version
                                );

  // Set device information
  NMEA2000.SetDeviceInformation(2, // Unique number. Use e.g. Serial number.
                                130, // Device function=Display. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                120, // Device class=Display. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
  // Uncomment 3 rows below to see, what device will send to bus

  NMEA2000.SetForwardStream(0); //&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  // NMEA2000.SetForwardOwnMessages();

  NMEA2000.EnableForward(false);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 50);
  pN2kDeviceList = new tN2kDeviceList(&NMEA2000);
  NMEA2000.Open();
  Serial.println("Device analyzer started.");
  Serial.println("  - Analyzer will automatically print new list on list changes.");
  Serial.println("  - Send 'u' to print latest list");
}

//*****************************************************************************
void PrintUlongList(const char *prefix, const unsigned long * List) {
  uint8_t i;
  if ( List!=0 ) {
    Serial.print(prefix);
    for (i=0; List[i]!=0; i++) { 
      if (i>0) Serial.print(", ");
      Serial.print(List[i]);
    }
    Serial.println();
  }
}

//*****************************************************************************
void PrintText(const char *Text, bool AddLineFeed=true) {
  if ( Text!=0 ) Serial.print(Text);
  if ( AddLineFeed ) Serial.println();   
}

//*****************************************************************************
void PrintDevice(const tNMEA2000::tDevice *pDevice) {
  if ( pDevice == 0 ) return;

  Serial.println("----------------------------------------------------------------------");
  Serial.println(pDevice->GetModelID());
  Serial.print("  Source: "); Serial.println(pDevice->GetSource());
  Serial.print("  Manufacturer code:        "); Serial.println(pDevice->GetManufacturerCode());
  Serial.print("  Unique number:            "); Serial.println(pDevice->GetUniqueNumber());
  Serial.print("  Software version:         "); Serial.println(pDevice->GetSwCode());
  Serial.print("  Model version:            "); Serial.println(pDevice->GetModelVersion());
  Serial.print("  Manufacturer Information: "); PrintText(pDevice->GetManufacturerInformation());
  Serial.print("  Installation description1: "); PrintText(pDevice->GetInstallationDescription1());
  Serial.print("  Installation description2: "); PrintText(pDevice->GetInstallationDescription2());
  PrintUlongList("  Transmit PGNs :",pDevice->GetTransmitPGNs());
  PrintUlongList("  Receive PGNs  :",pDevice->GetReceivePGNs());
  Serial.println();
}

#define START_DELAY_IN_S 8
//*****************************************************************************
void ListDevices(bool force = false) {
  static bool StartDelayDone=false;
  static int StartDelayCount=0;
  static unsigned long NextStartDelay=0;
  if ( !StartDelayDone ) { // We let system first collect data to avoid printing all changes
    if ( millis()>NextStartDelay ) {
      if ( StartDelayCount==0 ) {
        Serial.print("Reading device information from bus ");
        NextStartDelay=millis();
      }
      Serial.print(".");
      NextStartDelay+=1000;
      StartDelayCount++;
      if ( StartDelayCount>START_DELAY_IN_S ) {
        StartDelayDone=true;
        Serial.println();
      }
    }
    return;
  }
  if ( !force && !pN2kDeviceList->ReadResetIsListUpdated() ) return;

  Serial.println();
  Serial.println("**********************************************************************");
  for (uint8_t i = 0; i < N2kMaxBusDevices; i++) PrintDevice(pN2kDeviceList->FindDeviceBySource(i));
}

//*****************************************************************************
void CheckCommand() {
  if (Serial.available()) {
    char chr = Serial.read();
    switch ( chr ) {
      case 'u': ListDevices(true); break;
    }
  }
}

//*****************************************************************************
void loop() {
  NMEA2000.ParseMessages();
  ListDevices();
  CheckCommand();
}
