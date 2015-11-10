/* 
N2kMessages.cpp

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
*/

#include <N2kMessages.h>

//*****************************************************************************
// System time
void SetN2kPGN126992(tN2kMsg &N2kMsg, unsigned char SID, unsigned int SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource) {
    N2kMsg.SetPGN(126992L);
    N2kMsg.Priority=3;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte(TimeSource);
    N2kMsg.Add2ByteInt(SystemDate);
    N2kMsg.Add4ByteDouble(SystemTime,0.0001);
}

//*****************************************************************************
// Vessel Heading
// Angles should be in radians
void SetN2kPGN127250(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation, double Variation, tN2kHeadingReference ref) {
    N2kMsg.SetPGN(127250L);
    N2kMsg.Priority=2;
    N2kMsg.AddByte(SID);
    N2kMsg.Add2ByteDouble(Heading,0.0001);
    if (Deviation!=AngleUndef) { N2kMsg.Add2ByteDouble(Deviation,0.0001); } else { N2kMsg.Add2ByteInt(0x7fff); }
    if (Variation!=AngleUndef) { N2kMsg.Add2ByteDouble(Variation,0.0001); } else { N2kMsg.Add2ByteInt(0x7fff); }
    N2kMsg.AddByte(0xfc | ref);
}

//*****************************************************************************
// Engine rapid param
void SetN2kPGN127488(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed, 
                     int EngineBoostPressure, unsigned char EngineTiltTrim) {
    N2kMsg.SetPGN(127488L);
    N2kMsg.Priority=3;
    N2kMsg.AddByte(EngineInstance);
    N2kMsg.Add2ByteDouble(EngineSpeed,0.25);
    N2kMsg.Add2ByteInt(EngineBoostPressure);
    N2kMsg.AddByte(EngineTiltTrim);
    N2kMsg.AddByte(0xff); // Reserved
    N2kMsg.AddByte(0xff); // Reserved
}

//*****************************************************************************
void SetN2kPGN127489(tN2kMsg &N2kMsg, int EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress, double EngineFuelPress, float EngineLoad, float EngineTorque, bool flagCheckEngine,
                       bool flagOverTemp, bool flagLowOilPress, bool flagLowOilLevel, bool flagLowFuelPress, bool flagLowSystemVoltage, bool flagLowCoolantLevel,
                       bool flagWaterFlow, bool flagWaterInFuel, bool flagChargeIndicator, bool flagPreheatIndicator, bool flagHighBoostPress, bool flagRevLimitExceeded,
                       bool flagEgrSystem, bool flagTPS, bool flagEmergencyStopMode, bool flagWarning1, bool flagWarning2, bool flagPowerReduction,
                       bool flagMaintenanceNeeded, bool flagEngineCommError, bool flagSubThrottle, bool flagNeutralStartProtect, bool flagEngineShuttingDown) {
    N2kMsg.SetPGN(127489L);
    N2kMsg.Priority=6;
                       
  N2kMsg.AddByte(EngineInstance);
  N2kMsg.Add2ByteDouble(EngineOilPress, 1);
  N2kMsg.Add2ByteDouble(EngineOilTemp + 273.15, 0.01);
  N2kMsg.Add2ByteDouble(EngineCoolantTemp + 273.15, 0.01);
  N2kMsg.Add2ByteDouble(AltenatorVoltage, 0.01);
  N2kMsg.Add2ByteDouble(FuelRate, 0.1);
  N2kMsg.Add4ByteDouble(EngineHours, 1);
  N2kMsg.Add2ByteDouble(EngineCoolantPress, 1);
  N2kMsg.Add2ByteDouble(EngineFuelPress, 1);
  N2kMsg.AddByte(0xff);  // reserved
  N2kMsg.Add2ByteInt(0xffff);  // Discrete Status 1
  N2kMsg.Add2ByteInt(0xffff);  // Discrete Status 2

  int engineStatus1P1 = B00000000;
  int engineStatus1P2 = B00000000;
  int engineStatus2 = B00000000;
  if (flagCheckEngine) engineStatus1P1 |= B00000001;
  if (flagOverTemp) engineStatus1P1 |= B00000010;
  if (flagLowOilPress) engineStatus1P1 |= B00000100;
  if (flagLowOilLevel) engineStatus1P1 |= B00001000;
  if (flagLowFuelPress) engineStatus1P1 |= B00010000;
  if (flagLowSystemVoltage) engineStatus1P1 |= B00100000;
  if (flagLowCoolantLevel) engineStatus1P1 |= B01000000;
  if (flagWaterFlow) engineStatus1P1 |= B10000000;

  if (flagWaterInFuel) engineStatus1P2 |= B00000001;
  if (flagChargeIndicator) engineStatus1P2 |= B00000010;
  if (flagPreheatIndicator) engineStatus1P2 |= B00000100;
  if (flagHighBoostPress) engineStatus1P2 |= B00001000;
  if (flagRevLimitExceeded) engineStatus1P2 |= B00010000;
  if (flagEgrSystem) engineStatus1P2 |= B00100000;
  if (flagTPS) engineStatus1P2 |= B01000000;
  if (flagEmergencyStopMode) engineStatus1P2 |= B10000000;

  if (flagWarning1) engineStatus2 |= B00000001;
  if (flagWarning2) engineStatus2 |= B00000010;
  if (flagPowerReduction) engineStatus2 |= B00000100;
  if (flagMaintenanceNeeded) engineStatus2 |= B00001000;
  if (flagEngineCommError) engineStatus2 |= B00010000;
  if (flagSubThrottle) engineStatus2 |= B00100000;
  if (flagNeutralStartProtect) engineStatus2 |= B01000000;
  if (flagEngineShuttingDown) engineStatus2 |= B10000000;

  N2kMsg.AddByte(EngineLoad);
  N2kMsg.AddByte(EngineTorque);
}

//*****************************************************************************
// Fluid level
void SetN2kPGN127505(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity) {
    N2kMsg.SetPGN(127505L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(Instance&0x0f | (FluidType&0x0f)<<4);
    N2kMsg.Add2ByteDouble(Level,0.004);
    N2kMsg.Add4ByteDouble(Capacity,0.1);
    N2kMsg.AddByte(0xff); // Reserved
}

bool ParseN2kPGN127505(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity) {
  if (N2kMsg.PGN!=127505L) return false;

  int Index=0;
  unsigned char IFt=N2kMsg.GetByte(Index);
  
  Instance=IFt&0x0f;
  FluidType=(tN2kFluidType)((IFt>>4)&0x0f);
  Level=N2kMsg.Get2ByteDouble(0.004,Index);
  Capacity=N2kMsg.Get4ByteDouble(0.1,Index);
  
  return true;
}

//*****************************************************************************
// Boat speed
void SetN2kPGN128259(tN2kMsg &N2kMsg, unsigned char SID, double WaterRefereced, double GroundReferenced, tN2kSpeedWaterReferenceType SWRT) {
    N2kMsg.SetPGN(128259L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.Add2ByteDouble(WaterRefereced,0.01);
    if (GroundReferenced!=SpeedUndef) { N2kMsg.Add2ByteDouble(GroundReferenced,0.01); } else { N2kMsg.Add2ByteInt(0xffff); }
    N2kMsg.AddByte(SWRT);
    N2kMsg.AddByte(0xff); // Reserved
}

//*****************************************************************************
// Water depth
void SetN2kPGN128267(tN2kMsg &N2kMsg, unsigned char SID, double DepthBelowTransducer, double Offset) {
    N2kMsg.SetPGN(128267L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.Add4ByteDouble(DepthBelowTransducer,0.01);
    N2kMsg.Add2ByteDouble(Offset,0.001);
    N2kMsg.AddByte(0xff); // Reserved
}

bool ParseN2kPGN128267(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset) {
  if (N2kMsg.PGN!=128267L) return false;

  int Index=0;
  SID=N2kMsg.GetByte(Index);
  DepthBelowTransducer=N2kMsg.Get4ByteDouble(0.01,Index);
  Offset=N2kMsg.Get2ByteDouble(0.001,Index);
  
  return true;
}

//*****************************************************************************
// Lat long rapid
void SetN2kPGN129025(tN2kMsg &N2kMsg, double Latitude, double Longitude) {
    N2kMsg.SetPGN(129025L);
    N2kMsg.Priority=3;
    N2kMsg.Add4ByteDouble(Latitude,1e-7);
    N2kMsg.Add4ByteDouble(Longitude,1e-7);
}

//*****************************************************************************
// COG SOG rapid
// COG should be in radians
// SOG should be in m/s
void SetN2kPGN129026(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG) {
    N2kMsg.SetPGN(129026L);
    N2kMsg.Priority=3;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((ref==N2khr_true?0xfc:0xfd));
    N2kMsg.Add2ByteDouble(COG,0.0001); //0.0057295779513082332);
    N2kMsg.Add2ByteDouble(SOG,0.01);
    N2kMsg.AddByte(0xff);
    N2kMsg.AddByte(0xff);
}

//*****************************************************************************
// GNSS Position Data
void SetN2kPGN129029(tN2kMsg &N2kMsg, unsigned char SID, int DaysSince1970, double SecondsSinceMidnight, 
                     double Latitude, double Longitude, double Altitude, 
                     tN2kGNSStype GNSStype, tN2kGNSSmethod GNSSmethod,
                     unsigned char nSatellites, double HDOP, double PDOP, double GeoidalSeparation,
                     unsigned char nReferenceStations, tN2kGNSStype ReferenceStationType, int ReferenceSationID,
                     double AgeOfCorrection
                     ) {

                     
    N2kMsg.SetPGN(129029L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.Add2ByteInt(DaysSince1970);
    N2kMsg.Add4ByteDouble(SecondsSinceMidnight,0.0001);
    N2kMsg.Add8ByteDouble(Latitude,1e-16);
    N2kMsg.Add8ByteDouble(Longitude,1e-16);
    N2kMsg.Add8ByteDouble(Altitude,1e-6);
    N2kMsg.AddByte( (((unsigned char) GNSStype) & 0x0f) | (((unsigned char) GNSSmethod) & 0x0f)<<4 );
    N2kMsg.AddByte(1);  // Integrity 2 bit, reserved 6 bits
    N2kMsg.AddByte(nSatellites);
    N2kMsg.Add2ByteDouble(HDOP,0.01);
    N2kMsg.Add2ByteDouble(PDOP,0.01);
//    N2kMsg.AddByte(0);
//    N2kMsg.AddByte(0);
    N2kMsg.Add4ByteDouble(GeoidalSeparation,0.01);
    N2kMsg.AddByte(nReferenceStations);
//    N2kMsg.AddByte(0);
//    N2kMsg.AddByte(0);
    N2kMsg.Add2ByteInt( (((int)ReferenceStationType) & 0x0f) | ReferenceSationID<<4 );
    N2kMsg.Add2ByteDouble(AgeOfCorrection,0.01);
}

//*****************************************************************************
// Cross Track Error
void SetN2kPGN129283(tN2kMsg &N2kMsg, unsigned char SID, tN2kXTEMode XTEMode, bool NavigationTerminated, double XTE) {
    N2kMsg.SetPGN(129283L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((char)XTEMode | (NavigationTerminated?0x40:0));
    N2kMsg.Add4ByteDouble(XTE,0.01);
}

//*****************************************************************************
// Navigation info
void SetN2kPGN129284(tN2kMsg &N2kMsg, unsigned char SID, double DistanceToWaypoint, tN2kHeadingReference BearingReference,
                      bool PerpendicularCrossed, bool ArrivalCircleEntered, tN2kDistanceCalculationType CalculationType,
                      double ETATime, int ETADate, double BearingOriginToDestinationWaypoint, double BearingPositionToDestinationWaypoint,
                      unsigned long OriginWaypointNumber, unsigned long DestinationWaypointNumber, 
                      double DestinationLatitude, double DestinationLongitude, double WaypointClosingVelocity) {
    N2kMsg.SetPGN(129284L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.Add4ByteDouble(DistanceToWaypoint,0.01);
    N2kMsg.AddByte((char)BearingReference | (PerpendicularCrossed?0x04:0) | (ArrivalCircleEntered?0x10:0)  | (CalculationType==N2kdct_RhumbLine?0x40:0));
    if (ETATime>0) { N2kMsg.Add4ByteDouble(ETATime,0.0001); } else { N2kMsg.Add4ByteUInt(0xffffffff); }
    if (ETADate>0) { N2kMsg.Add2ByteInt(ETADate); } else { N2kMsg.Add2ByteInt(0xffff); }
    N2kMsg.Add2ByteDouble(BearingOriginToDestinationWaypoint,0.0001);
    N2kMsg.Add2ByteDouble(BearingPositionToDestinationWaypoint,0.0001);
    N2kMsg.Add4ByteUInt(OriginWaypointNumber);
    N2kMsg.Add4ByteUInt(DestinationWaypointNumber);
    N2kMsg.Add4ByteDouble(DestinationLatitude,1e-07);
    N2kMsg.Add4ByteDouble(DestinationLongitude,1e-07);
    N2kMsg.Add2ByteDouble(WaypointClosingVelocity,0.01);
}

//*****************************************************************************
// Wind Speed
void SetN2kPGN130306 (tN2kMsg &N2kMsg, unsigned char SID, double WindSpeed, double WindAngle, tN2kWindReference WindReference) {
    N2kMsg.SetPGN(130306L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.Add2ByteDouble(WindSpeed,0.01);
    N2kMsg.Add2ByteDouble(WindAngle,0.0001);
    N2kMsg.AddByte((unsigned char)WindReference);
    //N2kMsg.AddByte(0xff); // Reserved
    //N2kMsg.AddByte(0xff); // Reserved
}

//*****************************************************************************
// Outside Environmental parameters
void SetN2kPGN130310(tN2kMsg &N2kMsg, unsigned char SID, double WaterTemperature,
                     double OutsideAmbientAirTemperature, double AtmosphericPressure) {
    N2kMsg.SetPGN(130310L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    if (WaterTemperature!=TempUndef) { N2kMsg.Add2ByteDouble(WaterTemperature,0.01); } else { N2kMsg.Add2ByteInt(0xffff); }
    if (OutsideAmbientAirTemperature!=TempUndef) { N2kMsg.Add2ByteDouble(OutsideAmbientAirTemperature,0.01); } else { N2kMsg.Add2ByteInt(0xffff); }
    if (AtmosphericPressure!=PressureUndef) { N2kMsg.Add2ByteDouble(AtmosphericPressure,1); } else { N2kMsg.Add2ByteInt(0xffff); }
}
                     
//*****************************************************************************
// Environmental parameters
void SetN2kPGN130311(tN2kMsg &N2kMsg, unsigned char SID, tN2kTempSource TempInstance, double Temperature,
                     tN2kHumiditySource HumidityInstance, double Humidity, double AtmosphericPressure) {
    N2kMsg.SetPGN(130311L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte(((HumidityInstance) & 0x03)<<6 | (TempInstance & 0x3f));
    N2kMsg.Add2ByteDouble(Temperature,0.01);
    if (Humidity!=HumidityUndef) { N2kMsg.Add2ByteDouble(Humidity,0.004); } else { N2kMsg.Add2ByteInt(0xffff); }
    if (AtmosphericPressure!=PressureUndef) { N2kMsg.Add2ByteDouble(AtmosphericPressure,1); } else { N2kMsg.Add2ByteInt(0xffff); }
}

//*****************************************************************************
// Temperature
// Temperatures should be in Kelvins
void SetN2kPGN130312(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature) {
    N2kMsg.SetPGN(130312L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((unsigned char)TempInstance);
    N2kMsg.AddByte((unsigned char)TempSource);
    N2kMsg.Add2ByteDouble(ActualTemperature,0.01);
    (SetTemperature>-300?N2kMsg.Add2ByteDouble(SetTemperature,0.01):N2kMsg.Add2ByteInt(0xffff));
    N2kMsg.AddByte(0xff); // Reserved
}

bool ParseN2kPGN130312(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  if (N2kMsg.PGN!=130312L) return false;
  int Index=0;
  SID=N2kMsg.GetByte(Index);
  TempInstance=N2kMsg.GetByte(Index);
  TempSource=(tN2kTempSource)(N2kMsg.GetByte(Index));
  ActualTemperature=N2kMsg.Get2ByteDouble(0.01,Index);
  SetTemperature=N2kMsg.Get2ByteDouble(0.01,Index,TempUndef);
  
  return true;
}


