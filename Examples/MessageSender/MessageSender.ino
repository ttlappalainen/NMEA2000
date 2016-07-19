// Demo: NMEA2000 library. Send main cabin temperature to the bus.

#include <Arduino.h>
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple temp monitor",  // Manufacturer's Model ID
                                 "1.0.0.11 (2015-08-03)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-08-03)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                75, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
   Serial.begin(115200);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  // NMEA2000.SetForwardOwnMessages();

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  //NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  NMEA2000.Open();
}


void loop() {
  SendN2kRapidData();
  SendN2kSlowData();
  NMEA2000.ParseMessages();
}

double ReadCabinTemp() {
  return CToKelvin(21.11); // Read here the true temperature e.g. from analog input
}

double ReadWaterTemp() {
  return CToKelvin(15.5); // Read here the true temperature e.g. from analog input
}

#define SlowDataUpdatePeriod 1000

void SendN2kSlowData() {
  static unsigned long SlowDataUpdated=millis();
  tN2kMsg N2kMsg;
  // !delay is not good practise withing loops! I use it in this sample since
  // I tested Arduino Mega as receiver without inteerupt and it could not handle all messages
  // send without delay. With interrupt, it can do it and you can set DelayBetweenSend to 0.
  const int DelayBetweenSend=0; 
  
  if ( SlowDataUpdated+SlowDataUpdatePeriod<millis() ) {
    SlowDataUpdated=millis();

    SetN2kDCBatStatus(N2kMsg, 1, 12.72);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kDCBatStatus(N2kMsg, 0, 12.45, 5.08, CToKelvin(27.15));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kTemperatureExt(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kOutsideEnvironmentalParameters(N2kMsg, 1, ReadWaterTemp());
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kBatConf(N2kMsg,1,N2kDCbt_AGM,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(410),95,1.26,97);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Alternator,86,91,1420,0.21);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kAttitude(N2kMsg,1,DegToRad(-3.1),DegToRad(2.4),DegToRad(-7.8));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kEngineDynamicParam(N2kMsg,1,656000,CToKelvin(86.3),CToKelvin(82.1),14.21,5.67,hToSeconds(2137.55),N2kDoubleNA,N2kDoubleNA,N2kInt8NA,N2kInt8NA,true);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kTransmissionParameters(N2kMsg,1,N2kTG_Forward,750000, CToKelvin(65.5),0x6f);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kSystemTime(N2kMsg,1,17555,62000);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kGNSS(N2kMsg,1,17555,62000,-60.1,67.5,10.5,N2kGNSSt_GPS,N2kGNSSm_GNSSfix,12,0.8,0.5,15,1,N2kGNSSt_GPS,15,2);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    // Serial.print(millis()); Serial.println(", Temperature send ready");

    SetN2kMagneticHeading(N2kMsg, 0, DegToRad(127.5), DegToRad(0.0), DegToRad(7.5)); 
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
  }
}

#define RapidDataUpdatePeriod 167 // Some strange periot to cause Slow and rapid to run unsync.

void SendN2kRapidData() {
  static unsigned long RapidDataUpdated=millis();
  tN2kMsg N2kMsg;

  if ( RapidDataUpdated+RapidDataUpdatePeriod<millis() ) {
    RapidDataUpdated=millis();
    SetN2kEngineParamRapid(N2kMsg,1,4350); //,820000,48);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kCOGSOGRapid(N2kMsg,1,N2khr_true,DegToRad(115.6),0.1);
    NMEA2000.SendMsg(N2kMsg);
  }
}

