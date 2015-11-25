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
#define HumidityUndef -1
#define PressureUndef -1
#define EngineBoostUndef 0xffff
#define EngineTrimTiltUndef 0xff
#define AngleUndef -1000 
#define SpeedUndef -1000 

inline double RadToDeg(double v) { return v*180.0/3.1415926535897932384626433832795; }
inline double DegToRad(double v) { return v/180.0*3.1415926535897932384626433832795; }
inline double CToKelvin(double v) { return v+273.15; }
inline double mBarToPascal(double v) { return v*100; }


typedef enum tN2kHeadingReference {
                            N2khr_true=0,
                            N2khr_magnetic=1
                          };
typedef enum tN2kDistanceCalculationType {
                            N2kdct_GreatCircle=0,
                            N2kdct_RhumbLine=1
                          };
typedef enum tN2kXTEMode  {
                            N2kxtem_Autonomous=0,
                            N2kxtem_Differential=1,
                            N2kxtem_Estimated=2,
                            N2kxtem_Simulator=3,
                            N2kxtem_Manual=4
                          };
typedef enum tN2kGNSStype {
                            N2kGNSSt_GPS=0,
                            N2kGNSSt_GLONASS=1,
                            N2kGNSSt_GPSGLONASS=2,
                            N2kGNSSt_GPSSBASWAAS=3,
                            N2kGNSSt_GPSSBASWAASGLONASS=4
                          };
typedef enum tN2kGNSSmethod {
                            N2kGNSSm_noGNSS=0,
                            N2kGNSSm_GNSSfix=1,
                            N2kGNSSm_DGNSS=2,
                            N2kGNSSm_PreciseGNSS=3
                          };
typedef enum tN2kTempSource {
                            N2kts_SeaTemperature=0,
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

typedef enum tN2kHumiditySource {
                            N2khs_InsideHumidity=0,
                            N2khs_OutsideHumidity=1,
                            N2khs_Undef=1
                          };

typedef enum tN2kTimeSource {
                            N2ktimes_GPS=0,
                            N2ktimes_GLONASS=1,
                            N2ktimes_RadioStation=2,
                            N2ktimes_LocalCesiumClock=3,
                            N2ktimes_LocalRubidiumClock=4,
                            N2ktimes_LocalCrystalClock=5
                          };

typedef enum tN2kFluidType {
                            N2kft_Fuel=0,
                            N2kft_Water=1,
                            N2kft_GrayWater=2,
                            N2kft_LiveWell=3,
                            N2kft_Oil=4,
                            N2kft_BlackWater=5
                          };

typedef enum tN2kWindReference {
                            N2kWind_True_North=0,
                            N2kWind_Magnetic=1,
                            N2kWind_Apprent=2,
                            N2kWind_True_boat=3
                          };

typedef enum tN2kSpeedWaterReferenceType {
                            N2kSWRT_Paddle_wheel=0,
                            N2kSWRT_Pitot_tube=1,
                            N2kSWRT_Doppler_log=2,
                            N2kSWRT_Ultra_Sound=3,
                            N2kSWRT_Electro_magnetic=4
                          };

typedef enum tN2kRudderDirectionOrder {
                            N2kRDO_NoDirectioOrder=0,
                            N2kRDO_MoveToStarboard=1,
                            N2kRDO_MoveToPort=2
                          };
                             
//*****************************************************************************
// System date/time
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN126992(tN2kMsg &N2kMsg, unsigned char SID, unsigned int SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS);
                     
inline void SetN2kPGNSystemTime(tN2kMsg &N2kMsg, unsigned char SID, unsigned int SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS) {
  SetN2kPGN126992(N2kMsg,SID,SystemDate,SystemTime);
}

//*****************************************************************************
// Rudder
// Input:
// - RudderPosition         Current rudder postion in radians.
// - Instance               Rudder instance. 
// - RudderDirectionOrder   See tN2kRudderDirectionOrder. Direction, where rudder should be turned.
// - AngleOrder             In radians angle where rudder should be turned.
void SetN2kPGN127245(tN2kMsg &N2kMsg, double RudderPosition, unsigned char Instance=0, 
                     tN2kRudderDirectionOrder RudderDirectionOrder=N2kRDO_NoDirectioOrder, double AngleOrder=AngleUndef);

inline void SetN2kRudder(tN2kMsg &N2kMsg, double RudderPosition, unsigned char Instance=0, 
                     tN2kRudderDirectionOrder RudderDirectionOrder=N2kRDO_NoDirectioOrder, double AngleOrder=AngleUndef) {
  SetN2kPGN127245(N2kMsg,RudderPosition,Instance,RudderDirectionOrder,AngleOrder);
}
                     
//*****************************************************************************
// Vessel Heading
// Input:
//  - SID                   Sequence ID. If your device is e.g. boat speed and heading at same time, you can set same SID for different messages
//                          to indicate that they are measured at same time.
//  - Heading               Heading in radians
//  - Deviation             Magnetic deviation in radians. Use AngleUndef for undefined value.
//  - Variation             Magnetic variation in radians. Use AngleUndef for undefined value.
//  - ref                   Heading reference. See definition of tN2kHeadingReference.
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN127250(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation, double Variation, tN2kHeadingReference ref);

inline void SetN2kPGNTrueHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading) {
  SetN2kPGN127250(N2kMsg,SID,Heading,AngleUndef,AngleUndef,N2khr_true);
}

inline void SetN2kPGNMagneticHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation=AngleUndef, double Variation=AngleUndef) {
  SetN2kPGN127250(N2kMsg,SID,Heading,Deviation,Variation,N2khr_magnetic);
}

//*****************************************************************************
// Engine parameters rapid
// Input:
//  - EngineInstance        Engine instance.
//  - EngineSpeed           RPM (Revolutions Per Minute)
//  - EngineBoostPressure   in Pascal
//  - EngineTiltTrim        in %
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN127488(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed, 
                     int EngineBoostPressure=EngineBoostUndef, unsigned char EngineTiltTrim=EngineTrimTiltUndef);

inline void SetN2kEngineParamRapid(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed, 
                      int EngineBoostPressure=EngineBoostUndef, unsigned char EngineTiltTrim=EngineTrimTiltUndef) {
  SetN2kPGN127488(N2kMsg,EngineInstance,EngineSpeed,EngineBoostPressure,EngineTiltTrim);
}

//*****************************************************************************
// Engine parameters dynamic
// Input:
//  - EngineInstance        Engine instance.
//  - EngineOilPress        in Pascal
//  - EngineOilTemp         in Celcius
//  - EngineCoolantTemp     in Celcius
//  - AltenatorVoltage      in Voltage
//  - FuelRate              in litres/hour
//  - EngineHours           in seconds
//  - EngineCoolantPress    in Pascal
//  - EngineFuelPress       in Pascal
//  - EngineLoad            in %
//  - EngineTorque          in %
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
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
// Input:
//  - Instance              Tank instance. Different devices handles this a bit differently. So it is best to have instance unique over 
//                          all devices on the bus. 
//  - FluidType             Defines type of fluid. See definition of tN2kFluidType
//  - Level                 Tank level in % of full tank.
//  - Capacity              Tank Capacity in litres
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN127505(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity);

inline void SetN2kFluidLevel(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity) {
  SetN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

// Parse fluid level message
// Output:
//  - Instance              Tank instance.  
//  - FluidType             Defines type of fluid. See definition of tN2kFluidType
//  - Level                 Tank level in % of full tank.
//  - Capacity              Tank Capacity in litres
bool ParseN2kPGN127505(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity);

inline bool ParseN2kFluidLevel(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity) {
  return ParseN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

//*****************************************************************************
// Boat speed
// Input:
//  - SID                   Sequence ID. If your device is e.g. boat speed and wind at same time, you can set same SID for different messages
//                          to indicate that they are measured at same time.
//  - WaterRefereced        Speed over water in m/s
//  - GroundReferenced      Ground referenced speed in m/s
//  - SWRT                  Type of transducer. See definition for tN2kSpeedWaterReferenceType
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN128259(tN2kMsg &N2kMsg, unsigned char SID, double WaterRefereced, double GroundReferenced=SpeedUndef, tN2kSpeedWaterReferenceType SWRT=N2kSWRT_Paddle_wheel);

inline void SetN2kBoatSpeed(tN2kMsg &N2kMsg, unsigned char SID, double WaterRefereced, double GroundReferenced=SpeedUndef, tN2kSpeedWaterReferenceType SWRT=N2kSWRT_Paddle_wheel) {
  SetN2kPGN128259(N2kMsg,SID,WaterRefereced,GroundReferenced,SWRT);
}
 
//*****************************************************************************
// Water depth
// Input:
//  - SID                   Sequence ID. If your device is e.g. boat speed and depth at same time, you can set same SID for different messages
//                          to indicate that they are measured at same time.
//  - DepthBelowTransducer  Depth below transducer in meters
//  - Offset                Distance in meters between transducer and surface (positive) or transducer and keel (negative) 
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
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
// Input:
//  - Latitude               Latitude in degrees
//  - Longitude              Longitude in degrees
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN129025(tN2kMsg &N2kMsg, double Latitude, double Longitude);

inline void SetN2kLatLonRapid(tN2kMsg &N2kMsg, double Latitude, double Longitude) {
  SetN2kPGN129025(N2kMsg,Latitude,Longitude);
}

//*****************************************************************************
// COG SOG rapid
// Input:
//  - COG                   Cource Over Ground in radians
//  - SOG                   Speed Over Ground in m/s
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN129026(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG);

inline void SetN2kCOGSOGRapid(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG) {
  SetN2kPGN129026(N2kMsg,SID,ref,COG,SOG);
}

//*****************************************************************************
// GNSS Position Data
// Input:
//  - SID                   Sequence ID. If your device is e.g. boat speed and GPS at same time, you can set same SID for different messages
//                          to indicate that they are measured at same time.
//  - DaysSince1970         Days since 1.1.1970. You can find sample how to convert e.g. NMEA0183 date info to this on my NMEA0183 library on
//                          NMEA0183Messages.cpp on function NMEA0183ParseRMC_nc
//  - Latitude              Latitude in degrees
//  - Longitude             Longitude in degrees
//  - Altitude              Altitude in meters
//  - GNSStype              GNSS type. See definition of tN2kGNSStype
//  - GNSSmethod            GNSS method type. See definition of tN2kGNSSmethod
//  - nSatellites           Number of satellites used for data
//  - HDOP                  Horizontal Dilution Of Precision in meters.
//  - PDOP                  Probable dilution of precision in meters.
//  - GeoidalSeparation     Geoidal separation in meters
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
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
// Cross Track Error
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN129283(tN2kMsg &N2kMsg, unsigned char SID, tN2kXTEMode XTEMode, bool NavigationTerminated, double XTE);

inline void SetN2kXTE(tN2kMsg &N2kMsg, unsigned char SID, tN2kXTEMode XTEMode, bool NavigationTerminated, double XTE) {
  SetN2kPGN129283(N2kMsg, SID, XTEMode, NavigationTerminated, XTE);
}

//*****************************************************************************
// Navigation info
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN129284(tN2kMsg &N2kMsg, unsigned char SID, double DistanceToWaypoint, tN2kHeadingReference BearingReference,
                      bool PerpendicularCrossed, bool ArrivalCircleEntered, tN2kDistanceCalculationType CalculationType,
                      double ETATime, int ETADate, double BearingOriginToDestinationWaypoint, double BearingPositionToDestinationWaypoint,
                      unsigned long OriginWaypointNumber, unsigned long DestinationWaypointNumber, 
                      double DestinationLatitude, double DestinationLongitude, double WaypointClosingVelocity);

inline void SetN2kNavigationInfo(tN2kMsg &N2kMsg, unsigned char SID, double DistanceToWaypoint, tN2kHeadingReference BearingReference,
                      bool PerpendicularCrossed, bool ArrivalCircleEntered, tN2kDistanceCalculationType CalculationType,
                      double ETATime, int ETADate, double BearingOriginToDestinationWaypoint, double BearingPositionToDestinationWaypoint,
                      unsigned long OriginWaypointNumber, unsigned long DestinationWaypointNumber, 
                      double DestinationLatitude, double DestinationLongitude, double WaypointClosingVelocity) {
  SetN2kPGN129284(N2kMsg, SID, DistanceToWaypoint, BearingReference,
                      PerpendicularCrossed, ArrivalCircleEntered, CalculationType,
                      ETATime, ETADate, BearingOriginToDestinationWaypoint, BearingPositionToDestinationWaypoint,
                      OriginWaypointNumber, DestinationWaypointNumber, 
                      DestinationLatitude, DestinationLongitude, WaypointClosingVelocity);                      
}
                      
//*****************************************************************************
// Wind Speed
// Input:
//  - SID                   Sequence ID. If your device is e.g. boat speed and wind at same time, you can set same SID for different messages
//                          to indicate that they are measured at same time.
//  - WindSpeed             Measured wind speed in m/s
//  - WindAngle             Measured wind angle in radians. If you have value in degrees, use function DegToRad(myval) in call.
//  - WindReference         Wind reference, see definition of tN2kWindReference
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN130306 (tN2kMsg &N2kMsg, unsigned char SID, double WindSpeed, double WindAngle, tN2kWindReference WindReference);

inline void SetN2kWindSpeed(tN2kMsg &N2kMsg, unsigned char SID, double WindSpeed, double WindAngle, tN2kWindReference WindReference) {
  SetN2kPGN130306(N2kMsg,SID,WindSpeed,WindAngle,WindReference);
}

//bool ParseN2kPGN130306(const tN2kMsg &N2kMsg, unsigned char &SID, double &WindSpeed, double &WindAngle, tN2kWindType &WindType);

//inline bool ParseN2kPGN130306(const tN2kMsg &N2kMsg, unsigned char &SID, double &WindSpeed, double &Offset) {
//  return ParseN2kWaterDepth(N2kMsg, SID, DepthBelowTransducer, Offset);
//}

//*****************************************************************************
// Outside Environmental parameters
// Input:
//  - SID                   Sequence ID. 
//  - WaterTemperature      Water temperature in °K. Use function CToKelvin, if you want to use °C.
//  - OutsideAmbientAirTemperature      Outside ambient temperature in °K. Use function CToKelvin, if you want to use °C.
//  - AtmosphericPressure   Atmospheric pressure in Pascals. Use function mBarToPascal, if you like to use mBar
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN130310(tN2kMsg &N2kMsg, unsigned char SID, double WaterTemperature,
                     double OutsideAmbientAirTemperature=TempUndef, double AtmosphericPressure=PressureUndef);

inline void SetN2kOutsideEnvironmentalParameters(tN2kMsg &N2kMsg, unsigned char SID, double WaterTemperature,
                     double OutsideAmbientAirTemperature=TempUndef, double AtmosphericPressure=PressureUndef) {
  SetN2kPGN130310(N2kMsg,SID,WaterTemperature,OutsideAmbientAirTemperature,AtmosphericPressure);
}
//*****************************************************************************
// Environmental parameters
// Note that in PGN 130311 TempInstance is as TempSource in PGN 130312. I do not know why this
// renaming is confusing.
// Pressure has to be in pascal. Use function mBarToPascal, if you like to use mBar
// Input:
//  - SID                   Sequence ID. 
//  - TempInstance          see tN2kTempSource
//  - Temperature           Temperature in °K. Use function CToKelvin, if you want to use °C.
//  - HumidityInstance      see tN2kHumiditySource.
//  - Humidity              Humidity in %
//  - AtmosphericPressure   Atmospheric pressure in Pascals. Use function mBarToPascal, if you like to use mBar
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kPGN130311(tN2kMsg &N2kMsg, unsigned char SID, tN2kTempSource TempInstance, double Temperature,
                     tN2kHumiditySource HumidityInstance=N2khs_Undef, double Humidity=HumidityUndef, double AtmosphericPressure=PressureUndef);

inline void SetN2kEnvironmentalParameters(tN2kMsg &N2kMsg, unsigned char SID, tN2kTempSource TempInstance, double Temperature,
                     tN2kHumiditySource HumidityInstance=N2khs_Undef, double Humidity=HumidityUndef, double AtmosphericPressure=PressureUndef) {
  SetN2kPGN130311(N2kMsg,SID,TempInstance,Temperature,HumidityInstance,Humidity,AtmosphericPressure);
}

//*****************************************************************************
// Temperature
// Temperatures should be in Kelvins
// Input:
//  - SID                   Sequence ID. 
//  - TempInstance          This should be unic at least on one device. May be best to have it unic over all devices sending this PGN.
//  - TempSource            see tN2kTempSource
//  - ActualTemperature     Temperature in °K. Use function CToKelvin, if you want to use °C. 
//  - SetTemperature        Set temperature in °K. Use function CToKelvin, if you want to use °C. This is meaningfull for temperatures,
//                          which can be controlled like cabin, freezer, refridgeration temperature. God can use value for this for
//                          outside and sea temperature values.
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
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
