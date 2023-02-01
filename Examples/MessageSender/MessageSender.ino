// Demo: NMEA2000 library. Send different NMEA 2000 messages as example to the bus.
//
// MessageSender uses tN2kSyncScheduler, which sends all messages synchronized to library open time.
// As default messages will have their default(NMEA2000 specified) period. Messages also
// has different offsets to avoid filling up buffer at once. NMEA2000 bus can handle about
// 2.5 frame/ms. So two single frame messages could be sent at same time, but e.g. 7 frame
// GNSS position data takes about 3 ms. With unlapping offsets there should be only one message
// on buffer at time
//
// If you need to test new message for testing, write sender function - see. e.g. SendN2kTemperatureExt
// around line 360 (may change after update). Then add function to N2kSendMessages (somewhere line 504 )
// vector in similar way as SendN2kTemperatureExt. Set message period value according to message
// definition - most periods has been listed also on NMEA2000.cpp.
//
// Messages sending will be done by SendN2kMessages, which loops through message sender vector and sets
// loop NextSend scheduler to next nearest sent time. In this way function does not need every time
// loop through all messages. When it is time to send some messages, it loops them, sends necessary messages 
// and sets NextSend to next nearest time. In ESP32 message sending could be own task and NextSend
// could be used just delay to leave time for other tasks.
//
// MessageSender has simple command line interface. With simple commands you e.g. disable all messages and
// then enable them one by one. To get available commands type ? and line feed to console to get help.

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
#include <N2kMaretron.h>

using tN2kSendFunction=void (*)();

// Structure for holding message sending information
struct tN2kSendMessage {
  tN2kSendFunction SendFunction;
  const char *const Description;
  tN2kSyncScheduler Scheduler;

  tN2kSendMessage(tN2kSendFunction _SendFunction, const char *const _Description, uint32_t /* _NextTime */ 
                  ,uint32_t _Period, uint32_t _Offset, bool _Enabled) : 
                    SendFunction(_SendFunction), 
                    Description(_Description), 
                    Scheduler(_Enabled,_Period,_Offset) {}
  void Enable(bool state);
};

extern tN2kSendMessage N2kSendMessages[];
extern size_t nN2kSendMessages;

static unsigned long N2kMsgSentCount=0;
static unsigned long N2kMsgFailCount=0;
static bool ShowSentMessages=false;
static bool ShowStatistics=false;
static bool Sending=false;
static bool EnableForward=false;
static tN2kScheduler NextStatsTime;
static unsigned long StatisticsPeriod=2000;

// Forward declarations for functions
void CheckCommand(); 
void CheckLoopTime();
void OnN2kOpen();

// *****************************************************************************
void setup() {
  // Initialize serial port.
  Serial.begin(115200);
  delay(2000); // Give a bit time for console to be ready

  // Reserve enough buffer for sending all messages. 
  // This does not work on small memory devices like Uno or Mega
  // This version should work with smaller buffer, since message sending has been offsetted.
  NMEA2000.SetN2kCANSendFrameBufSize(250);
  // Set Product information
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Message sender example",  // Manufacturer's Model ID
                                 "1.1.2.35 (2022-10-01)",  // Manufacturer's Software version code
                                 "1.1.2.0 (2022-10-01)" // Manufacturer's Model version
                                 );
  // Set device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                132, // Device function=Analog to NMEA 2000 Gateway. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
   NMEA2000.SetForwardStream(&Serial);  // PC output to default serial port
   NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
   NMEA2000.SetForwardOwnMessages(false); // Do not print own messages.

  // We act as real node on bus. Some devices does not show messages, if they can not request information.
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.EnableForward(EnableForward); // Disable all msg forwarding to USB (=Serial)
  NMEA2000.SetOnOpen(OnN2kOpen);
  NMEA2000.Open();

  Serial.println("Starting message sender!");
  Serial.println("  Message sending to bus starts in 10 seconds.");
  Serial.println("  Type ?<lf> for available commands.");
}

// *****************************************************************************
void SendN2kMsg(const tN2kMsg &N2kMsg) {
  if ( NMEA2000.SendMsg(N2kMsg) ) {
    N2kMsgSentCount++;
  } else {
    N2kMsgFailCount++;
  }
  if ( ShowSentMessages ) N2kMsg.Print(&Serial);
}

// *****************************************************************************
// Function check is it time to send message. If it is, message will be sent and
// next send time will be updated.
// Function always returns next time it should be handled.
int64_t CheckSendMessage(tN2kSendMessage &N2kSendMessage) {
  if ( N2kSendMessage.Scheduler.IsDisabled() ) return N2kSendMessage.Scheduler.GetNextTime();

  if ( N2kSendMessage.Scheduler.IsTime() ) {
    N2kSendMessage.Scheduler.UpdateNextTime();
    N2kSendMessage.SendFunction();
  }

  return N2kSendMessage.Scheduler.GetNextTime();
}

// *****************************************************************************
// Function send enabled messages from tN2kSendMessage structure according to their
// period+offset.
void SendN2kMessages() {
  static uint64_t NextSend=0;
  uint64_t Now=N2kMillis64();

  if ( NextSend<Now ) {
    uint64_t NextMsgSend;
    NextSend=Now+2000;
    for ( size_t i=0; i<nN2kSendMessages; i++ ) {
      NextMsgSend=CheckSendMessage(N2kSendMessages[i]);
      if ( NextMsgSend<NextSend ) NextSend=NextMsgSend;
    }
  }
}

// *****************************************************************************
void loop() {
  if ( Sending ) SendN2kMessages();
  NMEA2000.ParseMessages();
  CheckCommand();

  if ( ShowStatistics ) {
    if ( NextStatsTime.IsTime() ) {
      NextStatsTime.FromNow(StatisticsPeriod);
      char buf[200];
      snprintf(buf,200,"\nMessages sent:%lu, sent failed:%lu\n",N2kMsgSentCount,N2kMsgFailCount);
      Serial.print(buf);
    }

    CheckLoopTime();
  }
}

// *****************************************************************************
double ReadCabinTemp() {
  return CToKelvin(21.11); // Read here the true temperature e.g. from analog input
}

// *****************************************************************************
double ReadWaterTemp() {
  return CToKelvin(15.5); // Read here the true temperature e.g. from analog input
}

// *****************************************************************************
void SendN2kAttitude() {
  tN2kMsg N2kMsg;
  SetN2kAttitude(N2kMsg,1,DegToRad(-3.1),DegToRad(2.4),DegToRad(-7.8));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kSystemTime() {
  tN2kMsg N2kMsg;
  SetN2kSystemTime(N2kMsg,1,17555,62000);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kLocalOffset() {
  tN2kMsg N2kMsg;
  SetN2kLocalOffset(N2kMsg,17555,62000,120);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kRudder() {
  tN2kMsg N2kMsg;
  SetN2kRudder(N2kMsg,DegToRad(5),1,N2kRDO_MoveToStarboard,DegToRad(-5));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kGNSS() {
  tN2kMsg N2kMsg;
  SetN2kGNSS(N2kMsg,1,17555,62000,60.436614990,22.237819671,10.5,N2kGNSSt_GPS,N2kGNSSm_GNSSfix,12,0.8,0.5,15,1,N2kGNSSt_GPS,15,2);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kGNSSDOPData() {
  tN2kMsg N2kMsg;
  SetN2kGNSSDOPData(N2kMsg,1,N2kGNSSdm_Auto,N2kGNSSdm_Auto,1.2,-0.8,N2kDoubleNA);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kGNSSSatsInView() {
  tN2kMsg N2kMsg;
  // Init message
  SetN2kGNSSSatellitesInView(N2kMsg,0xff,N2kDD072_RangeResidualsWereUsedToCalculateData);
  // Add satellites
  tSatelliteInfo SatelliteInfo;
  SatelliteInfo.PRN=21;
  SatelliteInfo.Elevation=DegToRad(44.94);
  SatelliteInfo.Azimuth=DegToRad(162.67);
  SatelliteInfo.SNR=47.6;
  SatelliteInfo.RangeResiduals=7951.32;
  SatelliteInfo.UsageStatus=N2kDD124_UsedInSolutionWithoutDifferentialCorrections;
  AppendSatelliteInfo(N2kMsg,SatelliteInfo);
  SatelliteInfo.PRN=19;
  SatelliteInfo.Elevation=DegToRad(27.39);
  SatelliteInfo.Azimuth=DegToRad(187.74);
  SatelliteInfo.SNR=44.1;
  SatelliteInfo.RangeResiduals=10105.22;
  SatelliteInfo.UsageStatus=N2kDD124_UsedInSolutionWithoutDifferentialCorrections;
  AppendSatelliteInfo(N2kMsg,SatelliteInfo);
  SatelliteInfo.PRN=31;
  SatelliteInfo.Elevation=DegToRad(26.70);
  SatelliteInfo.Azimuth=DegToRad(106.00);
  SatelliteInfo.SNR=43.9;
  SatelliteInfo.RangeResiduals=10177.9;
  SatelliteInfo.UsageStatus=N2kDD124_UsedInSolutionWithoutDifferentialCorrections;
  AppendSatelliteInfo(N2kMsg,SatelliteInfo);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendUserDatumData() {
  tN2kMsg N2kMsg;
  N2kMsg.SetPGN(129045L);
  N2kMsg.Priority=6;
  N2kMsg.Add4ByteDouble(3.25,1e-2); // Delta X
  N2kMsg.Add4ByteDouble(-3.19,1e-2); // Delta Y
  N2kMsg.Add4ByteDouble(1.1,1e-2); // Delta Z
  N2kMsg.AddFloat(DegToRad(0.123)); // Rotation in X
  N2kMsg.AddFloat(DegToRad(-0.0123)); // Rotation in Y
  N2kMsg.AddFloat(DegToRad(0.00123)); // Rotation in Z
  N2kMsg.AddFloat(1.001); // Scale
  N2kMsg.Add4ByteDouble(N2kDoubleNA,1e-7); // Ellipsoid Semi-major Axis
  N2kMsg.AddFloat(15.23456); // Ellipsoid Flattening Inverse
  N2kMsg.Add4ByteUInt(0xffffffff);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendTideStationData() {
  tN2kMsg N2kMsg;
  N2kMsg.SetPGN(130320L);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(0xc0); // Mode+Tide Tendency+reserved
  N2kMsg.Add2ByteUInt(18498); // Measurement Date
  N2kMsg.Add4ByteUDouble(43210,0.0001); // Measurement Time
  N2kMsg.Add4ByteDouble(60.436616000,1e-7); // Station location, latitude
  N2kMsg.Add4ByteDouble(22.237820000,1e-7); // Station location, longitude
  N2kMsg.Add2ByteDouble(3.25,1e-3); // Tide level
  N2kMsg.Add2ByteUDouble(1.56,1e-2); // Tide level standard deviation
  N2kMsg.AddVarStr("00022");
  N2kMsg.AddVarStr("Test");
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendSalinity() {
  tN2kMsg N2kMsg;
  N2kMsg.SetPGN(130321L);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(0xf1); // Mode+reserved
  N2kMsg.Add2ByteUInt(18498); // Measurement Date
  N2kMsg.Add4ByteUDouble(43210,0.0001); // Measurement Time
  N2kMsg.Add4ByteDouble(60.436616000,1e-7); // Station location, latitude
  N2kMsg.Add4ByteDouble(22.237820000,1e-7); // Station location, longitude
//  N2kMsg.Add4ByteDouble(N2kDoubleNA,1e-7); // Station location, latitude
//  N2kMsg.Add4ByteDouble(N2kDoubleNA,1e-7); // Station location, longitude
  N2kMsg.AddFloat(34.56789); // Salinity
  N2kMsg.Add2ByteUDouble(293.15,0.01);
  N2kMsg.AddVarStr("00022");
  N2kMsg.AddVarStr("Test");
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendHeave() {
  tN2kMsg N2kMsg;
  SetN2kHeave(N2kMsg,0xff,5.3,2.1,N2kDD374_UserDefined);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendRouteInfo() {
  tN2kMsg N2kMsg;
  SetN2kRouteWPInfo(N2kMsg,0,1,7,N2kdir_forward,"Back to home");
  AppendN2kPGN129285(N2kMsg,1,"Start point",60.0,21.0);
  AppendN2kPGN129285(N2kMsg,2,"Turn before rock",60.01,21.01);
  AppendN2kPGN129285(N2kMsg,3,"Home",60.02,21.02);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kProductInformation() {
  NMEA2000.SendProductInformation();
  N2kMsgSentCount++;
}

// *****************************************************************************
void SendN2kIsoAddressClaim() {
  // Note that sometime NMEA Reader gets grazy, when ISO Address claim will be sent periodically.
  // If that happens, reopen NMEA Reader.
  NMEA2000.SendIsoAddressClaim();
  N2kMsgSentCount++;
}

// *****************************************************************************
void SendN2kPressure() {
  tN2kMsg N2kMsg;
  SetN2kPressure(N2kMsg,0,2,N2kps_Atmospheric,mBarToPascal(1024));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kSetPressure() {
  tN2kMsg N2kMsg;
  SetN2kSetPressure(N2kMsg,0,2,N2kps_CompressedAir,1255);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kHumidity() {
  tN2kMsg N2kMsg;
  SetN2kHumidity(N2kMsg,0xff,1,N2khs_InsideHumidity,41.8); //,43);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kTemperatureExt() {
  tN2kMsg N2kMsg;
  SetN2kTemperatureExt(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kTemperature() {
  tN2kMsg N2kMsg;
  SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),CToKelvin(21.6));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendEnvironmentalParameters() {
  tN2kMsg N2kMsg;
  SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_MainCabinTemperature, ReadCabinTemp(),N2khs_InsideHumidity, 55, mBarToPascal(1013.5));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendOutsideEnvironmentalParameters() {
  tN2kMsg N2kMsg;
  SetN2kOutsideEnvironmentalParameters(N2kMsg, 1, ReadWaterTemp(), CToKelvin(25.3), mBarToPascal(1013.5));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kMaretronTempHR() {
  tN2kMsg N2kMsg;
  SetN2kMaretronTempHR(N2kMsg,0xff,1,N2kts_MainCabinTemperature,CToKelvin(24.7));
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kChargerStatus() {
  tN2kMsg N2kMsg;
  SetN2kChargerStatus(N2kMsg, 1, 2, N2kCS_Absorption, N2kCM_Primary, N2kOnOff_On, N2kOnOff_On, 180);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kDCBatStatus1() {
  tN2kMsg N2kMsg;
  SetN2kDCBatStatus(N2kMsg, 1, 12.72);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kDCBatStatus2() {
  tN2kMsg N2kMsg;
  static unsigned char sid=0;
//    SetN2kDCBatStatus(N2kMsg, 0, 12.45, 5.08, CToKelvin(27.15));
  SetN2kPGN127508(N2kMsg, 0, 13.8, 0.95, N2kDoubleNA, sid++);
  if ( sid>252 ) sid=0;
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kBatConf() {
  tN2kMsg N2kMsg;
  SetN2kBatConf(N2kMsg,1,N2kDCbt_AGM,N2kDCES_Yes,N2kDCbnv_12v,N2kDCbc_LeadAcid,AhToCoulomb(410),95,1.26,97);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kDCStatus() {
  tN2kMsg N2kMsg;
  SetN2kDCStatus(N2kMsg,1,1,N2kDCt_Alternator,86,91,1420,0.215,N2kDoubleNA);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kTransmissionParameters() {
  tN2kMsg N2kMsg;
  SetN2kTransmissionParameters(N2kMsg,0,N2kTG_Forward,750000, CToKelvin(65.5),true,false,true);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kAISClassAPosition() {
  tN2kMsg N2kMsg;
  SetN2kAISClassAPosition(N2kMsg, 1, tN2kAISRepeat::N2kaisr_First, 123456789, 26.396, -80.075, 1, 1, 1, 20, 20, N2kaischannel_A_VDL_reception, 30, 0, tN2kAISNavStatus::N2kaisns_At_Anchor);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kBinaryStatus1() {
  tN2kMsg N2kMsg;
  SetN2kBinaryStatus(N2kMsg,2,N2kOnOff_On,N2kOnOff_Unavailable,N2kOnOff_Off);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kBinaryStatus2() {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus SwitchBoard;
  N2kResetBinaryStatus(SwitchBoard);
  N2kSetStatusBinaryOnStatus(SwitchBoard,N2kOnOff_On,7);
  SetN2kBinaryStatus(N2kMsg,3,SwitchBoard);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kEngineTripParameters() {
  tN2kMsg N2kMsg;
  SetN2kEngineTripParameters(N2kMsg,0,237,3.6,4.5,4.4);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kEngineDynamicParam() {
  tN2kMsg N2kMsg;
  tN2kEngineDiscreteStatus1 Status1;
  Status1.Bits.LowOilLevel=1;
  Status1.Bits.OverTemperature=1;
  SetN2kEngineDynamicParam(N2kMsg,0,656000,CToKelvin(86.3),CToKelvin(82.1),14.21,5.67,hToSeconds(2137.55),N2kDoubleNA,N2kDoubleNA,N2kInt8NA,N2kInt8NA,Status1);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kEngineRapid() {
  tN2kMsg N2kMsg;
  SetN2kEngineParamRapid(N2kMsg,0,4350); //,820000,48);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kCOGSOGRapid() {
  tN2kMsg N2kMsg;
  SetN2kCOGSOGRapid(N2kMsg,1,N2khr_true,DegToRad(115.6),0.1);
  SendN2kMsg(N2kMsg);
}

// *****************************************************************************
void SendN2kMagneticHeading() {
  static double Heading=227.5;
  tN2kMsg N2kMsg;
  SetN2kMagneticHeading(N2kMsg, 0, DegToRad(Heading), DegToRad(-3.0), DegToRad(5.5)); 
  SendN2kMsg(N2kMsg);
}

// We add 300 ms as default for each offset to avoid failed sending at start. 
// Message sending is synchronized to open. After open there is 250 ms address claiming time when
// message sending fails.
#define AddSendPGN(fn,NextSend,Period,Offset,Enabled) {fn,#fn,NextSend,Period,Offset+300,Enabled}

tN2kSendMessage N2kSendMessages[]={
   AddSendPGN(SendN2kIsoAddressClaim,0,5000,0,false) // 60928 Not periodic
  ,AddSendPGN(SendN2kSystemTime,0,1000,0,true) // 126992
  ,AddSendPGN(SendN2kProductInformation,0,5000,60,false) // 126996 (20) Not periodic
  ,AddSendPGN(SendN2kRudder,0,100,0,true) // 127245
  ,AddSendPGN(SendN2kMagneticHeading,0,100,1,true) // 127250
  ,AddSendPGN(SendN2kAttitude,0,1000,20,true) // 127257
  ,AddSendPGN(SendN2kEngineRapid,0,100,2,true) // 127488
  ,AddSendPGN(SendN2kEngineDynamicParam,0,500,21,true) // 127489 (4)
  ,AddSendPGN(SendN2kTransmissionParameters,0,100,3,true) // 127493
  ,AddSendPGN(SendN2kEngineTripParameters,0,1000,25,true) // 127497 (2)
  ,AddSendPGN(SendN2kBinaryStatus1,0,2500,27,true) // 127501
  ,AddSendPGN(SendN2kBinaryStatus2,0,2500,28,true) // 127501
  ,AddSendPGN(SendN2kChargerStatus,0,1500,29,true) // 127507 
  ,AddSendPGN(SendN2kDCBatStatus1,0,1500,30,true) // 127508
  ,AddSendPGN(SendN2kDCBatStatus2,0,1500,31,true) // 127508
  ,AddSendPGN(SendN2kDCStatus,0,1500,32,true) // 127506 (2)
  ,AddSendPGN(SendN2kBatConf,0,5000,34,false) // 127513 Not periodic
  ,AddSendPGN(SendN2kCOGSOGRapid,0,250,0,true) // 129026
  ,AddSendPGN(SendN2kGNSS,0,1000,75,true) // 129029 (7)
  ,AddSendPGN(SendN2kLocalOffset,0,5000,36,false) // 129033 Not periodic
  ,AddSendPGN(SendN2kAISClassAPosition,0,5000,80,false) // 129038 (4) Not periodic
  ,AddSendPGN(SendUserDatumData,0,5000,82,false) // 129045 (6) Not periodic
  ,AddSendPGN(SendN2kGNSSDOPData,0,1000,37,false) // 129539
  ,AddSendPGN(SendN2kGNSSSatsInView,0,1000,37,false) // 129540 (max. 32)
  ,AddSendPGN(SendOutsideEnvironmentalParameters,0,500,38,true) // 130310
  ,AddSendPGN(SendEnvironmentalParameters,0,500,39,true) // 130311
  ,AddSendPGN(SendN2kTemperature,0,2000,40,true) // 130312
  ,AddSendPGN(SendN2kHumidity,0,2000,41,true) // 130313
  ,AddSendPGN(SendN2kPressure,0,2000,42,true) // 130314
  ,AddSendPGN(SendN2kSetPressure,0,5000,43,true) // 130315 Not periodic
  ,AddSendPGN(SendN2kTemperatureExt,0,2500,44,true) // 130316 Not periodic
  ,AddSendPGN(SendTideStationData,0,1000,86,false) // 130320 (8)
  ,AddSendPGN(SendSalinity,0,1000,90,false) // 130321 (8)
  ,AddSendPGN(SendHeave,0,100,94,true) // 127252
  ,AddSendPGN(SendRouteInfo,0,5000,94,true) // 129285 Not periodic
  ,AddSendPGN(SendN2kMaretronTempHR,0,2000,45,false) // 130823 (2)
};

size_t nN2kSendMessages=sizeof(N2kSendMessages)/sizeof(tN2kSendMessage);

// *****************************************************************************
// Call back for NMEA2000 open. This will be called, when library starts bus communication.
// See NMEA2000.SetOnOpen(OnN2kOpen); on setup()
// We initialize here all messages next sending time. Since we use tN2kSyncScheduler all messages
// send offset will be synchronized to libary.
void OnN2kOpen() {
  for ( size_t i=0; i<nN2kSendMessages; i++ ) {
    if ( N2kSendMessages[i].Scheduler.IsEnabled() ) N2kSendMessages[i].Scheduler.UpdateNextTime();
  }
  Sending=true;
}

// *****************************************************************************
// Command handling functions. These are environment functions for example not
// for how to use library.

// *****************************************************************************
void CheckLoopTime() {
#define LoopTimePeriod 1000
  static unsigned long NextCheck=millis()+LoopTimePeriod ;
  static unsigned long AvgCount=0;
  static float AvgSum=0;
  static unsigned long MaxLoopTime=0;
  static unsigned long StartTime=micros();

  unsigned long UsedTime=micros()-StartTime;
  if ( UsedTime>MaxLoopTime ) MaxLoopTime=UsedTime;
  AvgCount++;
  AvgSum+=UsedTime;

  if ( NextCheck<millis() ) {
    NextCheck=millis()+LoopTimePeriod;
    Serial.print("- Loop times max:"); 
    Serial.print(MaxLoopTime); 
    Serial.print(" us, avg:"); 
    Serial.print(AvgSum/AvgCount); 
    Serial.println(" us");
    MaxLoopTime=0;
    AvgSum=0;
    AvgCount=0;
  }

  StartTime=micros();
}

// *****************************************************************************
void tN2kSendMessage::Enable(bool state)  {
  if ( Scheduler.IsEnabled()!=state ) {
    if ( state ) {
      Scheduler.UpdateNextTime();
    } else {
      Scheduler.Disable();
    }
  }
}

using tCommandFunction=void (*)(const char *Params);

struct tCommand {
  const char *const Command;
  const char *const Usage;
  const char *const Description;
  tCommandFunction CommandFunction;
};

// *****************************************************************************
void ToggleStats(const char *) {
  ShowStatistics=!ShowStatistics;
  if ( ShowStatistics ) {
    NextStatsTime.FromNow(StatisticsPeriod);
  } else {
    NextStatsTime.Disable();
  }
}

// *****************************************************************************
void ToggleSending(const char *) {
  Sending=!Sending;
  Serial.print("Message sending ");
  Serial.println(Sending?"enabled":"disabled");
}

// *****************************************************************************
void ToggleShowSentMessages(const char *) {
  ShowSentMessages=!ShowSentMessages;
}

// *****************************************************************************
void ToggleForward(const char *) {
  EnableForward=!EnableForward;
  NMEA2000.EnableForward(EnableForward);

  Serial.print("Show bus messages ");
  Serial.println(EnableForward?"enabled":"disabled");
}

// *****************************************************************************
void ClearStatistics(const char *) {
  N2kMsgSentCount=0;
  N2kMsgFailCount=0;
  Serial.println("Message statistics cleared");
}

// *****************************************************************************
bool HasParam(const char *&Params) {
  if ( Params==0 ) return false;
  
  while ( *Params==' ' || *Params=='\t' ) Params++; // Pass spaces to check param exist
  if ( *Params==0 ) {
    return false; // Param not defined.
  }
  
  return true;
}

// *****************************************************************************
template<typename T> bool GetParam(const char *&Params, T &Value, T Def=0) {
  Value=Def;
  char* ParamEnd;
  Value=strtol(Params,&ParamEnd,10);
  bool ret = ( Params!=ParamEnd && (*ParamEnd==0 || *ParamEnd==' ' || *ParamEnd=='\t') );
  Params=ParamEnd;
  return ret;
}

// *****************************************************************************
void ToggleMsgSending(const char *Params) {
  if ( !HasParam(Params) ) {
    Serial.println("Message index not defined");
    return;
  }

  size_t MsgIndex;
  if ( !GetParam(Params,MsgIndex) || MsgIndex>=nN2kSendMessages ) {
    Serial.println("Invalid message index");
    return;
  }
  
  N2kSendMessages[MsgIndex].Enable(!N2kSendMessages[MsgIndex].Scheduler.IsEnabled());
  Serial.print(N2kSendMessages[MsgIndex].Description);
  Serial.print("  ");
  Serial.println(N2kSendMessages[MsgIndex].Scheduler.IsEnabled()?"enabled":"disabled");
}

// *****************************************************************************
void SendMessageNow(const char *Params) {
  if ( !HasParam(Params) ) {
    Serial.println("Message index not defined");
    return;
  }

  size_t MsgIndex;
  if ( !GetParam(Params,MsgIndex) || MsgIndex>=nN2kSendMessages ) {
    Serial.println("Invalid message index");
    return;
  }
  
  N2kSendMessages[MsgIndex].SendFunction();
  Serial.print(N2kSendMessages[MsgIndex].Description);
  Serial.println("  sent");
}

// *****************************************************************************
void SetMessagePeriod(const char *Params) {
  if ( !HasParam(Params) ) {
    Serial.println("Message index not defined");
    return;
  }

  size_t MsgIndex;
  if ( !GetParam(Params,MsgIndex) || MsgIndex>=nN2kSendMessages ) {
    Serial.println("Invalid message index");
    return;
  }
  
  if ( HasParam(Params) ) {
    uint32_t Period;
    if ( !GetParam(Params,Period) || Period>60000 ) {
      Serial.println("Invalid period");
      return;
    }
    
    N2kSendMessages[MsgIndex].Scheduler.SetPeriod(Period);
  }
  Serial.print(N2kSendMessages[MsgIndex].Description);
  Serial.print("  period is ");
  Serial.println(N2kSendMessages[MsgIndex].Scheduler.GetPeriod());
}

// *****************************************************************************
void SetMessageOffset(const char *Params) {
  if ( !HasParam(Params) ) {
    Serial.println("Message index not defined");
    return;
  }

  size_t MsgIndex;
  if ( !GetParam(Params,MsgIndex) || MsgIndex>=nN2kSendMessages ) {
    Serial.println("Invalid message index");
    return;
  }
  
  if ( HasParam(Params) ) {
    uint32_t Offset;
    if ( !GetParam(Params,Offset) || Offset>60000 ) {
      Serial.println("Invalid offset");
      return;
    }
    
    N2kSendMessages[MsgIndex].Scheduler.SetOffset(Offset);
  }
  Serial.print(N2kSendMessages[MsgIndex].Description);
  Serial.print("  offset is ");
  Serial.println(N2kSendMessages[MsgIndex].Scheduler.GetOffset());
}

// *****************************************************************************
void EnableAllMessages(const char *) {
  for ( size_t i=0; i<nN2kSendMessages; i++ ) {
    N2kSendMessages[i].Enable(true);
  }
  Serial.println("All messages enabled");
}

// *****************************************************************************
void DisableAllMessages(const char *) {
  for ( size_t i=0; i<nN2kSendMessages; i++ ) {
    N2kSendMessages[i].Enable(false);
  }
  Serial.println("All messages disabled");
}

// *****************************************************************************
void ListMessages(const char *) {
  for ( size_t i=0; i<nN2kSendMessages; i++ ) {
    Serial.print("    "); 
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(N2kSendMessages[i].Scheduler.IsEnabled()?"(enabled)  ":"(disabled) ");
    Serial.println(N2kSendMessages[i].Description);
  }
}

void ListCommands(const char *Command);

tCommand Commands[]={
  {"?",0,"List commands",ListCommands}
  ,{"p",0,"Toggle sending",ToggleSending}
  ,{"stat",0,"Toggle stat printing",ToggleStats}
  ,{"cl",0,"Clear statistics",ClearStatistics}
  ,{"e",0,"Enable all messages",EnableAllMessages}
  ,{"d",0,"Disable all messages",DisableAllMessages}
  ,{"s",0,"Toggle show sent messages",ToggleShowSentMessages}
  ,{"fwd",0,"Toggle show other bus messages",ToggleForward}
  ,{"msgs",0,"List messages",ListMessages}
  ,{"msg","msg <x>","Toggle message x sending",ToggleMsgSending}
  ,{"once","once <x>","Sends message x on request",SendMessageNow}
  ,{"period","period <x> [p]","Set message x period to p. If p is not defined, just shows period.",SetMessagePeriod}
  ,{"offset","offset <x> [o]","Set message x offset to o. If o is not defined, just shows offset.",SetMessageOffset}
};

size_t nCommands=sizeof(Commands)/sizeof(tCommand);

// *****************************************************************************
void ListCommands(const char *Command) {
  Serial.println();
  Serial.println("Commands:");
  char Info[200];
  for ( size_t i=0; i<nCommands; i++ ) {
    strcpy(Info,"  ");
    strcat(Info,Commands[i].Usage!=0?Commands[i].Usage:Commands[i].Command);
    size_t len=strlen(Info);
    if ( len<16 ) {
      memset(Info+len,' ',16-len);
      Info[16]=0;
    }
    strcat(Info,"- ");
    strcat(Info,Commands[i].Description);
    Serial.println(Info);
  }
  Serial.println();
}

// *****************************************************************************
void CheckCommand() {
  static char Command[20]={0};
  static size_t CommandPos=0;

  if ( Serial.available() ) {
    char ch=Serial.read();
    if ( ch=='\n') {
      char *Params=strchr(Command,' ');
      bool FoundCommand=false;
      if ( Params!=0 ) {
        
        *Params=0;
        Params++;
      }
      for ( size_t i=0; i<nCommands; i++ ) {
        if ( strcmp(Command,Commands[i].Command)==0 ) {
          Commands[i].CommandFunction(Params);
          FoundCommand=true;
          break;
        }
      }
      if ( !FoundCommand ) {
        Serial.print("Unknown command ");
        Serial.println(Command);
      }
      CommandPos=0;
      Command[CommandPos]=0;
    } else {
      if ( ch>=' ' && CommandPos+1<sizeof(Command) ) {
        Command[CommandPos]=ch;
        CommandPos++;
        Command[CommandPos]=0;
      }
    }
  }
}
