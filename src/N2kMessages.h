/* N2kMessages.h
 * 
 * Copyright (c) 2015-2024 Timo Lappalainen, Kave Oy, www.kave.fi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/************************************************************************//**
 * \file   N2kMessages.h
 *  \brief  This File contains all SetXXX functions which will be needed to
 *          transfer data with a specific PGN.
 * 
 * This is a collection of functions for handling NMEA2000 bus messages.
 * Library contains functions to e.g. create message named with PGN like
 * \ref SetN2kPGN129025 and alias easy for humans to read like \ref 
 * SetN2kLatLonRapid.
 *
 * Each SetN2kPNGxxx function sets related message PGN and its default
 * priority. The default priority can be different for each PGN. So if you
 * want to change priority on function, you have to do it after Setxxx call.
 * 
 * Each SetN2kPNGxxx function has a corresponding ParsN2kPGNxxx function which
 * can extract the data out of an incoming N2k Message. These functions have as
 * well an inline alias easy for humans to read like \ref ParseN2kPositionRapid.
 *
 * The content of the PGNs is specified by the NMEA2000 Standard under
 * 
 * - https://web.archive.org/web/20220515054117/https://www.nmea.org/Assets/july%202010%20nmea2000_v1-301_app_b_pgn_field_list.pdf
 * - https://web.archive.org/web/20200918140201/https://www.nmea.org/Assets/20151026%20nmea%202000%20pgn_website_description_list.pdf
 * 
 * If you do not send anything to NMEA2000 bus, you do not need this library.
 * Funtions for BUS handling PGN:s like 60928 "ISO Address Claim" has been
 * defined in bus device library
 * 
 *  NMEA2000.h
 * 
 * ********************************************************************/

#ifndef _N2kMessages_H_
#define _N2kMessages_H_

#include "N2kMsg.h"
#include "N2kTypes.h"
#include <string.h>
#include <stdint.h>

/************************************************************************//**
 * \brief Converting a value from Rad to Deg
 * \param   v   Input value in [rad]
 * \return      Corresponding value in [deg]
 */
inline double RadToDeg(double v) { return N2kIsNA(v)?v:v*180.0/3.1415926535897932384626433832795L; }

/************************************************************************//**
 * \brief Converting a value from Deg to Rad
 * \param   v   Input value in [deg]
 * \return      Corresponding value in [rad]
 */
inline double DegToRad(double v) { return N2kIsNA(v)?v:v/180.0*3.1415926535897932384626433832795L; }

/************************************************************************//**
 * \brief Converting a value from Celsius to Kelvin
 * \param   v   Input value in [degC]
 * \return      Corresponding value in [K]
 */
inline double CToKelvin(double v) { return N2kIsNA(v)?v:v+273.15L; }

/************************************************************************//**
 * \brief Converting a value from Kelvin to Celsius
 * \param   v   Input value in [K]
 * \return      Corresponding value in [degC]
 */
inline double KelvinToC(double v) { return N2kIsNA(v)?v:v-273.15L; }

/************************************************************************//**
 * \brief Converting a value from Fahrenheit to Kelvin
 * \param   v   Input value in [degF]
 * \return      Corresponding value in [K]
 */
inline double FToKelvin(double v) { return N2kIsNA(v)?v:(v-32)*5.0/9.0+273.15; }

/************************************************************************//**
 * \brief Converting a value from Kelvin to Fahrenheit
 * \param   v   Input value in [F]
 * \return      Corresponding value in [degF]
 */
inline double KelvinToF(double v) { return N2kIsNA(v)?v:(v-273.15)*9.0/5.0+32; }

/************************************************************************//**
 * \brief Converting a value from Millibar to Pascal
 * \param   v   Input value in [mBar]
 * \return      Corresponding value in [Pa]
 */
inline double mBarToPascal(double v) { return N2kIsNA(v)?v:v*100L; }

/************************************************************************//**
 * \brief Converting a value from Pascal to Millibar
 * \param   v   Input value in [Pa]
 * \return      Corresponding value in [mBar]
 */
inline double PascalTomBar(double v) { return N2kIsNA(v)?v:v/100L; }

/************************************************************************//**
 * \brief Converting a value from HectoPascal to Pascal
 * \param   v   Input value in [hPa]
 * \return      Corresponding value in [Pa]
 */
inline double hPAToPascal(double v) { return N2kIsNA(v)?v:v*100L; }

/************************************************************************//**
 * \brief Converting a value from Pascal to HectoPascal
 * \param   v   Input value in [Pa]
 * \return      Corresponding value in [hPa]
 */
inline double PascalTohPA(double v) { return N2kIsNA(v)?v:v/100L; }

/************************************************************************//**
 * \brief Converting a value from AmpereHours to Coulomb
 * \param   v   Input value in [Ah]
 * \return      Corresponding value in [C]
 */
inline double AhToCoulomb(double v) { return N2kIsNA(v)?v:v*3600L; }

/************************************************************************//**
 * \brief Converting a value from Coulomb to AmpereHours
 * \param   v   Input value in [C]
 * \return      Corresponding value in [Ah]
 */
inline double CoulombToAh(double v) { return N2kIsNA(v)?v:v/3600L; }

/************************************************************************//**
 * \brief Converting a value from Hours to Seconds
 * \param   v   Input value in [h]
 * \return      Corresponding value in [s]
 */
inline double hToSeconds(double v) { return N2kIsNA(v)?v:v*3600L; }

/************************************************************************//**
 * \brief Converting a value from Seconds to Hours
 * \param   v   Input value in [s]
 * \return      Corresponding value in [h]
 */
inline double SecondsToh(double v) { return N2kIsNA(v)?v:v/3600L; }

/************************************************************************//**
 * \brief Converting a value from MeterPerSecond to Knots
 * \param   v   Input value in [mps]
 * \return      Corresponding value in [kn]
 */
inline double msToKnots(double v) { return N2kIsNA(v)?v:v*1.9438444924406047516198704103672L; } // 3600L/1852.0L

/************************************************************************//**
 * \brief Converting a value from Knots to MeterPerSecond
 * \param   v   Input value in [kn]
 * \return      Corresponding value in [mps]
 */
inline double KnotsToms(double v) { return N2kIsNA(v)?v:v*0.51444444444444444444444444444444L; } // 1852L/3600.0L

/************************************************************************//**
 * \brief Setting up PGN126992 Message "System date/time"
 * \ingroup group_msgSetUp
 * 
 * The purpose of this PGN is to provide a regular transmission of UTC time
 * and date; optionally synchronized to other parameter groups from the same
 * source.
 * 
 * The default Priority is set to 3.
 * 
 * \param N2kMsg        Reference to a N2kMsg Object
 * \param SID           Sequence ID. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                      \n
 *                      SID has been designed to make difference between sensor sampling or calculation.
 *                      Same SID in different PGNs sent by same device defines that values on PGN:s has been sampled or 
 *                      calculated at same time.
 * \param SystemDate    Days since 1970-01-01 (UTC)
 * \param SystemTime    seconds since midnight (UTC)
 * \param TimeSource    Source of the Systemtime 
 * 
 * 
 * \sa SetN2kSystemTime, tN2kTimeSource
 */
void SetN2kPGN126992(tN2kMsg &N2kMsg, unsigned char SID, uint16_t SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS);

/************************************************************************//**
 * \brief Setting up Message "System date/time" - PGN 126992
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 126992. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN126992
 * 
 */
inline void SetN2kSystemTime(tN2kMsg &N2kMsg, unsigned char SID, uint16_t SystemDate,
                     double SystemTime, tN2kTimeSource TimeSource=N2ktimes_GPS) {
  SetN2kPGN126992(N2kMsg,SID,SystemDate,SystemTime,TimeSource);
}

/************************************************************************//**
 * \brief Parsing the Content of a PGN126992 Message - "System date/time" 
 * \ingroup group_msgParsers
 * 
 * The PGN126992 purpose is twofold: To provide a regular transmission of UTC
 * time and date. To provide synchronism for measurement data.
 * 
 * \param N2kMsg      N2kMsg Object, Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param SystemDate  Systemdate in days since 1970-01-01 (UTC)
 * \param SystemTime  System time seconds since midnight (UTC)
 * \param TimeSource  Source of the Systemtime
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 */
bool ParseN2kPGN126992(const tN2kMsg &N2kMsg, unsigned char &SID, uint16_t &SystemDate,
                     double &SystemTime, tN2kTimeSource &TimeSource);

/************************************************************************//**
 * \brief Parsing the content of a "System date/time" message - PGN 126992
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 126992. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN126992 
 */
inline bool ParseN2kSystemTime(const tN2kMsg &N2kMsg, unsigned char &SID, uint16_t &SystemDate,
                     double &SystemTime, tN2kTimeSource &TimeSource) {
  return ParseN2kPGN126992(N2kMsg,SID,SystemDate,SystemTime,TimeSource);
}

/************************************************************************//**
 * \brief Setting up PGN129802 Message "AIS Safety Related Broadcast Message"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides data associated with the ITU-R M.1371
 * Message 14 Safety Related Broadcast Message supporting broadcast
 * communication of safety related data. An AIS device may generate this
 * parameter group either upon receiving a VHF data link message 14,
 * or upon receipt of an ISO or NMEA request PGN.
 * 
 * \param N2kMsg                    Reference to a N2kMsg Object, 
 *                                  Output: NMEA2000 message ready to be send.
 * \param MessageID                 Message type
 * \param Repeat                    Repeat indicator
 * \param SourceID                  MMSI
 * \param AISTransceiverInformation Transceiver Information 
 * \param SafetyRelatedText         Safety related Text of the Broadcast
 *                                  communication
 * 
 * \sa tN2kAISTransceiverInformation
 */
void SetN2kPGN129802(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t SourceID,
      tN2kAISTransceiverInformation AISTransceiverInformation, char * SafetyRelatedText);

/************************************************************************//**
 * \brief Setting up Message "AIS Safety Related Broadcast Message" - 
 * PGN 129802
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129802. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129802
 * 
 */
inline void SetN2kAISSafetyRelatedBroadcastMsg(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t SourceID,
      tN2kAISTransceiverInformation AISTransceiverInformation, char * SafetyRelatedText) {
   SetN2kPGN129802(N2kMsg, MessageID, Repeat, SourceID, AISTransceiverInformation, SafetyRelatedText);
}

/************************************************************************//**
 * \brief Parsing the Content of a PGN129802 Message - "AIS Safety Related 
 * Broadcast Message"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides data associated with the ITU-R M.1371
 * Message 14 Safety Related Broadcast Message supporting broadcast
 * communication of safety related data. An AIS device may generate this
 * parameter group either upon receiving a VHF data link message 14,
 * or upon receipt of an ISO or NMEA request PGN.
 *
 * \param N2kMsg                    Reference to a N2kMsg Object, 
 *                                  Output: NMEA2000 message ready to be send.
 * \param MessageID                 Message type
 * \param Repeat                    Repeat indicator
 * \param SourceID                  MMSI
 * \param AISTransceiverInformation Transceiver Information 
 * \param SafetyRelatedText         Safety related Text of the Broadcast
 *                                  communication
 * \param SafetyRelatedTextMaxSize  size of SafetyRelatedText
 *
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 * \sa tN2kAISTransceiverInformation
 */
bool ParseN2kPGN129802(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &SourceID,
      tN2kAISTransceiverInformation &AISTransceiverInformation, char * SafetyRelatedText, size_t &SafetyRelatedTextMaxSize);

/************************************************************************//**
 * \brief Parsing the content of a "AIS Safety Related Broadcast Message" 
 *        message - PGN 129802
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129802. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129802 
 */
inline bool ParseN2kAISSafetyRelatedBroadcastMsg(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &SourceID,
      tN2kAISTransceiverInformation &AISTransceiverInformation, char * SafetyRelatedText, size_t &SafetyRelatedTextMaxSize) {
   return ParseN2kPGN129802(N2kMsg, MessageID, Repeat, SourceID, AISTransceiverInformation, SafetyRelatedText, SafetyRelatedTextMaxSize);
}

/************************************************************************//**
 * \brief Setting up PGN127233 Message "Man Overboard Notification"
 * \ingroup group_msgSetUp
 * 
 * The MOB PGN is intended to provide notification from a MOB monitoring
 * system. The included position information may be that of the vessel or the
 * MOB device itself as identified in field “X”, position source. Additional
 * information may include the current state of the MOB device, time of
 * activation, and MOB device battery status.

 * \param N2kMsg                Reference to a N2kMsg Object, 
 *                              Output: NMEA2000 message ready to be send.
 * \param SID                   Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                              \n
 *                              The sequence identifier field is used to tie different PGNs data together to same 
 *                              sampling or calculation time.
 * \param MobEmitterId          Identifier for each MOB emitter, unique to
 *                              the vessel
 * \param MOBStatus             MOB Status
 * \param ActivationTime        Time of day (UTC) when MOB was activated
 * \param PositionSource        Position Source
 * \param PositionDate          Date of MOB position
 * \param PositionTime          Time of day of MOB position (UTC)
 * \param Latitude              Latitude in degrees
 * \param Longitude             Longitude in degrees
 * \param COGReference          True or Magnetic
 * \param COG                   Course Over Ground in radians
 * \param SOG                   Speed Over Ground in m/s
 * \param MMSI                  MMSI
 * \param MOBEmitterBatteryStatus   Battery status
 * 
 */
void SetN2kPGN127233(tN2kMsg &N2kMsg,
      unsigned char SID,
      uint32_t MobEmitterId,
      tN2kMOBStatus MOBStatus,
      double ActivationTime,
      tN2kMOBPositionSource PositionSource,
      uint16_t PositionDate,
      double PositionTime,
      double Latitude,
      double Longitude,
      tN2kHeadingReference COGReference,
      double COG,
      double SOG,
      uint32_t MMSI,
      tN2kMOBEmitterBatteryStatus MOBEmitterBatteryStatus);

/************************************************************************//**
 * \brief Setting up Message "Man Overboard Notification" - PGN 127233
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127233. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127233
 * 
 */
inline void SetN2kMOBNotification(tN2kMsg &N2kMsg,
      unsigned char SID,
      uint32_t MobEmitterId,
      tN2kMOBStatus MOBStatus,
      double ActivationTime,
      tN2kMOBPositionSource PositionSource,
      uint16_t PositionDate,
      double PositionTime,
      double Latitude,
      double Longitude,
      tN2kHeadingReference COGReference,
      double COG,
      double SOG,
      uint32_t MMSI,
      tN2kMOBEmitterBatteryStatus MOBEmitterBatteryStatus) {
  SetN2kPGN127233(N2kMsg,SID,MobEmitterId,MOBStatus,ActivationTime,PositionSource,PositionDate,PositionTime,Latitude,Longitude,COGReference,COG,SOG,MMSI,MOBEmitterBatteryStatus);
}

/************************************************************************//**
 * \brief Parsing the Content of Message PGN127233 "Man Overboard Notification"
 * \ingroup group_msgParsers
 * 
 * The MOB PGN is intended to provide notification from a MOB monitoring
 * system. The included position information may be that of the vessel or the
 * MOB device itself as identified in field “X”, position source. Additional
 * information may include the current state of the MOB device, time of
 * activation, and MOB device battery status.
 * 
 * \param N2kMsg                Reference to a N2kMsg Object, 
 * \param SID                   Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param MobEmitterId          Identifier for each MOB emitter, unique to
 *                              the vessel
 * \param MOBStatus             MOB Status
 * \param ActivationTime        Time of day (UTC) when MOB was activated
 * \param PositionSource        Position Source
 * \param PositionDate          Date of MOB position
 * \param PositionTime          Time of day of MOB position (UTC)
 * \param Latitude              Latitude in degrees
 * \param Longitude             Longitude in degrees
 * \param COGReference          True or Magnetic
 * \param COG                   Course Over Ground in radians
 * \param SOG                   Speed Over Ground in m/s
 * \param MMSI                  MMSI
 * \param MOBEmitterBatteryStatus   Battery status
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127233(const tN2kMsg &N2kMsg,
      unsigned char &SID,
      uint32_t &MobEmitterId,
      tN2kMOBStatus &MOBStatus,
      double &ActivationTime,
      tN2kMOBPositionSource &PositionSource,
      uint16_t &PositionDate,
      double &PositionTime,
      double &Latitude,
      double &Longitude,
      tN2kHeadingReference &COGReference,
      double &COG,
      double &SOG,
      uint32_t &MMSI,
      tN2kMOBEmitterBatteryStatus &MOBEmitterBatteryStatus);

/************************************************************************//**
 * \brief Parsing the content of a "Man Overboard Notification" 
 *        message - PGN 127233
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127233. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127233 
 */
inline bool ParseN2kMOBNotification(const tN2kMsg &N2kMsg,
      unsigned char &SID,
      uint32_t &MobEmitterId,
      tN2kMOBStatus &MOBStatus,
      double &ActivationTime,
      tN2kMOBPositionSource &PositionSource,
      uint16_t &PositionDate,
      double &PositionTime,
      double &Latitude,
      double &Longitude,
      tN2kHeadingReference &COGReference,
      double &COG,
      double &SOG,
      uint32_t &MMSI,
      tN2kMOBEmitterBatteryStatus &MOBEmitterBatteryStatus) {
  return ParseN2kPGN127233(N2kMsg,SID,MobEmitterId,MOBStatus,ActivationTime,PositionSource,PositionDate,PositionTime,Latitude,Longitude,COGReference,COG,SOG,MMSI,MOBEmitterBatteryStatus);
}

/************************************************************************//**
 * \brief Setting up PGN127237 Message "Heading/Track control"
 * \ingroup group_msgSetUp
 * 
 * Sends commands to, and receives data from, heading control systems. Allows
 * for navigational (remote) control of a heading control system and direct
 * rudder control.
 * 
 * \param N2kMsg                Reference to a N2kMsg Object, 
 *                              Output: NMEA2000 message ready to be send.
 * \param RudderLimitExceeded       Yes/No
 * \param OffHeadingLimitExceeded   Yes/No
 * \param OffTrackLimitExceeded     Yes/No
 * \param Override                  Yes/No
 * \param SteeringMode              Steering mode
 * \param TurnMode                  Turn mode
 * \param HeadingReference          True or Magnetic
 * \param CommandedRudderDirection  Port or Starboard
 * \param CommandedRudderAngle      In radians
 * \param HeadingToSteerCourse      In radians
 * \param Track                     In radians
 * \param RudderLimit               In radians
 * \param OffHeadingLimit           In radians 
 * \param RadiusOfTurnOrder         In meter
 * \param RateOfTurnOrder           In radians/s
 * \param OffTrackLimit             In meters
 * \param VesselHeading             in radians
 * 
 */
void SetN2kPGN127237(tN2kMsg &N2kMsg,
      tN2kOnOff RudderLimitExceeded,
      tN2kOnOff OffHeadingLimitExceeded,
      tN2kOnOff OffTrackLimitExceeded,
      tN2kOnOff Override,
      tN2kSteeringMode SteeringMode,
      tN2kTurnMode TurnMode,
      tN2kHeadingReference HeadingReference,
      tN2kRudderDirectionOrder CommandedRudderDirection,
      double CommandedRudderAngle,
      double HeadingToSteerCourse,
      double Track,
      double RudderLimit,
      double OffHeadingLimit,
      double RadiusOfTurnOrder,
      double RateOfTurnOrder,
      double OffTrackLimit,
      double VesselHeading);

/************************************************************************//**
 * \brief Setting up Message "Heading/Track control" - PGN 127237
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127237. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127237
 * 
 */
inline void SetN2kHeadingTrackControl(tN2kMsg &N2kMsg,
      tN2kOnOff RudderLimitExceeded,
      tN2kOnOff OffHeadingLimitExceeded,
      tN2kOnOff OffTrackLimitExceeded,
      tN2kOnOff Override,
      tN2kSteeringMode SteeringMode,
      tN2kTurnMode TurnMode,
      tN2kHeadingReference HeadingReference,
      tN2kRudderDirectionOrder CommandedRudderDirection,
      double CommandedRudderAngle,
      double HeadingToSteerCourse,
      double Track,
      double RudderLimit,
      double OffHeadingLimit,
      double RadiusOfTurnOrder,
      double RateOfTurnOrder,
      double OffTrackLimit,
      double VesselHeading) {
  SetN2kPGN127237(N2kMsg, RudderLimitExceeded,OffHeadingLimitExceeded,OffTrackLimitExceeded,Override,SteeringMode,TurnMode,
         HeadingReference,CommandedRudderDirection,CommandedRudderAngle,HeadingToSteerCourse,Track,RudderLimit,OffHeadingLimit,
         RadiusOfTurnOrder,RateOfTurnOrder,OffTrackLimit,VesselHeading);
}

/************************************************************************//**
 * \brief Parsing the Content of Message PGN127237 "Heading/Track control"
 * \ingroup group_msgParsers
 * 
 * Sends commands to, and receives data from, heading control systems. Allows
 * for navigational (remote) control of a heading control system and direct
 * rudder control.
 * 
 * \param N2kMsg                Reference to a N2kMsg Object, 
 * \param RudderLimitExceeded       Yes/No
 * \param OffHeadingLimitExceeded   Yes/No
 * \param OffTrackLimitExceeded     Yes/No
 * \param Override                  Yes/No
 * \param SteeringMode              Steering mode
 * \param TurnMode                  Turn mode
 * \param HeadingReference          True or Magnetic
 * \param CommandedRudderDirection  Port or Starboard
 * \param CommandedRudderAngle      In radians
 * \param HeadingToSteerCourse      In radians
 * \param Track                     In radians
 * \param RudderLimit               In radians
 * \param OffHeadingLimit           In radians 
 * \param RadiusOfTurnOrder         In meter
 * \param RateOfTurnOrder           In radians/s
 * \param OffTrackLimit             In meters
 * \param VesselHeading             in radians
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
*/

bool ParseN2kPGN127237(const tN2kMsg &N2kMsg,
      tN2kOnOff &RudderLimitExceeded,
      tN2kOnOff &OffHeadingLimitExceeded,
      tN2kOnOff &OffTrackLimitExceeded,
      tN2kOnOff &Override,
      tN2kSteeringMode &SteeringMode,
      tN2kTurnMode &TurnMode,
      tN2kHeadingReference &HeadingReference,
      tN2kRudderDirectionOrder &CommandedRudderDirection,
      double &CommandedRudderAngle,
      double &HeadingToSteerCourse,
      double &Track,
      double &RudderLimit,
      double &OffHeadingLimit,
      double &RadiusOfTurnOrder,
      double &RateOfTurnOrder,
      double &OffTrackLimit,
      double &VesselHeading);

/************************************************************************//**
 * \brief Parsing the content of a "Heading/Track control" 
 *        message - PGN 127237
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127237. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127237 
 */
inline bool ParseN2kHeadingTrackControl(const tN2kMsg &N2kMsg,
      tN2kOnOff &RudderLimitExceeded,
      tN2kOnOff &OffHeadingLimitExceeded,
      tN2kOnOff &OffTrackLimitExceeded,
      tN2kOnOff &Override,
      tN2kSteeringMode &SteeringMode,
      tN2kTurnMode &TurnMode,
      tN2kHeadingReference &HeadingReference,
      tN2kRudderDirectionOrder &CommandedRudderDirection,
      double &CommandedRudderAngle,
      double &HeadingToSteerCourse,
      double &Track,
      double &RudderLimit,
      double &OffHeadingLimit,
      double &RadiusOfTurnOrder,
      double &RateOfTurnOrder,
      double &OffTrackLimit,
      double &VesselHeading)
{
  return ParseN2kPGN127237(N2kMsg,RudderLimitExceeded,OffHeadingLimitExceeded,OffTrackLimitExceeded,Override,SteeringMode,
         TurnMode,HeadingReference,CommandedRudderDirection, CommandedRudderAngle,HeadingToSteerCourse,Track,RudderLimit,
         OffHeadingLimit,RadiusOfTurnOrder,RateOfTurnOrder,OffTrackLimit,VesselHeading);
}

/************************************************************************//**
 * \brief Setting up PGN127245 Message "Rudder"
 * \ingroup group_msgSetUp
 * 
 * Rudder order command in direction or angle with current rudder angle reading.
 * 
 * \param N2kMsg                Reference to a N2kMsg Object, 
 *                              Output: NMEA2000 message ready to be send.
 * \param RudderPosition        Current rudder postion in radians.
 * \param Instance              Rudder instance.
 * \param RudderDirectionOrder  Direction, where rudder should be turned.
 * \param AngleOrder            In radians angle where rudder should be turned.
 * 
 * \sa tN2kRudderDirectionOrder
 */
void SetN2kPGN127245(tN2kMsg &N2kMsg, double RudderPosition, unsigned char Instance=0,
                     tN2kRudderDirectionOrder RudderDirectionOrder=N2kRDO_NoDirectionOrder, double AngleOrder=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Rudder" - PGN 127245
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127245. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127245
 * 
 */
inline void SetN2kRudder(tN2kMsg &N2kMsg, double RudderPosition, unsigned char Instance=0,
                     tN2kRudderDirectionOrder RudderDirectionOrder=N2kRDO_NoDirectionOrder, double AngleOrder=N2kDoubleNA) {
  SetN2kPGN127245(N2kMsg,RudderPosition,Instance,RudderDirectionOrder,AngleOrder);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN127245 "Rudder"
 * \ingroup group_msgParsers
 * 
 * Rudder order command in direction or angle with current rudder angle reading.
 * 
 * \param N2kMsg                Reference to a N2kMsg Object, 
 *                              Output: NMEA2000 message ready to be send.
 * \param RudderPosition        Current rudder postion in radians.
 * \param Instance              Rudder instance.
 * \param RudderDirectionOrder  Direction, where rudder should be turned.
 * \param AngleOrder            In radians angle where rudder should be turned.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 * \sa tN2kRudderDirectionOrder
 */
bool ParseN2kPGN127245(const tN2kMsg &N2kMsg, double &RudderPosition, unsigned char &Instance,
                     tN2kRudderDirectionOrder &RudderDirectionOrder, double &AngleOrder);

/************************************************************************//**
 * \brief Parsing the content of a "Rudder" 
 *        message - PGN 127245
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127245. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127245 
 */
inline bool ParseN2kRudder(const tN2kMsg &N2kMsg, double &RudderPosition, unsigned char &Instance,
                     tN2kRudderDirectionOrder &RudderDirectionOrder, double &AngleOrder) {
  return ParseN2kPGN127245(N2kMsg,RudderPosition,Instance,RudderDirectionOrder,AngleOrder);
}

/************************************************************************//**
 * \brief Parsing the content of a "Rudder" 
 *        message - PGN 127245
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127245. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127245 
 */
inline bool ParseN2kRudder(const tN2kMsg &N2kMsg, double &RudderPosition) {
  tN2kRudderDirectionOrder RudderDirectionOrder;
  double AngleOrder;
  unsigned char Instance;
  return ParseN2kPGN127245(N2kMsg,RudderPosition,Instance,RudderDirectionOrder,AngleOrder);
}

/************************************************************************//**
 * \brief Setting up PGN127250 Message "Vessel Heading"
 * \ingroup group_msgSetUp
 * 
 * Heading sensor value with a flag for True or Magnetic. If the sensor value
 * is Magnetic, the deviation field can be used to produce a Magnetic heading,
 * and the variation field can be used to correct the Magnetic heading to
 * produce a True heading.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param Heading     Heading in radians
 * \param Deviation   Magnetic deviation in radians. Use N2kDoubleNA
 *                    for undefined value.
 * \param Variation   Heading reference. See definition 
 *                    of \ref tN2kHeadingReference.
 * \param ref         Heading reference. See definition 
 *                    of \ref tN2kHeadingReference.
 */
void SetN2kPGN127250(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation, double Variation, tN2kHeadingReference ref);

/************************************************************************//**
 * \brief Setting up Message "Vessel Heading" - PGN 127250
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127250. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127250
  */
inline void SetN2kTrueHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading) {
  SetN2kPGN127250(N2kMsg,SID,Heading,N2kDoubleNA,N2kDoubleNA,N2khr_true);
}

/************************************************************************//**
 * \brief Setting up Message "Vessel Heading" - PGN 127250
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127250. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127250
  */
inline void SetN2kMagneticHeading(tN2kMsg &N2kMsg, unsigned char SID, double Heading, double Deviation=N2kDoubleNA, double Variation=N2kDoubleNA) {
  SetN2kPGN127250(N2kMsg,SID,Heading,Deviation,Variation,N2khr_magnetic);
}

/************************************************************************//**
 * \brief Parsing Content of Message PGN 127250 "Vessel Heading"
 * \ingroup group_msgParsers
 * 
 * Heading sensor value with a flag for True or Magnetic. If the sensor value
 * is Magnetic, the deviation field can be used to produce a Magnetic heading,
 * and the variation field can be used to correct the Magnetic heading to
 * produce a True heading.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Heading     Heading in radians
 * \param Deviation   Magnetic deviation in radians. Use N2kDoubleNA
 *                    for undefined value.
 * \param Variation   Heading reference. See definition 
 *                    of \ref tN2kHeadingReference.
 * \param ref         Heading reference. See definition 
 *                    of \ref tN2kHeadingReference.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127250(const tN2kMsg &N2kMsg, unsigned char &SID, double &Heading, double &Deviation, double &Variation, tN2kHeadingReference &ref);

/************************************************************************//**
 * \brief Parsing the content of a "Vessel Heading" 
 *        message - PGN 127250
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127250. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127250 
 */
inline bool ParseN2kHeading(const tN2kMsg &N2kMsg, unsigned char &SID, double &Heading, double &Deviation, double &Variation, tN2kHeadingReference &ref) {
  return ParseN2kPGN127250(N2kMsg,SID,Heading,Deviation,Variation,ref);
}

/************************************************************************//**
 * \brief Setting up PGN 127251 Message "Rate of Turn"
 *\ingroup group_msgSetUp
 * 
 * Rate of Turn is the rate of change of the Heading.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param RateOfTurn  Change in heading in radians per second
 * 
 */
void SetN2kPGN127251(tN2kMsg &N2kMsg, unsigned char SID, double RateOfTurn);

/************************************************************************//**
 * \brief Setting up Message "Rate of Turn" - PGN 127251
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127251. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127251
  */
inline void SetN2kRateOfTurn(tN2kMsg &N2kMsg, unsigned char SID, double RateOfTurn) {
  SetN2kPGN127251(N2kMsg,SID,RateOfTurn);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN127251 "Rate of Turn"
 * \ingroup group_msgParsers
 * 
 * Rate of Turn is the rate of change of the Heading.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param RateOfTurn  Change in heading in radians per second
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127251(const tN2kMsg &N2kMsg, unsigned char &SID, double &RateOfTurn);

/************************************************************************//**
 * \brief Parsing the content of a "Rate of Turn" 
 *        message - PGN 127251
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127251. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127251 
 */
inline bool ParseN2kRateOfTurn(const tN2kMsg &N2kMsg, unsigned char &SID, double &RateOfTurn) {
  return ParseN2kPGN127251(N2kMsg,SID,RateOfTurn);
}

/************************************************************************//**
 * \brief Setting up PGN 127252 Message "Heave"
 * \ingroup group_msgSetUp
 * 
 * Vertical displacement perpendicular to (smooth, wave-free water on)
 * the earth’s surface.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param Heave       Vertical displacement perpendicular to the earth’s
 *                    surface in meters
 * \param Delay       Delay added by calculations in seconds
 * \param DelaySource Delay Source, see \ref tN2kDelaySource
 */
void SetN2kPGN127252(tN2kMsg &N2kMsg, unsigned char SID, double Heave,
                     double Delay=N2kDoubleNA, tN2kDelaySource DelaySource=N2kDD374_DataNotAvailable);

/************************************************************************//**
 * \brief Setting up Message "Rate of Turn" - PGN 127252
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127252. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127252
  */
inline void SetN2kHeave(tN2kMsg &N2kMsg, unsigned char SID, double Heave,
                     double Delay=N2kDoubleNA, tN2kDelaySource DelaySource=N2kDD374_DataNotAvailable) {
  SetN2kPGN127252(N2kMsg, SID, Heave, Delay, DelaySource);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN127252 "Heave"
 * \ingroup group_msgParsers
 * 
 * Vertical displacement perpendicular to (smooth, wave-free water on)
 * the earth’s surface.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Heave       Vertical displacement perpendicular to the earth’s
 *                    surface in meters
 * \param Delay       Delay added by calculations in seconds
 * \param DelaySource Delay Source, see \ref tN2kDelaySource
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127252(const tN2kMsg &N2kMsg, unsigned char &SID, double &Heave, double &Delay, tN2kDelaySource &DelaySource);

/************************************************************************//**
 * \brief Parsing the content of a "Heave" 
 *        message - PGN 127252
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127252. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127252 
 */
inline bool ParseN2kHeave(const tN2kMsg &N2kMsg, unsigned char &SID, double &Heave) {
  double Delay;
  tN2kDelaySource DelaySource;
  return ParseN2kPGN127252(N2kMsg, SID, Heave, Delay, DelaySource);
}
/************************************************************************//**
 * \brief Parsing the content of a "Heave" 
 *        message - PGN 127252
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127252. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127252 
 */
inline bool ParseN2kHeave(const tN2kMsg &N2kMsg, unsigned char &SID, double &Heave, double &Delay, tN2kDelaySource &DelaySource) {
  return ParseN2kPGN127252(N2kMsg, SID, Heave, Delay, DelaySource);
}

/************************************************************************//**
 * \brief Setting up PGN 127257 Message "Attitude"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides a single transmission that describes the
 * position of a vessel relative to both horizontal and vertical
 * planes. This would typically be used for vessel stabilization, vessel
 * control and onboard platform stabilization.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param Yaw         Heading in radians.
 * \param Pitch       Pitch in radians. Positive, when your bow rises.
 * \param Roll        Roll in radians. Positive, when tilted right.
 * 
 */
void SetN2kPGN127257(tN2kMsg &N2kMsg, unsigned char SID, double Yaw, double Pitch, double Roll);

/************************************************************************//**
 * \brief Setting up Message "Attitude" - PGN 127257
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127257. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127257
  */
inline void SetN2kAttitude(tN2kMsg &N2kMsg, unsigned char SID, double Yaw, double Pitch, double Roll) {
  SetN2kPGN127257(N2kMsg,SID, Yaw, Pitch, Roll);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127257 "Attitude"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides a single transmission that describes the
 * position of a vessel relative to both horizontal and vertical
 * planes. This would typically be used for vessel stabilization, vessel
 * control and onboard platform stabilization.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Yaw         Heading in radians.
 * \param Pitch       Pitch in radians. Positive, when your bow rises.
 * \param Roll        Roll in radians. Positive, when tilted right.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127257(const tN2kMsg &N2kMsg, unsigned char &SID, double &Yaw, double &Pitch, double &Roll);

/************************************************************************//**
 * \brief Parsing the content of a "Attitude" 
 *        message - PGN 127257
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127257. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127257 
 */
inline bool ParseN2kAttitude(const tN2kMsg &N2kMsg, unsigned char &SID, double &Yaw, double &Pitch, double &Roll) {
  return ParseN2kPGN127257(N2kMsg,SID, Yaw, Pitch, Roll);
}

/************************************************************************//**
 * \brief Setting up PGN 127258 Message "Magnetic Variation"
 * \ingroup group_msgSetUp
 * 
 * Message for transmitting variation. The message contains a sequence number
 * to allow synchronization of other messages such as Heading or Course over
 * Ground. The quality of service and age of service are provided to enable
 * recipients to determine an appropriate level of service if multiple
 * transmissions exist.
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                        \n
 *                        The sequence identifier field is used to tie different PGNs data together to same 
 *                        sampling or calculation time.
 * \param Source          How was the variation value generated
 * \param DaysSince1970   Days since January 1, 1970
 * \param Variation       Magnetic variation/declination in radians
 * 
 */
void SetN2kPGN127258(tN2kMsg &N2kMsg, unsigned char SID, tN2kMagneticVariation Source, uint16_t DaysSince1970, double Variation);

/************************************************************************//**
 * \brief Setting up Message "Magnetic Variation" - PGN 127258
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127258. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127258
  */
inline void SetN2kMagneticVariation(tN2kMsg &N2kMsg, unsigned char SID, tN2kMagneticVariation Source, uint16_t DaysSince1970, double Variation) {
  SetN2kPGN127258(N2kMsg, SID, Source, DaysSince1970, Variation);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127258 "Magnetic Variation"
 * \ingroup group_msgParsers
 * 
 * Message for transmitting variation. The message contains a sequence number
 * to allow synchronization of other messages such as Heading or Course over
 * Ground. The quality of service and age of service are provided to enable
 * recipients to determine an appropriate level of service if multiple
 * transmissions exist.
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 * \param SID             Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Source          How was the variation value generated
 * \param DaysSince1970   Days since January 1, 1970
 * \param Variation       Magnetic variation/declination in radians
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127258(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kMagneticVariation &Source, uint16_t &DaysSince1970, double &Variation);

/************************************************************************//**
 * \brief Parsing the content of a "Magnetic Variation" 
 *        message - PGN 127258
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127258. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127258 
 */
inline bool ParseN2kMagneticVariation(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kMagneticVariation &Source, uint16_t &DaysSince1970, double &Variation) {
  return ParseN2kPGN127258(N2kMsg, SID, Source, DaysSince1970, Variation);
}

/************************************************************************//**
 * \brief Setting up PGN 127488 Message "Engine parameters rapid"
 * \ingroup group_msgSetUp
 * 
 * Provides data with a high update rate for a specific engine in a single
 * frame message. The first field provides information as to which engine.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param EngineInstance      Engine instance.
 * \param EngineSpeed         RPM (Revolutions Per Minute)
 * \param EngineBoostPressure in Pascal
 * \param EngineTiltTrim      in %
 * 
 */
void SetN2kPGN127488(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed,
                     double EngineBoostPressure=N2kDoubleNA, int8_t EngineTiltTrim=N2kInt8NA);

/************************************************************************//**
 * \brief Setting up Message "Engine parameters rapid" - PGN 127488
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127488. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127488
  */
inline void SetN2kEngineParamRapid(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineSpeed,
                      double EngineBoostPressure=N2kDoubleNA, int8_t EngineTiltTrim=N2kInt8NA) {
  SetN2kPGN127488(N2kMsg,EngineInstance,EngineSpeed,EngineBoostPressure,EngineTiltTrim);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127488  "Engine parameters rapid"
 * \ingroup group_msgParsers
 * 
 * Provides data with a high update rate for a specific engine in a single
 * frame message. The first field provides information as to which engine.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 * \param EngineInstance      Engine instance.
 * \param EngineSpeed         RPM (Revolutions Per Minute)
 * \param EngineBoostPressure in Pascal
 * \param EngineTiltTrim      in %
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127488(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineSpeed,
                     double &EngineBoostPressure, int8_t &EngineTiltTrim);

/************************************************************************//**
 * \brief Parsing the content of a "Engine parameters rapid" 
 *        message - PGN 127488
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127488. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127488 
 */
inline bool ParseN2kEngineParamRapid(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineSpeed,
                     double &EngineBoostPressure, int8_t &EngineTiltTrim) {
  return ParseN2kPGN127488(N2kMsg,EngineInstance,EngineSpeed,EngineBoostPressure,EngineTiltTrim);
}

/************************************************************************//**
 * \brief Setting up PGN 127489 Message "Engine parameters dynamic"
 * \ingroup group_msgSetUp
 * 
 * Used to provide real-time operational data and status relevant
 * to a specific engine, indicated by the engine instance field. This
 * message would normally be broadcasted periodically to provide information
 * for instrumentation or control functions.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param EngineInstance       Engine instance
 * \param EngineOilPress      in Pascal
 * \param EngineOilTemp       in Kelvin
 * \param EngineCoolantTemp   in Kelvin
 * \param AltenatorVoltage    in Voltage
 * \param FuelRate            in litres/hour
 * \param EngineHours         in seconds
 * \param EngineCoolantPress  in Pascal
 * \param EngineFuelPress     in Pascal
 * \param EngineLoad          in %
 * \param EngineTorque        in %
 * \param Status1             Engine Discrete Status 1
 *                            \ref tN2kEngineDiscreteStatus1
 * \param Status2             Engine Discrete Status 2
 *                            \ref tN2kEngineDiscreteStatus2
 * 
 */
void SetN2kPGN127489(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress=N2kDoubleNA, double EngineFuelPress=N2kDoubleNA,
                       int8_t EngineLoad=N2kInt8NA, int8_t EngineTorque=N2kInt8NA,
                       tN2kEngineDiscreteStatus1 Status1=0, tN2kEngineDiscreteStatus2 Status2=0);

/************************************************************************//**
 * \brief Setting up Message "Engine parameters dynamic" - PGN 127489
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127489
  */
inline void SetN2kEngineDynamicParam(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress=N2kDoubleNA, double EngineFuelPress=N2kDoubleNA,
                       int8_t EngineLoad=N2kInt8NA, int8_t EngineTorque=N2kInt8NA,
                       tN2kEngineDiscreteStatus1 Status1=0, tN2kEngineDiscreteStatus2 Status2=0) {
  SetN2kPGN127489(N2kMsg,EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                       FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque,
                       Status1,Status2);
}                         

/************************************************************************//**
 * \brief Setting up PGN 127489 Message "Engine parameters dynamic"
 * \ingroup group_msgSetUp
 * 
 * Used to provide real-time operational data and status relevant
 * to a specific engine, indicated by the engine instance field. This
 * message would normally be broadcasted periodically to provide information
 * for instrumentation or control functions.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param EngineInstance       Engine instance
 * \param EngineOilPress      in Pascal
 * \param EngineOilTemp       in Kelvin
 * \param EngineCoolantTemp   in Kelvin
 * \param AltenatorVoltage    in Voltage
 * \param FuelRate            in litres/hour
 * \param EngineHours         in seconds
 * \param EngineCoolantPress  in Pascal
 * \param EngineFuelPress     in Pascal
 * \param EngineLoad          in %
 * \param EngineTorque        in %
 * \param flagCheckEngine          Status flag CheckEngine
 * \param flagOverTemp             Status flag OverTemp
 * \param flagLowOilPress          Status flag LowOilPress
 * \param flagLowOilLevel          Status flag LowOilLevel
 * \param flagLowFuelPress         Status flag LowFuelPress
 * \param flagLowSystemVoltage     Status flag LowSystemVoltage
 * \param flagLowCoolantLevel      Status flag LowCoolantLevel
 * \param flagWaterFlow            Status flag WaterFlow
 * \param flagWaterInFuel          Status flag WaterInFuel
 * \param flagChargeIndicator      Status flag ChargeIndicator
 * \param flagPreheatIndicator     Status flag PreheatIndicator
 * \param flagHighBoostPress       Status flag HighBoostPress
 * \param flagRevLimitExceeded     Status flag RevLimitExceeded
 * \param flagEgrSystem            Status flag EgrSystem
 * \param flagTPS                  Status flag TPS
 * \param flagEmergencyStopMode    Status flag EmergencyStopMode
 * \param flagWarning1             Status flag Warning1
 * \param flagWarning2             Status flag Warning2
 * \param flagPowerReduction       Status flag PowerReduction
 * \param flagMaintenanceNeeded    Status flag MaintenanceNeeded
 * \param flagEngineCommError      Status flag EngineCommError
 * \param flagSubThrottle          Status flag SubThrottle
 * \param flagNeutralStartProtect  Status flag NeutralStartProtect
 * \param flagEngineShuttingDown   Status flag EngineShuttingDown
*/
inline void SetN2kPGN127489(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress=N2kDoubleNA, double EngineFuelPress=N2kDoubleNA,
                       int8_t EngineLoad=N2kInt8NA, int8_t EngineTorque=N2kInt8NA,
                       bool flagCheckEngine=false,       bool flagOverTemp=false,         bool flagLowOilPress=false,         bool flagLowOilLevel=false,
                       bool flagLowFuelPress=false,      bool flagLowSystemVoltage=false, bool flagLowCoolantLevel=false,     bool flagWaterFlow=false,
                       bool flagWaterInFuel=false,       bool flagChargeIndicator=false,  bool flagPreheatIndicator=false,    bool flagHighBoostPress=false,
                       bool flagRevLimitExceeded=false,  bool flagEgrSystem=false,        bool flagTPS=false,                 bool flagEmergencyStopMode=false,
                       bool flagWarning1=false,          bool flagWarning2=false,         bool flagPowerReduction=false,      bool flagMaintenanceNeeded=false,
                       bool flagEngineCommError=false,   bool flagSubThrottle=false,      bool flagNeutralStartProtect=false, bool flagEngineShuttingDown=false) {
  tN2kEngineDiscreteStatus1 Status1;
  tN2kEngineDiscreteStatus2 Status2;
  Status1.Bits.CheckEngine=flagCheckEngine;
  Status1.Bits.OverTemperature=flagOverTemp;
  Status1.Bits.LowOilPressure=flagLowOilPress;
  Status1.Bits.LowOilLevel=flagLowOilLevel;
  Status1.Bits.LowFuelPressure=flagLowFuelPress;
  Status1.Bits.LowSystemVoltage=flagLowSystemVoltage;
  Status1.Bits.LowCoolantLevel=flagLowCoolantLevel;
  Status1.Bits.WaterFlow=flagWaterFlow;
  Status1.Bits.WaterInFuel=flagWaterInFuel;
  Status1.Bits.ChargeIndicator=flagChargeIndicator;
  Status1.Bits.PreheatIndicator=flagPreheatIndicator;
  Status1.Bits.HighBoostPressure=flagHighBoostPress;
  Status1.Bits.RevLimitExceeded=flagRevLimitExceeded;
  Status1.Bits.EGRSystem=flagEgrSystem;
  Status1.Bits.ThrottlePositionSensor=flagTPS;
  Status1.Bits.EngineEmergencyStopMode=flagEmergencyStopMode;
  Status2.Bits.WarningLevel1=flagWarning1;
  Status2.Bits.WarningLevel2=flagWarning2;
  Status2.Bits.LowOiPowerReduction=flagPowerReduction;
  Status2.Bits.MaintenanceNeeded=flagMaintenanceNeeded;
  Status2.Bits.EngineCommError=flagEngineCommError;
  Status2.Bits.SubOrSecondaryThrottle=flagSubThrottle;
  Status2.Bits.NeutralStartProtect=flagNeutralStartProtect;
  Status2.Bits.EngineShuttingDown=flagEngineShuttingDown;

  SetN2kPGN127489(N2kMsg,EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                       FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque,
                       Status1,Status2);
}

/************************************************************************//**
 * \brief Setting up Message "Engine parameters dynamic" - PGN 127489
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127489
  */
inline void SetN2kEngineDynamicParam(tN2kMsg &N2kMsg, unsigned char EngineInstance, double EngineOilPress, double EngineOilTemp, double EngineCoolantTemp, double AltenatorVoltage,
                       double FuelRate, double EngineHours, double EngineCoolantPress=N2kDoubleNA, double EngineFuelPress=N2kDoubleNA,
                       int8_t EngineLoad=N2kInt8NA, int8_t EngineTorque=N2kInt8NA,
                       bool flagCheckEngine=false,       bool flagOverTemp=false,         bool flagLowOilPress=false,         bool flagLowOilLevel=false,
                       bool flagLowFuelPress=false,      bool flagLowSystemVoltage=false, bool flagLowCoolantLevel=false,     bool flagWaterFlow=false,
                       bool flagWaterInFuel=false,       bool flagChargeIndicator=false,  bool flagPreheatIndicator=false,    bool flagHighBoostPress=false,
                       bool flagRevLimitExceeded=false,  bool flagEgrSystem=false,        bool flagTPS=false,                 bool flagEmergencyStopMode=false,
                       bool flagWarning1=false,          bool flagWarning2=false,         bool flagPowerReduction=false,      bool flagMaintenanceNeeded=false,
                       bool flagEngineCommError=false,   bool flagSubThrottle=false,      bool flagNeutralStartProtect=false, bool flagEngineShuttingDown=false) {
  SetN2kPGN127489(N2kMsg,EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                       FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque,
                       flagCheckEngine, flagOverTemp, flagLowOilPress, flagLowOilLevel,
                       flagLowFuelPress, flagLowSystemVoltage, flagLowCoolantLevel, flagWaterFlow,
                       flagWaterInFuel, flagChargeIndicator, flagPreheatIndicator, flagHighBoostPress,
                       flagRevLimitExceeded, flagEgrSystem, flagTPS, flagEmergencyStopMode,
                       flagWarning1, flagWarning2, flagPowerReduction, flagMaintenanceNeeded,
                       flagEngineCommError, flagSubThrottle, flagNeutralStartProtect, flagEngineShuttingDown);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127489 "Engine parameters dynamic"
 * \ingroup group_msgParsers
 * 
 * Used to provide real-time operational data and status relevant
 * to a specific engine, indicated by the engine instance field. This
 * message would normally be broadcasted periodically to provide information
 * for instrumentation or control functions.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 * \param EngineInstance       Engine instance
 * \param EngineOilPress      in Pascal
 * \param EngineOilTemp       in Kelvin
 * \param EngineCoolantTemp   in Kelvin
 * \param AltenatorVoltage    in Voltage
 * \param FuelRate            in litres/hour
 * \param EngineHours         in seconds
 * \param EngineCoolantPress  in Pascal
 * \param EngineFuelPress     in Pascal
 * \param EngineLoad          in %
 * \param EngineTorque        in %
 * \param Status1             Engine Discrete Status 1
 *                            \ref tN2kEngineDiscreteStatus1
 * \param Status2             Engine Discrete Status 2
 *                            \ref tN2kEngineDiscreteStatus2
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127489(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineOilPress,
                      double &EngineOilTemp, double &EngineCoolantTemp, double &AltenatorVoltage,
                      double &FuelRate, double &EngineHours, double &EngineCoolantPress, double &EngineFuelPress,
                      int8_t &EngineLoad, int8_t &EngineTorque,
                      tN2kEngineDiscreteStatus1 &Status1, tN2kEngineDiscreteStatus2 &Status2);

/************************************************************************//**
 * \brief Parsing the content of a "Engine parameters dynamic" 
 *        message - PGN 127489
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127489 
 */
inline bool ParseN2kPGN127489(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineOilPress,
                      double &EngineOilTemp, double &EngineCoolantTemp, double &AltenatorVoltage,
                      double &FuelRate, double &EngineHours, double &EngineCoolantPress, double &EngineFuelPress,
                      int8_t &EngineLoad, int8_t &EngineTorque) {
  tN2kEngineDiscreteStatus1 Status1;
  tN2kEngineDiscreteStatus2 Status2;
  return ParseN2kPGN127489(N2kMsg, EngineInstance, EngineOilPress,
                    EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                    FuelRate, EngineHours,EngineCoolantPress, EngineFuelPress,
                    EngineLoad, EngineTorque,Status1,Status2);
}

/************************************************************************//**
 * \brief Parsing the content of a "Engine parameters dynamic" 
 *        message - PGN 127489
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127489 
 */
inline bool ParseN2kEngineDynamicParam(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineOilPress,
                      double &EngineOilTemp, double &EngineCoolantTemp, double &AltenatorVoltage,
                      double &FuelRate, double &EngineHours, double &EngineCoolantPress, double &EngineFuelPress,
                      int8_t &EngineLoad, int8_t &EngineTorque) {
    return ParseN2kPGN127489(N2kMsg, EngineInstance, EngineOilPress,
                      EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                      FuelRate, EngineHours,EngineCoolantPress, EngineFuelPress,
                      EngineLoad, EngineTorque);
}

/************************************************************************//**
 * \brief Parsing the content of a "Engine parameters dynamic" 
 *        message - PGN 127489
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127489 
 */
inline bool ParseN2kEngineDynamicParam(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineOilPress,
                      double &EngineOilTemp, double &EngineCoolantTemp, double &AltenatorVoltage,
                      double &FuelRate, double &EngineHours, double &EngineCoolantPress, double &EngineFuelPress,
                      int8_t &EngineLoad, int8_t &EngineTorque,
                      tN2kEngineDiscreteStatus1 &Status1, tN2kEngineDiscreteStatus2 &Status2) {
    return ParseN2kPGN127489(N2kMsg, EngineInstance, EngineOilPress,
                      EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                      FuelRate, EngineHours,EngineCoolantPress, EngineFuelPress,
                      EngineLoad, EngineTorque,
                      Status1, Status2);
}

/************************************************************************//**
 * \brief Parsing the content of a "Engine parameters dynamic" 
 *        message - PGN 127489
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127489. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127489 
 */
inline bool ParseN2kEngineDynamicParam(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &EngineOilPress,
                      double &EngineOilTemp, double &EngineCoolantTemp, double &AltenatorVoltage,
                      double &FuelRate, double &EngineHours) {
    double EngineCoolantPress, EngineFuelPress;
    int8_t EngineLoad, EngineTorque;
    tN2kEngineDiscreteStatus1 Status1;
    tN2kEngineDiscreteStatus2 Status2;
    return ParseN2kPGN127489(N2kMsg, EngineInstance, EngineOilPress,
                      EngineOilTemp, EngineCoolantTemp, AltenatorVoltage,
                      FuelRate, EngineHours,EngineCoolantPress, EngineFuelPress,
                      EngineLoad, EngineTorque,Status1,Status2);
}

/************************************************************************//**
 * \brief Setting up PGN 127493 Message "Transmission parameters, dynamic"
 * \ingroup group_msgSetUp
 * 
 * Used to provide the operational state and internal operating parameters of
 * a specific transmission, indicated by the transmission instance field. This
 * message would normally be broadcasted periodically to provide information
 * for instrumentation or control functions.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param EngineInstance      Engine instance.
 * \param TransmissionGear    Selected transmission. \ref tN2kTransmissionGear
 * \param OilPressure         in Pascal
 * \param OilTemperature      in Kelvin
 * \param DiscreteStatus1     in %
 * 
 */
void SetN2kPGN127493(tN2kMsg &N2kMsg, unsigned char EngineInstance, tN2kTransmissionGear TransmissionGear,
                     double OilPressure, double OilTemperature, unsigned char DiscreteStatus1=0);

/************************************************************************//**
 * \brief Setting up Message "Transmission parameters, dynamic" - PGN 127493
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127493. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127493
  */
inline void SetN2kTransmissionParameters(tN2kMsg &N2kMsg, unsigned char EngineInstance, tN2kTransmissionGear TransmissionGear,
                     double OilPressure, double OilTemperature, unsigned char DiscreteStatus1=0) {
  SetN2kPGN127493(N2kMsg, EngineInstance, TransmissionGear, OilPressure, OilTemperature, DiscreteStatus1);
}

/************************************************************************//**
 * \brief Setting up Message "Transmission parameters, dynamic" - PGN 127493
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127493. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127493
  */
inline void SetN2kTransmissionParameters(tN2kMsg &N2kMsg, unsigned char EngineInstance, tN2kTransmissionGear TransmissionGear,
                     double OilPressure, double OilTemperature,
                     bool flagCheck,       bool flagOverTemp,         bool flagLowOilPressure=false,         bool flagLowOilLevel=false,
                     bool flagSailDrive=false) {
  unsigned char DiscreteStatus1=0;

  if (flagCheck) DiscreteStatus1          |= BIT(0);
  if (flagOverTemp) DiscreteStatus1       |= BIT(1);
  if (flagLowOilPressure) DiscreteStatus1 |= BIT(2);
  if (flagLowOilLevel) DiscreteStatus1    |= BIT(3);
  if (flagSailDrive) DiscreteStatus1      |= BIT(4);
  SetN2kPGN127493(N2kMsg, EngineInstance, TransmissionGear, OilPressure, OilTemperature,DiscreteStatus1);
}
/************************************************************************//**
 * \brief Parsing the content of Message PGN 127493 "Transmission parameters, 
 *        dynamic"
 * \ingroup group_msgParsers
 * 
 * Used to provide the operational state and internal operating parameters of
 * a specific transmission, indicated by the transmission instance field. This
 * message would normally be broadcasted periodically to provide information
 * for instrumentation or control functions.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 * \param EngineInstance      Engine instance.
 * \param TransmissionGear    Selected transmission. \ref tN2kTransmissionGear
 * \param OilPressure         in Pascal
 * \param OilTemperature      in Kelvin
 * \param DiscreteStatus1     in %
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127493(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, tN2kTransmissionGear &TransmissionGear,
                     double &OilPressure, double &OilTemperature, unsigned char &DiscreteStatus1);

/************************************************************************//**
 * \brief Parsing the content of a "Transmission parameters, dynamic" 
 *        message - PGN 127493
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127493. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127493 
 */
inline bool ParseN2kTransmissionParameters(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, tN2kTransmissionGear &TransmissionGear,
                     double &OilPressure, double &OilTemperature, unsigned char &DiscreteStatus1) {
  return ParseN2kPGN127493(N2kMsg, EngineInstance, TransmissionGear, OilPressure, OilTemperature, DiscreteStatus1);
}

/************************************************************************//**
 * \brief Parsing the content of a "Transmission parameters, dynamic" 
 *        message - PGN 127493
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127493. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127493 
 */
inline bool ParseN2kTransmissionParameters(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, tN2kTransmissionGear &TransmissionGear,
                     double &OilPressure, double &OilTemperature,
                     bool &flagCheck,       bool &flagOverTemp,         bool &flagLowOilPressure,         bool &flagLowOilLevel,
                     bool &flagSailDrive) {
  unsigned char DiscreteStatus1;
  bool ret=ParseN2kPGN127493(N2kMsg, EngineInstance, TransmissionGear, OilPressure, OilTemperature, DiscreteStatus1);
  if (ret) {
    flagCheck          = ((DiscreteStatus1 & BIT(0))!=0);
    flagOverTemp       = ((DiscreteStatus1 & BIT(1))!=0);
    flagLowOilPressure = ((DiscreteStatus1 & BIT(2))!=0);
    flagLowOilLevel    = ((DiscreteStatus1 & BIT(3))!=0);
    flagSailDrive      = ((DiscreteStatus1 & BIT(4))!=0);
  }
  return ret;
}

/************************************************************************//**
 * \brief Setting up PGN 127497 Message "Trip Parameters, Engine"
 * \ingroup group_msgSetUp
 * 
 * Engine related trip / fuel information.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param EngineInstance            Engine instance
 * \param TripFuelUsed              in litres
 * \param FuelRateAverage           in litres/hour
 * \param FuelRateEconomy           in litres/hour
 * \param InstantaneousFuelEconomy  in litres/hour
 * 
 */
void SetN2kPGN127497(tN2kMsg &N2kMsg, unsigned char EngineInstance, double TripFuelUsed,
                     double FuelRateAverage,
                     double FuelRateEconomy=N2kDoubleNA, double InstantaneousFuelEconomy=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Trip Parameters, Engine" - PGN 127497
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127497. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127497
  */
inline void SetN2kEngineTripParameters(tN2kMsg &N2kMsg, unsigned char EngineInstance, double TripFuelUsed,
                     double FuelRateAverage,
                     double FuelRateEconomy=N2kDoubleNA, double InstantaneousFuelEconomy=N2kDoubleNA) {
  SetN2kPGN127497(N2kMsg,EngineInstance,TripFuelUsed,FuelRateAverage,FuelRateEconomy,InstantaneousFuelEconomy);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127497 "Trip Parameters, Engine"
 * \ingroup group_msgParsers
 * 
 * Engine related trip / fuel information.
 *  
 * \param N2kMsg              Reference to a N2kMsg Object, 
 * \param EngineInstance            Engine instance
 * \param TripFuelUsed              in litres
 * \param FuelRateAverage           in litres/hour
 * \param FuelRateEconomy           in litres/hour
 * \param InstantaneousFuelEconomy  in litres/hour
 *
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127497(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &TripFuelUsed,
                     double &FuelRateAverage,
                     double &FuelRateEconomy, double &InstantaneousFuelEconomy);

/************************************************************************//**
 * \brief Parsing the content of a "Trip Parameters, Engine" 
 *        message - PGN 127497
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127497. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127497 
 */
inline bool ParseN2kEngineTripParameters(const tN2kMsg &N2kMsg, unsigned char &EngineInstance, double &TripFuelUsed,
                     double &FuelRateAverage,
                     double &FuelRateEconomy, double &InstantaneousFuelEconomy) {
  return ParseN2kPGN127497(N2kMsg,EngineInstance,TripFuelUsed,FuelRateAverage,FuelRateEconomy, InstantaneousFuelEconomy);
}

/************************************************************************//**
 * \brief 64bit Binary Status value of a full bank status, that
 * can handle up to 28 item states in one value.
 * 
 * \sa ParseN2kPGN127501 or ParseN2kBinaryStatus
 */
typedef uint64_t tN2kBinaryStatus;

/************************************************************************//**
 * \brief Get single status to full binary bank status.
 * 
 * This helper function gets a single item statue \ref tN2kOnOff out
 * of a 64bit (8byte) bank status value. The bank status can carry up to 
 * 28 individual item states.
 * 
 * \param BankStatus    64bit full bank status \ref tN2kBinaryStatus
 * \param ItemIndex     index of the item [1 .. 28]
 * 
 * \return tN2kOnOff Status of an individual item
 * 
 * \sa ParseN2kPGN127501 or ParseN2kBinaryStatus
 * 
 */
tN2kOnOff N2kGetStatusOnBinaryStatus(tN2kBinaryStatus BankStatus, uint8_t ItemIndex=1);

/************************************************************************//**
 * \brief Reset all single binary status values to not available
 * 
 * This helper function resets the full 64bit bank status. For each individual
 * item the status will be 3 (0b11 - Unavailable \ref tN2kOnOff)
 * 
 * \param BankStatus 64bit full bank status \ref tN2kBinaryStatus
 */
inline void N2kResetBinaryStatus(tN2kBinaryStatus &BankStatus) { BankStatus=0xffffffffffffffffULL; }

/************************************************************************//**
 * \brief Set single status to full binary bank status.
 * 
 * This helper function sets a single item statue \ref tN2kOnOff in
 * a 64bit (8byte) bank status value. The bank status can carry up to 
 * 28 individual item states.
 * 
 * \param BankStatus    64bit full bank status \ref tN2kBinaryStatus
 * \param ItemStatus    individual item status
 * \param ItemIndex     index of the item [1 .. 28]
 */
void N2kSetStatusBinaryOnStatus(tN2kBinaryStatus &BankStatus, tN2kOnOff ItemStatus, uint8_t ItemIndex=1);

/************************************************************************//**
 * \brief Setting up PGN 127501 Message "Universal Binary Status Report"
 * \ingroup group_msgSetUp
 * 
 * Universal status report for binary state devices in banks of up to
 * 28 devices each. That means that a Device can report up to 32 states.
 * The meaning of these states depends on the implementation by
 * the manufacture.
 *
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param DeviceBankInstance  Device or Bank Instance
 * \param BankStatus          Full bank status. Read single status by using
 *                            \ref N2kGetStatusOnBinaryStatus
 */
void SetN2kPGN127501(tN2kMsg &N2kMsg, unsigned char DeviceBankInstance, tN2kBinaryStatus BankStatus);

/************************************************************************//**
 * \brief Setting up Message "Universal Binary Status Report" - PGN 127501
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127501. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127501
  */
inline void SetN2kBinaryStatus(tN2kMsg &N2kMsg, unsigned char DeviceBankInstance, tN2kBinaryStatus BankStatus) {
	SetN2kPGN127501(N2kMsg,DeviceBankInstance,BankStatus);
}

/************************************************************************//**
 * \brief Setting up PGN 127501 Message "Universal Binary Status Report"
 * \ingroup group_msgSetUp
 * 
 * \note This function uses only the first four states of the bank status.
 * 
 * Universal status report for binary state devices in banks of up to
 * 28 devices each. That means that a Device can report up to 32 states.
 * The meaning of these states depends on the implementation by
 * the manufacture.
 *
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param DeviceBankInstance  Device or Bank Instance
 * \param Status1             On/Off for Status1
 * \param Status2             On/Off for Status2
 * \param Status3             On/Off for Status3
 * \param Status4             On/Off for Status4
 * 
 */
void SetN2kPGN127501(tN2kMsg &N2kMsg, unsigned char DeviceBankInstance
                      ,tN2kOnOff Status1
                      ,tN2kOnOff Status2=N2kOnOff_Unavailable
                      ,tN2kOnOff Status3=N2kOnOff_Unavailable
                      ,tN2kOnOff Status4=N2kOnOff_Unavailable
                    );

/************************************************************************//**
 * \brief Setting up Message "Universal Binary Status Report" - PGN 127501
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127501. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127501
  */
inline void SetN2kBinaryStatus(tN2kMsg &N2kMsg, unsigned char DeviceBankInstance
                      ,tN2kOnOff Status1
                      ,tN2kOnOff Status2=N2kOnOff_Unavailable
                      ,tN2kOnOff Status3=N2kOnOff_Unavailable
                      ,tN2kOnOff Status4=N2kOnOff_Unavailable
					) {
  SetN2kPGN127501(N2kMsg, DeviceBankInstance,Status1,Status2,Status3,Status4);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127501 Message "Universal Binary
 * Status Report"
 * \ingroup group_msgParsers
 * 
 * \note This function parses the first four states of the bank status.
 * 
 * Universal status report for binary state devices in banks of up to
 * 28 devices each. That means that a Device can report up to 32 states.
 * The meaning of these states depends on the implementation by
 * the manufacture.
 *
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param DeviceBankInstance  Device or Bank Instance
 * \param Status1             On/Off for Status1
 * \param Status2             On/Off for Status2
 * \param Status3             On/Off for Status3
 * \param Status4             On/Off for Status4
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127501(const tN2kMsg &N2kMsg, unsigned char &DeviceBankInstance
                      ,tN2kOnOff &Status1
                      ,tN2kOnOff &Status2
                      ,tN2kOnOff &Status3
                      ,tN2kOnOff &Status4
                    );

/************************************************************************//**
 * \brief Parsing the content of a "Universal Binary Status Report" 
 *        message - PGN 127501
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127501. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127501 
 */
inline bool ParseN2kBinaryStatus(const tN2kMsg &N2kMsg, unsigned char &DeviceBankInstance
                      ,tN2kOnOff &Status1
                      ,tN2kOnOff &Status2
                      ,tN2kOnOff &Status3
                      ,tN2kOnOff &Status4
                    ) {
 return ParseN2kPGN127501(N2kMsg,DeviceBankInstance,Status1,Status2,Status3,Status4);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 127501 Message "Universal Binary
 * Status Report"
 * \ingroup group_msgParsers
 * 
 * Universal status report for binary state devices in banks of up to
 * 28 devices each. That means that a Device can report up to 32 states.
 * The meaning of these states depends on the implementation by
 * the manufacture.
 *
 * \note Use N2kGetStatusOnBinaryStatus to read specific status
 * 
 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param DeviceBankInstance  Device or Bank Instance
 * \param BankStatus          Full bank status. Read single status by using
 *                            \ref N2kGetStatusOnBinaryStatus
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127501(const tN2kMsg &N2kMsg, unsigned char &DeviceBankInstance, tN2kBinaryStatus &BankStatus);

/************************************************************************//**
 * \brief Parsing the content of a "Universal Binary Status Report" 
 *        message - PGN 127501
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127501. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127501 
 */
inline bool ParseN2kBinaryStatus(const tN2kMsg &N2kMsg, unsigned char &DeviceBankInstance, tN2kBinaryStatus &BankStatus) {
 return ParseN2kPGN127501(N2kMsg,DeviceBankInstance,BankStatus);
}

/************************************************************************//**
 * \brief Parse the content of a PGN 127502 (Switch Bank Control) message.
 * \ingroup group_msgParsers
 * 
 * Review \ref N2kGetStatusOnBinaryStatus and the documentation of tN2kOnOff
 * for information on how to process the bank status data returned by this
 * function.
 * 
 * \note This PGN is deprecated by NMEA and modern switch bank devices may
 *       well not support it, favouring PGN 126208 Command Group Function.
 *  
 * \param N2kMsg              Reference to the N2kMsg object to be parsed. If
 *                            this contains a valid PGN 127502 message then
 *                            the following return values will be set. 
 * \param TargetBankInstance  Instance number of the switchbank that was
 *                            targetted by this switchbank control message.
 * \param BankStatus          The binary status component of the switchbank
 *                            control containing the commanded state of channels
 *                            on the target switchbank.
 * 
 * \return true               Parsing of PGN Message successful
 * \return false              Parsing of PGN Message aborted
 */
bool ParseN2kPGN127502(const tN2kMsg &N2kMsg, unsigned char &TargetBankInstance, tN2kBinaryStatus &BankStatus);

/************************************************************************//**
 * \brief Parse PGN 127502 "Switch Bank Control" message.
 * \ingroup group_msgSetUp
 * 
 * Alias of \ref ParseN2kPGN127502. This alias was introduced to improve the
 * readability of the source code.
 */
inline bool ParseN2kSwitchbankControl(const tN2kMsg &N2kMsg, unsigned char &TargetBankInstance, tN2kBinaryStatus &BankStatus) {
  return ParseN2kPGN127502(N2kMsg, TargetBankInstance, BankStatus);
}

/************************************************************************//**
 * \brief Set up PGN 127502 "Switch Bank Control" message.
 * \ingroup group_msgSetUp
 * 
 * Command channel states on a remote switch bank. Up to 28 remote binary
 * states can be controlled.
 * 
 * When you create a tN2kBinaryStatus object for use with this function
 * you should ensure that you only command (that is set ON or OFF) those
 * channels which you intend to operate. Channels in which you have no
 * interest should not be commanded but set not available.
 * 
 * Review \ref N2kResetBinaryStatus, \ref N2kSetStatusOnBinaryStatus and the
 * documentation of tN2kOnOff for information on how to set up bank status.
 * 
 * Remember as well, that transmission of a PGN 127502 message is equivalent
 * to issuing a command, so do not send the same message repeatdly: once should
 * be enough. You can always check that the target switchbank has responded
 * by checking its PGN 127501 broadcasts.
 * 
 * \note This PGN is deprecated by NMEA and modern switch bank devices may
 *       well not support it, favouring PGN 126208 Command Group Function.
 *
 * \param N2kMsg              Reference to an N2kMsg Object which will be
 *                            configured for sending on the NMEA bus. 
 * \param DeviceBankInstance  Instance number of the target switchbank (i.e.
 *                            the device to be controlled).
 * \param BankStatus          Full bank status containing the channel states
 *                            to be commanded on the remote switchbank.
 *                            \ref N2kGetStatusOnBinaryStatus
 */
void SetN2kPGN127502(tN2kMsg &N2kMsg, unsigned char TargetBankInstance, tN2kBinaryStatus BankStatus);

/************************************************************************//**
 * \brief Set up PGN 127502 "Switch Bank Control" message.
 * \ingroup group_msgSetUp
 * 
 * Alias of \ref SetN2kPGN127502. This alias was introduced to improve the
 * readability of the source code.
 */
inline void SetN2kSwitchbankControl(tN2kMsg &N2kMsg, unsigned char TargetBankInstance, tN2kBinaryStatus BankStatus) {
	SetN2kPGN127502(N2kMsg, TargetBankInstance, BankStatus);
}

/************************************************************************//**
 * \brief Setting up PGN 127505 Message "Fluid level"
 * \ingroup group_msgSetUp
 * 
 * Fluid Level contains an instance number, type of fluid, level of fluid, and
 * tank capacity. For example the fluid instance may be the level of fuel in a
 * tank or the level of water in the bilge. Used primarily by display or
 * instrumentation devices.
 *
 * \param N2kMsg       Reference to a N2kMsg Object, 
 *                     Output: NMEA2000 message ready to be send.
 * \param Instance     Tank instance. Different devices handles this a
 *                     bit differently. So it is best to have instance
 *                     unique over
 * \param FluidType    Defines type of fluid. See definition of tN2kFluidType
 * \param Level        Tank level in % of full tank.
 * \param Capacity     Tank Capacity in litres
 * 
 */
void SetN2kPGN127505(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity);

/************************************************************************//**
 * \brief Setting up Message "Fluid level" - PGN 127505
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127505. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127505
  */
inline void SetN2kFluidLevel(tN2kMsg &N2kMsg, unsigned char Instance, tN2kFluidType FluidType, double Level, double Capacity) {
  SetN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 127505 "Fluid level"
 * \ingroup group_msgParsers
 * 
 * Fluid Level contains an instance number, type of fluid, level of fluid, and
 * tank capacity. For example the fluid instance may be the level of fuel in a
 * tank or the level of water in the bilge. Used primarily by display or
 * instrumentation devices.
 *
 * \param N2kMsg       Reference to a N2kMsg Object, 
 * \param Instance     Tank instance. Different devices handles this a
 *                     bit differently. So it is best to have instance
 *                     unique over
 * \param FluidType    Defines type of fluid. See definition of tN2kFluidType
 * \param Level        Tank level in % of full tank.
 * \param Capacity     Tank Capacity in litres
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127505(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity);

/************************************************************************//**
 * \brief Parsing the content of a "Fluid level" 
 *        message - PGN 127505
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127505. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127505 
 */
inline bool ParseN2kFluidLevel(const tN2kMsg &N2kMsg, unsigned char &Instance, tN2kFluidType &FluidType, double &Level, double &Capacity) {
  return ParseN2kPGN127505(N2kMsg, Instance, FluidType, Level, Capacity);
}

/************************************************************************//**
 * \brief Setting up PGN 127506 Message "DC Detailed Status"
 * \ingroup group_msgSetUp
 * 
 * Provides parametric data for a specific battery, indicated by the battery
 * instance field. Used primarily by display or instrumentation devices, but
 * may also be used by battery management controls.
 *
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                        \n
 *                        The sequence identifier field is used to tie different PGNs data together to same 
 *                        sampling or calculation time.
 * \param DCInstance      DC instance
 * \param DCType          Defines type of DC source. See definition of 
 *                        \ref tN2kDCType
 * \param StateOfCharge   % of charge
 * \param StateOfHealth   % of health
 * \param TimeRemaining   Time remaining in seconds
 * \param RippleVoltage   DC output voltage ripple in V
 * \param Capacity        Battery capacity in coulombs
 * 
 */
void SetN2kPGN127506(tN2kMsg &N2kMsg, unsigned char SID, unsigned char DCInstance, tN2kDCType DCType,
                     unsigned char StateOfCharge, unsigned char StateOfHealth, double TimeRemaining, double RippleVoltage=N2kDoubleNA, double Capacity=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "DC Detailed Status" - PGN 127506
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127506. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127506
  */
inline void SetN2kDCStatus(tN2kMsg &N2kMsg, unsigned char SID, unsigned char DCInstance, tN2kDCType DCType,
                     unsigned char StateOfCharge, unsigned char StateOfHealth, double TimeRemaining, double RippleVoltage=N2kDoubleNA, double Capacity=N2kDoubleNA) {
  SetN2kPGN127506(N2kMsg,SID,DCInstance,DCType,StateOfCharge,StateOfHealth,TimeRemaining,RippleVoltage,Capacity);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 127506 "DC Detailed Status"
 * \ingroup group_msgParsers
 * 
 * Provides parametric data for a specific battery, indicated by the battery
 * instance field. Used primarily by display or instrumentation devices, but
 * may also be used by battery management controls.
 *
 * \param N2kMsg       Reference to a N2kMsg Object, 
 * \param SID             Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param DCInstance      DC instance
 * \param DCType          Defines type of DC source. See definition of 
 *                        \ref tN2kDCType
 * \param StateOfCharge   % of charge
 * \param StateOfHealth   % of health
 * \param TimeRemaining   Time remaining in seconds
 * \param RippleVoltage   DC output voltage ripple in V
 * \param Capacity        Battery capacity in coulombs
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *  
 */
bool ParseN2kPGN127506(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &DCInstance, tN2kDCType &DCType,
                     unsigned char &StateOfCharge, unsigned char &StateOfHealth, double &TimeRemaining, double &RippleVoltage, double &Capacity);

/************************************************************************//**
 * \brief Parsing the content of a "DC Detailed Status" 
 *        message - PGN 127506
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127506. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127506 
 */
inline bool ParseN2kDCStatus(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &DCInstance, tN2kDCType &DCType,
                     unsigned char &StateOfCharge, unsigned char &StateOfHealth, double &TimeRemaining, double &RippleVoltage, double &Capacity) {
  return ParseN2kPGN127506(N2kMsg,SID,DCInstance,DCType,StateOfCharge,StateOfHealth,TimeRemaining,RippleVoltage, Capacity);
}

/************************************************************************//**
 * \brief Setting up PGN 127507 Message "DC Charger Status"
 * \ingroup group_msgSetUp
 * 
 * Any device capable of charging a battery may transmit this message.
 * \note This PGN is DEPRECATED
 * 
 * \param N2kMsg       Reference to a N2kMsg Object, 
 *                     Output: NMEA2000 message ready to be send.
 * \param Instance                    ChargerInstance
 * \param BatteryInstance             BatteryInstance
 * \param ChargeState                 see \ref tN2kChargeState
 * \param ChargerMode                 see \ref tN2kChargerMode
 * \param Enabled                     set \ref tN2kOnOff
 * \param EqualizationPending         set \ref tN2kOnOff
 * \param EqualizationTimeRemaining   double seconds
 * 
 */
void SetN2kPGN127507(tN2kMsg &N2kMsg, unsigned char Instance, unsigned char BatteryInstance,
                     tN2kChargeState ChargeState, tN2kChargerMode ChargerMode=N2kCM_Standalone,
                     tN2kOnOff Enabled=N2kOnOff_On, tN2kOnOff EqualizationPending=N2kOnOff_Unavailable, double EqualizationTimeRemaining=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "DC Charger Status" - PGN 127507
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127507. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127507
  */
inline void SetN2kChargerStatus(tN2kMsg &N2kMsg, unsigned char Instance, unsigned char BatteryInstance,
                     tN2kChargeState ChargeState, tN2kChargerMode ChargerMode=N2kCM_Standalone,
                     tN2kOnOff Enabled=N2kOnOff_On, tN2kOnOff EqualizationPending=N2kOnOff_Unavailable, double EqualizationTimeRemaining=N2kDoubleNA) {
 SetN2kPGN127507(N2kMsg, Instance,BatteryInstance,ChargeState,ChargerMode,Enabled,EqualizationPending,EqualizationTimeRemaining);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 127507 "DC Charger Status"
 * \ingroup group_msgParsers
 * 
 * Any device capable of charging a battery may transmit this message.
 * \note This PGN is DEPRECATED
 * 
 * \param N2kMsg       Reference to a N2kMsg Object, 
 *                     Output: NMEA2000 message ready to be send.
 * \param Instance                    ChargerInstance
 * \param BatteryInstance             BatteryInstance
 * \param ChargeState                 see \ref tN2kChargeState
 * \param ChargerMode                 see \ref tN2kChargerMode
 * \param Enabled                     set \ref tN2kOnOff
 * \param EqualizationPending         set \ref tN2kOnOff
 * \param EqualizationTimeRemaining   double seconds
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127507(const tN2kMsg &N2kMsg, unsigned char &Instance, unsigned char &BatteryInstance,
                     tN2kChargeState &ChargeState, tN2kChargerMode &ChargerMode,
                     tN2kOnOff &Enabled, tN2kOnOff &EqualizationPending, double &EqualizationTimeRemaining);

/************************************************************************//**
 * \brief Parsing the content of a "DC Charger Status" 
 *        message - PGN 127507
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127507. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127507 
 */
inline bool ParseN2kChargerStatus(const tN2kMsg &N2kMsg, unsigned char &Instance, unsigned char &BatteryInstance,
                     tN2kChargeState &ChargeState, tN2kChargerMode &ChargerMode,
                     tN2kOnOff &Enabled, tN2kOnOff &EqualizationPending, double &EqualizationTimeRemaining) {
 return ParseN2kPGN127507(N2kMsg, Instance,BatteryInstance,ChargeState,ChargerMode,Enabled,EqualizationPending,EqualizationTimeRemaining);
}

/************************************************************************//**
 * \brief Setting up PGN 127508 Message "Battery Status"
 * \ingroup group_msgSetUp
 * 
 * Provides parametric data for a specific DC Source, indicated by the
 * instance field. The type of DC Source can be identified from the
 * DC Detailed Status PGN. Used primarily by display or instrumentation
 * devices, but may also be used by power management.

 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param BatteryInstance     BatteryInstance.
 * \param BatteryVoltage      Battery voltage in V
 * \param BatteryCurrent      Current in A
 * \param BatteryTemperature  Battery temperature in K. Use function
 *                            CToKelvin, if you want to use °C.
 * \param SID                 Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                            \n
 *                            The sequence identifier field is used to tie different PGNs data together to same 
 *                            sampling or calculation time.
 */
void SetN2kPGN127508(tN2kMsg &N2kMsg, unsigned char BatteryInstance, double BatteryVoltage, double BatteryCurrent=N2kDoubleNA,
                     double BatteryTemperature=N2kDoubleNA, unsigned char SID=0xff);

/************************************************************************//**
 * \brief Setting up Message "Battery Status" - PGN 127508
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127508. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127508
  */
inline void SetN2kDCBatStatus(tN2kMsg &N2kMsg, unsigned char BatteryInstance, double BatteryVoltage, double BatteryCurrent=N2kDoubleNA,
                     double BatteryTemperature=N2kDoubleNA, unsigned char SID=1) {
  SetN2kPGN127508(N2kMsg,BatteryInstance,BatteryVoltage,BatteryCurrent,BatteryTemperature,SID);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 127508 "Battery Status"
 * \ingroup group_msgParsers
 * 
 * Provides parametric data for a specific DC Source, indicated by the
 * instance field. The type of DC Source can be identified from the
 * DC Detailed Status PGN. Used primarily by display or instrumentation
 * devices, but may also be used by power management.

 * \param N2kMsg              Reference to a N2kMsg Object, 
 *                            Output: NMEA2000 message ready to be send.
 * \param BatteryInstance     BatteryInstance.
 * \param BatteryVoltage      Battery voltage in V
 * \param BatteryCurrent      Current in A
 * \param BatteryTemperature  Battery temperature in K. Use function
 *                            CToKelvin, if you want to use °C.
 * \param SID                 Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN127508(const tN2kMsg &N2kMsg, unsigned char &BatteryInstance, double &BatteryVoltage, double &BatteryCurrent,
                     double &BatteryTemperature, unsigned char &SID);

/************************************************************************//**
 * \brief Parsing the content of a "Battery Status" 
 *        message - PGN 127508
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127508. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127508 
 */
inline bool ParseN2kDCBatStatus(const tN2kMsg &N2kMsg, unsigned char &BatteryInstance, double &BatteryVoltage, double &BatteryCurrent,
                     double &BatteryTemperature, unsigned char &SID) {
  return ParseN2kPGN127508(N2kMsg, BatteryInstance, BatteryVoltage, BatteryCurrent, BatteryTemperature, SID);
}

/************************************************************************//**
 * \brief Setting up PGN 127513 Message "Battery Configuration Status"
 * \ingroup group_msgSetUp
 * 
 * Any device connected to a battery may transmit this message.
 * 
 * \param N2kMsg       Reference to a N2kMsg Object, 
 *                     Output: NMEA2000 message ready to be send.
 * \param BatInstance          BatteryInstance
 * \param BatType              Type of battery. See \ref tN2kBatType
 * \param SupportsEqual        Supports equalization. See \ref tN2kBatEqSupport
 * \param BatNominalVoltage    Battery nominal voltage. See \ref tN2kBatNomVolt
 * \param BatChemistry         Battery Chemistry See \ref tN2kBatChem
 * \param BatCapacity          Battery capacity in Coulombs. Use AhToCoulombs,
 *                             if you have your value in Ah.
 * \param BatTemperatureCoefficient Battery temperature coefficient in %
 * \param PeukertExponent      Peukert Exponent
 * \param ChargeEfficiencyFactor      Charge efficiency factor
 * 
 */
void SetN2kPGN127513(tN2kMsg &N2kMsg, unsigned char BatInstance, tN2kBatType BatType, tN2kBatEqSupport SupportsEqual,
                     tN2kBatNomVolt BatNominalVoltage, tN2kBatChem BatChemistry, double BatCapacity, int8_t BatTemperatureCoefficient,
				double PeukertExponent, int8_t ChargeEfficiencyFactor);

/************************************************************************//**
 * \brief Setting up Message "Battery Configuration Status" - PGN 127513
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 127513. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN127513
  */
inline void SetN2kBatConf(tN2kMsg &N2kMsg, unsigned char BatInstance, tN2kBatType BatType, tN2kBatEqSupport SupportsEqual,
                     tN2kBatNomVolt BatNominalVoltage, tN2kBatChem BatChemistry, double BatCapacity, int8_t BatTemperatureCoefficient,
				double PeukertExponent, int8_t ChargeEfficiencyFactor) {
   SetN2kPGN127513(N2kMsg,BatInstance,BatType,SupportsEqual,BatNominalVoltage,BatChemistry,BatCapacity,BatTemperatureCoefficient,
				PeukertExponent,ChargeEfficiencyFactor);
}
/************************************************************************//**
 * \brief Parsing the content of message PGN 127513 "Battery 
 * Configuration Status"
 * \ingroup group_msgParsers
 * 
 * Any device connected to a battery may transmit this message.
 * 
 * \param N2kMsg       Reference to a N2kMsg Object, 
 *                     Output: NMEA2000 message ready to be send.
 * \param BatInstance          BatteryInstance
 * \param BatType              Type of battery. See \ref tN2kBatType
 * \param SupportsEqual        Supports equalization. See \ref tN2kBatEqSupport
 * \param BatNominalVoltage    Battery nominal voltage. See \ref tN2kBatNomVolt
 * \param BatChemistry         Battery Chemistry See \ref tN2kBatChem
 * \param BatCapacity          Battery capacity in Coulombs. Use AhToCoulombs,
 *                             if you have your value in Ah.
 * \param BatTemperatureCoefficient Battery temperature coefficient in %
 * \param PeukertExponent      Peukert Exponent
 * \param ChargeEfficiencyFactor      Charge efficiency factor
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN127513(const tN2kMsg &N2kMsg, unsigned char &BatInstance, tN2kBatType &BatType, tN2kBatEqSupport &SupportsEqual,
                     tN2kBatNomVolt &BatNominalVoltage, tN2kBatChem &BatChemistry, double &BatCapacity, int8_t &BatTemperatureCoefficient,
				double &PeukertExponent, int8_t &ChargeEfficiencyFactor);

/************************************************************************//**
 * \brief Parsing the content of a "Battery Configuration Status" 
 *        message - PGN 127513
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 127513. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN127513 
 */
inline bool ParseN2kBatConf(const tN2kMsg &N2kMsg, unsigned char &BatInstance, tN2kBatType &BatType, tN2kBatEqSupport &SupportsEqual,
                     tN2kBatNomVolt &BatNominalVoltage, tN2kBatChem &BatChemistry, double &BatCapacity, int8_t &BatTemperatureCoefficient,
				double &PeukertExponent, int8_t &ChargeEfficiencyFactor) {
	return ParseN2kPGN127513(N2kMsg,BatInstance,BatType,SupportsEqual,BatNominalVoltage,BatChemistry,BatCapacity,BatTemperatureCoefficient,
				PeukertExponent,ChargeEfficiencyFactor);
}

/************************************************************************//**
 * \brief Setting up PGN 128000 Message "Nautical Leeway Angle"
 * \ingroup group_msgSetUp
 * 
 * This PGN provides the Nautical Leeway Angle, which is defined as the
 * angle between the vessel’s heading (direction to which the vessel’s
 * bow points) and its course (direction of its motion (track) through 
 * the water). The Nautical Leeway Angle is water referenced and
 * does not account for slip due to current.
 * This is commonly provided by dual-axis speed sensors. Dual axis
 * speed sensors are able to measure accurately the ship's speed in 
 * a longitudinal direction and a transverse direction. By measuring
 * both speed components (i.e. the velocity vector), the Nautical Leeway
 * Angle can be determined, and this angle can be used to optimize the
 * vessel's course.
 * 
 * \note This Nautical Leeway Angle is used primarily in the sailing
 * segment of the maritime industry and may differ from oceanographic
 * or scientific definitions of Leeway.
 * 
 * The Sequence ID field (SID) is used to link this PGN to other
 * related PGN’s from the same source address. When no linkage exists,
 * the value of the SID shall be set to 255
 * 
 * [NMEA2000 document](https://web.archive.org/web/20170609074636/https://www.nmea.org/Assets/20170204%20nmea%202000%20leeway%20pgn%20final.pdf)
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param Leeway      Positive angles indicate slippage to starboard, that
 *                    is, the vessel is tracking to the right of its heading,
 *                    and negative angles indicate slippage to port
 */
void SetN2kPGN128000(tN2kMsg &N2kMsg, unsigned char SID, double Leeway);

/************************************************************************//**
 * \brief Setting up Message "Nautical Leeway Angle" - PGN 128000
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 128000. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN128000
  */
inline void SetN2kLeeway(tN2kMsg &N2kMsg, unsigned char SID, double Leeway) {
  SetN2kPGN128000(N2kMsg,SID,Leeway);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 128000 "Nautical Leeway Angle"
 * \ingroup group_msgParsers
 * 
 * This PGN provides the Nautical Leeway Angle, which is defined as the
 * angle between the vessel’s heading (direction to which the vessel’s
 * bow points) and its course (direction of its motion (track) through 
 * the water). The Nautical Leeway Angle is water referenced and
 * does not account for slip due to current.
 * This is commonly provided by dual-axis speed sensors. Dual axis
 * speed sensors are able to measure accurately the ship's speed in 
 * a longitudinal direction and a transverse direction. By measuring
 * both speed components (i.e. the velocity vector), the Nautical Leeway
 * Angle can be determined, and this angle can be used to optimize the
 * vessel's course.
 * 
 * \note This Nautical Leeway Angle is used primarily in the sailing
 * segment of the maritime industry and may differ from oceanographic
 * or scientific definitions of Leeway.
 * 
 * The Sequence ID field (SID) is used to link this PGN to other
 * related PGN’s from the same source address. When no linkage exists,
 * the value of the SID shall be set to 255
 * 
 * [NMEA2000 document](https://www.nmea.org/Assets/20170204%20nmea%202000%20leeway%20pgn%20final.pdf)
 * \param N2kMsg    Reference to a N2kMsg Object, 
 *                  Output: NMEA2000 message ready to be send.
 * \param SID       Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Leeway    Positive angles indicate slippage to starboard, that
 *                  is, the vessel is tracking to the right of its heading,
 *                  and negative angles indicate slippage to port
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN128000(const tN2kMsg &N2kMsg, unsigned char &SID, double &Leeway);

/************************************************************************//**
 * \brief Parsing the content of a "Nautical Leeway Angle" 
 *        message - PGN 128000
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 128000. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN128000 
 */
inline bool ParseN2kLeeway(const tN2kMsg &N2kMsg, unsigned char &SID, double &Leeway) {
  return ParseN2kPGN128000(N2kMsg, SID, Leeway);
}

/************************************************************************//**
 * \brief Setting up PGN 128259 Message "Boat Speed, Water Referenced"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides a single transmission that describes the
 * motion of a vessel.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WaterReferenced   Speed over water in m/s
 * \param GroundReferenced  Speed over Ground in m/s
 * \param SWRT        Type of transducer. See \ref tN2kSpeedWaterReferenceType
 * 
 * \todo  There is an additional 4bit field "speed direction" in this PGN
 *         this could be added for the future.
 */
void SetN2kPGN128259(tN2kMsg &N2kMsg, unsigned char SID, double WaterReferenced, double GroundReferenced=N2kDoubleNA, tN2kSpeedWaterReferenceType SWRT=N2kSWRT_Paddle_wheel);

/************************************************************************//**
 * \brief Setting up Message "Boat Speed, Water Referenced" - PGN 128259
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 128259. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN128259
  */
inline void SetN2kBoatSpeed(tN2kMsg &N2kMsg, unsigned char SID, double WaterReferenced, double GroundReferenced=N2kDoubleNA, tN2kSpeedWaterReferenceType SWRT=N2kSWRT_Paddle_wheel) {
  SetN2kPGN128259(N2kMsg,SID,WaterReferenced,GroundReferenced,SWRT);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 128259 "Boat Speed, Water
 *        Referenced"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides a single transmission that describes the
 * motion of a vessel.
 * 
 * \param N2kMsg    Reference to a N2kMsg Object, 
 *                  Output: NMEA2000 message ready to be send.
 * \param SID       Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WaterReferenced   Speed over water in m/s
 * \param GroundReferenced  Speed over Ground in m/s
 * \param SWRT      Type of transducer. See \ref tN2kSpeedWaterReferenceType
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN128259(const tN2kMsg &N2kMsg, unsigned char &SID, double &WaterReferenced, double &GroundReferenced, tN2kSpeedWaterReferenceType &SWRT);

/************************************************************************//**
 * \brief Parsing the content of a "Boat Speed, Water Referenced" 
 *        message - PGN 128259
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 128259. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN128259 
 */
inline bool ParseN2kBoatSpeed(const tN2kMsg &N2kMsg, unsigned char &SID, double &WaterReferenced, double &GroundReferenced, tN2kSpeedWaterReferenceType &SWRT) {
  return ParseN2kPGN128259(N2kMsg, SID, WaterReferenced, GroundReferenced, SWRT);
}

/************************************************************************//**
 * \brief Setting up PGN 128267 Message "Water depth"
 * \ingroup group_msgSetUp
 * 
 * Water depth relative to the transducer and offset of the measuring 
 * transducer. Positive offset numbers provide the distance from the
 * transducer to the waterline.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param DepthBelowTransducer  Water depth below transducer in meters
 * \param Offset      Distance in meters between transducer and water surface
 *                    (positive) or transducer and keel (negative)
 * \param Range       maximum depth range tha can be measured
 * 
 */
void SetN2kPGN128267(tN2kMsg &N2kMsg, unsigned char SID, double DepthBelowTransducer, double Offset, double Range=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Water depth" - PGN 128267
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 128267. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN128267
  */
inline void SetN2kWaterDepth(tN2kMsg &N2kMsg, unsigned char SID, double DepthBelowTransducer, double Offset, double Range=N2kDoubleNA) {
  SetN2kPGN128267(N2kMsg,SID,DepthBelowTransducer,Offset,Range);
}

/************************************************************************//**
 * \brief Parsing the content of message  PGN 128267 "Water depth"
 * \ingroup group_msgParsers
 * 
 * Water depth relative to the transducer and offset of the measuring 
 * transducer. Positive offset numbers provide the distance from the
 * transducer to the waterline.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param DepthBelowTransducer  Water depth below transducer in meters
 * \param Offset      Distance in meters between transducer and water surface
 *                    (positive) or transducer and keel (negative)
 * \param Range       maximum depth range tha can be measured
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN128267(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset, double &Range);

/************************************************************************//**
 * \brief Parsing the content of a "Water depth" 
 *        message - PGN 128267
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 128267. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN128267 
 */
inline bool ParseN2kWaterDepth(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset) {
  double Range;
  return ParseN2kPGN128267(N2kMsg, SID, DepthBelowTransducer, Offset, Range);
}

/************************************************************************//**
 * \brief Parsing the content of a "Water depth" 
 *        message - PGN 128267
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 128267. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN128267 
 */
inline bool ParseN2kWaterDepth(const tN2kMsg &N2kMsg, unsigned char &SID, double &DepthBelowTransducer, double &Offset, double &Range) {
  return ParseN2kPGN128267(N2kMsg, SID, DepthBelowTransducer, Offset, Range);
}


/************************************************************************//**
 * \brief Setting up PGN 128275 Message "Distance log"
 * \ingroup group_msgSetUp
 * 
 * This PGN provides the cumulative voyage distance traveled since the last
 * reset. The distance is tagged with the time and date of
 * the distance measurement.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param DaysSince1970         Timestamp
 * \param SecondsSinceMidnight  Timestamp
 * \param Log                   This field indicates the total distance 
 *                              traveled through the water since the device was 
 *                              installed in units of 1 m.
 * \param TripLog               This field indicates the total distance 
 *                              traveled through the water since last reset 
 *                              in units of 1 m.
 */
void SetN2kPGN128275(tN2kMsg &N2kMsg, uint16_t DaysSince1970, double SecondsSinceMidnight, uint32_t Log, uint32_t TripLog);

/************************************************************************//**
 * \brief Setting up Message "Distance log" - PGN 128275
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 128275. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN128275
  */
inline void SetN2kDistanceLog(tN2kMsg &N2kMsg, uint16_t DaysSince1970, double SecondsSinceMidnight, uint32_t Log, uint32_t TripLog) {
  SetN2kPGN128275(N2kMsg,DaysSince1970,SecondsSinceMidnight,Log,TripLog);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 128275 "Distance log"
 * \ingroup group_msgParsers
 * 
 * This PGN provides the cumulative voyage distance traveled since the last
 * reset. The distance is tagged with the time and date of
 * the distance measurement.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param DaysSince1970         Timestamp
 * \param SecondsSinceMidnight  Timestamp
 * \param Log                   This field indicates the total distance 
 *                              traveled through the water since the device was 
 *                              installed in units of 1 m.
 * \param TripLog               This field indicates the total distance 
 *                              traveled through the water since last reset 
 *                              in units of 1 m.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN128275(const tN2kMsg &N2kMsg, uint16_t &DaysSince1970, double &SecondsSinceMidnight, uint32_t &Log, uint32_t &TripLog);

/************************************************************************//**
 * \brief Parsing the content of a "Distance log" 
 *        message - PGN 128275
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 128275. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN128275 
 */
inline bool ParseN2kDistanceLog(const tN2kMsg &N2kMsg, uint16_t &DaysSince1970, double &SecondsSinceMidnight, uint32_t &Log, uint32_t &TripLog) {
  return ParseN2kPGN128275(N2kMsg,DaysSince1970,SecondsSinceMidnight,Log,TripLog);
}

/************************************************************************//**
 * \brief Setting up PGN 128776 Message "Anchor Windlass Control Status"
 * \ingroup group_msgSetUp
 * 
 * This PGN is used to report status of anchor windlass controls and can be
 * used with Command Group Function (PGN 126208) to command the windlass and
 * anchoring equipment.
 * The Windlass Identifier Field (Field 2) specifies which windlass the PGN
 * message is intended for, and all following data fields refer only
 * to that windlass.
 * This PGN has several transmission rates dependent upon windlass operation:
 * Static is 5s, Dynamic is 500ms, Control is 250ms.
 * The message will be broadcasted periodically, but can also be requested as
 * required. The default static update rate is 5 seconds to
 * reduce bandwidth use when the windlass is not operating, this update rate
 * is used when all control commands are OFF.
 * 
 * \sa <https://www.nmea.org/Assets/20190613%20windlass%20amendment,%20128776,%20128777,%20128778.pdf>
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WindlassIdentifier          Unique Identifier of a function within a 
 *                                    Virtual Device: 0 - 255
 * \param WindlassDirectionControl    see \ref tN2kWindlassDirectionControl
 * \param SpeedControl                Single speed 0=off, 1-100=on; 
 *                                    Dual speed: 0=off, 1-49=slow, 50-100=fast;
 *                                    Proporional speed: 0=off, 1-100=speed
 * \param SpeedControlType (optional)           see \ref tN2kSpeedType
 * \param AnchorDockingControl (optional)       see \ref tN2kGenericStatusPair
 * \param PowerEnable (optional)                see \ref tN2kGenericStatusPair
 * \param MechanicalLock (optional)             see \ref tN2kGenericStatusPair
 * \param DeckAndAnchorWash (optional)          see \ref tN2kGenericStatusPair
 * \param AnchorLight (optional)                see \ref tN2kGenericStatusPair
 * \param CommandTimeout (optional)   range 0.25 ... 1.26sec -> If timeout 
 *                                    elapses the thruster stops operating and 
 *                                    reverts to static mode.
 * \param WindlassControlEvents (optional)  see \ref tN2kWindlassControlEvents 
 *                                          -> Events are cleared when the 
 *                                          condition is no longer true, or by
 *                                          manually resetting using the 
 *                                          Command Group Function (PGN 126208).
 * 
 */
void SetN2kPGN128776(
  tN2kMsg &N2kMsg,
  unsigned char SID,
  unsigned char WindlassIdentifier,
  tN2kWindlassDirectionControl WindlassDirectionControl,
  unsigned char SpeedControl,
  tN2kSpeedType SpeedControlType = N2kDD488_DataNotAvailable,
  tN2kGenericStatusPair AnchorDockingControl = N2kDD002_Unavailable,
  tN2kGenericStatusPair PowerEnable = N2kDD002_Unavailable,
  tN2kGenericStatusPair MechanicalLock = N2kDD002_Unavailable,
  tN2kGenericStatusPair DeckAndAnchorWash = N2kDD002_Unavailable,
  tN2kGenericStatusPair AnchorLight = N2kDD002_Unavailable,
  double CommandTimeout = 0.4,
  const tN2kWindlassControlEvents &WindlassControlEvents = tN2kWindlassControlEvents()
);

/************************************************************************//**
 * \brief Parsing the content of message PGN 128776 "Anchor Windlass
 *        Control Status"
 * \ingroup group_msgParsers
 * 
 * This PGN is used to report status of anchor windlass controls and can be
 * used with Command Group Function (PGN 126208) to command the windlass and
 * anchoring equipment.
 * The Windlass Identifier Field (Field 2) specifies which windlass the PGN
 * message is intended for, and all following data fields refer only
 * to that windlass.
 * This PGN has several transmission rates dependent upon windlass operation:
 * Static is 5s, Dynamic is 500ms, Control is 250ms.
 * The message will be broadcasted periodically, but can also be requested as
 * required. The default static update rate is 5 seconds to
 * reduce bandwidth use when the windlass is not operating, this update rate
 * is used when all control commands are OFF.
 * 
 * \sa <https://www.nmea.org/Assets/20190613%20windlass%20amendment,%20128776,%20128777,%20128778.pdf>
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WindlassIdentifier          Unique Identifier of a function within a 
 *                                    Virtual Device: 0 - 255
 * \param WindlassDirectionControl    see \ref tN2kWindlassDirectionControl
 * \param SpeedControl                Single speed 0=off, 1-100=on; 
 *                                    Dual speed: 0=off, 1-49=slow, 50-100=fast;
 *                                    Proporional speed: 0=off, 1-100=speed
 * \param SpeedControlType                      see \ref tN2kSpeedType
 * \param AnchorDockingControl                  see \ref tN2kGenericStatusPair
 * \param PowerEnable                           see \ref tN2kGenericStatusPair
 * \param MechanicalLock                        see \ref tN2kGenericStatusPair
 * \param DeckAndAnchorWash                     see \ref tN2kGenericStatusPair
 * \param AnchorLight                           see \ref tN2kGenericStatusPair
 * \param CommandTimeout              range 0.25 ... 1.26sec -> If timeout 
 *                                    elapses the thruster stops operating and 
 *                                    reverts to static mode.
 * \param WindlassControlEvents             see \ref tN2kWindlassControlEvents 
 *                                          -> Events are cleared when the 
 *                                          condition is no longer true, or by
 *                                          manually resetting using the 
 *                                          Command Group Function (PGN 126208).
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN128776(
  const tN2kMsg &N2kMsg,
  unsigned char &SID,
  unsigned char &WindlassIdentifier,
  tN2kWindlassDirectionControl &WindlassDirectionControl,
  unsigned char &SpeedControl,
  tN2kSpeedType &SpeedControlType,
  tN2kGenericStatusPair &AnchorDockingControl,
  tN2kGenericStatusPair &PowerEnable,
  tN2kGenericStatusPair &MechanicalLock,
  tN2kGenericStatusPair &DeckAndAnchorWash,
  tN2kGenericStatusPair &AnchorLight,
  double &CommandTimeout,
  tN2kWindlassControlEvents &WindlassControlEvents
);

/************************************************************************//**
 * \brief Setting up PGN 128777 Message "Anchor Windlass Operating Status"
 * \ingroup group_msgSetUp
 * 
 * This PGN is used to provide the operating status and data relevant to a
 * specific Anchor Windlass and can be used with Command Group Function (PGN
 * 126208) to command windlass and anchoring equipment.  
 * The Windlass Identifier (Field 2) specifies which windlass the PGN message
 * relates to and all following data fields refer only to that windlass.  
 * This PGN has several transmission rates dependent upon windlass operation:
 * Static is 5s, Dynamic is 500ms. The message will be broadcasted
 * periodically, but can also be requested as required. The default static
 * update rate is 5 seconds to reduce bandwidth use when the windlass is not
 * operating.  
 * The default dynamic update rate is 500ms to provide the status of activity
 * while the windlass is operating. This PGN can be used with PGN 128776 to
 * provide the windlass control status, and PGN 128778 to provide the
 * monitoring status. The Sequence ID can be used to link the three windlass
 * PGNs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WindlassIdentifier      Unique Identifier of a function within a 
 *                                Virtual Device: 0 - 255
 * \param RodeCounterValue        amount of rode deployed in meters
 * \param WindlassLineSpeed (optional)    Amount of rode deployed or retrieved 
 *                                        per minute.
 * \param WindlassMotionStatus (optional) Indicates current physical state of 
 *                                        windlass movement. see \ref 
 *                                        tN2kWindlassMotionStates
 * \param RodeTypeStatus (optional)  Indicates whether chain or rope 
 *                                   is currently being deployed or retrieved.
 *                                   see \ref tN2kRodeTypeStates
 * \param AnchorDockingStatus (optional)  Indicates current physical state of
 *                                        anchor docking. Used in automatic
 *                                        docking situations to report when the
 *                                        anchor has been fully docked.
 * \param WindlassOperatingEvents see \ref tN2kWindlassOperatingEvents
 * 
 */
void SetN2kPGN128777(
  tN2kMsg &N2kMsg,
  unsigned char SID,
  unsigned char WindlassIdentifier,
  double RodeCounterValue,
  double WindlassLineSpeed = N2kDoubleNA,
  tN2kWindlassMotionStates WindlassMotionStatus = N2kDD480_Unavailable,
  tN2kRodeTypeStates RodeTypeStatus = N2kDD481_Unavailable,
  tN2kDD482 AnchorDockingStatus = N2kDD482_DataNotAvailable,
  const tN2kWindlassOperatingEvents &WindlassOperatingEvents = tN2kWindlassOperatingEvents()
);

/************************************************************************//**
 * \brief Parsing the content of message PGN 128777 "Anchor Windlass
 *        Operating Status"
 * \ingroup group_msgParsers
 * 
 * This PGN is used to provide the operating status and data relevant to a
 * specific Anchor Windlass and can be used with Command Group Function (PGN
 * 126208) to command windlass and anchoring equipment.  
 * The Windlass Identifier (Field 2) specifies which windlass the PGN message
 * relates to and all following data fields refer only to that windlass.  
 * This PGN has several transmission rates dependent upon windlass operation:
 * Static is 5s, Dynamic is 500ms. The message will be broadcasted
 * periodically, but can also be requested as required. The default static
 * update rate is 5 seconds to reduce bandwidth use when the windlass is not
 * operating.  
 * The default dynamic update rate is 500ms to provide the status of activity
 * while the windlass is operating. This PGN can be used with PGN 128776 to
 * provide the windlass control status, and PGN 128778 to provide the
 * monitoring status. The Sequence ID can be used to link the three windlass
 * PGNs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WindlassIdentifier      Unique Identifier of a function within a 
 *                                Virtual Device: 0 - 255
 * \param RodeCounterValue        amount of rode deployed in meters
 * \param WindlassLineSpeed               Amount of rode deployed or retrieved 
 *                                        per minute.
 * \param WindlassMotionStatus            Indicates current physical state of 
 *                                        windlass movement. see \ref 
 *                                        tN2kWindlassMotionStates
 * \param RodeTypeStatus             Indicates whether chain or rope 
 *                                   is currently being deployed or retrieved.
 *                                   see \ref tN2kRodeTypeStates
 * \param AnchorDockingStatus             Indicates current physical state of
 *                                        anchor docking. Used in automatic
 *                                        docking situations to report when the
 *                                        anchor has been fully docked.
 * \param WindlassOperatingEvents see \ref tN2kWindlassOperatingEvents
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN128777(
  const tN2kMsg &N2kMsg,
  unsigned char &SID,
  unsigned char &WindlassIdentifier,
  double &RodeCounterValue,
  double &WindlassLineSpeed,
  tN2kWindlassMotionStates &WindlassMotionStatus,
  tN2kRodeTypeStates &RodeTypeStatus,
  tN2kAnchorDockingStates &AnchorDockingStatus,
  tN2kWindlassOperatingEvents &WindlassOperatingEvents
);

/************************************************************************//**
 * \brief Setting up PGN 128778Message "Anchor Windlass Monitoring Status"
 * \ingroup group_msgSetUp
 * 
 * This PGN is used to provide the monitoring status and data relevant to a
 * specific Anchor Windlass and can be used with Command Group Function (PGN
 * 126208) to command windlass and anchoring equipment.  
 * The Windlass Identifier Field (Field 2) specifies which windlass the PGN 
 * message relates to and all following data fields refer only to
 * that windlass.  
 * This PGN has several transmission rates dependent upon windlass operation: 
 * Static is 5s, Dynamic is 500ms. The message will be broadcasted 
 * periodically, but can also be requested as required. The default static 
 * update rate is 5 seconds to reduce bandwidth use when the windlass is not
 * operating.
 * The default dynamic update rate is 500ms to provide the status of activity
 * while the windlass is operating. This PGN can be used with PGN 128776 to
 * provide the windlass control status, and PGN 128777 to provide the operating
 * status. The Sequence ID can be used to link the three windlass PGNs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WindlassIdentifier      Unique Identifier of a function within a 
 *                                Virtual Device: 0 - 255
 * \param TotalMotorTime          Total time windlass has operated since last 
 *                                reset in seconds
 * \param ControllerVoltage (optional)  Supply voltage of electronic windlass
 *                                      controller in Volt
 * \param MotorCurrent  (optional)      Load current of electric windlass motor
 *                                      in Ampere
 * \param WindlassMonitoringEvents (optional)  Events are cleared when the 
 *                                      condition is no longer true, or by 
 *                                      manually resetting using the Command
 *                                      Group Function (PGN 126208).
 *                                      see \ref tN2kWindlassMonitoringEvents
 * 
 */
void SetN2kPGN128778(
  tN2kMsg &N2kMsg,
  unsigned char SID,
  unsigned char WindlassIdentifier,
  double TotalMotorTime,
  double ControllerVoltage = N2kDoubleNA,
  double MotorCurrent = N2kDoubleNA,
  const tN2kWindlassMonitoringEvents &WindlassMonitoringEvents = tN2kWindlassMonitoringEvents()
);

/************************************************************************//**
 * \brief Parsing the content of message PGN128778 "Anchor Windlass
 *        Monitoring Status"
 * \ingroup group_msgParsers
 * 
 * This PGN is used to provide the monitoring status and data relevant to a
 * specific Anchor Windlass and can be used with Command Group Function (PGN
 * 126208) to command windlass and anchoring equipment.  
 * The Windlass Identifier Field (Field 2) specifies which windlass the PGN 
 * message relates to and all following data fields refer only to
 * that windlass.  
 * This PGN has several transmission rates dependent upon windlass operation: 
 * Static is 5s, Dynamic is 500ms. The message will be broadcasted 
 * periodically, but can also be requested as required. The default static 
 * update rate is 5 seconds to reduce bandwidth use when the windlass is not
 * operating.
 * The default dynamic update rate is 500ms to provide the status of activity
 * while the windlass is operating. This PGN can be used with PGN 128776 to
 * provide the windlass control status, and PGN 128777 to provide the operating
 * status. The Sequence ID can be used to link the three windlass PGNs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WindlassIdentifier      Unique Identifier of a function within a 
 *                                Virtual Device: 0 - 255
 * \param TotalMotorTime          Total time windlass has operated since last 
 *                                reset in seconds
 * \param ControllerVoltage             Supply voltage of electronic windlass
 *                                      controller in Volt
 * \param MotorCurrent                  Load current of electric windlass motor
 *                                      in Ampere
 * \param WindlassMonitoringEvents      Events are cleared when the 
 *                                      condition is no longer true, or by 
 *                                      manually resetting using the Command
 *                                      Group Function (PGN 126208).
 *                                      see \ref tN2kWindlassMonitoringEvents
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *  
 */
bool ParseN2kPGN128778(
  const tN2kMsg &N2kMsg,
  unsigned char &SID,
  unsigned char &WindlassIdentifier,
  double &TotalMotorTime,
  double &ControllerVoltage,
  double &MotorCurrent,
  tN2kWindlassMonitoringEvents &WindlassMonitoringEvents
);

/************************************************************************//**
 * \brief Setting up PGN 129025 Message "Position, Rapid Update"
 * \ingroup group_msgSetUp
 * 
 * This PGN provides latitude and longitude referenced to WGS84. Being defined
 * as single frame message, as opposed to other PGNs that include latitude and
 * longitude and are defined as fast or multi-packet, this PGN lends itself to
 * being transmitted more frequently without using up excessive bandwidth on
 * the bus for the benefit of receiving equipment that may require rapid 
 * position updates.

 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param Latitude    Latitude in degrees
 * \param Longitude   Longitude in degrees
 * 
 */
void SetN2kPGN129025(tN2kMsg &N2kMsg, double Latitude, double Longitude);

/************************************************************************//**
 * \brief Setting up Message "Position, Rapid Update" - PGN 129025
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129025. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129025
  */
inline void SetN2kLatLonRapid(tN2kMsg &N2kMsg, double Latitude, double Longitude) {
  SetN2kPGN129025(N2kMsg,Latitude,Longitude);
}

/************************************************************************//**
 * \brief Parsing the content of PGN 129025 "Position, Rapid Update"
 * \ingroup group_msgParsers
 * 
 * This PGN provides latitude and longitude referenced to WGS84. Being defined
 * as single frame message, as opposed to other PGNs that include latitude and
 * longitude and are defined as fast or multi-packet, this PGN lends itself to
 * being transmitted more frequently without using up excessive bandwidth on
 * the bus for the benefit of receiving equipment that may require rapid 
 * position updates.

 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param Latitude    Latitude in degrees
 * \param Longitude   Longitude in degrees
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *   
 */
bool ParseN2kPGN129025(const tN2kMsg &N2kMsg, double &Latitude, double &Longitude);

/************************************************************************//**
 * \brief Parsing the content of a "Position, Rapid Update" 
 *        message - PGN 129025
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129025. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129025 
 */
inline bool ParseN2kPositionRapid(const tN2kMsg &N2kMsg, double &Latitude, double &Longitude) {
	return ParseN2kPGN129025(N2kMsg, Latitude, Longitude);
}

/************************************************************************//**
 * \brief Setting up PGN 129026 Message "COG SOG rapid update"
 * \ingroup group_msgSetUp
 * 
 * This parameter group is a single frame parameter group that provides Course
 * Over Ground (COG) and Speed Over Ground (SOG).
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param ref         COG reference, see \ref tN2kHeadingReference
 * \param COG         Course Over Ground in radians
 * \param SOG         Speed Over Ground in m/s
 * 
 */
void SetN2kPGN129026(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG);

/************************************************************************//**
 * \brief Setting up Message "COG SOG rapid update" - PGN 129026
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129026. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129026
  */
inline void SetN2kCOGSOGRapid(tN2kMsg &N2kMsg, unsigned char SID, tN2kHeadingReference ref, double COG, double SOG) {
  SetN2kPGN129026(N2kMsg,SID,ref,COG,SOG);
}

/************************************************************************//**
 * \brief Parsing the content of PGN 129026 "COG SOG rapid update"
 * \ingroup group_msgParsers
 * 
 * This parameter group is a single frame parameter group that provides Course
 * Over Ground (COG) and Speed Over Ground (SOG).
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param ref         COG reference, see \ref tN2kHeadingReference
 * \param COG         Course Over Ground in radians
 * \param SOG         Speed Over Ground in m/s
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *  
 */
bool ParseN2kPGN129026(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kHeadingReference &ref, double &COG, double &SOG);

/************************************************************************//**
 * \brief Parsing the content of a "COG SOG Rapid Update" 
 *        message - PGN 129026
 * 
 * Alias of PGN 129026. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129026 
 */
inline bool ParseN2kCOGSOGRapid(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kHeadingReference &ref, double &COG, double &SOG) {
  return ParseN2kPGN129026(N2kMsg,SID,ref,COG,SOG);
}

/************************************************************************//**
 * \brief Setting up PGN 129029 Message "GNSS Position Data"
 * \ingroup group_msgSetUp
 * 
 * This parameter group conveys a comprehensive set of Global Navigation 
 * Satellite System (GNSS) parameters, including position information.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param DaysSince1970         UTC Date in resolution of 1 day (The number of
 *                              days since January 1, 1970).
 * \param SecondsSinceMidnight  UTC time second since midnight
 * \param Latitude              Latitude in degrees ("-" = south, “+” = north)
 * \param Longitude             Longitude in degrees ("-" = west, “+” = east) 
 * \param Altitude              Altitude referenced to WGS-84 in meters
 * \param GNSStype              GNSS type. See \ref tN2kGNSStype
 * \param GNSSmethod            GNSS method type. See \ref tN2kGNSSmethod
 * \param nSatellites           Indicate the number of satellites used for data
 * \param HDOP                  Horizontal Dilution Of Precision in meters.
 * \param PDOP                  Probable dilution of precision in meters.
 * \param GeoidalSeparation     Geoidal separation in meters
 * \param nReferenceStations    Number of Reference Stations 
 * \param ReferenceStationType  Reference Station Type, see \ref N2kGNSSt_GPS
 * \param ReferenceSationID     Reference Station ID 
 * \param AgeOfCorrection       Age of DGNSS Corrections
 * 
 */
void SetN2kPGN129029(tN2kMsg &N2kMsg, unsigned char SID, uint16_t DaysSince1970, double SecondsSinceMidnight,
                     double Latitude, double Longitude, double Altitude,
                     tN2kGNSStype GNSStype, tN2kGNSSmethod GNSSmethod,
                     unsigned char nSatellites, double HDOP, double PDOP=0, double GeoidalSeparation=0,
                     unsigned char nReferenceStations=0, tN2kGNSStype ReferenceStationType=N2kGNSSt_GPS, uint16_t ReferenceSationID=0,
                     double AgeOfCorrection=0
                     );

/************************************************************************//**
 * \brief Setting up Message "GNSS Position Data" - PGN 129029
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129029. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129029
  */
inline void SetN2kGNSS(tN2kMsg &N2kMsg, unsigned char SID, uint16_t DaysSince1970, double SecondsSinceMidnight,
                     double Latitude, double Longitude, double Altitude,
                     tN2kGNSStype GNSStype, tN2kGNSSmethod GNSSmethod,
                     unsigned char nSatellites, double HDOP, double PDOP=0, double GeoidalSeparation=0,
                     unsigned char nReferenceStations=0, tN2kGNSStype ReferenceStationType=N2kGNSSt_GPS, uint16_t ReferenceSationID=0,
                     double AgeOfCorrection=0
                     ) {
  SetN2kPGN129029(N2kMsg,SID,DaysSince1970,SecondsSinceMidnight,
                  Latitude,Longitude,Altitude,
                  GNSStype,GNSSmethod,
                  nSatellites,HDOP,PDOP,GeoidalSeparation,
                  nReferenceStations,ReferenceStationType,ReferenceSationID,
                  AgeOfCorrection);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129029 "GNSS Position Data"
 * \ingroup group_msgParsers
 * 
 * This parameter group conveys a comprehensive set of Global Navigation 
 * Satellite System (GNSS) parameters, including position information.
 * 
 * The parameters passed to ReferenceStationType, ReferenceStationID and
 * AgeOfCorrection are set to N2kGNSSt_GPS, N2kInt16NA and N2kDoubleNA
 * respectively, when there are no reference stations present in the message.
 *
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param DaysSince1970         UTC Date in resolution of 1 day (The number of
 *                              days since January 1, 1970).
 * \param SecondsSinceMidnight  UTC time second since midnight
 * \param Latitude              Latitude in degrees ("-" = south, “+” = north)
 * \param Longitude             Longitude in degrees ("-" = west, “+” = east) 
 * \param Altitude              Altitude referenced to WGS-84 in meters
 * \param GNSStype              GNSS type. See \ref tN2kGNSStype
 * \param GNSSmethod            GNSS method type. See \ref tN2kGNSSmethod
 * \param nSatellites           Indicate the number of satellites used for data
 * \param HDOP                  Horizontal Dilution Of Precision in meters.
 * \param PDOP                  Probable dilution of precision in meters.
 * \param GeoidalSeparation     Geoidal separation in meters
 * \param nReferenceStations    Number of Reference Stations 
 * \param ReferenceStationType  Reference Station Type, see \ref N2kGNSSt_GPS
 * \param ReferenceSationID     Reference Station ID 
 * \param AgeOfCorrection       Age of DGNSS Corrections
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129029(const tN2kMsg &N2kMsg, unsigned char &SID, uint16_t &DaysSince1970, double &SecondsSinceMidnight,
                     double &Latitude, double &Longitude, double &Altitude,
                     tN2kGNSStype &GNSStype, tN2kGNSSmethod &GNSSmethod,
                     unsigned char &nSatellites, double &HDOP, double &PDOP, double &GeoidalSeparation,
                     unsigned char &nReferenceStations, tN2kGNSStype &ReferenceStationType, uint16_t &ReferenceSationID,
                     double &AgeOfCorrection
                     );

/************************************************************************//**
 * \brief Parsing the content of a "GNSS Position Data" 
 *        message - PGN 129029
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129029. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129029 
 */
inline bool ParseN2kGNSS(const tN2kMsg &N2kMsg, unsigned char &SID, uint16_t &DaysSince1970, double &SecondsSinceMidnight,
                     double &Latitude, double &Longitude, double &Altitude,
                     tN2kGNSStype &GNSStype, tN2kGNSSmethod &GNSSmethod,
                     unsigned char &nSatellites, double &HDOP, double &PDOP, double &GeoidalSeparation,
                     unsigned char &nReferenceStations, tN2kGNSStype &ReferenceStationType, uint16_t &ReferenceSationID,
                     double &AgeOfCorrection
                     ) {
  return ParseN2kPGN129029(N2kMsg, SID, DaysSince1970, SecondsSinceMidnight,
                     Latitude, Longitude, Altitude,
                     GNSStype, GNSSmethod,
                     nSatellites, HDOP, PDOP, GeoidalSeparation,
                     nReferenceStations, ReferenceStationType, ReferenceSationID,
                     AgeOfCorrection
                     );
}

/************************************************************************//**
 * \brief Setting up PGN 129033 Message "Date,Time & Local offset"
 * \ingroup group_msgSetUp
 * 
 * This parameter group has a single transmission that provides: UTC time, UTC 
 * date and Local Offset Datum Local geodetic datum and datum offsets from a
 * reference datum. 
 * 
 * \sa SetN2kPGN126992 , ParseN2kPGN126992
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param DaysSince1970         UTC Date in resolution of 1 day (The number of
 *                              days since January 1, 1970).
 * \param SecondsSinceMidnight  Seconds since midnight
 * \param LocalOffset           Local offset in minutes
 * 
 */
void SetN2kPGN129033(tN2kMsg &N2kMsg, uint16_t DaysSince1970, double SecondsSinceMidnight, int16_t LocalOffset);

/************************************************************************//**
 * \brief Setting up Message "Date,Time & Local offset" - PGN 129033
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129033. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129033
  */
inline void SetN2kLocalOffset(tN2kMsg &N2kMsg, uint16_t DaysSince1970, double SecondsSinceMidnight, int16_t LocalOffset) {
  SetN2kPGN129033(N2kMsg,DaysSince1970,SecondsSinceMidnight,LocalOffset);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129033 "Date,Time & Local offset"
 * \ingroup group_msgParsers
 * 
 * This parameter group has a single transmission that provides: UTC time, UTC 
 * date and Local Offset Datum Local geodetic datum and datum offsets from a
 * reference datum. 
 * 
 * \sa SetN2kPGN126992 , ParseN2kPGN126992
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param DaysSince1970         UTC Date in resolution of 1 day (The number of
 *                              days since January 1, 1970).
 * \param SecondsSinceMidnight  Seconds since midnight
 * \param LocalOffset           Local offset in minutes
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN129033(const tN2kMsg &N2kMsg, uint16_t &DaysSince1970, double &SecondsSinceMidnight, int16_t &LocalOffset);

/************************************************************************//**
 * \brief Parsing the content of a "Date,Time & Local offset" 
 *        message - PGN 129033
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129033. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129033 
 */
inline bool ParseN2kLocalOffset(const tN2kMsg &N2kMsg, uint16_t &DaysSince1970, double &SecondsSinceMidnight, int16_t &LocalOffset) {
  return ParseN2kPGN129033(N2kMsg,DaysSince1970,SecondsSinceMidnight,LocalOffset);
}

/************************************************************************//**
 * \brief Setting up PGN 129038 Message "AIS position reports for Class A"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides data associated with the ITU-R M.1371 Messages
 * 1, 2, and 3 Position Reports, autonomous, assigned, and response to
 * interrogation, respectively. An AIS device may generate this parameter group
 * either upon receiving a VHF data link message 1,2 or 3, or upon receipt of
 * an ISO or NMEA request PGN (see ITU-R M.1371-1 for additional information).
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param MessageID   Message Type ID according to e ITU-R M.1371 
 * \param Repeat      Repeat indicator, Used by the repeater to indicate how
 *                    many times a message has been repeated. Refer to
 *                    § 4.6.1, Annex 2; 0-3; 0 = default; 3 = do not repeat
 *                    any more
 * \param UserID      MMSI number
 * \param Latitude    latitude of position degree
 * \param Longitude   longitude of position degree
 * \param Accuracy {bool}   position accuracy 
 *                    - 0 = default
 *                    - 1 = high (≤10 m)
 *                    - 0 = low (>10 m)
 * \param RAIM  {bool}Receiver autonomous integrity monitoring (RAIM) flag of
 *                    electronic position fixing device
 *                    - 0 = RAIM not in use = default
 *                    - 1 = RAIM in use
 * \param Seconds     UTC second when the report was generated by the EPFS
 *                    (0-59 or 60) if time stamp is not available, which should
 *                    also be the default value or 61 if positioning system is
 *                    in manual input mode or 62 if electronic position fixing
 *                    system operates in estimated (dead reckoning) mode, or 63
 *                    if the positioning system is inoperative)
 * \param COG         Course over ground
 * \param SOG         Speed over ground
 * \param AISTransceiverInformation  see \ref tN2kAISTransceiverInformation
 * \param Heading     heading
 * \param ROT         Rate of turn
 * \param NavStatus   Navigational status, see \ref tN2kAISNavStatus
 * 
 */
void SetN2kPGN129038(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID, 
                        double Latitude, double Longitude, bool Accuracy, bool RAIM, 
                        uint8_t Seconds, double COG, double SOG, tN2kAISTransceiverInformation AISTransceiverInformation, 
                        double Heading, double ROT, tN2kAISNavStatus NavStatus);
/************************************************************************//**
 * \brief Setting up Message "AIS position reports for Class A" - PGN 129038
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129038. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129038
  */
inline void SetN2kAISClassAPosition(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID, 
                        double Latitude, double Longitude, bool Accuracy, bool RAIM, 
                        uint8_t Seconds, double COG, double SOG, tN2kAISTransceiverInformation AISTransceiverInformation,  
                        double Heading, double ROT, tN2kAISNavStatus NavStatus) {
  SetN2kPGN129038(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, 
                  Accuracy, RAIM, Seconds, COG, SOG, AISTransceiverInformation, Heading, ROT, NavStatus);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129038 "AIS position reports 
 *        for Class A"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides data associated with the ITU-R M.1371 Messages
 * 1, 2, and 3 Position Reports, autonomous, assigned, and response to
 * interrogation, respectively. An AIS device may generate this parameter group
 * either upon receiving a VHF data link message 1,2 or 3, or upon receipt of
 * an ISO or NMEA request PGN (see ITU-R M.1371-1 for additional information).
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param MessageID   Message Type ID according to e ITU-R M.1371 
 * \param Repeat      Repeat indicator, Used by the repeater to indicate how
 *                    many times a message has been repeated. Refer to
 *                    § 4.6.1, Annex 2; 0-3; 0 = default; 3 = do not repeat
 *                    any more
 * \param UserID      MMSI number
 * \param Latitude    latitude of position degree
 * \param Longitude   longitude of position degree
 * \param Accuracy {bool}   position accuracy 
 *                    - 0 = default
 *                    - 1 = high (≤10 m)
 *                    - 0 = low (>10 m)
 * \param RAIM  {bool}Receiver autonomous integrity monitoring (RAIM) flag of
 *                    electronic position fixing device
 *                    - 0 = RAIM not in use = default
 *                    - 1 = RAIM in use
 * \param Seconds     UTC second when the report was generated by the EPFS
 *                    (0-59 or 60) if time stamp is not available, which should
 *                    also be the default value or 61 if positioning system is
 *                    in manual input mode or 62 if electronic position fixing
 *                    system operates in estimated (dead reckoning) mode, or 63
 *                    if the positioning system is inoperative)
 * \param COG         Course over ground
 * \param SOG         Speed over ground
 * \param Heading     heading
 * \param ROT         Rate of turn
 * \param NavStatus   Navigational status, see \ref tN2kAISNavStatus
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129038(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID, double &Latitude, double &Longitude,
                        bool &Accuracy, bool &RAIM, uint8_t &Seconds, double &COG, double &SOG, double &Heading, double &ROT, tN2kAISNavStatus &NavStatus);

/************************************************************************//**
 * \brief Parsing the content of a "AIS position reports for Class A" 
 *        message - PGN 129038
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129038. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129038 
 */
inline bool ParseN2kAISClassAPosition(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID, double &Latitude, double &Longitude,
                        bool &Accuracy, bool &RAIM, uint8_t &Seconds, double &COG, double &SOG, double &Heading, double &ROT, tN2kAISNavStatus & NavStatus) {
  return ParseN2kPGN129038(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, Accuracy, RAIM, Seconds, COG, SOG, Heading, ROT, NavStatus);
}

/************************************************************************//**
 * \brief Setting up PGN 129039 Message "AIS position reports for Class B"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides data associated with the ITU-R M.1371 
 * Message18 Standard Class B Equipment Position Report. An AIS device may
 * generate this parameter group either upon receiving a VHF data link
 * message 18, or upon receipt of an ISO or NMEA request PGN (see 
 * ITU-R M.1371-1 for additional information).
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param MessageID   Message Type ID according to e ITU-R M.1371 
 * \param Repeat      Repeat indicator, Used by the repeater to indicate how
 *                    many times a message has been repeated. Refer to
 *                    § 4.6.1, Annex 2; 0-3; 0 = default; 3 = do not repeat
 *                    any more
 * \param UserID      MMSI number
 * \param Latitude    latitude of position degree
 * \param Longitude   longitude of position degree
 * \param Accuracy {bool}   position accuracy 
 *                    - 0 = default
 *                    - 1 = high (≤10 m)
 *                    - 0 = low (>10 m)
 * \param RAIM  {bool}Receiver autonomous integrity monitoring (RAIM) flag of
 *                    electronic position fixing device
 *                    - 0 = RAIM not in use = default
 *                    - 1 = RAIM in use
 * \param Seconds     TimeStamp UTC second when the report was generated by the
 *                    EPFS (0-59 or 60) if time stamp is not available, which
 *                    should also be the default value or 61 if positioning
 *                    system is in manual input mode or 62 if electronic
 *                    position fixing system operates in estimated (dead
 *                    reckoning) mode, or 63 if the positioning system is 
 *                    inoperative)
 * \param COG         Course over ground
 * \param SOG         Speed over ground
 * \param AISTransceiverInformation  AIS Transceiver Information, see 
 *                                   \ref tN2kAISTransceiverInformation
 * \param Heading     Heading
 * \param Unit        Class B unit flag, see \ref tN2kAISUnit
 * \param Display     Class B display flag, 
 *                    - 0 = No display available; not capable of displaying
 *                      AIS Message 12 and 14
 *                    - 1 = Equipped with integrated display 
 *                      displaying AIS Message 12 and 14
 * \param DSC         Class B DSC flag
 *                    - 0 = Not equipped with DSC function
 *                    - 1 = Equipped with DSC function (dedicated
 *                      or time-shared)
 * \param Band        Class B band flag
 *                    - 0 = Capable of operating over the upper 525 kHz band
 *                      of the marine band
 *                    - 1 = Capable of operating over the whole marine band
 *                      (irrelevant if “Class B Message 22 flag” is 0)
 * \param Msg22       Class B Message22 flag
 *                    - 0 = No frequency management via Message 22 , operating
 *                      on AIS1, AIS2 only
 *                    - 1 = Frequency management via Message 22
 * \param Mode        Station Operating Mode flag, see \ref tN2kAISMode
 * \param State       Communication State Selector Flag
 *                    - 0 = SOTDMA communication state follows
 *                    - 1 = ITDMA communication state follows
 *                    (always “1” for Class-B “CS”)
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * 
 */
void SetN2kPGN129039(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                        double Latitude, double Longitude, bool Accuracy, bool RAIM,
                        uint8_t Seconds, double COG, double SOG, tN2kAISTransceiverInformation AISTransceiverInformation,
                        double Heading, tN2kAISUnit Unit, bool Display, bool DSC, bool Band, bool Msg22, tN2kAISMode Mode,
                        bool State, unsigned char SID=0xff);

/************************************************************************//**
 * \brief Setting up Message "AIS position reports for Class B" - PGN 129039
 * (Latest inline version)
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129039. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129039
  */
inline void SetN2kAISClassBPosition(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                        double Latitude, double Longitude, bool Accuracy, bool RAIM,
                        uint8_t Seconds, double COG, double SOG, tN2kAISTransceiverInformation AISTransceiverInformation,
                        double Heading, tN2kAISUnit Unit, bool Display, bool DSC, bool Band, bool Msg22, tN2kAISMode Mode,
                        bool State, unsigned char SID=0xff) {
  SetN2kPGN129039(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, Accuracy, RAIM, Seconds,
                    COG, SOG, AISTransceiverInformation, Heading, Unit, Display, DSC, Band, Msg22, Mode, State, SID);
}

/************************************************************************//**
 * \brief Setting up Message "AIS position reports for Class B" - PGN 129039 
 * [DEPRECATED]
 * \ingroup group_msgSetUp
 * 
 * \note PPrevious inline version for backwards compatibility, using 
 * N2kaischannel_A_VDL_reception(0) as default value
 * 
 * Alias of PGN 129039. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129039
  */
inline void SetN2kAISClassBPosition(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                        double Latitude, double Longitude, bool Accuracy, bool RAIM,
                        uint8_t Seconds, double COG, double SOG, double Heading, tN2kAISUnit Unit,
                        bool Display, bool DSC, bool Band, bool Msg22, tN2kAISMode Mode, bool State) {
  SetN2kPGN129039(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, Accuracy, RAIM, Seconds,
                    COG, SOG, N2kaischannel_A_VDL_reception, Heading, Unit, Display, DSC, Band, Msg22, Mode, State);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129039 "AIS position reports
 *        for Class B"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides data associated with the ITU-R M.1371 
 * Message18 Standard Class B Equipment Position Report. An AIS device may
 * generate this parameter group either upon receiving a VHF data link
 * message 18, or upon receipt of an ISO or NMEA request PGN (see 
 * ITU-R M.1371-1 for additional information).
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param MessageID   Message Type ID according to e ITU-R M.1371 
 * \param Repeat      Repeat indicator, Used by the repeater to indicate how
 *                    many times a message has been repeated. Refer to
 *                    § 4.6.1, Annex 2; 0-3; 0 = default; 3 = do not repeat
 *                    any more
 * \param UserID      MMSI number
 * \param Latitude    latitude of position degree
 * \param Longitude   longitude of position degree
 * \param Accuracy {bool}   position accuracy 
 *                    - 0 = default
 *                    - 1 = high (≤10 m)
 *                    - 0 = low (>10 m)
 * \param RAIM  {bool}Receiver autonomous integrity monitoring (RAIM) flag of
 *                    electronic position fixing device
 *                    - 0 = RAIM not in use = default
 *                    - 1 = RAIM in use
 * \param Seconds     TimeStamp UTC second when the report was generated by the
 *                    EPFS (0-59 or 60) if time stamp is not available, which
 *                    should also be the default value or 61 if positioning
 *                    system is in manual input mode or 62 if electronic
 *                    position fixing system operates in estimated (dead
 *                    reckoning) mode, or 63 if the positioning system is 
 *                    inoperative)
 * \param COG         Course over ground
 * \param SOG         Speed over ground
 * \param AISTransceiverInformation  AIS Transceiver Information, see 
 *                                   \ref tN2kAISTransceiverInformation
 * \param Heading     Heading
 * \param Unit        Class B unit flag, see \ref tN2kAISUnit
 * \param Display     Class B display flag, 
 *                    - 0 = No display available; not capable of displaying
 *                      AIS Message 12 and 14
 *                    - 1 = Equipped with integrated display 
 *                      displaying AIS Message 12 and 14
 * \param DSC         Class B DSC flag
 *                    - 0 = Not equipped with DSC function
 *                    - 1 = Equipped with DSC function (dedicated
 *                      or time-shared)
 * \param Band        Class B band flag
 *                    - 0 = Capable of operating over the upper 525 kHz band
 *                      of the marine band
 *                    - 1 = Capable of operating over the whole marine band
 *                      (irrelevant if “Class B Message 22 flag” is 0)
 * \param Msg22       Class B Message22 flag
 *                    - 0 = No frequency management via Message 22 , operating
 *                      on AIS1, AIS2 only
 *                    - 1 = Frequency management via Message 22
 * \param Mode        Station Operating Mode flag, see \ref tN2kAISMode
 * \param State       Communication State Selector Flag
 *                    - 0 = SOTDMA communication state follows
 *                    - 1 = ITDMA communication state follows
 *                    (always “1” for Class-B “CS”)
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129039(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                        double &Latitude, double &Longitude, bool &Accuracy, bool &RAIM, uint8_t &Seconds, double &COG,
                        double &SOG, tN2kAISTransceiverInformation &AISTransceiverInformation, double &Heading,
                        tN2kAISUnit &Unit, bool &Display, bool &DSC, bool &Band, bool &Msg22, tN2kAISMode &Mode, bool &State);

/************************************************************************//**
 * \brief Parsing the content of a "AIS position reports for Class B" 
 *        message - PGN 129039
 * \ingroup group_msgParsers
 * 
 * (Latest inline version)
 * 
 * Alias of PGN 129039. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129039 
 */
inline bool ParseN2kAISClassBPosition(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                        double &Latitude, double &Longitude, bool &Accuracy, bool &RAIM,
                        uint8_t &Seconds, double &COG, double &SOG, tN2kAISTransceiverInformation &AISTransceiverInformation,
                        double &Heading, tN2kAISUnit &Unit, bool &Display, bool &DSC, bool &Band, bool &Msg22, tN2kAISMode &Mode,
                        bool &State) {
  return ParseN2kPGN129039(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, Accuracy, RAIM, Seconds, COG, SOG, AISTransceiverInformation, Heading, Unit, Display, DSC, Band, Msg22, Mode, State);
}

/************************************************************************//**
 * \brief Parsing the content of a "AIS position reports for Class B" 
 *        message - PGN 129039
 * \ingroup group_msgParsers
 * 
 * \note Previous inline version for backwards compatibility, using temporary
 * value to parse unused parameter
 * 
 * Alias of PGN 129039. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129039 
 */
inline bool ParseN2kAISClassBPosition(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                        double &Latitude, double &Longitude, bool &Accuracy, bool &RAIM,
                        uint8_t &Seconds, double &COG, double &SOG, double &Heading, tN2kAISUnit &Unit,
                        bool &Display, bool &DSC, bool &Band, bool &Msg22, tN2kAISMode &Mode, bool &State) {
  tN2kAISTransceiverInformation AISTransceiverInformation; // for backwards compatibility
  return ParseN2kPGN129039(N2kMsg, MessageID, Repeat, UserID, Latitude, Longitude, Accuracy,
                            RAIM, Seconds, COG, SOG, AISTransceiverInformation, Heading, Unit, Display, DSC, Band, Msg22, Mode, State);
}

/************************************************************************//**
 * \brief Dedicated structure to handle AIS Aids to Navigation (AtoN) Report
 * 
 * This structure should provide an a more convenient way to process data
 * of an AIS Aids to Navigation (AtoN) Report. The data can be used in 
 * PGN 129041. It also handles initialization of all values.
 * 
 * \sa SetN2kPGN129041 and ParseN2kPGN129041
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 */
struct tN2kAISAtoNReportData {
  /** \brief Message Type ID according to e ITU-R M.1371 */
  uint8_t MessageID;
  /** \brief Repeat indicator, Used by the repeater to indicate how many times a message has been repeated. see \ref tN2kAISRepeat*/
  tN2kAISRepeat Repeat; 
  /** \brief MMSI number */
  uint32_t UserID;
  /** \brief longitude of position [degree] */
  double Longitude;
  /** \brief latitude of position [degree] */
  double Latitude;

  /** \brief position accuracy 
   * - 0 = default
   * - 1 = high (≤10 m)
   * - 0 = low (>10 m)
  */
  bool Accuracy;

  /** \brief Receiver autonomous integrity monitoring (RAIM) flag of
   *  electronic position fixing device
   *    - 0 = RAIM not in use = default
   *    - 1 = RAIM in use
   */
  bool RAIM;

  /**********************************************************************//**
   * \brief TimeStamp UTC second when the report was generated
   * 
   * TimeStamp UTC second when the report was generated by the EPFS
   * (0-59 or 60) if time stamp is not available, which should also
   * be the default value or 61 if positioning system is in manual
   * input mode or 62 if electronic position fixing system operates
   * in estimated (dead reckoning) mode, or 63 if the positioning
   * system is inoperative)
   */
  uint8_t Seconds;
  /** \brief AtoN Structure Length/Diameter in meters*/
  double Length;
  /** \brief AtoN Structure Beam/Diameter in meters*/
  double Beam;
  /** \brief Position Reference Point from Starboard Structure Edge/Radius */
  double PositionReferenceStarboard ;
  /** \brief Position Reference Point from True North facing Structure 
   * Edge/Radius */
  double PositionReferenceTrueNorth;
  /** \brief Aid to Navigation (AtoN) Type, see \ref tN2kAISAtoNType*/
  tN2kAISAtoNType AtoNType;

  /**********************************************************************//**
   * \brief Off Position Indicator
   * For floating AtoN, only
   * - 0 = on position
   * - 1 = off position.
   * 
   * \note This flag should only be considered valid by receiving station,
   * if the AtoN is a floating aid, and if time stamp is equal to or below 59. 
   * For floating AtoN the guard zone parameters should be set on installation
  */
  bool OffPositionIndicator;

  /**********************************************************************//**
   * \brief Virtual AtoN flag
   * - 0 = default = real AtoN at indicated position
   * - 1 = virtual AtoN, does not physically exist.
  */
  bool VirtualAtoNFlag;

  /**********************************************************************//**
   * \brief Assigned Mode Flag
   * - 0 = Station operating in autonomous and continuous mode = default
   * - 1 = Station operating in assigned mode
  */
  bool AssignedModeFlag;

  /**********************************************************************//**
   * \brief Type of electronic position fixing device
   * See \ref tN2kGNSStype
  */
  tN2kGNSStype GNSSType;

  /**********************************************************************//**
   * \brief AtoN Status byte
   * Reserved for the indication of the AtoN status 00000000 = default
  */
  uint8_t AtoNStatus;

  /** \brief AIS Transceiver Information
   * see \ref tN2kAISTransceiverInformation
  */
  tN2kAISTransceiverInformation AISTransceiverInformation; 

  /************************************************************************//**
   * \brief Name of AtoN Object
   * 
   * According to e ITU-R M.1371  
   * 
   * This parameter of up to 14 additional 6-bit-ASCII characters for a
   * 2-slot message may be combined with the parameter “Name of 
   * Aid-to-Navigation” at the end of that parameter, when more than 20
   * characters are needed for the name of the AtoN. This parameter
   * should be omitted when no more than 20 characters for the name of
   * the A-to-N are needed in total. Only the required number of
   * characters should be transmitted, i.e. no @-character should be used
  */
  char AtoNName[34 + 1];

  /**********************************************************************//**
   * \brief Construct a new t N2k AIS AtoN Report Data object
   * 
   * This constructor takes care of a proper initialisation.
   */
  tN2kAISAtoNReportData():
    MessageID(N2kUInt8NA),
    Repeat(N2kaisr_Initial),
    UserID(N2kUInt32NA),
    Longitude(N2kDoubleNA),
    Latitude(N2kDoubleNA),
    Accuracy(false),
    RAIM(false),
    Seconds(N2kUInt8NA),
    Length(N2kDoubleNA),
    Beam(N2kDoubleNA),
    PositionReferenceStarboard(N2kDoubleNA),
    PositionReferenceTrueNorth(N2kDoubleNA),
    AtoNType(N2kAISAtoN_not_specified),
    OffPositionIndicator(false),
    VirtualAtoNFlag(false),
    AssignedModeFlag(false),
    GNSSType(N2kGNSSt_GPS),
    AtoNStatus(N2kUInt8NA),
    AISTransceiverInformation(N2kaischannel_A_VDL_reception) { 
      AtoNName[0]=0; 
   }

  /**********************************************************************//**
   * \brief Set the AtoN Name
   * 
   * \param name Name of Aid-to-Navigation
   */
  void SetAtoNName(const char *name) {
    strncpy(AtoNName, name, sizeof(AtoNName));
    AtoNName[sizeof(AtoNName) - 1] = 0;
  }
};

/************************************************************************//**
 * \brief Setting up PGN 129041 Message "AIS Aids to Navigation (AtoN) Report"
 * \ingroup group_msgSetUp
 * 
 * This PGN provides information received from an AtoN AIS station conforming
 * to ITU-R M.1371-4 Message 21.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param N2kData     structure that consists all relevant data, see 
 *                    \ref tN2kAISAtoNReportData
 */
void SetN2kPGN129041(tN2kMsg &N2kMsg, const tN2kAISAtoNReportData &N2kData);

/************************************************************************//**
 * \brief Setting up Message "AIS Aids to Navigation (AtoN) Report" - PGN 129041
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129041. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129041
  */
inline void SetN2kAISAtoNReport(tN2kMsg &N2kMsg, const tN2kAISAtoNReportData &N2kData) {
  SetN2kPGN129041(N2kMsg, N2kData);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129041 "AIS Aids to
 *        Navigation (AtoN) Report"
 * \ingroup group_msgParsers
 * 
 * This PGN provides information received from an AtoN AIS station conforming
 * to ITU-R M.1371-4 Message 21.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param N2kData     structure that consists all relevant data, see 
 *                    \ref tN2kAISAtoNReportData
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129041(const tN2kMsg &N2kMsg, tN2kAISAtoNReportData &N2kData);

/************************************************************************//**
 * \brief Parsing the content of a "AIS Aids to Navigation (AtoN) Report" 
 *        message - PGN 129041
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129041. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129041 
 */
inline bool ParseN2kAISAtoNReport(const tN2kMsg &N2kMsg, tN2kAISAtoNReportData &N2kData) {
  return ParseN2kPGN129041(N2kMsg, N2kData);
}

/************************************************************************//**
 * \brief Setting up PGN 129283 Message "Cross Track Error"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides the magnitude of position error perpendicular
 * to the desired course.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param XTEMode     CrossTrackError Mode, see \ref tN2kXTEMode
 * \param NavigationTerminated {bool] Navigation has been terminated
 * \param XTE         CrossTrackError in meter
 * 
*/
void SetN2kPGN129283(tN2kMsg &N2kMsg, unsigned char SID, tN2kXTEMode XTEMode, bool NavigationTerminated, double XTE);

/************************************************************************//**
 * \brief Setting up Message "Cross Track Error" - PGN 129283
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129283. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129283
  */
inline void SetN2kXTE(tN2kMsg &N2kMsg, unsigned char SID, tN2kXTEMode XTEMode, bool NavigationTerminated, double XTE) {
  SetN2kPGN129283(N2kMsg, SID, XTEMode, NavigationTerminated, XTE);
}

/************************************************************************//**
 * \brief Parsing the content of Message PGN 129283 Message "Cross Track Error"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides the magnitude of position error perpendicular
 * to the desired course.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param XTEMode     CrossTrackError Mode, see \ref tN2kXTEMode
 * \param NavigationTerminated {bool] Navigation has been terminated
 * \param XTE         CrossTrackError in meter
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
*/
bool ParseN2kPGN129283(const tN2kMsg &N2kMsg, unsigned char& SID, tN2kXTEMode& XTEMode, bool& NavigationTerminated, double& XTE);

/************************************************************************//**
 * \brief Parsing the content of a "Cross Track Error" 
 *        message - PGN 129283
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129283. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129283 
 */
inline bool ParseN2kXTE(const tN2kMsg &N2kMsg, unsigned char& SID, tN2kXTEMode& XTEMode, bool& NavigationTerminated, double& XTE) {
   return ParseN2kPGN129283(N2kMsg, SID, XTEMode, NavigationTerminated, XTE);
}

/************************************************************************//**
 * \brief Setting up PGN 129284 Message "Navigation Data"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides essential navigation data for following a
 * route. Transmissions will originate from products that can create and manage
 * routes using waypoints. This information is intended for navigational
 * repeaters.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param DistanceToWaypoint          Distance to Destination Waypoint
 * \param BearingReference            Course / Bearing Reference, see \ref 
 *                                    tN2kHeadingReference
 * \param PerpendicularCrossed        {bool}Perpendicular Crossed
 * \param ArrivalCircleEntered        {bool}Arrival Circle Entered
 * \param CalculationType             Calculation Type, see 
 *                                    \ref tN2kDistanceCalculationType
 * \param ETATime                     Estimated Time at Arrival - Time
 * \param ETADate                     Estimated Time at Arrival - Date
 * \param BearingOriginToDestinationWaypoint    Bearing, Origin To Destination
 *                                              Waypoint
 * \param BearingPositionToDestinationWaypoint  Bearing, Position To 
 *                                              Destination Waypoint
 * \param OriginWaypointNumber         Origin Waypoint Number
 * \param DestinationWaypointNumber    Destination Waypoint Number
 * \param DestinationLatitude          Destination Wpt Latitude
 * \param DestinationLongitude         Destination Wpt Longitude
 * \param WaypointClosingVelocity      Waypoint Closing Velocity
 * 
 */
void SetN2kPGN129284(tN2kMsg &N2kMsg, unsigned char SID, double DistanceToWaypoint, tN2kHeadingReference BearingReference,
                      bool PerpendicularCrossed, bool ArrivalCircleEntered, tN2kDistanceCalculationType CalculationType,
                      double ETATime, int16_t ETADate, double BearingOriginToDestinationWaypoint, double BearingPositionToDestinationWaypoint,
                      uint32_t OriginWaypointNumber, uint32_t DestinationWaypointNumber,
                      double DestinationLatitude, double DestinationLongitude, double WaypointClosingVelocity);

/************************************************************************//**
 * \brief Setting up Message "Navigation Data" - PGN 129284
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129284. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129284
  */
inline void SetN2kNavigationInfo(tN2kMsg &N2kMsg, unsigned char SID, double DistanceToWaypoint, tN2kHeadingReference BearingReference,
                      bool PerpendicularCrossed, bool ArrivalCircleEntered, tN2kDistanceCalculationType CalculationType,
                      double ETATime, int16_t ETADate, double BearingOriginToDestinationWaypoint, double BearingPositionToDestinationWaypoint,
                      uint32_t OriginWaypointNumber, uint32_t DestinationWaypointNumber,
                      double DestinationLatitude, double DestinationLongitude, double WaypointClosingVelocity) {
  SetN2kPGN129284(N2kMsg, SID, DistanceToWaypoint, BearingReference,
                      PerpendicularCrossed, ArrivalCircleEntered, CalculationType,
                      ETATime, ETADate, BearingOriginToDestinationWaypoint, BearingPositionToDestinationWaypoint,
                      OriginWaypointNumber, DestinationWaypointNumber,
                      DestinationLatitude, DestinationLongitude, WaypointClosingVelocity);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129284 "Navigation Data"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides essential navigation data for following a
 * route. Transmissions will originate from products that can create and manage
 * routes using waypoints. This information is intended for navigational
 * repeaters.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param DistanceToWaypoint          Distance to Destination Waypoint
 * \param BearingReference            Course / Bearing Reference, see \ref 
 *                                    tN2kHeadingReference
 * \param PerpendicularCrossed        {bool}Perpendicular Crossed
 * \param ArrivalCircleEntered        {bool}Arrival Circle Entered
 * \param CalculationType             Calculation Type, see 
 *                                    \ref tN2kDistanceCalculationType
 * \param ETATime                     Estimated Time at Arrival - Time
 * \param ETADate                     Estimated Time at Arrival - Date
 * \param BearingOriginToDestinationWaypoint    Bearing, Origin To Destination
 *                                              Waypoint
 * \param BearingPositionToDestinationWaypoint  Bearing, Position To 
 *                                              Destination Waypoint
 * \param OriginWaypointNumber         Origin Waypoint Number
 * \param DestinationWaypointNumber    Destination Waypoint Number
 * \param DestinationLatitude          Destination Wpt Latitude
 * \param DestinationLongitude         Destination Wpt Longitude
 * \param WaypointClosingVelocity      Waypoint Closing Velocity
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129284(const tN2kMsg &N2kMsg, unsigned char& SID, double& DistanceToWaypoint, tN2kHeadingReference& BearingReference,
                      bool& PerpendicularCrossed, bool& ArrivalCircleEntered, tN2kDistanceCalculationType& CalculationType,
                      double& ETATime, int16_t& ETADate, double& BearingOriginToDestinationWaypoint, double& BearingPositionToDestinationWaypoint,
                      uint32_t& OriginWaypointNumber, uint32_t& DestinationWaypointNumber,
                      double& DestinationLatitude, double& DestinationLongitude, double& WaypointClosingVelocity);


/************************************************************************//**
 * \brief Parsing the content of a "Navigation Data" 
 *        message - PGN 129284
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129284. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129284 
 */
inline bool ParseN2kNavigationInfo(const tN2kMsg &N2kMsg, unsigned char& SID, double& DistanceToWaypoint, tN2kHeadingReference& BearingReference,
                      bool& PerpendicularCrossed, bool& ArrivalCircleEntered, tN2kDistanceCalculationType& CalculationType,
                      double& ETATime, int16_t& ETADate, double& BearingOriginToDestinationWaypoint, double& BearingPositionToDestinationWaypoint,
                      uint32_t& OriginWaypointNumber, uint32_t& DestinationWaypointNumber,
                      double& DestinationLatitude, double& DestinationLongitude, double& WaypointClosingVelocity) {
   return ParseN2kPGN129284(N2kMsg, SID, DistanceToWaypoint, BearingReference, PerpendicularCrossed, ArrivalCircleEntered, CalculationType,
                            ETATime, ETADate, BearingOriginToDestinationWaypoint, BearingPositionToDestinationWaypoint,
                            OriginWaypointNumber, DestinationWaypointNumber, DestinationLatitude, DestinationLongitude, WaypointClosingVelocity);
}

/************************************************************************//**
 * \brief Setting up PGN 129285 Message "Route/WP information"
 * \ingroup group_msgSetUp
 * 
 * This parameter group returns Route and WP data ahead in the Active Route. It
 * can be requested or may be transmitted without a request, typically at each
 * Waypoint advance.
 *
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param Start       The ID of the first waypoint
 * \param Database    Database ID
 * \param Route       Route ID
 * \param NavDirection    Navigation direction in route, see 
 *                        \ref tN2kNavigationDirection
 * \param SupplementaryData   Supplementary Route/WP data available
 * \param RouteName   The name of the current route
 * 
 */
void SetN2kPGN129285(tN2kMsg &N2kMsg, uint16_t Start, uint16_t Database, uint16_t Route,
      tN2kNavigationDirection NavDirection, const char* RouteName, tN2kGenericStatusPair SupplementaryData=N2kDD002_No);

/************************************************************************//**
 * \brief Setting up Message "Route/WP information" - PGN 129285
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129285. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129285
  */
inline void SetN2kRouteWPInfo(tN2kMsg &N2kMsg, uint16_t Start, uint16_t Database, uint16_t Route,
      tN2kNavigationDirection NavDirection, const char* RouteName, tN2kGenericStatusPair SupplementaryData=N2kDD002_No) {
  SetN2kPGN129285(N2kMsg, Start, Database, Route, NavDirection, RouteName, SupplementaryData);
}

/************************************************************************//**
 * \brief Setting up Message "Route/WP information" - PGN 129285
 * (for backwards compatibility)
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129285. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129285
  */
inline void SetN2kPGN129285(tN2kMsg &N2kMsg, uint16_t Start, uint16_t Database, uint16_t Route,
                        bool NavDirection, bool SupplementaryData, char* RouteName)
{
   tN2kNavigationDirection NavDirection1 = NavDirection?N2kdir_reverse:N2kdir_forward;
   tN2kGenericStatusPair N2kSupplementaryData = SupplementaryData?N2kDD002_Yes:N2kDD002_No;
	SetN2kPGN129285(N2kMsg, Start, Database, Route, NavDirection1, RouteName, N2kSupplementaryData);
}                        

/************************************************************************//**
 * \brief  Append another Waypoint to PGN 129285 "Route/WP information"
 * \ingroup group_msgSetUp
 * 
 * This function appends another waypoint data to an N2kMsg created
 * with \ref SetN2kPGN129285.
 *
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param WPID        The ID of the current waypoint
 * \param WPName      The name of the current waypoint
 * \param Latitude    The latitude of the current waypoint
 * \param Longitude   The longitude of the current waypoint
 * 
 * \return true       if there was enough space in the message
 * \return false 
 * 
 */
bool AppendN2kPGN129285(tN2kMsg &N2kMsg, uint16_t WPID, const char* WPName, double Latitude, double Longitude);

/************************************************************************//**
 * \brief Append another Waypoint to "Route/WP information"- PGN 129285
 * \ingroup group_msgSetUp
 * 
 * This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref AppendN2kPGN129285
  */
inline bool AppendN2kRouteWPInfo(tN2kMsg &N2kMsg, uint16_t WPID, const char* WPName, double Latitude, double Longitude) {
   return AppendN2kPGN129285(N2kMsg, WPID, WPName, Latitude, Longitude);
}

/************************************************************************//**
 * \brief Setting up PGN 129539 Message "GNSS DOP data"
 * \ingroup group_msgSetUp
 * 
 * This PGN provides a single transmission containing GNSS status and dilution
 * of precision components (DOP) that indicate the contribution of satellite
 * geometry to the overall positioning error. There are three DOP parameters
 * reported, horizontal (HDOP), Vertical (VDOP) and time (TDOP).
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param SID           Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                      \n
 *                      The sequence identifier field is used to tie different PGNs data together to same 
 *                      sampling or calculation time.
 * \param DesiredMode   Desired Dilution Of Precision mode,
 *                      see \ref tN2kGNSSDOPmode
 * \param ActualMode    Actual Dilution Of Precision mode.
 *                      see \ref tN2kGNSSDOPmode
 * \param HDOP          Horizontal Dilution Of Precision in meters.
 * \param VDOP          Probable dilution of precision in meters.
 * \param TDOP          Time dilution of precision
 * 
 */
void SetN2kPGN129539(tN2kMsg& N2kMsg, unsigned char SID, tN2kGNSSDOPmode DesiredMode, tN2kGNSSDOPmode ActualMode,
                     double HDOP, double VDOP, double TDOP);

/************************************************************************//**
 * \brief Setting up Message "GNSS DOP data - PGN 129539
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129539. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129539
  */
inline void SetN2kGNSSDOPData(tN2kMsg& N2kMsg, unsigned char SID, tN2kGNSSDOPmode DesiredMode, tN2kGNSSDOPmode ActualMode,
                              double HDOP, double VDOP, double TDOP)
{
    SetN2kPGN129539(N2kMsg, SID, DesiredMode, ActualMode, HDOP, VDOP, TDOP);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129539 "GNSS DOP data"
 * \ingroup group_msgParsers
 * 
 * This PGN provides a single transmission containing GNSS status and dilution
 * of precision components (DOP) that indicate the contribution of satellite
 * geometry to the overall positioning error. There are three DOP parameters
 * reported, horizontal (HDOP), Vertical (VDOP) and time (TDOP).
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 * \param SID           Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param DesiredMode   Desired Dilution Of Precision mode,
 *                      see \ref tN2kGNSSDOPmode
 * \param ActualMode    Actual Dilution Of Precision mode.
 *                      see \ref tN2kGNSSDOPmode
 * \param HDOP          Horizontal Dilution Of Precision in meters.
 * \param VDOP          Probable dilution of precision in meters.
 * \param TDOP          Time dilution of precision
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 */
bool ParseN2kPgn129539(const tN2kMsg& N2kMsg, unsigned char& SID, tN2kGNSSDOPmode& DesiredMode, tN2kGNSSDOPmode& ActualMode,
                       double& HDOP, double& VDOP, double& TDOP);

/************************************************************************//**
 * \brief Parsing the content of a "GNSS DOP data" 
 *        message - PGN 129539
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129539. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPgn129539 
 */
inline bool ParseN2kGNSSDOPData(const tN2kMsg& N2kMsg, unsigned char& SID, tN2kGNSSDOPmode& DesiredMode, tN2kGNSSDOPmode& ActualMode,
                         double& HDOP, double& VDOP, double& TDOP)
{
    return ParseN2kPgn129539(N2kMsg, SID, DesiredMode, ActualMode, HDOP, VDOP, TDOP);
}

/************************************************************************//**
 * \brief Dedicated structure to handle GNSS satellite information
 * \ingroup group_msgSetUp
 * 
 * This structure should provide a more convenient way to process data
 * of satellites used in the GNSS system.
 * 
 * \sa SetN2kPGN129540 and AppendN2kPGN129540
 */
struct tSatelliteInfo {
  unsigned char PRN;        ///< PRN number [ 01 .. 32] of the satellite 
  double Elevation;         ///< Elevation of the satellite
  double Azimuth;           ///< Azimuth of the satellite
  double SNR;               ///< SignalToNoiseRatio of the satellite
  double RangeResiduals;    ///< Range Residual
  tN2kPRNUsageStatus UsageStatus;   ///<  Usage Status, see tN2kPRNUsageStatus
};

/************************************************************************//**
 * \brief Setting up PGN 129540 Message "GNSS Satellites in View"
 * \ingroup group_msgSetUp
 * 
 * GNSS information on current satellites in view tagged by sequence ID.
 * Information includes PRN, elevation, azimuth, SNR, defines the number
 * of satellites; defines the satellite number and the information.
 * 
 * To add satellites use \ref AppendN2kPGN129540 and \ref tSatelliteInfo
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param SID           Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                      \n
 *                      The sequence identifier field is used to tie different PGNs data together to same 
 *                      sampling or calculation time.
 * \param Mode          Range residual mode, see \ref tN2kRangeResidualMode
 * 
 * \sa AppendN2kPGN129540, SetN2kGNSSSatellitesInView
 */
void SetN2kPGN129540(tN2kMsg& N2kMsg, unsigned char SID=0xff, tN2kRangeResidualMode Mode=N2kDD072_Unavailable);

/************************************************************************//**
 * \brief Append a new satellite info to PGN 129540 Message
 * "GNSS Satellites in View"
 * \ingroup group_msgSetUp
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param SatelliteInfo Requested satellite info, see \ref tSatelliteInfo
 * 
 * \return true         function succeeded
 * \return false        no more satellites can be added or try to use wrong or
 *                      uninitialized message.
 * 
 * \sa SetN2kPGN129540
 */
bool AppendN2kPGN129540(tN2kMsg& N2kMsg, const tSatelliteInfo& SatelliteInfo);

/************************************************************************//**
 * \brief Setting up Message "GNSS DOP data" - PGN 129540
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129540. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129540
  */
inline void SetN2kGNSSSatellitesInView(tN2kMsg& N2kMsg, unsigned char SID=0xff, tN2kRangeResidualMode Mode=N2kDD072_Unavailable) {
  SetN2kPGN129540(N2kMsg,SID,Mode);
}

/************************************************************************//**
 * \brief Append a new satellite info to Message "GNSS Satellites in View" - 
 * PGN 129540 
 * 
 * Alias of PGN 129540. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref AppendN2kPGN129540
  */
inline bool AppendSatelliteInfo(tN2kMsg& N2kMsg, const tSatelliteInfo& SatelliteInfo) {
  return AppendN2kPGN129540(N2kMsg,SatelliteInfo);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129540 "GNSS Satellites in View"
 * \ingroup group_msgParsers
 * 
 * GNSS information on current satellites in view tagged by sequence ID.
 * Information includes PRN, elevation, azimuth, SNR, defines the number
 * of satellites; defines the satellite number and the information.
 * 
 * Use first (THIS) function to get basic information and specially 
 * NumberOfSVs. 
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 * \param SID           Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param Mode          Range residual mode, see \ref tN2kRangeResidualMode
 * \param NumberOfSVs   Number of satellite infos in message.
 * 
 * \return true         function succeeded
 * \return false        called with wrong message
 * 
 */
bool ParseN2kPGN129540(const tN2kMsg& N2kMsg, unsigned char& SID, tN2kRangeResidualMode &Mode, uint8_t& NumberOfSVs);

/************************************************************************//**
 * \brief Parsing the content of message PGN 129540 "GNSS Satellites in View"
 * \ingroup group_msgParsers
 * 
 * GNSS information on current satellites in view tagged by sequence ID.
 * Information includes PRN, elevation, azimuth, SNR, defines the number
 * of satellites; defines the satellite number and the information.
 * 
 * Use second (THIS) function to get information for specific satellite. It is 
 * possible to use just this function, but it returns also false for wrong
 * message.
 *  
 * \param N2kMsg        Reference to a N2kMsg Object, 
 * \param SVIndex       Index of saellite info requested.
 * \param SatelliteInfo Requested satellite info
 * 
 * \return true         function succeeded
 * \return false        called with wrong message or SVIndex is out of range.
 * 
 */
bool ParseN2kPGN129540(const tN2kMsg& N2kMsg, uint8_t SVIndex, tSatelliteInfo& SatelliteInfo);

/************************************************************************//**
 * \brief Parsing the content of a "GNSS Satellites in View" 
 *        message - PGN 129540
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129540. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129540 
 */
inline bool ParseN2kPGNSatellitesInView(const tN2kMsg& N2kMsg, unsigned char& SID, tN2kRangeResidualMode &Mode, uint8_t& NumberOfSVs){
  return ParseN2kPGN129540(N2kMsg,SID,Mode,NumberOfSVs);
}

/************************************************************************//**
 * \brief Parsing the content of a "GNSS Satellites in View" 
 *        message - PGN 129540
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129540. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129540 
 */
inline bool ParseN2kPGNSatellitesInView(const tN2kMsg& N2kMsg, uint8_t SVIndex, tSatelliteInfo& SatelliteInfo){
  return ParseN2kPGN129540(N2kMsg,SVIndex,SatelliteInfo);
}


/************************************************************************//**
 * \brief Setting up PGN 129794 Message "AIS static data class A"
 * \ingroup group_msgSetUp
 * 
 * This parameter group provides data associated with the ITU-R M.1371 
 * Message 5 Ship Static and Voyage Related Data Message. An AIS device
 * may generate this parameter group either upon receiving a VHF data link
 * message 5, or upon receipt of an ISO or NMEA request PGN.
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param IMOnumber       Ship identification number by IMO  
 *                        [1 .. 999999999]; 0 = not available = default  
 *                        Not applicable to SAR aircraft
 * \param Callsign        Call Sign - 7x -> 6 bit ASCII characters,
 * \param Name            Name of the vessel; 
 *                        Maximum 20 characters 6 bit ASCII;
 *                        For SAR aircraft, it should be set
 *                        to “SAR AIRCRAFT NNNNNNN” where NNNNNNN equals
 *                        the aircraft registration number
 * \param VesselType      Vessel Type 
 *                        0 = not available or no ship = default
 *                        1-99 = as defined in § 3.3.2
 *                        100-199 = reserved, for regional use
 *                        200-255 = reserved, for future use
 *                        Not applicable to SAR aircraft
 * \param Length          Length/Diameter in meters
 * \param Beam            Beam/Diameter in meters
 * \param PosRefStbd      Position Reference Point from Starboard 
 * \param PosRefBow       Position Reference Point from the Bow
 * \param ETAdate         EstimatedTimeOfArrival - Days since 1970-01-01 (UTC)
 * \param ETAtime         EstimatedTimeOfArrival -seconds since midnight (UTC)
 * \param Draught         Maximum present static draught
 * \param Destination     Destination - 
 *                        Maximum 20 characters using 6-bit ASCII  
 * \param AISversion      AIS version, see \ref tN2kAISVersion
 * \param GNSStype        Type of GNSS, see \ref tN2kGNSStype
 * \param DTE             Data terminal equipment (DTE) ready
 *                        - 0 = available
 *                        - 1 = not available = default
 * \param AISinfo         AIS Transceiver Information,
 *                        see \ref tN2kAISTransceiverInformation
 */
void SetN2kPGN129794(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                        uint32_t IMOnumber, char *Callsign, char *Name, uint8_t VesselType, double Length,
                        double Beam, double PosRefStbd, double PosRefBow, uint16_t ETAdate, double ETAtime,
                        double Draught, char *Destination, tN2kAISVersion AISversion, tN2kGNSStype GNSStype,
                        tN2kAISDTE DTE, tN2kAISTransceiverInformation AISinfo);

/************************************************************************//**
 * \brief Setting up Message "AIS static data class A" - PGN 129794
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129794. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129794
  */
inline void SetN2kAISClassAStatic(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                        uint32_t IMOnumber, char *Callsign, char *Name, uint8_t VesselType, double Length,
                        double Beam, double PosRefStbd, double PosRefBow, uint16_t ETAdate, double ETAtime,
                        double Draught, char *Destination, tN2kAISVersion AISversion, tN2kGNSStype GNSStype,
                        tN2kAISDTE DTE, tN2kAISTransceiverInformation AISinfo) {
  SetN2kPGN129794(N2kMsg, MessageID, Repeat, UserID, IMOnumber, Callsign, Name, VesselType, Length,
                  Beam, PosRefStbd, PosRefBow, ETAdate, ETAtime, Draught, Destination, AISversion, GNSStype, DTE, AISinfo);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129794 "AIS static data class A"
 * \ingroup group_msgParsers
 * 
 * This parameter group provides data associated with the ITU-R M.1371 
 * Message 5 Ship Static and Voyage Related Data Message. An AIS device
 * may generate this parameter group either upon receiving a VHF data link
 * message 5, or upon receipt of an ISO or NMEA request PGN.
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param IMOnumber       Ship identification number by IMO
 *                        [1 .. 999999999]; 0 = not available = default  
 *                        Not applicable to SAR aircraft
 * \param Callsign        Call Sign - 7x -> 6 bit ASCII characters,
 * \param Name            Name of the vessel; 
 *                        Maximum 20 characters 6 bit ASCII;
 *                        For SAR aircraft, it should be set
 *                        to “SAR AIRCRAFT NNNNNNN” where NNNNNNN equals
 *                        the aircraft registration number
 * \param VesselType      Vessel Type 
 *                        0 = not available or no ship = default
 *                        1-99 = as defined in § 3.3.2
 *                        100-199 = reserved, for regional use
 *                        200-255 = reserved, for future use
 *                        Not applicable to SAR aircraft
 * \param Length          Length/Diameter in meters
 * \param Beam            Beam/Diameter in meters
 * \param PosRefStbd      Position Reference Point from Starboard 
 * \param PosRefBow       Position Reference Point from the Bow
 * \param ETAdate         EstimatedTimeOfArrival - Days since 1970-01-01 (UTC)
 * \param ETAtime         EstimatedTimeOfArrival -seconds since midnight (UTC)
 * \param Draught         Maximum present static draught
 * \param Destination     Destination - 
 *                        Maximum 20 characters using 6-bit ASCII  
 * \param AISversion      AIS version, see \ref tN2kAISVersion
 * \param GNSStype        Type of GNSS, see \ref tN2kGNSStype
 * \param DTE             Data terminal equipment (DTE) ready
 *                        - 0 = available
 *                        - 1 = not available = default
 * \param AISinfo         AIS Transceiver Information,
 *                        see \ref tN2kAISTransceiverInformation
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129794(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                        uint32_t &IMOnumber, char *Callsign, char *Name, uint8_t &VesselType, double &Length,
                        double &Beam, double &PosRefStbd, double &PosRefBow, uint16_t &ETAdate, double &ETAtime,
                        double &Draught, char *Destination, tN2kAISVersion &AISversion, tN2kGNSStype &GNSStype,
                        tN2kAISDTE &DTE, tN2kAISTransceiverInformation &AISinfo);

/************************************************************************//**
 * \brief Parsing the content of a "AIS static data class A" 
 *        message - PGN 129794
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129794. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129794 
 */
inline bool ParseN2kAISClassAStatic(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                        uint32_t & IMOnumber, char *Callsign, char *Name, uint8_t &VesselType, double &Length,
                        double &Beam, double &PosRefStbd, double &PosRefBow, uint16_t &ETAdate, double &ETAtime,
                        double &Draught, char *Destination, tN2kAISVersion &AISversion, tN2kGNSStype &GNSStype,
                        tN2kAISDTE &DTE, tN2kAISTransceiverInformation &AISinfo) {
  return ParseN2kPGN129794(N2kMsg, MessageID, Repeat, UserID, IMOnumber, Callsign, Name, VesselType, Length,
                          Beam, PosRefStbd, PosRefBow, ETAdate, ETAtime, Draught, Destination, AISversion,
                          GNSStype, DTE, AISinfo);
}

/************************************************************************//**
 * \brief Setting up PGN 129809 Message "AIS static data class B part A"
 * \ingroup group_msgSetUp
 * 
 * This parameter group is used by Class B "CS" shipborne mobile equipment
 * each time Part A of ITU-R M.1372 Message 24 is received.
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * \sa SetN2kPGN129810 and ParseN2kPGN129810
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param Name            Name of the vessel; 
 *                        Maximum 20 characters 6 bit ASCII;
 *                        For SAR aircraft, it should be set
 *                        to “SAR AIRCRAFT NNNNNNN” where NNNNNNN equals
 *                        the aircraft registration number
 * 
 */
void SetN2kPGN129809(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID, char *Name);

/************************************************************************//**
 * \brief Setting up Message "AIS static data class B part A" - PGN 129809
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129809. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129809
  */
inline void SetN2kAISClassBStaticPartA(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID, char *Name) {
  SetN2kPGN129809(N2kMsg, MessageID, Repeat, UserID, Name);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129809 "AIS static data
 *        class B part A"
 * \ingroup group_msgParsers
 * 
 * This parameter group is used by Class B "CS" shipborne mobile equipment
 * each time Part A of ITU-R M.1372 Message 24 is received.
 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * \sa SetN2kPGN129810 and ParseN2kPGN129810
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param Name            Name of the vessel; 
 *                        Maximum 20 characters 6 bit ASCII;
 *                        For SAR aircraft, it should be set
 *                        to “SAR AIRCRAFT NNNNNNN” where NNNNNNN equals
 *                        the aircraft registration number
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 */
bool ParseN2kPGN129809(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID, char *Name);

/************************************************************************//**
 * \brief Parsing the content of a "AIS static data class B part A" 
 *        message - PGN 129809
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129809. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129809 
 */
inline bool ParseN2kAISClassBStaticPartA(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID, char *Name) {
  return ParseN2kPGN129809(N2kMsg, MessageID, Repeat, UserID, Name);
}

/************************************************************************//**
 * \brief Setting up PGN 129810 Message "AIS static data class B part B"
 * \ingroup group_msgSetUp
 * 
 * This parameter group is used by Class B "CS" shipborne mobile equipment each
 * time Part B of ITU-R M.1372 Message 24 is received.

 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * \sa SetN2kPGN129809 and ParseN2kPGN129809
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param VesselType      Vessel Type 
 *                        0 = not available or no ship = default
 *                        1-99 = as defined in § 3.3.2
 *                        100-199 = reserved, for regional use
 *                        200-255 = reserved, for future use
 *                        Not applicable to SAR aircraft
 * \param Vendor          Unique identification of the Unit by a number as
 *                        defined by the manufacturer
 * \param Callsign        Call Sign - 7x -> 6 bit ASCII characters
 * \param Length          Length/Diameter in meters
 * \param Beam            Beam/Diameter in meters
 * \param PosRefStbd      Position Reference Point from Starboard 
 * \param PosRefBow       Position Reference Point from the Bow
 * \param MothershipID    MMSI of the mothership 
 * 
 */
void SetN2kPGN129810(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                      uint8_t VesselType, char *Vendor, char *Callsign, double Length, double Beam,
                      double PosRefStbd, double PosRefBow, uint32_t MothershipID);

/************************************************************************//**
 * \brief Setting up Message "AIS static data class B part B" - PGN 129810
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 129810. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN129810
  */
inline void SetN2kAISClassBStaticPartB(tN2kMsg &N2kMsg, uint8_t MessageID, tN2kAISRepeat Repeat, uint32_t UserID,
                      uint8_t VesselType, char *Vendor, char *Callsign, double Length, double Beam,
                      double PosRefStbd, double PosRefBow, uint32_t MothershipID) {
  SetN2kPGN129810(N2kMsg, MessageID, Repeat, UserID, VesselType, Vendor, Callsign, Length, Beam,
                  PosRefStbd, PosRefBow, MothershipID);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 129810 "AIS static data
 *        class B part B"
 * \ingroup group_msgParsers
 * 
 * This parameter group is used by Class B "CS" shipborne mobile equipment each
 * time Part B of ITU-R M.1372 Message 24 is received.

 * 
 * \sa [ITU-R M.1371](https://www.itu.int/rec/R-REC-M.1371)
 * \sa SetN2kPGN129809 and ParseN2kPGN129809
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param MessageID       Message Type ID according to e ITU-R M.1371
 * \param Repeat          Repeat indicator, Used by the repeater to indicate
 *                        how many times a message has been repeated.  
 *                        see \ref tN2kAISRepeat
 * \param UserID          MMSI Number
 * \param VesselType      Vessel Type 
 *                        0 = not available or no ship = default
 *                        1-99 = as defined in § 3.3.2
 *                        100-199 = reserved, for regional use
 *                        200-255 = reserved, for future use
 *                        Not applicable to SAR aircraft
 * \param Vendor          Unique identification of the Unit by a number as
 *                        defined by the manufacturer
 * \param Callsign        Call Sign - 7x -> 6 bit ASCII characters
 * \param Length          Length/Diameter in meters
 * \param Beam            Beam/Diameter in meters
 * \param PosRefStbd      Position Reference Point from Starboard 
 * \param PosRefBow       Position Reference Point from the Bow
 * \param MothershipID    MMSI of the mothership 
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN129810(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                      uint8_t &VesselType, char *Vendor, char *Callsign, double &Length, double &Beam,
                      double &PosRefStbd, double &PosRefBow, uint32_t &MothershipID);

/************************************************************************//**
 * \brief Parsing the content of a "AIS static data class B part B" 
 *        message - PGN 129810
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 129810. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN129810 
 */
inline bool ParseN2kAISClassBStaticPartB(const tN2kMsg &N2kMsg, uint8_t &MessageID, tN2kAISRepeat &Repeat, uint32_t &UserID,
                      uint8_t &VesselType, char *Vendor, char *Callsign, double &Length, double &Beam,
                      double &PosRefStbd, double &PosRefBow, uint32_t &MothershipID) {
  return ParseN2kPGN129810(N2kMsg, MessageID, Repeat, UserID, VesselType, Vendor, Callsign,
                                Length, Beam, PosRefStbd, PosRefBow, MothershipID);
}

/************************************************************************//**
 * \brief Setting up PGN 130074 Message " Route and WP Service - WP List - 
 *        WP Name & Position"
 * \ingroup group_msgSetUp
 * 
 * Complex request of this PGN should return the Waypoints of a WP-List.
 * 
 * This function sets up the basic Message with some general information 
 * about the waypoint list. To add certain waypoints see 
 * \ref AppendN2kPGN130074.
 * 
 * \sa AppendN2kPGN130074
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param Start         The ID of the first waypoint
 * \param NumWaypoints  Number of valid WPs in the WP-List
 * \param Database      Database ID
 */
void SetN2kPGN130074(tN2kMsg &N2kMsg, uint16_t Start, uint16_t NumWaypoints, uint16_t Database);

/************************************************************************//**
 * \brief Setting up Message "Route and WP Service - WP List - 
 *        WP Name & Position" - PGN 130074
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130074. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130074
  */
inline void SetN2kWaypointList(tN2kMsg &N2kMsg, uint16_t Start, uint16_t NumWaypoints, uint16_t Database) {
  SetN2kPGN130074(N2kMsg,Start,NumWaypoints,Database);
}

/************************************************************************//**
 * \brief Append a Waypoint to PGN 130074 Message " Route and WP Service - 
 *        WP List - WP Name & Position"
 * \ingroup group_msgSetUp
 * 
 * This functions adds a waypoint to the "Route and WP Service - WP List - 
 * WP Name & Position" (PGN130074)
 * 
 * \sa SetN2kPGN130074
 * 
 * \param N2kMsg        Reference to a N2kMsg Object, 
 *                      Output: NMEA2000 message ready to be send.
 * \param ID            The ID of the waypoint
 * \param Name          The name of the waypoint
 * \param Latitude      The latitude of the waypoint
 * \param Longitude     The Longitude of the waypoint
 * 
 * \return true       waypoint successfully added 
 * \return false      aborted
 */
bool AppendN2kPGN130074(tN2kMsg &N2kMsg, uint16_t ID, char* Name, double Latitude, double Longitude);

/************************************************************************//**
 * \brief Append a Waypoint to Message "Route and WP Service - WP List - 
 *        WP Name & Position" - PGN 130074
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130074. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref AppendN2kPGN130074
  */
inline bool AppendN2kWaypointList(tN2kMsg &N2kMsg, uint16_t ID, char* Name, double Latitude, double Longitude) {
  return AppendN2kPGN130074(N2kMsg,ID,Name,Latitude,Longitude);
}

/************************************************************************//**
 * \brief Setting up PGN 130306 Message "Wind Data"
 * \ingroup group_msgSetUp
 * 
 * Direction and speed of Wind. True wind can be referenced to the vessel or
 * to the ground. The Apparent Wind is what is felt standing on the (moving)
 * ship, I.e., the wind measured by the typical mast head instruments. The
 * boat referenced true wind is given by the vector sum of Apparent wind
 * and vessel's heading and speed though the water. The ground referenced
 * true wind is given by the vector sum of Apparent wind and vessel's
 * heading and speed over ground.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WindSpeed     Measured wind speed in m/s
 * \param WindAngle     Measured wind angle in radians. If you have value
 *                      in degrees, use function DegToRad() in call.
 * \param WindReference Wind reference, see definition of tN2kWindReference
 * 
 */
void SetN2kPGN130306(tN2kMsg &N2kMsg, unsigned char SID, double WindSpeed, double WindAngle, tN2kWindReference WindReference);

/************************************************************************//**
 * \brief Setting up Message "Wind Data" - PGN 130306
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130306. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130306
  */
inline void SetN2kWindSpeed(tN2kMsg &N2kMsg, unsigned char SID, double WindSpeed, double WindAngle, tN2kWindReference WindReference) {
  SetN2kPGN130306(N2kMsg,SID,WindSpeed,WindAngle,WindReference);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130306 "Wind Data"
 * \ingroup group_msgParsers
 * 
 * Direction and speed of Wind. True wind can be referenced to the vessel or
 * to the ground. The Apparent Wind is what is felt standing on the (moving)
 * ship, I.e., the wind measured by the typical mast head instruments. The
 * boat referenced true wind is given by the vector sum of Apparent wind
 * and vessel's heading and speed though the water. The ground referenced
 * true wind is given by the vector sum of Apparent wind and vessel's
 * heading and speed over ground.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WindSpeed     Measured wind speed in m/s
 * \param WindAngle     Measured wind angle in radians. If you have value
 *                      in degrees, use function DegToRad() in call.
 * \param WindReference Wind reference, see definition of tN2kWindReference
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN130306(const tN2kMsg &N2kMsg, unsigned char &SID, double &WindSpeed, double &WindAngle, tN2kWindReference &WindReference);

/************************************************************************//**
 * \brief Parsing the content of a "Wind Data" 
 *        message - PGN 130306
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130306. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130306 
 */
inline bool ParseN2kWindSpeed(const tN2kMsg &N2kMsg, unsigned char &SID, double &WindSpeed, double &WindAngle, tN2kWindReference &WindReference) {
  return ParseN2kPGN130306(N2kMsg,SID,WindSpeed,WindAngle,WindReference);
}

/************************************************************************//**
 * \brief Setting up PGN 130310 Message " Environmental Parameters - DEPRECATED"
 * \ingroup group_msgSetUp
 * 
 * Local atmospheric environmental conditions.
 * 
 * \note This PGN has been depricated. Specific PGNs 130316 Temperature,
 * 130313 Relative Humidity, 130314 Actual Pressure, 130315 Set Pressure
 * shall be used.
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param WaterTemperature                Water temperature in K. Use 
 *                                        \ref CToKelvin, if you want to use °C.
 * \param OutsideAmbientAirTemperature    Outside ambient temperature in K.Use 
 *                                        \ref CToKelvin, if you want to use °C.
 * \param AtmosphericPressure      Atmospheric pressure in Pascals. Use
 *                                 \ref mBarToPascal, if you like to use mBar
 * 
 */
void SetN2kPGN130310(tN2kMsg &N2kMsg, unsigned char SID, double WaterTemperature,
                     double OutsideAmbientAirTemperature=N2kDoubleNA, double AtmosphericPressure=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Environmental Parameters - DEPRECATED" - 
 *        PGN 130310
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130310. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130310
  */
inline void SetN2kOutsideEnvironmentalParameters(tN2kMsg &N2kMsg, unsigned char SID, double WaterTemperature,
                     double OutsideAmbientAirTemperature=N2kDoubleNA, double AtmosphericPressure=N2kDoubleNA) {
  SetN2kPGN130310(N2kMsg,SID,WaterTemperature,OutsideAmbientAirTemperature,AtmosphericPressure);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130310
 *        "Environmental Parameters - DEPRECATED"
 * \ingroup group_msgParsers
 * 
 * Local atmospheric environmental conditions.
 * 
 * \note This PGN has been deprecated. Specific PGNs 130316 Temperature,
 * 130313 Relative Humidity, 130314 Actual Pressure, 130315 Set Pressure
 * shall be used.
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param WaterTemperature                Water temperature in K. Use 
 *                                        \ref CToKelvin, if you want to use °C.
 * \param OutsideAmbientAirTemperature    Outside ambient temperature in K.Use 
 *                                        \ref CToKelvin, if you want to use °C.
 * \param AtmosphericPressure      Atmospheric pressure in Pascals. Use
 *                                 \ref mBarToPascal, if you like to use mBar
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN130310(const tN2kMsg &N2kMsg, unsigned char &SID, double &WaterTemperature,
                     double &OutsideAmbientAirTemperature, double &AtmosphericPressure);

/************************************************************************//**
 * \brief Parsing the content of a "Environmental Parameters - DEPRECATED" 
 *        message - PGN 130310
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130310. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130310 
 */
inline bool ParseN2kOutsideEnvironmentalParameters(const tN2kMsg &N2kMsg, unsigned char &SID, double &WaterTemperature,
                     double &OutsideAmbientAirTemperature, double &AtmosphericPressure) {
  return ParseN2kPGN130310(N2kMsg, SID,WaterTemperature,OutsideAmbientAirTemperature,AtmosphericPressure);
}

/************************************************************************//**
 * \brief Setting up PGN 130311 Message "Environmental Parameters - DEPRECATED"
 * \ingroup group_msgSetUp
 * 
 * \note This PGN has been deprecated. Specific PGNs 130316 Temperature,
 * 130313 Relative Humidity, 130314 Actual Pressure, 130315 Set
 * Pressure shall be used.
 * 
 * \note In PGN 130311 TempInstance is named as TempSource
 * (other then in PGN 130312 where TempSource really means source of
 * temperature)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param TempSource      see tN2kTempSource
 * \param Temperature     Temperature in K. Use \ref CToKelvin, if you
 *                        want to use °C.
 * \param HumiditySource  see tN2kHumiditySource.
 * \param Humidity        Humidity in %
 * \param AtmosphericPressure   Atmospheric pressure in Pascals. Use 
 *                              \ref mBarToPascal, if you like to use mBar
 * 
 */
void SetN2kPGN130311(tN2kMsg &N2kMsg, unsigned char SID, tN2kTempSource TempSource, double Temperature,
                     tN2kHumiditySource HumiditySource=N2khs_Undef, double Humidity=N2kDoubleNA, double AtmosphericPressure=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Environmental Parameters - DEPRECATED" - 
 *        PGN 130311
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130311. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130311
  */
inline void SetN2kEnvironmentalParameters(tN2kMsg &N2kMsg, unsigned char SID, tN2kTempSource TempSource, double Temperature,
                     tN2kHumiditySource HumiditySource=N2khs_Undef, double Humidity=N2kDoubleNA, double AtmosphericPressure=N2kDoubleNA) {
  SetN2kPGN130311(N2kMsg,SID,TempSource,Temperature,HumiditySource,Humidity,AtmosphericPressure);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130311 
 * "Environmental Parameters - DEPRECATED"
 * \ingroup group_msgParsers
 * 
 * \note This PGN has been deprecated. Specific PGNs 130316 Temperature,
 * 130313 Relative Humidity, 130314 Actual Pressure, 130315 Set
 * Pressure shall be used.
 * 
 * \note In PGN 130311 TempInstance is named as TempSource
 * (other then in PGN 130312 where TempSource really means source of
 * temperature)
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param TempSource      see tN2kTempSource
 * \param Temperature     Temperature in K. Use \ref CToKelvin, if you
 *                        want to use °C.
 * \param HumiditySource  see tN2kHumiditySource.
 * \param Humidity        Humidity in %
 * \param AtmosphericPressure   Atmospheric pressure in Pascals. Use 
 *                              \ref mBarToPascal, if you like to use mBar
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN130311(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kTempSource &TempSource, double &Temperature,
                     tN2kHumiditySource &HumiditySource, double &Humidity, double &AtmosphericPressure);

/************************************************************************//**
 * \brief Parsing the content of a "Environmental Parameters - DEPRECATED" 
 *        message - PGN 130311
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130311. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130311 
 */
inline bool ParseN2kEnvironmentalParameters(const tN2kMsg &N2kMsg, unsigned char &SID, tN2kTempSource &TempSource, double &Temperature,
                     tN2kHumiditySource &HumiditySource, double &Humidity, double &AtmosphericPressure) {
  return ParseN2kPGN130311(N2kMsg,SID,TempSource,Temperature,HumiditySource,Humidity,AtmosphericPressure);
}

/************************************************************************//**
 * \brief Setting up PGN 130312 Message "Temperature - DEPRECATED"
 * \ingroup group_msgSetUp
 * 
 * \note Temperature as measured by a specific temperature source. This
 * PGN has been deprecated. Please use PGN 130316 (Temperature-Extended Range)
 * for all new designs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param TempInstance        This should be unique at least on one device. May
 *                            be best to have it unique over all devices
 *                            sending this PGN.
 * \param TempSource          see tN2kTempSource
 * \param ActualTemperature   Temperature in K. Use \ref CToKelvin, if you
 *                            want to use °C.
 * \param SetTemperature      Temperature set point in K. Use \ref CToKelvin,
 *                            if you want to use °C. This is meaningful for
 *                            temperatures, which can be controlled like cabin,
 *                            freezer, refridgeration temperature. God can 
 *                            use value for this for outside and sea 
 *                            temperature values.
 */
void SetN2kPGN130312(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Temperature - DEPRECATED" - PGN 130312
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130312. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130312
  */
inline void SetN2kTemperature(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA) {
  SetN2kPGN130312(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130312 "Temperature - DEPRECATED"
 * \ingroup group_msgParsers
 * 
 * \note Temperature as measured by a specific temperature source. This
 * PGN has been deprecated. Please use PGN 130316 (Temperature-Extended Range)
 * for all new designs.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param TempInstance        This should be unique at least on one device. May
 *                            be best to have it unique over all devices
 *                            sending this PGN.
 * \param TempSource          see tN2kTempSource
 * \param ActualTemperature   Temperature in K. Use \ref CToKelvin, if you
 *                            want to use °C.
 * \param SetTemperature      Temperature set point in K. Use \ref CToKelvin,
 *                            if you want to use °C. This is meaningful for
 *                            temperatures, which can be controlled like cabin,
 *                            freezer, refridgeration temperature. God can 
 *                            use value for this for outside and sea 
 *                            temperature values.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN130312(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature);

/************************************************************************//**
 * \brief Parsing the content of a "Temperature - DEPRECATED" 
 *        message - PGN 130312
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130312. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130312 
 */
inline bool ParseN2kTemperature(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  return ParseN2kPGN130312(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature);
}

/************************************************************************//**
 * \brief Setting up PGN 130313 Message "Humidity"
 * \ingroup group_msgSetUp
 * 
 * Humidity as measured by a specific humidity source.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param HumidityInstance  This should be unique at least on one device. May 
 *                          be best to have it unique over all devices sending
 *                          this PGN.
 * \param HumiditySource    source of measurement, see \ref tN2kHumiditySource
 * \param ActualHumidity    Humidity in percent
 * \param SetHumidity       Set value of Humidity in percent
 */
void SetN2kPGN130313(tN2kMsg &N2kMsg, unsigned char SID, unsigned char HumidityInstance,
                     tN2kHumiditySource HumiditySource, double ActualHumidity, double SetHumidity=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Humidity" - PGN 130313
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130313. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130313
  */
inline void SetN2kHumidity(tN2kMsg &N2kMsg, unsigned char SID, unsigned char HumidityInstance,
                     tN2kHumiditySource HumiditySource, double ActualHumidity, double SetHumidity=N2kDoubleNA) {
  SetN2kPGN130313(N2kMsg, SID, HumidityInstance, HumiditySource, ActualHumidity,SetHumidity);
}
/************************************************************************//**
 * \brief Parsing the content of message PGN 130313 "Humidity"
 * \ingroup group_msgParsers
 * 
 * Humidity as measured by a specific humidity source.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param HumidityInstance  This should be unique at least on one device. May 
 *                          be best to have it unique over all devices sending
 *                          this PGN.
 * \param HumiditySource    source of measurement, see \ref tN2kHumiditySource
 * \param ActualHumidity    Humidity in percent
 * \param SetHumidity       Set value of Humidity in percent
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kPGN130313(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &HumidityInstance,
                       tN2kHumiditySource &HumiditySource, double &ActualHumidity, double &SetHumidity);

/************************************************************************//**
 * \brief Parsing the content of a "Humidity" 
 *        message - PGN 130313
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130313. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130313 
 */
inline bool ParseN2kHumidity(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &HumidityInstance,
                       tN2kHumiditySource &HumiditySource, double &ActualHumidity, double &SetHumidity) {
  return ParseN2kPGN130313(N2kMsg, SID, HumidityInstance, HumiditySource, ActualHumidity, SetHumidity);
}

/************************************************************************//**
 * \brief Parsing the content of a "Humidity" 
 *        message - PGN 130313
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130313. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130313 
 */
inline bool ParseN2kPGN130313(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &HumidityInstance,
                       tN2kHumiditySource &HumiditySource, double &ActualHumidity) {
  double SetHumidity;
  return ParseN2kPGN130313(N2kMsg, SID, HumidityInstance, HumiditySource, ActualHumidity, SetHumidity);
}

/************************************************************************//**
 * \brief Parsing the content of a "Humidity" 
 *        message - PGN 130313
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130313. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130313 
 */
inline bool ParseN2kHumidity(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &HumidityInstance,
                       tN2kHumiditySource &HumiditySource, double &ActualHumidity) {
  return ParseN2kPGN130313(N2kMsg, SID, HumidityInstance, HumiditySource, ActualHumidity);
}

/************************************************************************//**
 * \brief Setting up PGN 130314 Message "Actual Pressure"
 * \ingroup group_msgSetUp
 * 
 * Pressure as measured by a specific pressure source
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param PressureInstance  This should be unique at least on one device. May be
 *                          best to have it unique over all devices sending
 *                          this PGN.
 * \param PressureSource    source of measurement, see \ref tN2kPressureSource
 * \param Pressure          actual pressure in Pascals. Use function 
 *                          mBarToPascal, if you like to use mBar
 * 
 */
void SetN2kPGN130314(tN2kMsg &N2kMsg, unsigned char SID, unsigned char PressureInstance,
                     tN2kPressureSource PressureSource, double Pressure);

/************************************************************************//**
 * \brief Setting up Message "Actual Pressure" - PGN 130314
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130314. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130314
  */
inline void SetN2kPressure(tN2kMsg &N2kMsg, unsigned char SID, unsigned char PressureInstance,
                           tN2kPressureSource PressureSource, double Pressure) {
  SetN2kPGN130314(N2kMsg, SID, PressureInstance, PressureSource, Pressure);
}
/************************************************************************//**
 * \brief Parsing the content of message PGN 130314 "Actual Pressure"
 * \ingroup group_msgParsers
 * 
 * Pressure as measured by a specific pressure source
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param PressureInstance  This should be unique at least on one device. May be
 *                          best to have it unique over all devices sending
 *                          this PGN.
 * \param PressureSource    source of measurement, see \ref tN2kPressureSource
 * \param Pressure          actual in Pascals. Use function mBarToPascal, 
 *                          if you like to use mBar
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *  
 */
bool ParseN2kPGN130314(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &PressureInstance,
                       tN2kPressureSource &PressureSource, double &Pressure);

/************************************************************************//**
 * \brief Parsing the content of a "Actual Pressure" 
 *        message - PGN 130314
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130314. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130314 
 */
inline bool ParseN2kPressure(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &PressureInstance,
                       tN2kPressureSource &PressureSource, double &Pressure) {
  return ParseN2kPGN130314(N2kMsg, SID, PressureInstance, PressureSource, Pressure);
}

/************************************************************************//**
 * \brief Setting up PGN 130315 Message "Set Pressure"
 * \ingroup group_msgSetUp
 * 
 * This parameter group can be sent to a device that controls pressure to
 * change its targeted pressure, or it can be sent out by the control device
 * to indicate its current targeted pressure.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param PressureInstance  This should be unique at least on one device. May be
 *                          best to have it unique over all devices sending
 *                          this PGN.
 * \param PressureSource    source of measurement, see \ref tN2kPressureSource
 * \param SetPressure       Set pressure in Pascals. Use function mBarToPascal, 
 *                          if you like to use mBar
 * 
 */
void SetN2kPGN130315(tN2kMsg &N2kMsg, unsigned char SID, unsigned char PressureInstance,
                     tN2kPressureSource PressureSource, double SetPressure);

/************************************************************************//**
 * \brief Setting up Message "Set Pressure" - PGN 130315
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130315. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130315
  */
inline void SetN2kSetPressure(tN2kMsg &N2kMsg, unsigned char SID, unsigned char PressureInstance,
                           tN2kPressureSource PressureSource, double SetPressure) {
  SetN2kPGN130315(N2kMsg, SID, PressureInstance, PressureSource, SetPressure);
}

/************************************************************************//**
 * \brief Setting up PGN 130316 Message "Temperature, Extended Range"
 * \ingroup group_msgSetUp
 * 
 * This parameter group is used to report a wide variety of temperature 
 * measurements.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param TempInstance      This should be unique at least on one device. May be
 *                          best to have it unique over all devices sending
 *                          this PGN.
 * \param TempSource        source of measurement, see \ref tN2kTempSource
 * \param ActualTemperature Actual Temperature in K. Use \ref CToKelvin, if 
 *                          you want to use °C.
 * \param SetTemperature    Set Temperature in K. Use \ref CToKelvin, if
 *                          you want to use °C.
 * 
 */
void SetN2kPGN130316(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Message "Temperature, Extended Range" - PGN 130316
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130316. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130316
  */
inline void SetN2kTemperatureExt(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA) {
  SetN2kPGN130316(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130316 "Temperature, 
 *        Extended Range"
 * \ingroup group_msgParsers
 * 
 * This parameter group is used to report a wide variety of temperature
 * measurements.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param TempInstance      This should be unique at least on one device. May be
 *                          best to have it unique over all devices sending
 *                          this PGN.
 * \param TempSource        source of measurement, see \ref tN2kTempSource
 * \param ActualTemperature Actual Temperature in K. Use \ref CToKelvin, if 
 *                          you want to use °C.
 * \param SetTemperature    Set Temperature in K. Use \ref CToKelvin, if
 *                          you want to use °C.
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 * 
 */
bool ParseN2kPGN130316(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature);

/************************************************************************//**
 * \brief Parsing the content of a "Temperature, Extended Range" 
 *        message - PGN 130316
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130316. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130316 
 */
inline bool ParseN2kTemperatureExt(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  return ParseN2kPGN130316(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature);
}


/************************************************************************//**
 * \brief Dedicated structure to handle Data of a meterological station
 * 
 * This structure should provide an a more convenient way to process data
 * of a meterological station. 
 * 
 * \sa SetN2kPGN130323 and ParseN2kPGN130323
 */

struct tN2kMeteorlogicalStationData {
  /** \brief  */
  tN2kAISMode Mode;
  /** \brief  Days since 1970-01-01*/
  uint16_t SystemDate;
  /** \brief  seconds since midnight*/
  double SystemTime;
   /** \brief The latitude of the current waypoint [degree] */
  double Latitude;
  /** \brief The longitude of the current waypoint [degree] */
  double Longitude;
  /** \brief  Measured wind speed in m/s */
  double WindSpeed;
  /** \brief  Measured wind direction in radians. If you have value 
   * in degrees, use \ref DegToRad() in call.*/
  double WindDirection;
  /** \brief  Wind reference, see definition \ref tN2kWindReference*/
  tN2kWindReference WindReference;
  /** \brief  Measured wind gusts speed in m/s*/
  double WindGusts;
  /** \brief  Atmospheric pressure in Pascals. Use \ref mBarToPascal,
   * if you like to use mBar*/
  double AtmosphericPressure;
  /** \brief  Outside ambient temperature in K. Use \ref CToKelvin,
   * if you want to use °C.*/
  double OutsideAmbientAirTemperature;
  /** \brief  Identifier of the transmitting weather station.
   * (15 bytes max)*/
  char StationID[15 + 1];
  /** \brief  Friendly name of the transmitting weather station. (
   * 50 bytes max)*/
  char StationName[50 + 1];

  tN2kMeteorlogicalStationData():
    Mode(N2kaismode_Autonomous),
    SystemDate(N2kUInt16NA),
    SystemTime(N2kDoubleNA),
    Latitude(N2kDoubleNA),
    Longitude(N2kDoubleNA),
    WindSpeed(N2kDoubleNA),
    WindDirection(N2kDoubleNA),
    WindReference(N2kWind_Unavailable),
    WindGusts(N2kDoubleNA),
    AtmosphericPressure(N2kDoubleNA),
    OutsideAmbientAirTemperature(N2kDoubleNA) {
      StationID[0] = 0;
      StationName[0] = 0;
    }

  /**********************************************************************//**
   * \brief Set the StationID object
   * 
   * \param id  Identifier of the transmitting weather station
   *            (15 bytes max)
   */
  void SetStationID(const char *id) {
    strncpy(StationID, id, sizeof(StationID));
    StationID[sizeof(StationID) - 1] = 0;
  }

  /**********************************************************************//**
   * \brief Set the Station Name object
   * 
   * \param name  Friendly name of the transmitting weather 
   *              station. (50 bytes max)
   */
  void SetStationName(const char *name) {
    strncpy(StationName, name, sizeof(StationName));
    StationName[sizeof(StationName) - 1] = 0;
  }
};

/************************************************************************//**
 * \brief Setting up PGN 130323 Message "Meterological Station Data"
 * \ingroup group_msgSetUp
 * 
 * Meteorological station measurement data including station location,
 * numeric identifier, and name.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param N2kData     Meterological Station Data, see 
 *                    \ref tN2kMeteorlogicalStationData
 */

void SetN2kPGN130323(tN2kMsg &N2kMsg, const tN2kMeteorlogicalStationData &N2kData);

/************************************************************************//**
 * \brief Setting up Message "Meterological Station Data" - PGN 130323
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130323. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130323
  */
inline void SetN2kMeteorlogicalStationData(tN2kMsg &N2kMsg, const tN2kMeteorlogicalStationData &N2kData) { SetN2kPGN130323(N2kMsg, N2kData); }

/************************************************************************//**
 * \brief Parsing the content of message PGN 130323 "Meterological Station Data"
 * \ingroup group_msgParsers
 * 
 * Meteorological station measurement data including station location,
 * numeric identifier, and name.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param N2kData     Meterological Station Data, see 
 *                    \ref tN2kMeteorlogicalStationData
 */
bool ParseN2kPGN130323(const tN2kMsg &N2kMsg, tN2kMeteorlogicalStationData &N2kData);

/************************************************************************//**
 * \brief Parsing the content of a "Meterological Station Data" 
 *        message - PGN 130323
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130323. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130323 
 */
inline bool ParseN2kMeteorlogicalStationData(const tN2kMsg &N2kMsg, tN2kMeteorlogicalStationData &N2kData) {
  return ParseN2kPGN130323(N2kMsg, N2kData);
}

/************************************************************************//**
 * \brief Setting up PGN 130576 Message "Trim Tab Status"
 * \ingroup group_msgSetUp
 * 
 * Provides data on various small craft control surfaces and speed through
 * the water. Used primarily by display or instrumentation. Trim tab position 
 * is a percentage 0 to 100% where 0 is fully retracted and 100 is fully 
 * extended.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param PortTrimTab Port trim tab position
 * \param StbdTrimTab Starboard trim tab position
 */
void SetN2kPGN130576(tN2kMsg &N2kMsg, int8_t PortTrimTab, int8_t StbdTrimTab);

/************************************************************************//**
 * \brief Setting up Message "Trim Tab Status" - PGN 130576
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130576. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130576
  */
inline void SetN2kTrimTab(tN2kMsg &N2kMsg, int8_t PortTrimTab, int8_t StbdTrimTab){
  SetN2kPGN130576(N2kMsg,PortTrimTab, StbdTrimTab);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130576 "Trim Tab Status"
 * \ingroup group_msgParsers
 * 
 * Provides data on various small craft control surfaces and speed through
 * the water. Used primarily by display or instrumentation. Trim tab position 
 * is a percentage 0 to 100% where 0 is fully retracted and 100 is fully 
 * extended.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 * \param PortTrimTab Port trim tab position
 * \param StbdTrimTab Starboard trim tab position
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *  
 */
bool ParseN2kPGN130576(const tN2kMsg &N2kMsg, int8_t &PortTrimTab, int8_t &StbdTrimTab);

/************************************************************************//**
 * \brief Parsing the content of a "Trim Tab Status" 
 *        message - PGN 130576
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130576. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130576 
 */
inline bool ParseN2kTrimTab(const tN2kMsg &N2kMsg, int8_t &PortTrimTab, int8_t &StbdTrimTab) {
  return ParseN2kPGN130576(N2kMsg, PortTrimTab, StbdTrimTab);
}

/************************************************************************//**
 * \brief Setting up PGN 130577 Message "Direction Data"
 * \ingroup group_msgSetUp
 * 
 * The purpose of this PGN is to group three fundamental vectors related
 * to vessel motion, speed and heading referenced to the water, speed and
 * course referenced to ground and current speed and flow direction.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param DataMode             Data mode, see \ref tN2kDataMode
 * \param CogReference         True or Magnetic
 * \param SID         Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                    \n
 *                    The sequence identifier field is used to tie different PGNs data together to same 
 *                    sampling or calculation time.
 * \param COG         Course Over Ground in radians
 * \param SOG         Speed Over Ground in m/s
 * \param Heading     Heading in radians
 * \param SpeedThroughWater    Speed through water in m/s 
 * \param Set                  Set in radians
 * \param Drift                drift in m/s
 */
void SetN2kPGN130577(tN2kMsg &N2kMsg, tN2kDataMode DataMode, tN2kHeadingReference CogReference, unsigned char SID, double COG, double SOG,
      double Heading, double SpeedThroughWater, double Set, double Drift);

/************************************************************************//**
 * \brief Setting up Message "Direction Data" - PGN 130577
 * \ingroup group_msgSetUp
 * 
 * Alias of PGN 130577. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kPGN130577
  */
inline void SetN2kDirectionData(tN2kMsg &N2kMsg, tN2kDataMode DataMode, tN2kHeadingReference CogReference, unsigned char SID, double COG, double SOG,
      double Heading, double SpeedThroughWater, double Set, double Drift){

   SetN2kPGN130577(N2kMsg,DataMode,CogReference,SID,COG,SOG,Heading,SpeedThroughWater,Set,Drift);
}

/************************************************************************//**
 * \brief Parsing the content of message PGN 130577 "Direction Data"
 * \ingroup group_msgParsers
 * 
 * The purpose of this PGN is to group three fundamental vectors related
 * to vessel motion, speed and heading referenced to the water, speed and
 * course referenced to ground and current speed and flow direction.
 * 
 * \param N2kMsg      Reference to a N2kMsg Object, 
 *                    Output: NMEA2000 message ready to be send.
 * \param DataMode             Data mode, see \ref tN2kDataMode
 * \param CogReference         True or Magnetic
 * \param SID         Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param COG         Course Over Ground in radians
 * \param SOG         Speed Over Ground in m/s
 * \param Heading     Heading in radians
 * \param SpeedThroughWater    Speed through water in m/s 
 * \param Set                  Set in radians
 * \param Drift                drift in m/s
 * 
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 */
bool ParseN2kPGN130577(const tN2kMsg &N2kMsg,tN2kDataMode &DataMode, tN2kHeadingReference &CogReference,unsigned char &SID,double &COG,
      double &SOG,double &Heading,double &SpeedThroughWater,double &Set,double &Drift);

/************************************************************************//**
 * \brief Parsing the content of a "Direction Data" 
 *        message - PGN 130577
 * \ingroup group_msgParsers
 * 
 * Alias of PGN 130577. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kPGN130577 
 */
inline bool ParseN2kDirectionData(const tN2kMsg &N2kMsg,tN2kDataMode &DataMode, tN2kHeadingReference &CogReference,unsigned char &SID,double &COG,
      double &SOG,double &Heading,double &SpeedThroughWater,double &Set,double &Drift) {

   return ParseN2kPGN130577(N2kMsg,DataMode,CogReference,SID,COG,SOG,Heading,SpeedThroughWater,Set,Drift);
}


#endif
