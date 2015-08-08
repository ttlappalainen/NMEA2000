/* 
N2kMessages.h

2015 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA

  
This is collection of functions for handling NMEA2000 bus messages. 
Library contains functions to e.g. create message named with PGN like SetN2kPGN129025 and
inline alias for them like SetN2kLatLonRapid.

Each Setxxx funtion sets related message PGN and its default priority. So if you want to change
priority on function, you have to do it after Setxxx call.

If you do not send anything to NMEA2000 bus, you do not need this library. Funtions for
BUS handling PGN:s like 60928 "ISO Address Claim" has been defined in bus device library
NMEA2000.h 
*/

#ifndef _N2kMessages_H_
#define _N2kMessages_H_

#include <N2kMsg.h>

#define TempUndef -300
#define EngineBoostUndef 0xffff
#define EngineTrimTiltUndef 0xff
#define AngleUndef -1000 

inline double RadToDeg(double v) { return v*180.0/3.1415926535897932384626433832795; }
inline double CToKelvin(double v) { return v+273.15; }


typedef enum tN2kHeadingReference {N2khr_true=0,
                                   N2khr_magnetic=1
                                   };
typedef enum tN2kGNSStype {N2kGNSSt_GPS=0,
                           N2kGNSSt_GLONASS=1,
                           N2kGNSSt_GPSGLONASS=2,
                           N2kGNSSt_GPSSBASWAAS=3,
                           N2kGNSSt_GPSSBASWAASGLONASS=4
                           };
typedef enum tN2kGNSSmethod {N2kGNSSm_noGNSS=0,
                             N2kGNSSm_GNSSfix=1,
                             N2kGNSSm_DGNSS=2,
                             N2kGNSSm_PreciseGNSS=3
                             };
typedef enum tN2kTempSource {N2kts_SeaTemperature=0,
                             N2kts_OutsideTemperature=1,
                             N2kts_InsideTemperature=2,
                             N2kts_EngineRoomTemperature=3,
                             N2kts_MainCabinTemperature=4,
                             N2kts_LiveWellTemperature=5,
                             N2kts_BaitWellTemperature=6,
                             N2kts_RefridgerationTemperature=7,
                             N2kts_HeatingSystemTemperature=8,
                             N2kts_FreezerTemperature=9
                             };

typedef enum tN2kTimeSource {N2ktimes_GPS=0,
                             N2ktimes_GLONASS=1,
                             N2ktimes_RadioStation=2,
                             N2ktimes_LocalCesiumClock=3,
                             N2ktimes_LocalRubidiumClock=4,
                             N2ktimes_LocalCrystalClock=5
                             };

typedef enum tN2kFluidType {N2kft_Fuel=0,
                            N2kft_Water=1,
                            N2kft_GrayWater=2,
                            N2kft_LiveWell=3,
                            N2kft_Oil=4,
                            N2kft_BlackWater=5
                             };

//*****************************************************************************
// System date/time
void SetN2kPGN126992(tN2kMsg &N2kMsg, unsigned char SID, unsigned int SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS);
                     
inline void SetN2kPGNSystemTime(tN2kMsg &N2kMsg, unsigned char SID, unsigned int SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS) {
  SetN2kPGN126992(N2kMsg,SID,SystemDate,SystemTime);
}

//*****************************************************************************
// Vessel Heading
// Angles should be in radians
void SetN2kPGN127250(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation, double Variation, tN2kHeadingReference ref);

inline void SetN2kPGNTrueHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading) {
  SetN2kPGN127250(N2kMsg,SID,Heading,AngleUndef,AngleUndef,N2khr_true);
}

inline void SetN2kPGNMagneticHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation=AngleUndef, double Variation=AngleUndef) {
  SetN2kPGN127250(N2kMsg,SID,Heading,Deviation,Variation,N2khr_magnetic);
}

//*****************************************************************************
// Engine parameters rapid
void SetN2kPGN127488(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed, 
                     int EngineBoostPressure=EngineBoostUndef, unsigned char EngineTiltTrim=EngineTrimTiltUndef);

inline void SetN2kEngineParamRapid(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed, 
                      int EngineBoostPressure=EngineBoostUndef, unsigned char EngineTiltTrim=EngineTrimTiltUndef) {
  SetN2kPGN127488(N2kMsg,EngineInstance,EngineSpeed,EngineBoostPressure,EngineTiltTrim);
}

//*****************************************************************************
// Engine parameters dynamic
void SetN2kPGN127489(tN2kMsg &N2kMsg, int EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress, double EngineFuelPress, float EngineLoad, float EngineTorque, bool flagCheckEngine,
                       bool flagOverTemp, bool flagLowOilPress, bool flagLowOilLevel, bool flagLowFuelPress, bool flagLowSystemVoltage, bool flagLowCoolantLevel,
                       bool flagWaterFlow, bool flagWaterInFuel, bool flagChargeIndicator, bool flagPreheatIndicator, bool flagHighBoostPress, bool flagRevLimitExceeded,
                       bool flagEgrSystem, bool flagTPS, bool flagEmergencyStopMode, bool flagWarning1, bool flagWarning2, bool flagPowerReduction,
                       bool flagMaintenanceNeeded, bool flagEngineCommError, bool flagSubThrottle, bool flagNeutralStartProtect, bool flagEngineShuttingDown);
inline void SetN2kEngineDynamicParam(tN2kMsg &N2kMsg, int EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress, double EngineFuelPress, float EngineLoad, float EngineTorque, bool flagCheckEngine,
                       bool flagOverTemp, bool flagLowOilPress, bool flagLowOilLevel, bool flagLowFuelPress, bool flagLowSystemVoltage, bool flagLowCoolantLevel,
                       bool flagWaterFlow, bool flagWaterInFuel, bool flagChargeIndicator, bool flagPreheatIndicator, bool flagHighBoostPress, bool flagRevLimitExceeded,
                       bool flagEgrSystem, bool flagTPS, bool flagEmergencyStopMode, bool flagWarning1, bool flagWarning2, bool flagPowerReduction,
                       bool flagMaintenanceNeeded, bool flagEngineCommError, bool flagSubThrottle, bool flagNeutralStartProtect, bool flagEngineShuttingDown) {
  SetN2kPGN127489(N2kMsg,EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                       FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque, flagCheckEngine,
                       flagOverTemp, flagLowOilPress, flagLowOilLevel, flagLowFuelPress, flagLowSystemVoltage, flagLowCoolantLevel,
                       flagWaterFlow, flagWaterInFuel, flagChargeIndicator, flagPreheatIndicator, flagHighBoostPress, flagRevLimitExceeded,
                       flagEgrSystem, flagTPS, flagEmergencyStopMode, flagWarning1, flagWarning2, flagPowerReduction,
                       flagMaintenanceNeeded, flagEngineCommError, flagSubThrottle, flagNeutralStartProtect, flagEngineShuttingDown);
}
                       
//*****************************************************************************
// Fluid level
void SetN2kPGN127505(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity);

inline void SetN2kFluidLevel(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity) {
  SetN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

bool ParseN2kPGN127505(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity);

inline bool ParseN2kFluidLevel(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity) {
  return ParseN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

//*****************************************************************************
// Water depth
void SetN2kPGN128267(tN2kMsg &N2kMsg, unsigned char SID, double DepthBelowTransducer, double Offset);

inline void SetN2kWaterDepth(tN2kMsg &N2kMsg, unsigned char SID, double DepthBelowTransducer, double Offset) {
  SetN2kPGN128267(N2kMsg,SID,DepthBelowTransducer,Offset);
}

bool ParseN2kPGN128267(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset);

inline bool ParseN2kWaterDepth(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset) {
  return ParseN2kPGN128267(N2kMsg, SID, DepthBelowTransducer, Offset);
}

//*****************************************************************************
// Lat/lon rapid
void SetN2kPGN129025(tN2kMsg &N2kMsg, double Latitude, double Longitude);

inline void SetN2kLatLonRapid(tN2kMsg &N2kMsg, double Latitude, double Longitude) {
  SetN2kPGN129025(N2kMsg,Latitude,Longitude);
}

//*****************************************************************************
// COG SOG rapid
// COG should be in radians
// SOG should be in m/s
void SetN2kPGN129026(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG);

inline void SetN2kCOGSOGRapid(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG) {
  SetN2kPGN129026(N2kMsg,SID,ref,COG,SOG);
}

//*****************************************************************************
void SetN2kPGN129029(tN2kMsg &N2kMsg, unsigned char SID, int DaysSince1970, double SecondsSinceMidnight, 
                     double Latitude, double Longitude, double Altitude, 
                     tN2kGNSStype GNSStype, tN2kGNSSmethod GNSSmethod,
                     unsigned char nSatellites, double HDOP, double PDOP=0, double GeoidalSeparation=0,
                     unsigned char nReferenceStations=0, tN2kGNSStype ReferenceStationType=N2kGNSSt_GPS, int ReferenceSationID=0,
                     double AgeOfCorrection=0
                     );

inline void SetN2kGNSS(tN2kMsg &N2kMsg, unsigned char SID, int DaysSince1970, double SecondsSinceMidnight, 
                     double Latitude, double Longitude, double Altitude, 
                     tN2kGNSStype GNSStype, tN2kGNSSmethod GNSSmethod,
                     unsigned char nSatellites, double HDOP, double PDOP=0, double GeoidalSeparation=0,
                     unsigned char nReferenceStations=0, tN2kGNSStype ReferenceStationType=N2kGNSSt_GPS, int ReferenceSationID=0,
                     double AgeOfCorrection=0
                     ) {
  SetN2kPGN129029(N2kMsg,SID,DaysSince1970,SecondsSinceMidnight,
                  Latitude,Longitude,Altitude,
                  GNSStype,GNSSmethod,
                  nSatellites,HDOP,PDOP,GeoidalSeparation,
                  nReferenceStations,ReferenceStationType,ReferenceSationID,
                  AgeOfCorrection);
}

//*****************************************************************************
// Temperature
// Temperatures should be in Kelvins
void SetN2kPGN130312(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=TempUndef);

inline void SetN2kTemperature(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=TempUndef) {
  SetN2kPGN130312(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature);
}

bool ParseN2kPGN130312(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature);
inline bool ParseN2kTemperature(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  return ParseN2kPGN130312(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature);
}

#endif
