/*
N2kDataToNMEA0183.cpp

Copyright (c) 2015-2018 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "N2kDataToNMEA0183.h"
#include <N2kMessages.h>
#include <NMEA0183Messages.h>
#include <math.h>


const double radToDeg=180.0/M_PI;

// Never turn an unavailable angle into a numeric heading by adding an offset.
static double OffsetHeading(double heading, double offset) {
  if ( N2kIsNA(heading) || N2kIsNA(offset) ) return N2kDoubleNA;
  double result=fmod(heading+offset,2*M_PI);
  return result<0 ? result+2*M_PI : result;
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleMsg(const tN2kMsg &N2kMsg) {
  switch (N2kMsg.PGN) {
    case 126992UL: HandleSystemDateTime(N2kMsg);
    case 127250UL: HandleHeading(N2kMsg);
    case 127258UL: HandleVariation(N2kMsg);
    case 128259UL: HandleBoatSpeed(N2kMsg);
    case 128267UL: HandleDepth(N2kMsg);
    case 129025UL: HandlePosition(N2kMsg);
    case 129026UL: HandleCOGSOG(N2kMsg);
    case 129029UL: HandleGNSS(N2kMsg);
    case 130306UL: HandleWind(N2kMsg);
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::Update() {
  SendRMC();
  if ( LastHeadingTime+2000<millis() ) { Heading=N2kDoubleNA; MagneticHeading=N2kDoubleNA; }
  if ( LastCOGSOGTime+2000<millis() ) { COG=N2kDoubleNA; SOG=N2kDoubleNA; }
  if ( LastPositionTime+4000<millis() ) { Latitude=N2kDoubleNA; Longitude=N2kDoubleNA; }
  if ( LastWindTime+2000<millis() ) { WindSpeed=N2kDoubleNA; WindAngle=N2kDoubleNA; }
}

//*****************************************************************************
void tN2kDataToNMEA0183::SendMessage(const tNMEA0183Msg &NMEA0183Msg) {
  if ( pNMEA0183!=0 ) pNMEA0183->SendMessage(NMEA0183Msg);
  if ( SendNMEA0183MessageCallback!=0 ) SendNMEA0183MessageCallback(NMEA0183Msg);
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleSystemDateTime(const tN2kMsg& N2kMsg) {
unsigned char SID;
uint16_t SystemDate;
double SystemTime;
tN2kTimeSource TimeSource;
tNMEA0183Msg NMEA0183Msg;

  if (ParseN2kSystemTime(N2kMsg, SID, SystemDate, SystemTime, TimeSource)) {
      time_t t = tNMEA0183Msg::daysToTime_t(SystemDate);
      tmElements_t tm;
      tNMEA0183Msg::breakTime(t, tm);
      int GPSDay = tNMEA0183Msg::GetDay(tm);
      int GPSMonth = tNMEA0183Msg::GetMonth(tm);
      int GPSYear = tNMEA0183Msg::GetYear(tm);
      int LZD = 0;
      int LZMD = 0;
      if (NMEA0183SetZDA(NMEA0183Msg, SystemTime, GPSDay, GPSMonth, GPSYear, LZD, LZMD, "GP")) {
          SendMessage(NMEA0183Msg);
      }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleHeading(const tN2kMsg &N2kMsg) {
unsigned char SID;
tN2kHeadingReference ref;
double _Deviation;
double _Variation;
double _Heading;
tNMEA0183Msg NMEA0183Msg;

  if ( ParseN2kHeading(N2kMsg, SID, _Heading, _Deviation, _Variation, ref) ) {
    if ( ref!=N2khr_true && ref!=N2khr_magnetic ) {
      Heading=N2kDoubleNA;
      MagneticHeading=N2kDoubleNA;
      return;
    }
    if ( !N2kIsNA(_Variation) ) Variation=_Variation;
    double CompassHeading=N2kDoubleNA;
    if ( ref==N2khr_magnetic ) {
      // Keep deviation for a following true-heading message, where it may be NA.
      Deviation=N2kIsNA(_Deviation) ? 0 : _Deviation;
      CompassHeading=_Heading;
      MagneticHeading=OffsetHeading(_Heading,Deviation);
      Heading=OffsetHeading(MagneticHeading,Variation);
    } else {
      Heading=_Heading;
      MagneticHeading=OffsetHeading(Heading,N2kIsNA(Variation) ? N2kDoubleNA : -Variation);
      CompassHeading=OffsetHeading(MagneticHeading,-Deviation);
    }
    LastHeadingTime=millis();
    // Send one sentence in the available reference, not true heading labelled HDG.
    if ( !N2kIsNA(CompassHeading) ) {
      if ( !N2kIsNA(Variation) ) {
        if ( NMEA0183SetHDG(NMEA0183Msg,CompassHeading,Deviation,Variation) ) SendMessage(NMEA0183Msg);
      } else {
        if ( NMEA0183SetHDM(NMEA0183Msg,MagneticHeading) ) SendMessage(NMEA0183Msg);
      }
    } else if ( !N2kIsNA(Heading) ) {
      if ( NMEA0183SetHDT(NMEA0183Msg,Heading) ) SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleVariation(const tN2kMsg &N2kMsg) {
unsigned char SID;
tN2kMagneticVariation Source;

  ParseN2kMagneticVariation(N2kMsg,SID,Source,DaysSince1970,Variation);
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleBoatSpeed(const tN2kMsg &N2kMsg) {
unsigned char SID;
double WaterReferenced;
double GroundReferenced;
tN2kSpeedWaterReferenceType SWRT;

  if ( ParseN2kBoatSpeed(N2kMsg,SID,WaterReferenced,GroundReferenced,SWRT) ) {
    tNMEA0183Msg NMEA0183Msg;
    if ( NMEA0183SetVHW(NMEA0183Msg,Heading,MagneticHeading,WaterReferenced) ) {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleDepth(const tN2kMsg &N2kMsg) {
unsigned char SID;
double DepthBelowTransducer;
double Offset;
double Range;

  if ( ParseN2kWaterDepth(N2kMsg,SID,DepthBelowTransducer,Offset,Range) ) {
      tNMEA0183Msg NMEA0183Msg;
      if ( NMEA0183SetDPT(NMEA0183Msg,DepthBelowTransducer,Offset) ) {
        SendMessage(NMEA0183Msg);
      }
      if ( NMEA0183SetDBx(NMEA0183Msg,DepthBelowTransducer,Offset) ) {
        SendMessage(NMEA0183Msg);
      }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandlePosition(const tN2kMsg &N2kMsg) {

  if ( ParseN2kPGN129025(N2kMsg, Latitude, Longitude) ) {
    LastPositionTime=millis();
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleCOGSOG(const tN2kMsg &N2kMsg) {
unsigned char SID;
tN2kHeadingReference HeadingReference;
tNMEA0183Msg NMEA0183Msg;

  if ( ParseN2kCOGSOGRapid(N2kMsg,SID,HeadingReference,COG,SOG) ) {
    LastCOGSOGTime=millis();
    double MCOG=NMEA0183DoubleNA;
    if ( HeadingReference==N2khr_magnetic ) {
      MCOG=COG;
      COG=OffsetHeading(COG,Variation);
    } else if ( HeadingReference==N2khr_true ) {
      MCOG=OffsetHeading(COG,N2kIsNA(Variation) ? N2kDoubleNA : -Variation);
    } else {
      COG=N2kDoubleNA;
    }
    if ( NMEA0183SetVTG(NMEA0183Msg,COG,MCOG,SOG) ) {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleGNSS(const tN2kMsg &N2kMsg) {
unsigned char SID;
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
tNMEA0183Msg NMEA0183Msg;

  if ( ParseN2kGNSS(N2kMsg,SID,DaysSince1970,SecondsSinceMidnight,Latitude,Longitude,Altitude,GNSStype,GNSSmethod,
                    nSatellites,HDOP,PDOP,GeoidalSeparation,
                    nReferenceStations,ReferenceStationType,ReferenceSationID,AgeOfCorrection) ) {
    LastPositionTime=millis();
    if (NMEA0183SetGGA(NMEA0183Msg, SecondsSinceMidnight, Latitude, Longitude, static_cast<uint32_t>(GNSSmethod), nSatellites, HDOP, Altitude, GeoidalSeparation, AgeOfCorrection, ReferenceSationID== N2kInt16NA ? NMEA0183UInt32NA : ReferenceSationID)) {
        SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::HandleWind(const tN2kMsg &N2kMsg) {
unsigned char SID;
tN2kWindReference WindReference;
tNMEA0183WindReference NMEA0183Reference=NMEA0183Wind_True;

  if ( ParseN2kWindSpeed(N2kMsg,SID,WindSpeed,WindAngle,WindReference) ) {
    tNMEA0183Msg NMEA0183Msg;
    LastWindTime=millis();
    if ( WindReference==N2kWind_Apparent ) NMEA0183Reference=NMEA0183Wind_Apparent;

    if ( NMEA0183SetMWV(NMEA0183Msg, WindAngle*radToDeg, NMEA0183Reference , WindSpeed) ) {
      SendMessage(NMEA0183Msg);
    }
  }
}

//*****************************************************************************
void tN2kDataToNMEA0183::SendRMC() {
    if ( NextRMCSend<=millis() && !N2kIsNA(Latitude) ) {
      tNMEA0183Msg NMEA0183Msg;
      if ( NMEA0183SetRMC(NMEA0183Msg,SecondsSinceMidnight,Latitude,Longitude,COG,SOG,DaysSince1970,Variation) ) {
        SendMessage(NMEA0183Msg);
      }
      SetNextRMCSend();
    }
}

