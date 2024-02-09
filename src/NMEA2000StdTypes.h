/*
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
/*************************************************************************//**
 * \file  NMEA2000StdTypes.h
 * \brief NMEA2000 standard type definitions.
 * 
 *
 * This file contains (and must only contain) type definitions of data types
 * defined in the NMEA2000 standard.  These all have names of the form "DDnnn".
 * The language of the definitions should follow exactly that of the standard
 * document from which the information is drawn.  No abbreviated names. No
 * mangled capitalisation.

 * The entries here are mostly enums describing bit field types. Where an enum
 * member has multiple meanings, each meaning is aliased (see tN2kDD0002 for an
 * example).

 * This file is included by N2kTypes.h where the standard code names are aliased
 * into corresponding long names (once again, exactly as described by NMEA). 
 * 
 * 
 *  Resources NMEA 2000 ®  Appendix B.2 -- Data Dictionary
 * 
 *  - https://hobbydocbox.com/Radio/106823300-Data-dictionary-report-by-number-national-marine-electronics-association-nmea-appendix-b-2-data-dictionary.html
 *  - https://www.nmea.org/Assets/20190613%20thruster%20amendment%20128006,%20128007,%20128008.pdf
 *  - https://www.nmea.org/Assets/20190613%20windlass%20amendment,%20128776,%20128777,%20128778.pdf 
 */

#ifndef _NMEA2000StdTypes_H_
#define _NMEA2000StdTypes_H_

#include <stdint.h>

/** @{ \ingroup  group_msgTypes */

/*************************************************************************//**
 * \enum tN2kDD002
 * \brief DD002 - Generic Status Pair
 * 
 * DD002     Generic status pair    DF52        2bit     MSB/LSB: 
 * - 00 = [No, Off, Disabled, Reset, "0"]
 * - 01 = [Yes, On, Enabled, Set, "1"]
 * - 10 = Error
 * - 11= [Unavailable, Unknown]
 */
enum tN2kDD002 {
                  N2kDD002_No=0,                          ///< paired status is NO
                  N2kDD002_Yes=1,                         ///< paired status is YES
                  N2kDD002_Error=2,                       ///< paired status is Error
                  N2kDD002_Unavailable=3,                 ///< paired status is UNKNOWN
                  N2kDD002_Off=N2kDD002_No,               ///< paired status is NO
                  N2kDD002_Disabled=N2kDD002_No,          ///< paired status is NO
                  N2kDD002_Reset=N2kDD002_No,             ///< paired status is NO
                  N2kDD002_0=N2kDD002_No,                 ///< paired status is NO
                  N2kDD002_On=N2kDD002_Yes,               ///< paired status is YES
                  N2kDD002_Enabled=N2kDD002_Yes,          ///< paired status is YES
                  N2kDD002_Set=N2kDD002_Yes,              ///< paired status is YES
                  N2kDD002_1=N2kDD002_Yes,                ///< paired status is YES
                  N2kDD002_Unknown=N2kDD002_Unavailable   ///< paired status is UNKNOWN
                };

/*************************************************************************//**
 * \enum tN2kDD025
 * \brief DD025 - Mode, Data
 * 
 * DD025 - Mode, Data          DF52          4bits
 * - 0x0 = Autonomous mode
 * - 0x1 = Differential, enhanced mode 
 * - 0x2 = Estimated mode 
 * - 0x3 = Simulator mode
 * - 0x4 = Manual mode
 * - 0x5 to 0xD = Reserved, 
 * - 0xE = Error
 * - 0xF = Data not available
 */
enum tN2kDD025 {
                  N2kDD025_Autonomous=0,    ///< autonomous mode
                  N2kDD025_Differential=1,  ///< differential mode
                  N2kDD025_Estimated=2,     ///< estimated mode
                  N2kDD025_Simulator=3,     ///< simulator mode
                  N2kDD025_Manual=4,        ///< manual mode
                  N2kDD025_Error=0xe,       ///< error occurred
                  N2kDD025_Unavailable=0xf  ///< data not available
                };

/*************************************************************************//**
 * \enum tN2kDD072
 * \brief  DD072 - Range Residual Mode
 * 
 * DD072 Range Residual Mode     DF52       2bits
 * Range Residual used in position calculation or range residuals were calculated after the position.  
 * - 0=range residuals were used to calculate data
 * - 1=range residuals were calculated after the position.
 * - 2=Error
 * - 3=Null
 */
enum tN2kDD072 {
                  N2kDD072_RangeResidualsWereUsedToCalculateData=0,       ///< range residuals were used to calculate data
                  N2kDD072_RangeResidualsWereCalculatedAfterPosition=1,   ///< range residuals were calculated after the position
                  N2kDD072_Error=2,                                       ///< error occurred
                  N2kDD072_Unavailable=3                                  ///< data not available
                };

/*************************************************************************//**
 * \enum  tN2kDD124
 * \brief DD124 - PRN Usage Status
 * 
 * DD124  PRN Usage Status       DF52       4bit 
 * - 0 = Not Tracked
 * - 1 = Tracked but not used in solution
 * - 2 = Used in solution without Differential corrections
 * - 3 = Differential Corrections available
 * - 4 = Tracked with Differential Corrections
 * - 5 = used with Differential Corrections
 * - 6-13 =Reserved
 * - 14 = Error
 * - 15 = No Selection
 */
enum tN2kDD124 {
                  N2kDD124_NotTracked=0,                                      ///< Not Tracked
                  N2kDD124_TrackedButNotUsedInSolution=1,                     ///< Tracked but not used in solution
                  N2kDD124_UsedInSolutionWithoutDifferentialCorrections=2,    ///< Used in solution without Differential corrections
                  N2kDD124_DifferentialCorrectionsAvailable=3,                ///< Differential Corrections available
                  N2kDD124_TrackedWithDifferentialCorrections=4,              ///< Tracked with Differential Corrections
                  N2kDD124_UsedWithDifferentialCorrections=5,                 ///< used with Differential Corrections
                  N2kDD124_Error=14,                                          ///< error occurred
                  N2kDD124_Unavailable=15,                                    ///< no selection, unavailable
                };

/*************************************************************************//**
 * \union tN2kDD206
 * \brief DD206 - Engine Discrete Warning Status 
 * 
 * This status can be used to check the actual status of the boat engine.
 * 
 * DD206  Engine Discrete Warning Status DF52       16bit
 * - xxxx xxxx  xxxx xxx1 = Check Engine, 
 * - xxxx xxxx  xxxx xx1x = Over Temperature, 
 * - xxxx xxxx  xxxx x1xx = Low Oil Pressure,
 * - xxxx xxxx  xxxx 1xxx = Low Oil Level,
 * - xxxx xxxx  xxx1 xxxx = Low Fuel Pressure,
 * - xxxx xxxx  xx1x xxxx = Low System Voltage,
 * - xxxx xxxx  x1xx xxxx = Low Coolant Level,
 * - xxxx xxxx  1xxx xxxx = Water Flow,
 * - xxxx xxx1  xxxx xxxx = Water in Fuel,
 * - xxxx xx1x  xxxx xxxx = Charge Indicator,
 * - xxxx x1xx  xxxx xxxx = Preheat Indicator,
 * - xxxx 1xxx  xxxx xxxx = High Boost Pressure,
 * - xxx1 xxxx  xxxx xxxx = Rev Limit Exceeded, 
 * - xx1x xxxx  xxxx xxxx = EGR System,
 * - x1xx xxxx  xxxx xxxx = Throttle Position Sensor,
 * - 1xxx xxxx  xxxx xxxx = Engine Emergency Stop Mode
 * 
 * where x = don't care
 */
union tN2kDD206 {
          uint16_t Status;                                      ///< 2byte status with all individual status bits included
          struct {
              uint16_t CheckEngine:1;                           ///< check engine status bit
              uint16_t OverTemperature:1;                       ///< over temperature status bit
              uint16_t LowOilPressure:1;                        ///< low oil pressure status bit
              uint16_t LowOilLevel:1;                           ///< low oil level status bit
              uint16_t LowFuelPressure:1;                       ///< low fuel pressure status bit
              uint16_t LowSystemVoltage:1;                      ///< low system voltage status bit
              uint16_t LowCoolantLevel:1;                       ///< low coolant status bit
              uint16_t WaterFlow:1;                             ///< water flow status bit
              uint16_t WaterInFuel:1;                           ///< water in fuel status bit
              uint16_t ChargeIndicator:1;                       ///< charge indicator status bit
              uint16_t PreheatIndicator:1;                      ///< pre heat indicator status bit
              uint16_t HighBoostPressure:1;                     ///< high boost pressure status bit  
              uint16_t RevLimitExceeded:1;                      ///< rev limit exceeded status bit
              uint16_t EGRSystem:1;                             ///< egr system status bit
              uint16_t ThrottlePositionSensor:1;                ///< throttle position sensor status bit
              uint16_t EngineEmergencyStopMode:1;               ///< engine emergency stop mode status bit
          } Bits;
          tN2kDD206(uint16_t _Status=0): Status(_Status) {};
};

/*************************************************************************//**
 * \union tN2kDD223
 * \brief DD223 - Engine Discrete Warning Status
 * 
 * This status can be used to check the actual status of the boat engine.
 * 
 * DD223 - Engine Discrete Warning Status        DF52   16bit
 * - xxxx xxxx xxxx xxx1 = Warning Level 1, 
 * - xxxx xxxx xxxx xx1x = Warning Level 2,
 * - xxxx xxxx xxxx x1xx = Power Reduction,
 * - xxxx xxxx xxxx 1xxx = Maintenance Needed,
 * - xxxx xxxx xxx1 xxxx = Engine Comm Error,
 * - xxxx xxxx xx1x xxxx = Sub or Secondary Throttle,
 * - xxxx xxxx x1xx xxxx = Neutral Start Protect,
 * - xxxx xxxx 1xxx xxxx = Engine Shutting Down,
 * - xxxx xxx1 xxxx xxxx = reserved,
 * - xxxx xx1x xxxx xxxx = reserved,
 * - xxxx x1xx xxxx xxxx = reserved,
 * - xxxx 1xxx xxxx xxxx = reserved,
 * - xxx1 xxxx xxxx xxxx = reserved,
 * - xx1x xxxx xxxx xxxx = reserved,
 * - x1xx xxxx xxxx xxxx = reserved,
 * - 1xxx xxxx xxxx xxxx = reserved,
 *
 * where x = don't care
 */
union tN2kDD223 {
          uint16_t Status;                          ///< 2byte status with all individual status bits included
          struct {
              uint16_t WarningLevel1:1;             ///< engine warning level 1 status bit
              uint16_t WarningLevel2:1;             ///< engine warning level 2 status bit
              uint16_t LowOiPowerReduction:1;       ///< engine power reduction due to low oil pressure status bit
              uint16_t MaintenanceNeeded:1;         ///< engine maintenance needed status bit
              uint16_t EngineCommError:1;           ///< engine communication error status bit
              uint16_t SubOrSecondaryThrottle:1;    ///< sub or secondary throttle is used status bit
              uint16_t NeutralStartProtect:1;       ///< neutral start protection status bit
              uint16_t EngineShuttingDown:1;        ///< engine shuting down status bit
              uint16_t Manufacturer1:1;             ///< reserved
              uint16_t Manufacturer2:1;             ///< reserved
              uint16_t Manufacturer3:1;             ///< reserved
              uint16_t Manufacturer4:1;             ///< reserved
              uint16_t Manufacturer5:1;             ///< reserved
              uint16_t Manufacturer6:1;             ///< reserved
              uint16_t Manufacturer7:1;             ///< reserved
              uint16_t Manufacturer8:1;             ///< reserved
          } Bits;
          tN2kDD223(uint16_t _Status=0): Status(_Status) {};
          uint16_t operator= (uint16_t val) { Status=val & 0x00ff; return Status;}
};

/*************************************************************************//**
 * \enum tN2kDD305
 * \brief Enumeration of Aid to Navigation (AtoN) Type according to PGN129041
 * 
 * Type of AtoN.
 * 
 * Message 21: Aids-to-navigation report (AtoN)
 * - https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1371-5-201402-I!!PDF-E.pdf (Table74 p.133)
 * 
 * ITU-R M.1371-4
 * This message should be used by an AtoN AIS station. This station may be mounted on an
 * aid-to-navigation or this message may be transmitted by a fixed station when the functionality of an
 * AtoN station is integrated into the fixed station. This message should be transmitted 
 * autonomously at a Rr of once every three (3) min or it may be assigned by an assigned mode 
 * command (Message16) via the VHF data link, or by an external command. This message should 
 * not occupy more than two slots
 */
enum tN2kDD305 {
          N2kAISAtoN_not_specified=0,                       ///< Default, type of AtoN not specified
          N2kAISAtoN_reference_point=1,                     ///< AtoN type reference point
          N2kAISAtoN_RACON=2,                               ///< AtoN type RACON
          N2kAISAtoN_fixed_structure=3,                     ///< AtoN type Fixed structures off-shore, such as oil platforms, wind farms
          N2kAISAtoN_emergency_wreck_marking_buoy=4,        ///< AtoN type Emergency Wreck Marking Buoy
          N2kAISAtoN_light_without_sectors=5,               ///< Fixed AtoN type Light, without sectors
          N2kAISAtoN_light_with_sectors=6,                  ///< Fixed AtoN type Light, with sectors
          N2kAISAtoN_leading_light_front=7,                 ///< Fixed AtoN type Leading Light Front
          N2kAISAtoN_leading_light_rear=8,                  ///< Fixed AtoN type Leading Light Rear
          N2kAISAtoN_beacon_cardinal_N=9,                   ///< Fixed AtoN type Beacon, Cardinal N
          N2kAISAtoN_beacon_cardinal_E=10,                  ///< Fixed AtoN type Beacon, Cardinal E
          N2kAISAtoN_beacon_cardinal_S=11,                  ///< Fixed AtoN type Beacon, Cardinal S
          N2kAISAtoN_beacon_cardinal_W=12,                  ///< Fixed AtoN type Beacon, Cardinal W
          N2kAISAtoN_beacon_port_hand=13,                   ///< Fixed AtoN type Beacon, Port hand
          N2kAISAtoN_beacon_starboard_hand=14,              ///< Fixed AtoN type Beacon, starboard hand
          N2kAISAtoN_beacon_preferred_ch_port_hand=15,      ///< Fixed AtoN type Beacon, Preferred Channel port hand
          N2kAISAtoN_beacon_preferred_ch_starboard_hand=16, ///< Fixed AtoN type Beacon, Preferred Channel starboard hand
          N2kAISAtoN_beacon_isolated_danger=17,             ///< Fixed AtoN type Beacon, Isolated danger
          N2kAISAtoN_beacon_safe_water=18,                  ///< Fixed AtoN type Beacon, Safe water
          N2kAISAtoN_beacon_special_mark=19,                ///< Fixed AtoN type Beacon, Special mark
          N2kAISAtoN_cardinal_mark_N=20,                    ///< Floating AtoN type Cardinal Mark N
          N2kAISAtoN_cardinal_mark_E=21,                    ///< Floating AtoN type Cardinal Mark E
          N2kAISAtoN_cardinal_mark_S=22,                    ///< Floating AtoN type Cardinal Mark S
          N2kAISAtoN_cardinal_mark_W=23,                    ///< Floating AtoN type Cardinal Mark W
          N2kAISAtoN_port_hand_mark=24,                     ///< Floating AtoN type Port hand Mark
          N2kAISAtoN_starboard_hand_mark=25,                ///< Floating AtoN type Starboard hand Mark
          N2kAISAtoN_preferred_channel_port_hand=26,        ///< Floating AtoN type Preferred Channel Port hand
          N2kAISAtoN_preferred_channel_starboard_hand=27,   ///< Floating AtoN type Preferred Channel Starboard hand
          N2kAISAtoN_isolated_danger=28,                    ///< Floating AtoN type Isolated danger
          N2kAISAtoN_safe_water=29,                         ///< Floating AtoN type Safe Water
          N2kAISAtoN_special_mark=30,                       ///< Floating AtoN type Special Mark
          N2kAISAtoN_light_vessel_lanby_rigs=31,            ///< Floating AtoN type Light Vessel/LANBY/Rigs
};

/*************************************************************************//**
 * \enum tN2kDD374
 * \brief Measurement delay source
 * 
 */
enum tN2kDD374 {
          /** \brief Data is user defined  */
          N2kDD374_UserDefined=0,
          /** \brief Data is a defined minimum */
          N2kDD374_MinimumDefined=1,
          /** \brief Data is Factory Default */
          N2kDD374_FactoryDefault=2,
          /** \brief Maximum  */
          N2kDD374_MaximumSupported=3,
          /** \brief Data is out of range */
          N2kDD374_OutOfRange=14,
          /** \brief Data is not available */
          N2kDD374_DataNotAvailable=15
};

/*************************************************************************//**
 * \union tN2kDD471
 * \brief DD471 - Thruster Motor Events according to PGN 128008
 * 
 * DD471 Thruster Motor Events     DF52 8bit
 * 
 * - 0b0000 0000 = No errors present
 * - 0bxxxx xxx1 = Motor over temperature cutout
 * - 0bxxxx xx1x = Motor over current cutout
 * - 0bxxxx x1xx = Low oil level warning
 * - 0bxxxx 1xxx = Oil over temperature warning
 * - 0bxxx1 xxxx = Controller under voltage cutout
 * - 0bxx1x xxxx = Manufacturer defined
 * - 0bx1xx xxxx = Reserved
 * - 0b1xxx xxxx = Data Not Available
 * 
 * 
 * Events are cleared when the condition is no longer true or by manually resetting using the Command Group Function
 */
union tN2kDD471 {
	unsigned char Events;                             ///< 1byte with all Thruster Motor Events, 0x00 -> No errors present
	struct {
		unsigned char MotorOverTemperatureCutout:1;     ///< status bit for Motor over temperature cutout
		unsigned char MotorOverCurrentCutout:1;         ///< status bit for Motor over current cutout
		unsigned char LowOilLevelWarning:1;             ///< status bit for Low oil level warning
		unsigned char OilOverTemperatureWarning:1;      ///< status bit for Oil over temperature warning
		unsigned char ControllerUnderVoltageCutout:1;   ///< status bit for Controller under voltage cutout
		unsigned char ManufacturerDefined:1;            ///< Manufacturer defined
		unsigned char Reserved:1;                       ///< Reserved 
		unsigned char DataNotAvailable:1;               ///< Data Not Available
	} Event;
	tN2kDD471(): Events(0) {};
	void SetEvents(unsigned char _Events) { Events = (_Events & 0xbf); }
};

/*************************************************************************//**
 * \enum tN2kDD473
 * \brief DD473 - Thruster Direction Control according to PGN 128006
 * 
 * DD473 - Thruster Direction Control   DF52    4bit
 * - 0 = OFF
 * - 1 = Thruster Ready
 * - 2 = Thruster to PORT
 * - 3 = Thruster to STARBOARD
 * - 4-15 = Reserved
 * 
 */
enum tN2kDD473 {
	N2kDD473_OFF=0,                     ///< Thruster is Off
	N2kDD473_ThrusterReady=1,           ///< Truster is Ready
	N2kDD473_ThrusterToPORT=2,          ///< Truster to port
	N2kDD473_ThrusterToSTARBOARD=3      ///< Thruster to starboard
};

/*************************************************************************//**
 * \enum  tN2kDD474
 * \brief DD474 - Thruster Retraction according to PGN 128006
 * 
 * DD474 - Thruster Retraction    DF52   2bit
 * - 0 = OFF
 * - 1 = Extend
 * - 2 = Retract
 * - 3 = Reserved 
 * 
 */
enum tN2kDD474 {
	N2kDD474_OFF=0,             ///< Thruster retraction Off
	N2kDD474_Extend=1,          ///< Thruster retraction Extend
	N2kDD474_Retract=2,         ///< Thruster retraction Retract
    N2kDD474_Reserved=3         ///< Thruster retraction  Reserved
};

/*************************************************************************//**
 * \union tN2kDD475
 * \brief DD475 - Thruster Control Events according to PGN 128006
 * 
 * DD475 - Thruster Control Events    DF52    8bit
 * - 0b00000000 = No errors /events present
 * - 0bxxxxxxx1 = Another device controlling thruster
 * - 0bxxxxxx1x = Boat speed too fast to safely use thruster
 * - 0bxxxxx1xx to 0b1xxxxxxx = Reserved
 
 */
union tN2kDD475 {
	unsigned char Events;               ///< 1byte of all Thruster Control Events, 0x00 would be NoErrors
	struct {
		unsigned char AnotherDeviceControllingThruster:1;   ///< Another device is controlling thruster
		unsigned char BoatSpeedTooFast:1;                   ///< Boat speed too fast to safely use thruster
	} Event;
	tN2kDD475(): Events(0) {};
	void SetEvents(unsigned char _Events) { Events = (_Events & 0x03); }
};

/*************************************************************************//**
 * \union tN2kDD477
 * \brief DD477 - Windlass Monitoring Events according to PGN 128778
 * 
 * DD477 - Windlass Monitoring Events   DF52    8bit
 * - 0b0000 0000 = No errors present
 * - 0bxxxx xxx1 = Controller under voltage cut-out
 * - 0bxxxx xx1x = Controller over current cut-out
 * - 0bxxxx x1xx = Controller over temperature cut-out
 * - 0bxxxx 1xxx = Manufacturer defined
 * 
 */
union tN2kDD477 {
          unsigned char Events;     ///< 1byte of all Windlass Monitoring Events, 0x00 would be NoErrors present
          struct {
            unsigned char ControllerUnderVoltageCutout:1;       ///< Controller under voltage cut-out
            unsigned char ControllerOverCurrentCutout:1;        ///< Controller over current cut-out
            unsigned char ControllerOverTemperatureCutout:1;    ///< Controller over temperature cut-out
          } Event;
          tN2kDD477(): Events(0) {};
          void SetEvents(unsigned char _Events) { Events = (_Events & 0x07); }
        };

/*************************************************************************//**
 * \union tN2kDD478
 * \brief DD478 - Windlass Control Events according to PGN 128776
 * 
 * DD478 - Windlass Control Events    DF52      4bit
 * - 0b0000 = No errors present
 * - 0bxxx1 = Another device controlling windlass
 * - 0bxx1x = Reserved
 * - 0bx1xx = Reserved
 * - 0b1xxx = Reserved
 */
union tN2kDD478 {
          unsigned char Events;     ///< 1byte of all Windlass Control Events, 0x00 would be NoErrors present
          struct {
            unsigned char AnotherDeviceControllingWindlass:1;     ///< Another device controlling windlass
          } Event;
          tN2kDD478(): Events(0) {};
          void SetEvents(unsigned char _Events) { Events = (_Events & 0x01); }
        };

/*************************************************************************//**
 * \enum  tN2kDD480
 * \brief DD480 - Windlass Motion States according to PGN 128777
 * 
 * DD480 - Windlass Motion States   DF52      2bit
 * - 0 = Windlass stopped
 * - 1 = Deployment occurring
 * - 2 = Retrieval occurring
 * - 3 = Unavailable
 * 
 */                          
enum tN2kDD480 {
                N2kDD480_WindlassStopped=0,       ///< Windlass stopped
                N2kDD480_DeploymentOccurring=1,   ///< Deployment occurring
                N2kDD480_RetrievalOccurring=2,    ///< Retrieval occurring
                N2kDD480_Unavailable=3            ///< Unavailable
              };

/*************************************************************************//**
 * \enum tN2kDD481
 * \brief DD481 - Rode Type States according to PGN 128777
 * 
 * DD481 - Rode Type States   Df52    2bit
 * - 0 = Chain presently detected
 * - 1 = Rope presently detected
 * - 2 = Error
 * - 3 = Unavailable
 */            
enum tN2kDD481 {
                N2kDD481_ChainPresentlyDetected=0,  ///< Chain presently detected
                N2kDD481_RopePresentlyDetected=1,   ///< Rope presently detected
                N2kDD481_Error=2,                   ///< Error
                N2kDD481_Unavailable=3              ///< Unavailable
              };

/*************************************************************************//**
 * \enum  tN2kDD482
 * \brief DD482 - Anchor Docking States according to PGN 128777
 * 
 * DD482 - Anchor Docking States      DF52      2bit
 * - 0 = Not docked
 * - 1 = Fully docked
 * - 2 = Error
 * - 3 = Data not available
 */
enum tN2kDD482 {
                N2kDD482_NotDocked=0,         ///< Not docked
                N2kDD482_FullyDocked=1,       ///< Fully docked
                N2kDD482_Error=2,             ///< Error
                N2kDD482_DataNotAvailable=3   ///< Data not available
              };

/*************************************************************************//**
 * \union tN2kDD483
 * \brief DD483 - Windlass Operating Events according to PGN 128777
 * 
 * DD483 - Windlass Operating Events        DF52      6bit
 * - 0b000000 = No errors / events present
 * - 0bxxxxx1 = System error
 * - 0bxxxx1x = Sensor error
 * - 0bxxx1xx = No windlass motion detected
 * - 0bxx1xxx = Retrieval docking distance reached (retrieving anchor is nearing the boat, so more care can be taken to complete docking)
 * - 0bx1xxxx = End of rode reached (rode has been fully deployed)
 * - 0b1xxxxx = Reserved
 */
union tN2kDD483 {
          unsigned char Events;     ///< 1byte of all Windlass Operating Events, 0x00 would be NoErrors present
          struct {
            unsigned char SystemError:1;                        ///< System error
            unsigned char SensorError:1;                        ///< Sensor error
            unsigned char NoWindlassMotionDetected:1;           ///< No windlass motion detected
            unsigned char RetrievalDockingDistanceReached:1;    ///< Retrieval docking distance reached (retrieving anchor is nearing the boat, so more care can be taken to complete docking)
            unsigned char EndOfRodeReached:1;                   ///< End of rode reached (rode has been fully deployed)
          } Event;
          tN2kDD483(): Events(0) {};
          void SetEvents(unsigned char _Events) { Events = (_Events & 0x1F); }
        };                         

/*************************************************************************//**
 * \enum  tN2kDD484
 * \brief DD484 - Windlass Direction Control according to PGN 128776
 * 
 * DD484 - Windlass Direction Control 
 * - 0 = OFF (Status Only / cannot command)
 * - 1 = DOWN
 * - 2 = UP
 * - 3 = Reserved
 */            
enum tN2kDD484 {
                N2kDD484_Off=0,       ///< Status only / cannot command
                N2kDD484_Down=1,      ///< Windlass DOWN
                N2kDD484_Up=2,        ///< Windlass UP
                N2kDD484_Reserved=3   ///< Reserved
                };

/*************************************************************************//**
 * \enum  tN2kDD487
 * \brief DD487 - Motor Power Type according to PGN 128007
 * 
 * DD487 - Motor Power Type          DF52     4bit
 * - 0 = 12VDC
 * - 1 = 24VDC
 * - 2 = 48VDC
 * - 3 = 24VAC
 * - 4 = Hydraulic
 * - 5-15 = Reserved for future assignment
 */
enum tN2kDD487 {
                N2kDD487_12VDC=0,       ///< Motor Power Type is 12VDC
                N2kDD487_24VDC=1,       ///< Motor Power Type is 24VDC
                N2kDD487_48VDC=2,       ///< Motor Power Type is 48VDC
                N2kDD487_24VAC=3,       ///< Motor Power Type is 24VAC
                N2kDD487_Hydraulic=4    ///< Motor Power Type is hydraulic
                };

/*************************************************************************//**
 * \enum  tN2kDD488
 * \brief DD488 - Speed Control Type according to PGN 128776
 * 
 * DD488 - Speed Control Type according to PGN 128776
 * - 0 = Single Speed
 * - 1 = Dual Speed
 * - 2 = Proportional Speed
 * - 3 = Data Not Available
 */          
enum tN2kDD488 {
                N2kDD488_SingleSpeed=0,         ///< Single Speed
                N2kDD488_DualSpeed=1,           ///< Dual Speed
                N2kDD488_ProportionalSpeed=2,   ///< Proportional Speed
                N2kDD488_DataNotAvailable=3     ///< Data Not Available
                };

#endif

/** @} */
