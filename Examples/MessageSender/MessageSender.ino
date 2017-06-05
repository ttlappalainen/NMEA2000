// Demo: NMEA2000 library. Send different NMEA 2000 messages as example to the bus.

#include <Arduino.h>
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>

void setup() {
  // Set Product information
  NMEA2000.SetN2kCANSendFrameBufSize(100);
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Message sender example",  // Manufacturer's Model ID
                                 "1.0.0.15 (2017-01-01)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2017-01-01)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                132, // Device function=Analog to NMEA 2000 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
   Serial.begin(115200);
   NMEA2000.SetForwardStream(&Serial);  // PC output on due programming port
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
  static double Heading=227.5;
  
  // !delay is not good practise withing loops! I use it in this sample since
  // I tested Arduino Mega as receiver without inteerupt and it could not handle all messages
  // send without delay. With interrupt, it can do it and you can set DelayBetweenSend to 0.
  const int DelayBetweenSend=0; 
  
  if ( SlowDataUpdated+SlowDataUpdatePeriod<millis() ) {
    SlowDataUpdated=millis();

    SetN2kDCBatStatus(N2kMsg, 1, 12.72);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    static unsigned char sid=0;
//    SetN2kDCBatStatus(N2kMsg, 0, 12.45, 5.08, CToKelvin(27.15));
    SetN2kPGN127508(N2kMsg, 0, 13.8, 0.95, N2kDoubleNA, sid++);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kTemperatureExt(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),N2khs_InsideHumidity, 55, mBarToPascal(1013.5));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kOutsideEnvironmentalParameters(N2kMsg, 1, ReadWaterTemp(), CToKelvin(25.3), mBarToPascal(1013.5));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kBatConf(N2kMsg,1,N2kDCbt_AGM,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(410),95,1.26,97);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Alternator,86,91,1420,0.21);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kAttitude(N2kMsg,1,DegToRad(-3.1),DegToRad(2.4),DegToRad(-7.8));
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kEngineDynamicParam(N2kMsg,0,656000,CToKelvin(86.3),CToKelvin(82.1),14.21,5.67,hToSeconds(2137.55),N2kDoubleNA,N2kDoubleNA,N2kInt8NA,N2kInt8NA,true);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kTransmissionParameters(N2kMsg,0,N2kTG_Forward,750000, CToKelvin(65.5),true,false,true);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kSystemTime(N2kMsg,1,17555,62000);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    
    SetN2kGNSS(N2kMsg,1,17555,62000,60.1,22.5,10.5,N2kGNSSt_GPS,N2kGNSSm_GNSSfix,12,0.8,0.5,15,1,N2kGNSSt_GPS,15,2);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);

    SetN2kGNSSDOPData(N2kMsg,1,N2kGNSSdm_Auto,N2kGNSSdm_Auto,1.2,-0.8,N2kDoubleNA);
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    // Serial.print(millis()); Serial.println(", Temperature send ready");

    SetN2kMagneticHeading(N2kMsg, 0, DegToRad(Heading), DegToRad(-3.0), DegToRad(5.5)); 
    delay(DelayBetweenSend); NMEA2000.SendMsg(N2kMsg);
    //Heading+=1; if (Heading>=360.0 ) Heading-=360.0;

    SetN2kAISClassAPosition(N2kMsg, 1, tN2kAISRepeat::N2kaisr_First, 123456789, 26.396, -80.075, 1, 1, 1, 20, 20, 30, 0, tN2kAISNavStatus::N2kaisns_At_Anchor);
    NMEA2000.SendMsg(N2kMsg);

    SetN2kBinaryStatus(N2kMsg,2,N2kOnOff_On,N2kOnOff_Unavailable,N2kOnOff_Off);
    NMEA2000.SendMsg(N2kMsg);

    SetN2kPressure(N2kMsg,0,2,N2kps_Atmospheric,mBarToPascal(1024));
    NMEA2000.SendMsg(N2kMsg);

    SetN2kRudder(N2kMsg,DegToRad(5),1,N2kRDO_MoveToStarboard,DegToRad(-5));
    NMEA2000.SendMsg(N2kMsg);

    tN2kBinaryStatus SwitchBoard;
    N2kResetBinaryStatus(SwitchBoard);
    N2kSetStatusBinaryOnStatus(SwitchBoard,N2kOnOff_On,7);
    SetN2kBinaryStatus(N2kMsg,3,SwitchBoard);
    NMEA2000.SendMsg(N2kMsg);
  }
}

#define RapidDataUpdatePeriod 167 // Some strange periot to cause Slow and rapid to run unsync.

void SendN2kRapidData() {
  static unsigned long RapidDataUpdated=millis();
  tN2kMsg N2kMsg;

  if ( RapidDataUpdated+RapidDataUpdatePeriod<millis() ) {
    RapidDataUpdated=millis();
    SetN2kEngineParamRapid(N2kMsg,0,4350); //,820000,48);
    NMEA2000.SendMsg(N2kMsg);
    SetN2kCOGSOGRapid(N2kMsg,1,N2khr_true,DegToRad(115.6),0.1);
    NMEA2000.SendMsg(N2kMsg);
  }
}

