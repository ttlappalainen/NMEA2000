// Demo: NMEA2000 library. 
// This demo reads messages from NMEA 2000 bus and
// sends them translated to clear text to Serial.

// Note! If you use this on Arduino Mega, I prefer to also connect interrupt line
// for MCP2515 and define N2k_CAN_INT_PIN to related line. E.g. MessageSender
// sends so many meaages that Meaga can not handle them. If you only use
// received messages internally without slow operations, then youmay survive
// without interrupt.

#include <Arduino.h>
//#include <Time.h>  // 
//#define N2k_CAN_INT_PIN 21
#include <NMEA2000_CAN.h>
#include <N2kMessages.h>
#include <N2kMessagesEnumToStr.h>

typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg); 
} tNMEA2000Handler;

void SystemTime(const tN2kMsg &N2kMsg);
void EngineRapid(const tN2kMsg &N2kMsg);
void TransmissionParameters(const tN2kMsg &N2kMsg);
void WaterDepth(const tN2kMsg &N2kMsg);
void FluidLevel(const tN2kMsg &N2kMsg);
void OutsideEnvironmental(const tN2kMsg &N2kMsg);
void Temperature(const tN2kMsg &N2kMsg);
void TemperatureExt(const tN2kMsg &N2kMsg);
void DCStatus(const tN2kMsg &N2kMsg);
void BatteryConfigurationStatus(const tN2kMsg &N2kMsg);
void COGSOG(const tN2kMsg &N2kMsg);
void GNSS(const tN2kMsg &N2kMsg);

tNMEA2000Handler NMEA2000Handlers[]={
  {126992L,&SystemTime},
  {127488L,&EngineRapid},
  {127493L,&TransmissionParameters},
  {127505L,&FluidLevel},
  {127506L,&DCStatus},
  {127513L,&BatteryConfigurationStatus},
  {128267L,&WaterDepth},
  {129026L,&COGSOG},
  {129029L,&GNSS},
  {130310L,&OutsideEnvironmental},
  {130312L,&Temperature},
  {130316L,&TemperatureExt},
  {0,0}
};

Stream *OutputStream;

void setup() {
  Serial.begin(115200);
  OutputStream=&Serial;
//   while (!Serial) 
   
  // Do not forward bus messages at all
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  NMEA2000.SetForwardStream(OutputStream);
  // Set false below, if you do not want to see messages parsed to HEX withing library
  NMEA2000.EnableForward(false);
  NMEA2000.SetMsgHandler(HandleNMEA2000Msg);
  NMEA2000.Open();
  OutputStream->print("Running...");
}

//*****************************************************************************
void SystemTime(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    uint16_t SystemDate;
    double SystemTime;
    tN2kTimeSource TimeSource;
    
    if (ParseN2kPGNSystemTime(N2kMsg,SID,SystemDate,SystemTime,TimeSource) ) {
      OutputStream->print("System time: "); OutputStream->println(SID);
      OutputStream->print("  days since 1.1.1970: "); OutputStream->println(SystemDate);
      OutputStream->print("  seconds since midnight: "); OutputStream->println(SystemTime);
      OutputStream->print("  time source: "); PrintN2kEnumType(TimeSource,OutputStream);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void EngineRapid(const tN2kMsg &N2kMsg) {
    unsigned char EngineInstance;
    double EngineSpeed;
    double EngineBoostPressure;
    unsigned char EngineTiltTrim;
    
    if (ParseN2kEngineParamRapid(N2kMsg,EngineInstance,EngineSpeed,EngineBoostPressure,EngineTiltTrim) ) {
      OutputStream->print("Engine rapid params: "); OutputStream->println(EngineInstance);
      OutputStream->print("  RPM: "); OutputStream->println(EngineSpeed);
      OutputStream->print("  boost pressure: "); OutputStream->println(EngineBoostPressure);
      OutputStream->print("  tilt trim: "); OutputStream->println(EngineTiltTrim);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void TransmissionParameters(const tN2kMsg &N2kMsg) {
    unsigned char EngineInstance;
    tN2kTransmissionGear TransmissionGear;
    double OilPressure;
    double OilTemperature;
    unsigned char DiscreteStatus1;
    
    if (ParseN2kTransmissionParameters(N2kMsg,EngineInstance, TransmissionGear, OilPressure, OilTemperature, DiscreteStatus1) ) {
      OutputStream->print("Transmission params: "); OutputStream->println(EngineInstance);
      OutputStream->print("  gear: "); PrintN2kEnumType(TransmissionGear,OutputStream);
      OutputStream->print("  oil pressure: "); OutputStream->println(OilPressure);
      OutputStream->print("  oil temperature: "); OutputStream->println(KelvinToC(OilTemperature));
      OutputStream->print("  discrete status: "); OutputStream->println(DiscreteStatus1);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void COGSOG(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    tN2kHeadingReference HeadingReference;
    double COG;
    double SOG;
    
    if (ParseN2kCOGSOGRapid(N2kMsg,SID,HeadingReference,COG,SOG) ) {
      OutputStream->print("COG/SOG: "); OutputStream->println(SID);
      OutputStream->print("  reference: "); PrintN2kEnumType(HeadingReference,OutputStream);
      OutputStream->print("  COG: "); OutputStream->println(RadToDeg(COG));
      OutputStream->print("  SOG: "); OutputStream->println(SOG);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void GNSS(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    uint16_t DaysSince1970;
    double SecondsSinceMidnight; 
    double Latitude;
    double Longitude;
    double Altitude; 
    tN2kGNSStype GNSStype;
    tN2kGNSSmethod GNSSmethod;
    unsigned char nSatellites;
    double HDOP;
    double PDOP;
    double GeoidalSeparation;
    unsigned char nReferenceStations;
    tN2kGNSStype ReferenceStationType;
    uint16_t ReferenceSationID;
    double AgeOfCorrection;

    if (ParseN2kGNSS(N2kMsg,SID,DaysSince1970,SecondsSinceMidnight,
                  Latitude,Longitude,Altitude,
                  GNSStype,GNSSmethod,
                  nSatellites,HDOP,PDOP,GeoidalSeparation,
                  nReferenceStations,ReferenceStationType,ReferenceSationID,
                  AgeOfCorrection) ) {
      OutputStream->print("GNSS info: "); OutputStream->println(SID);
      OutputStream->print("  days since 1.1.1970: "); OutputStream->println(DaysSince1970);
      OutputStream->print("  seconds since midnight: "); OutputStream->println(SecondsSinceMidnight);
      OutputStream->print("  latitude: "); OutputStream->println(Latitude);
      OutputStream->print("  longitude: "); OutputStream->println(Longitude);
      OutputStream->print("  altitude: "); OutputStream->println(Altitude);
      OutputStream->print("  GNSS type: "); PrintN2kEnumType(GNSStype,OutputStream);
      OutputStream->print("  GNSS method: "); PrintN2kEnumType(GNSSmethod,OutputStream);
      OutputStream->print("  satellite count: "); OutputStream->println(nSatellites);
      OutputStream->print("  HDOP: "); OutputStream->println(HDOP);
      OutputStream->print("  PDOP: "); OutputStream->println(PDOP);
      OutputStream->print("  geoidal separation: "); OutputStream->println(GeoidalSeparation);
      OutputStream->print("  reference stations: "); OutputStream->println(nReferenceStations);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void OutsideEnvironmental(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    double WaterTemperature;
    double OutsideAmbientAirTemperature;
    double AtmosphericPressure;
    
    if (ParseN2kOutsideEnvironmentalParameters(N2kMsg,SID,WaterTemperature,OutsideAmbientAirTemperature,AtmosphericPressure) ) {
      OutputStream->print("Water temp: "); OutputStream->print(KelvinToC(WaterTemperature));
      OutputStream->print(", outside ambient temp: "); 
      if (OutsideAmbientAirTemperature!=TempUndef) {
        OutputStream->print(KelvinToC(OutsideAmbientAirTemperature));
      } else OutputStream->print("not available");   
      OutputStream->print(", pressure: ");
      if (AtmosphericPressure!=PressureUndef) {
        OutputStream->println(PascalTomBar(AtmosphericPressure));    
      } else OutputStream->println("not available");
    } else {
      OutputStream->print("Failed to parse PGN: ");  OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void Temperature(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    unsigned char TempInstance;
    tN2kTempSource TempSource;
    double ActualTemperature;
    double SetTemperature;
    
    if (ParseN2kTemperature(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature) ) {
      OutputStream->print("Temperature source: "); PrintN2kEnumType(TempSource,OutputStream,false);
      OutputStream->print(", actual temperature: "); OutputStream->print(KelvinToC(ActualTemperature));
      OutputStream->print(", set temperature: ");
      if (SetTemperature>TempUndef) {
        OutputStream->println(KelvinToC(SetTemperature));
      } else {
        OutputStream->println("undefined");
      }
    } else {
      OutputStream->print("Failed to parse PGN: ");  OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void TemperatureExt(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    unsigned char TempInstance;
    tN2kTempSource TempSource;
    double ActualTemperature;
    double SetTemperature;
    
    if (ParseN2kTemperatureExt(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature) ) {
      OutputStream->print("Temperature source: "); PrintN2kEnumType(TempSource,OutputStream,false);
      OutputStream->print(", actual temperature: "); OutputStream->print(KelvinToC(ActualTemperature));
      OutputStream->print(", set temperature: ");
      if (SetTemperature>TempUndef) {
        OutputStream->println(KelvinToC(SetTemperature));
      } else {
        OutputStream->println("undefined");
      }
    } else {
      OutputStream->print("Failed to parse PGN: ");  OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void BatteryConfigurationStatus(const tN2kMsg &N2kMsg) {
    unsigned char BatInstance;
    tN2kBatType BatType;
    tN2kBatEqSupport SupportsEqual;
    tN2kBatNomVolt BatNominalVoltage;
    tN2kBatChem BatChemistry;
    double BatCapacity;
    double BatTemperatureCoefficient;
    double PeukertExponent; 
    double ChargeEfficiencyFactor;

    if (ParseN2kBatConf(N2kMsg,BatInstance,BatType,SupportsEqual,BatNominalVoltage,BatChemistry,BatCapacity,BatTemperatureCoefficient,PeukertExponent,ChargeEfficiencyFactor) ) {
      OutputStream->print("Battery instance: ");
      OutputStream->println(BatInstance);
      OutputStream->print("  - type: "); PrintN2kEnumType(BatType,OutputStream);
      OutputStream->print("  - support equal.: "); PrintN2kEnumType(SupportsEqual,OutputStream);
      OutputStream->print("  - nominal voltage: "); PrintN2kEnumType(BatNominalVoltage,OutputStream);
      OutputStream->print("  - chemistry: "); PrintN2kEnumType(BatChemistry,OutputStream);
      OutputStream->print("  - capacity (Ah): "); OutputStream->println(CoulombToAh(BatCapacity));
      OutputStream->print("  - temperature coefficient: "); OutputStream->println(BatTemperatureCoefficient);
      OutputStream->print("  - peukert exponent: "); OutputStream->println(PeukertExponent);
      OutputStream->print("  - charge efficiency factor: "); OutputStream->println(ChargeEfficiencyFactor);
    } else {
      OutputStream->print("Failed to parse PGN: "); OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void DCStatus(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    unsigned char DCInstance;
    tN2kDCType DCType;
    unsigned char StateOfCharge;
    unsigned char StateOfHealth;
    double TimeRemaining;
    double RippleVoltage;
    
    if (ParseN2kDCStatus(N2kMsg,SID,DCInstance,DCType,StateOfCharge,StateOfHealth,TimeRemaining,RippleVoltage) ) {
      OutputStream->print("DC instance: ");
      OutputStream->println(DCInstance);
      OutputStream->print("  - type: "); PrintN2kEnumType(DCType,OutputStream);
      OutputStream->print("  - state of charge (%): "); OutputStream->println(StateOfCharge);
      OutputStream->print("  - state of health (%): "); OutputStream->println(StateOfHealth);
      OutputStream->print("  - time remaining (h): "); OutputStream->println(TimeRemaining/60);
      OutputStream->print("  - ripple voltage: "); OutputStream->println(RippleVoltage);
    } else {
      OutputStream->print("Failed to parse PGN: ");  OutputStream->println(N2kMsg.PGN);
    }
}

//*****************************************************************************
void WaterDepth(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    double DepthBelowTransducer;
    double Offset;

    if (ParseN2kWaterDepth(N2kMsg,SID,DepthBelowTransducer,Offset) ) {
      if (Offset>0) {
        OutputStream->print("Water depth:");
      } else {
        OutputStream->print("Depth below keel:");
      }
      OutputStream->println(DepthBelowTransducer+Offset);
    }
}

//*****************************************************************************
void FluidLevel(const tN2kMsg &N2kMsg) {
    unsigned char Instance;
    tN2kFluidType FluidType;
    double Level=0;
    double Capacity=0;

    if (ParseN2kFluidLevel(N2kMsg,Instance,FluidType,Level,Capacity) ) {
      switch (FluidType) {
        case N2kft_Fuel:
          OutputStream->print("Fuel level :");
          break;
        case N2kft_Water:
          OutputStream->print("Water level :");
          break;
        case N2kft_GrayWater:
          OutputStream->print("Gray water level :");
          break;
        case N2kft_LiveWell:
          OutputStream->print("Live well level :");
          break;
        case N2kft_Oil:
          OutputStream->print("Oil level :");
          break;
        case N2kft_BlackWater:
          OutputStream->print("Black water level :");
          break;
      }
      OutputStream->print(Level); OutputStream->print("%"); 
      OutputStream->print(" ("); OutputStream->print(Capacity*Level/100); OutputStream->print("l)");
      OutputStream->print(" capacity :"); OutputStream->println(Capacity);
    }
}

//*****************************************************************************
//NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  int iHandler;
  
  // Find handler
  OutputStream->print("In Main Handler: "); OutputStream->println(N2kMsg.PGN);
  for (iHandler=0; NMEA2000Handlers[iHandler].PGN!=0 && !(N2kMsg.PGN==NMEA2000Handlers[iHandler].PGN); iHandler++);
  
  if (NMEA2000Handlers[iHandler].PGN!=0) {
    NMEA2000Handlers[iHandler].Handler(N2kMsg); 
  }
}

//*****************************************************************************
void loop() 
{ 
  NMEA2000.ParseMessages();
}

