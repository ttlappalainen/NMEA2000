/*
 * Copyright (c) 2019-2024 Timo Lappalainen, Kave Oy, www.kave.fi
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
 * \file  N2kTypes.h
 * \brief This File contains all specific Enumerations to make NMEA2000 
 *        Messages easier for humans to read.
 * 
 * Each NMEA2000 message has a specific CanID (address) which consists a
 * PGN Number (eg. PGN127802) and carries 8byte of raw data. For every PGN
 * exists a detailed description how these 8 bytes transfer into usable values.
 * To save bandwidth not all of values have the size of 1byte. All sizes from
 * 1bit to 64bits are allowed. The Enumerations of this file make these values
 * easier for humans to read, and are used in the corresponding SetXXX and
 * ParseXXX functions of \ref N2kMessages.h .
 */

/** @{ \ingroup  group_msgTypes */

#ifndef _N2kTypes_H_
#define _N2kTypes_H_

#include "NMEA2000StdTypes.h"

/*************************************************************************//**
 * \enum tN2kNavigationDirection
 * \brief Enumeration of the navigation direction according to PGN129285
 */
enum tN2kNavigationDirection {
                            N2kdir_forward=0,     ///< navigating forward
                            N2kdir_reverse=1,     ///< navigating reverse
                            N2kdir_reserved1=2,   ///< reserved 1
                            N2kdir_reserved2=3,   ///< reserved 2
                            N2kdir_reserved3=4,   ///< reserved 3
                            N2kdir_reserved4=5,   ///< reserved 4
                            N2kdir_error=6,       ///< error occurred
                            N2kdir_unknown=7      ///< unknown
                              };
/*************************************************************************//**
 * \enum tN2kHeadingReference
 * \brief Enumeration of the heading type according to PGN129233, PGN129237 ,
 * PGN129250, PGN129026 and PGN129284
 */
enum tN2kHeadingReference {
                            N2khr_true=0,         ///< heading true (eg. GNSS) direction 
                            N2khr_magnetic=1,     ///< heading magnetic compass direction
                            N2khr_error=2,        ///< error occurred
                            N2khr_Unavailable=3   ///< unavailable
                          };

/*************************************************************************//**
 * \enum  tN2kDistanceCalculationType
 * \brief Enumeration of distance calculation types for navigation according to
 * PGN129284
 * 
 * - A great circle is the shortest path between two points along the surface
 *   of a sphere.
 * - A rhumb line is a curve that crosses each meridian at the same angle. This 
 *   curve is also referred to as a loxodrome (from the Greek loxos, slanted,
 *   and drome, path). Although a great circle is a shortest path, it is 
 *   difficult to navigate because your bearing (or azimuth) continuously 
 *   changes as you proceed. Following a rhumb line covers more distance than 
 *   following a geodesic, but it is easier to navigate.
 */
enum tN2kDistanceCalculationType {
                            N2kdct_GreatCircle=0,     ///< great circle calculation
                            N2kdct_RhumbLine=1        ///< rhumb line calculation
                          };
/*************************************************************************//**
 * \enum  tN2kXTEMode
 * \brief Enumeration of Cross Track Error modes according to PGN129283
 * 
 * Cross-track error (XTE) is an essential at-a-glance indication of your
 * distance to port or starboard of your intended straight track between two
 * waypoints.
 */                          
enum tN2kXTEMode  {
                            N2kxtem_Autonomous=0,     ///< autonomous mode
                            N2kxtem_Differential=1,   ///< differential mode
                            N2kxtem_Estimated=2,      ///< estimated mode
                            N2kxtem_Simulator=3,      ///< simulator mode
                            N2kxtem_Manual=4          ///< manual mode
                          };

/*************************************************************************//**
 * \enum  tN2kGNSStype
 * \brief Enumeration of the GPS system which is in use according to PGN129029,
 * PGN129041 and PGN129794
 */
enum tN2kGNSStype {
                            N2kGNSSt_GPS=0,                   ///< only GPS satellite network
                            N2kGNSSt_GLONASS=1,               ///< only the GLONASS satellite network
                            N2kGNSSt_GPSGLONASS=2,            ///< GPS + GLONASS satellite network
                            N2kGNSSt_GPSSBASWAAS=3,           ///< GPS + WAAS correction satellite network
                            N2kGNSSt_GPSSBASWAASGLONASS=4,    ///< GPS + GLONASS + WAAS correction satellite network
                            N2kGNSSt_Chayka=5,                ///< Russia's Chayka system is like the erstwhile LORAN-C radio navigation architecture
                            N2kGNSSt_integrated=6,            ///< integrated mode
                            N2kGNSSt_surveyed=7,              ///< surveyed mode
                            N2kGNSSt_Galileo=8                ///< European Galileo satellite network
                          };
/*************************************************************************//**
 * \enum tN2kGNSSmethod
 * \brief Enumeration of GNSS working methods according to PGN129029
 * 
 * These working methods of the GNSS determine the precision of the GNSS position
 * 
 */                          
enum tN2kGNSSmethod {
                            N2kGNSSm_noGNSS=0,        ///< no GNSS available
                            N2kGNSSm_GNSSfix=1,       ///< GNSS is fixed and available
                            N2kGNSSm_DGNSS=2,         ///< differential GNSS is available
                            N2kGNSSm_PreciseGNSS=3,   ///< precise GNSS is available
                            N2kGNSSm_RTKFixed=4,      ///< GNSS with RealTimeKinematic (fixed) is available
                            N2kGNSSm_RTKFloat=5,      ///< GNSS with RealTimeKinematic (float) is available
                            N2kGNSSm_Error=14,        ///< error occurred
                            N2kGNSSm_Unavailable=15   ///< GNSS is unavailable
                          };
/*************************************************************************//**
 * \enum tN2kGNSSDOPmode
 * \brief Enumeration of GNSS DOP Mode according to PGN129539
 * 
 * Dilution of precision (DOP), or geometric dilution of precision (GDOP), is
 * a term used in satellite navigation and geomatics engineering to specify the
 * error propagation as a mathematical effect of navigation satellite geometry
 * on positional measurement precision.
 * 
 * DOP can be expressed as a number of separate measurements:
 *
 *   - HDOP horizontal (1D) dilution of precision
 *   - VDOP vertical (2D) dilution of precision
 *   - PDOP position (3D) dilution of precision
 *   - TDOP time dilution of precision
 *   - GDOP geometric dilution of precision
 * 
 * https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)
 * 
 */
enum tN2kGNSSDOPmode {
                            N2kGNSSdm_1D=0,               ///< 1D dilution of precision
                            N2kGNSSdm_2D=1,               ///< 2D dilution of precision
                            N2kGNSSdm_3D=2,               ///< 3D dilution of precision
                            N2kGNSSdm_Auto=3,             ///< automatic dilution of precision
                            N2kGNSSdm_Reserved=4,         ///< reserved
                            N2kGNSSdm_Reserved2=5,        ///< reserved
                            N2kGNSSdm_Error=6,            ///< error occurred
                            N2kGNSSdm_Unavailable=7       ///< unavailable
                          };
/*************************************************************************//**
 * \enum tN2kTempSource
 * \brief Enumeration of sources for a temperature value according to
 * PGN130311, PGN130312 and PGN130316
 */
enum tN2kTempSource {
                            N2kts_SeaTemperature=0,                         ///< value is representing a sea water temperature
                            N2kts_OutsideTemperature=1,                     ///< value is representing an outside temperature
                            N2kts_InsideTemperature=2,                      ///< value is representing an inside temperature
                            N2kts_EngineRoomTemperature=3,                  ///< value is representing an engine room temperature 
                            N2kts_MainCabinTemperature=4,                   ///< value is representing a main cabin temperature 
                            N2kts_LiveWellTemperature=5,                    ///< value is representing a live well temperature
                            N2kts_BaitWellTemperature=6,                    ///< value is representing a bait well temperature
                            N2kts_RefridgerationTemperature=7,              ///< value is representing a refrigeration temperature
                            N2kts_HeatingSystemTemperature=8,               ///< value is representing a heating system temperature
                            N2kts_DewPointTemperature=9,                    ///< value is representing a dew point temperature
                            N2kts_ApparentWindChillTemperature=10,          ///< value is representing an apparent wind chill temperature
                            N2kts_TheoreticalWindChillTemperature=11,       ///< value is representing a theoretical wind chill temperature
                            N2kts_HeatIndexTemperature=12,                  ///< value is representing a heat index temperature
                            N2kts_FreezerTemperature=13,                    ///< value is representing a freezer temperature
                            N2kts_ExhaustGasTemperature=14,                 ///< value is representing a exhaust gas temperature
                            N2kts_ShaftSealTemperature=15					///< value is representing a shaft seal temperature
                          };
/*************************************************************************//**
 * \enum tN2kHumiditySource
 * \brief Enumeration of sources for a humidity value according to PGN130311
 * and PGN130313
 */
enum tN2kHumiditySource {
                            N2khs_InsideHumidity=0,       ///< humidity is measured inside
                            N2khs_OutsideHumidity=1,      ///< humidity is measured outside
                            N2khs_Undef=0xff              ///< source is undefined
                          };
/*************************************************************************//**
 * \enum tN2kPressureSource
 * \brief Enumeration of sources for a pressure value  according to PGN130314
 * and PGN130315
 */
 enum tN2kPressureSource {
                            N2kps_Atmospheric = 0,        ///< value is representing an atmospheric air pressure
                            N2kps_Water = 1,              ///< value is representing a water pressure
                            N2kps_Steam = 2,              ///< value is representing a steam pressure
                            N2kps_CompressedAir = 3,      ///< value is representing a compressed air pressure
                            N2kps_Hydraulic = 4,          ///< value is representing a hydraulic pressure
                            N2kps_Filter = 5,             ///< value is representing a filter pressure
                            N2kps_AltimeterSetting = 6,   ///< value is representing an altimeter setting
                            N2kps_Oil = 7,                ///< value is representing an oil pressure
                            N2kps_Fuel = 8,               ///< value is representing a fuel pressure
                            N2kps_Reserved = 253,         ///< reserved
                            N2kps_Error = 254,            ///< error occurred
                            N2kps_Unavailable = 255       ///< source is unavailable
                          };
/*************************************************************************//**
 * \enum  tN2kTimeSource
 * \brief Enumeration of sources for the system time according to PGN126992
 */
enum tN2kTimeSource {
                            N2ktimes_GPS=0,                 ///< Source is Global Positioning System GPS
                            N2ktimes_GLONASS=1,             ///< Source is Glonass
                            N2ktimes_RadioStation=2,        ///< Source is a radio station
                            N2ktimes_LocalCesiumClock=3,    ///< Source is a local cesium clock
                            N2ktimes_LocalRubidiumClock=4,  ///< Source is a local rubidium clock
                            N2ktimes_LocalCrystalClock=5    ///< Source is a local crystal clock
                          };
/*************************************************************************//**
 * \enum tN2kFluidType
 * \brief Enumeration of fluid types according to PGN127505
 */
enum tN2kFluidType {
                            N2kft_Fuel=0,             ///< fluid type is fuel
                            N2kft_Water=1,            ///< fluid type is water
                            N2kft_GrayWater=2,        ///< fluid type is gray water
                            N2kft_LiveWell=3,         ///< fluid type is live well
                            N2kft_Oil=4,              ///< fluid type is oil
                            N2kft_BlackWater=5,       ///< fluid type is black water
                            N2kft_FuelGasoline=6,     ///< fluid type is gasoline fuel
                            N2kft_Error=14,           ///< error occurred
                            N2kft_Unavailable=15      ///< unavailable
                          };

/*************************************************************************//**
 * \enum tN2kWindReference
 * \brief Enumeration of wind reference types according to PGN130306
 * 
 * Details found on page 12 of https://www.rocktheboatmarinestereo.com/specs/MSNRX200I.pdf
 */
enum tN2kWindReference {
                            N2kWind_True_North=0,     ///< Theoretical Wind (ground referenced, referenced to True North; calculated using COG/SOG)
                            N2kWind_Magnetic=1,       ///< Theoretical Wind (ground referenced, referenced to Magnetic North; calculated using COG/SOG)
                            N2kWind_Apparent=2,       ///< Apparent Wind (relative to the vessel centerline)
                            N2kWind_Apprent=2,        ///< Deprecated - We had the typo in older version of the library
                            N2kWind_True_boat=3,      ///< Theoretical (Calculated to Centerline of the vessel, referenced to ground; calculated using COG/SOG)
                            N2kWind_True_water=4,     ///< Theoretical (Calculated to Centerline of the vessel, referenced to water; calculated using Heading/Speed through Water)
                            N2kWind_Error=6,          ///< error occurred
                            N2kWind_Unavailable=7     ///< unavailable
                          };
/*************************************************************************//**
 * \enum tN2kSpeedWaterReferenceType
 * \brief Enumeration of speed thru water sensors types according to PGN128259
 */
enum tN2kSpeedWaterReferenceType {
                            N2kSWRT_Paddle_wheel=0,     ///< paddle wheel sensor is used
                            N2kSWRT_Pitot_tube=1,       ///< pitot tube sensor is used
                            N2kSWRT_Doppler_log=2,      ///< doppler sensor is used
                            N2kSWRT_Ultra_Sound=3,      ///< ultra sound sensor is used
                            N2kSWRT_Electro_magnetic=4, ///< electro magnetic sensor is used
                            N2kSWRT_Error=254,          ///< error occurred
                            N2kSWRT_Unavailable=255     ///< unavailable
                          };
/*************************************************************************//**
 * \enum tN2kRudderDirectionOrder
 * \brief Enumeration of Rudder Direction Order according to PGN127237, 127245
 */
enum tN2kRudderDirectionOrder {
                            N2kRDO_NoDirectionOrder=0,  ///< no direction order
                            N2kRDO_MoveToStarboard=1,   ///< move to starboard
                            N2kRDO_MoveToPort=2,        ///< move to port
                            N2kRDO_Unavailable=7        ///< unavailable
                          };
/*************************************************************************//**
 * \enum tN2kDCType
 * \brief Enumeration of DC power sources according to PGN 127506 
 * 
 * DC Type for PGN 127506 «DC Detailed Status». Legacy NMEA 2000 devices
 * may not support this PGN
 */
enum tN2kDCType {
                            N2kDCt_Battery=0,           ///< DC power from battery
                            N2kDCt_Alternator=1,        ///< DC power from alternator
                            N2kDCt_Converter=2,         ///< DC power from converter
                            N2kDCt_SolarCell=3,         ///< DC power from solar cell
                            N2kDCt_WindGenerator=4      ///< DC power from wind generator
                          };
/*************************************************************************//**
 * \enum tN2kBatType
 * \brief Enumeration of Battery types according to PGN 127513
 * 
 * Battery type in PGN 127513 «Battery Configuration Status» (for reporting
 * only).
 */
enum tN2kBatType  {
                            N2kDCbt_Flooded=0,          ///< battery type is an flooded lead acid 
                            N2kDCbt_Gel=1,              ///< battery type is GEL
                            N2kDCbt_AGM=2               ///< battery type is AGM
                          };
/*************************************************************************//**
 * \enum tN2kBatEqSupport
 * \brief Enumeration of Supports Equalization according to PGN 127513
 * 
 * «Supports Equalization» parameter in PGN 127513 «Battery Configuration
 * Status» (used for reporting only,not for calculations).
 */
enum tN2kBatEqSupport  {
                            N2kDCES_No=0,         ///< No, Off, Disabled
                            N2kDCES_Yes=1,        ///< Yes, On, Enabled
                            N2kDCES_Error=2,      ///< Error
                            N2kDCES_Unavailable=3 ///< Unavailable
                          };
/*************************************************************************//**
 * \enum tN2kBatChem
 * \brief Enumeration of battery chemistries according to PGN 127513
 * 
 * «Battery Chemistry» parameter in PGN 127513 «Battery Configuration Status».
 * Used for reporting, to choose default values of settings and for
 * calculations.
 */
enum tN2kBatChem {
                            N2kDCbc_LeadAcid=0,   ///< battery chemistry is lead acid
                            N2kDCbc_LiIon=1,      ///< battery chemistry is lithium ion
                            N2kDCbc_NiCad=2,      ///< battery chemistry is nickel cadmium
                            N2kDCbc_ZnO=3,		  ///< battery chemistry is zink oxide
                            N2kDCbc_NiMh=4        ///< battery chemistry is nickel metal hydride
                          };
/*************************************************************************//**
 * \enum tN2kBatNomVolt
 * \brief Enumeration of nominal battery voltage according to PGN 127513
 * 
 * Nominal voltage in PGN 127513 «Battery Configuration Status». 
 */
enum tN2kBatNomVolt {
                            N2kDCbnv_6v=0,    ///< nominal voltage is 6V
                            N2kDCbnv_12v=1,   ///< nominal voltage is 12V
                            N2kDCbnv_24v=2,   ///< nominal voltage is 24V
                            N2kDCbnv_32v=3,   ///< nominal voltage is 32V
                            N2kDCbnv_62v=4,   ///< nominal voltage is 62V
                            N2kDCbnv_42v=5,   ///< nominal voltage is 42V
                            N2kDCbnv_48v=6    ///< nominal voltage is 48V
                          };
/*************************************************************************//**
 * \enum tN2kTransmissionGear
 * \brief Enumeration of transmission state according to PGN 127493
 */
enum tN2kTransmissionGear {
                            N2kTG_Forward=0,  ///< transmission state is forward
                            N2kTG_Neutral=1,  ///< transmission state is neutral
                            N2kTG_Reverse=2,  ///< transmission state is reverse
                            N2kTG_Unknown=3,  ///< transmission state is unknown
                          };
/*************************************************************************//**
 * \enum tN2kAISRepeat
 * \brief Enumeration of AIS repeat indicator according to  PGN129802, PGN129809, PGN129810, PGN129038, PGN129041,PGN129794
 */
enum tN2kAISRepeat {
                            N2kaisr_Initial=0,  ///< initial AIS repeat
                            N2kaisr_First=1,    ///< first AIS repeat
                            N2kaisr_Second=2,   ///< second AIS repeat
                            N2kaisr_Final=3,    ///< final AIS repeat
                          };

/*************************************************************************//**
 * \enum tN2kAISVersion
 * \brief Enumeration of AIS version which is used in PGN 129794
 * 
 * M.1371 : Technical characteristics for an automatic identification system
 * using time division multiple access in the VHF maritime mobile frequency
 * band.  
 * 
 * - https://www.itu.int/rec/R-REC-M.1371
 * 
 * \note The newest standard available is 1371_5 (from 2014), maybe an update
 * is needed here
 */
enum tN2kAISVersion {
                            N2kaisv_ITU_R_M_1371_1=0,   ///< version ITU-R M.1371_1
                            N2kaisv_ITU_R_M_1371_3=1,   ///< version ITU-R M.1371_3
                          };

/*************************************************************************//**
 * \enum tN2kAISNavStatus
 * \brief Enumeration of navigational status of an ship sending out an AIS
 * signal according to PGN 129038
 */
enum tN2kAISNavStatus {
                            N2kaisns_Under_Way_Motoring=0,                  ///< Ship is under way and motoring
                            N2kaisns_At_Anchor=1,                           ///< Ship is at anchor
                            N2kaisns_Not_Under_Command=2,                   ///< Ship is not under command
                            N2kaisns_Restricted_Manoeuverability=3,         ///< Ship has a restricted manoeuverability 
                            N2kaisns_Constrained_By_Draught=4,              ///< Ship has constrains by drought
                            N2kaisns_Moored=5,                              ///< Ship is moored
                            N2kaisns_Aground=6,                             ///< Ship is aground
                            N2kaisns_Fishing=7,                             ///< Ship is fishing
                            N2kaisns_Under_Way_Sailing=8,                   ///< Ship is under way and sailing
                            N2kaisns_Hazardous_Material_High_Speed=9,       ///< Hazardous material in the water and moving with high speed
                            N2kaisns_Hazardous_Material_Wing_In_Ground=10,  ///< Hazardous material in the water and wing in ground
                            N2kaisns_AIS_SART=14,                           ///< AIS Search And Rescue Transmission
                          };
/*************************************************************************//**
 * \enum tN2kAISDTE
 * \brief Enumeration of Data Terminal Equipment according to PNG 129794
 */
enum tN2kAISDTE {
                            N2kaisdte_Ready=0,      ///< equipment is ready
                            N2kaisdte_NotReady=1,   ///< equipment is not ready
                          };
/*************************************************************************//**
 * \enum tN2kAISUnit
 * \brief Enumeration of AIS unit (transmission) type according to PGN 129039
 */
enum tN2kAISUnit {
                            N2kaisunit_ClassB_SOTDMA=0,   ///< AIS Unit uses SOTDMA (Self Organizing Time Division Multiple Access)
                            N2kaisunit_ClassB_CS=1,       ///< AIS unit uses CSTDMA (Carrier Sense Time Division Multiple Access)
                          };
/*************************************************************************//**
 * \enum tN2kAISMode
 * \brief Enumeration of AIS mode type according to PGN 129039
 * 
 * https://www.marineteacher.com/ais
 * 
 * There are 3 modes of operation of AIS:
 * Autonomous and continuous mode: In this mode AIS automatically defines its
 * own reporting rate in accordance with its navigational mode, speed and
 * course. The unit also selects its own time slots. This is normal mode for 
 * operation in all areas but the mode may be switched from/to Assigned or
 * Polled/Controlled mode by a competent authority via a base station on shore.
 * Assigned/ Controlled Mode: A competent authority responsible for traffic
 * monitoring may remotely set transmissions intervals and/or time slots for
 * the vessel mobile stations. In Assigned mode the mobile station does not
 * change its reporting rate when changing course and speed. Polled Mode: In
 * this mode, mobile station will automatically respond to interrogation
 * (polling) messages from a ship or competent authority. The response is
 * transmitted on the channel where the interrogation message was received.
 * Operation in Polled mode does not conflict with the operation in other two
 * modes.
 * 
 */
enum tN2kAISMode {
                            N2kaismode_Autonomous=0,    ///< AIS is in Autonomous mode
                            N2kaismode_Assigned=1,      ///< AIS is in assigned mode
                          };
/*************************************************************************//**
 * \enum tN2kAISTransceiverInformation
 * \brief Enumeration of 129039, 129041, 129802
 */
enum tN2kAISTransceiverInformation {
                           N2kaischannel_A_VDL_reception=0,         ///< Channel A VDL reception
                           N2kaischannel_B_VDL_reception=1,         ///< Channel B VDL reception
                           N2kaischannel_A_VDL_transmission=2,      ///< Channel A VDL transmission
                           N2kaischannel_B_VDL_transmission=3,      ///< Channel B VDL transmission
                           N2kaisown_information_not_broadcast=4    ///< Own information not broadcast
                          };
/*************************************************************************//**
 * \enum tN2kMagneticVariation
 * \brief Enumeration of magnetic variation according to PGN 127258
 * 
 * The World Magnetic Modell (WMM) can be found under 
 * https://www.ngdc.noaa.gov/geomag/WMM/WMM_old_reports.shtml 
 */
enum tN2kMagneticVariation {
                            N2kmagvar_Manual=0,     ///< manual entered magnetic deviation
                            N2kmagvar_Chart=1,      ///< magnetic deviation derived from the charts
                            N2kmagvar_Table=2,      ///< magnetic deviation derived from a table
                            N2kmagvar_Calc=3,       ///< magnetic deviation calculated
                            N2kmagvar_WMM2000=4,    ///< magnetic deviation derived from the WMM 2000 report
                            N2kmagvar_WMM2005=5,    ///< magnetic deviation derived from the WMM 2005 report
                            N2kmagvar_WMM2010=6,    ///< magnetic deviation derived from the WMM 2010 report
                            N2kmagvar_WMM2015=7,    ///< magnetic deviation derived from the WMM 2015 report
                            N2kmagvar_WMM2020=8,    ///< magnetic deviation derived from the WMM 2020 report
                          };
/*************************************************************************//**
 * \enum tN2kOnOff
 * \brief Enumeration of On/Off status at a NMEA 2000 network
 */
enum tN2kOnOff  {
                            N2kOnOff_Off=0,         ///< No, Off, Disabled
                            N2kOnOff_On=1,          ///< Yes, On, Enabled
                            N2kOnOff_Error=2,       ///< Error
                            N2kOnOff_Unavailable=3  ///< Unavailable
                          };

/*************************************************************************//**
 * \enum tN2kChargeState
 * \brief Enumeration of state of the battery charger operation according to
 * PGN127507
 */
enum tN2kChargeState  {
                            N2kCS_Not_Charging=0,     ///< Charger operation state is not charging
                            N2kCS_Bulk=1,             ///< Charger operation state is in bulk
                            N2kCS_Absorption=2,       ///< Charger operation state is in absorption
                            N2kCS_Overcharge=3,       ///< Charger operation state is in overcharge
                            N2kCS_Equalise=4,         ///< Charger operation state is in equalise
                            N2kCS_Float=5,            ///< Charger operation state is in float
                            N2kCS_No_Float=6,         ///< Charger operation state is in no float
                            N2kCS_Constant_VI=7,      ///< Charger operation state is in constant power
                            N2kCS_Disabled=8,         ///< Charger operation state is disabled
                            N2kCS_Fault=9,            ///< Charger operation state is in fault
                            N2kCS_Unavailable=15      ///< Charger operation state unavailable
                          };

/*************************************************************************//**
 * \enum tN2kChargerMode
 * \brief Enumeration of charger modes according to PGN 127507
 */
enum tN2kChargerMode {
                            N2kCM_Standalone=0,   ///< Charger mode is stand alone
                            N2kCM_Primary=1,      ///< Charger mode is primary charger
                            N2kCM_Secondary=2,    ///< Charger mode is secondary charger
                            N2kCM_Echo=3,         ///< Charger mode is echo
                            N2kCM_Unavailable=15  ///< Charger mode is unvailable 
                          };

/*************************************************************************//**
 * \enum tN2kSteeringMode
 * \brief Enumeration of the steering mode according to PGN127237
 */
enum tN2kSteeringMode {
                            N2kSM_MainSteering=0,               ///< The device is the main steering
                            N2kSM_NonFollowUpDevice=1,          ///< The device is a non followup device
                            N2kSM_FollowUpDevice=2,             ///< The device is a follup device
                            N2kSM_HeadingControlStandalone=3,   ///< The heading controlsystem is a standalone 
                            N2kSM_HeadingControl=4,             ///< The device is a HeadingControl
                            N2kSM_TrackControl=5,               ///< The device is a TrackControl
                            N2kSM_Unavailable=7                 ///< unavailable
                          };
                          
/*************************************************************************//**
 * \enum tN2kTurnMode
 * \brief Enumeration of tunr control modes according to PGN127237
 */
enum tN2kTurnMode {
                            N2kTM_RudderLimitControlled=0,      ///< The turning is controlled by the rudder limit
                            N2kTM_TurnRateControlled=1,         ///< The turning is controlled by the turn rate
                            N2kTM_RadiusControlled=2,           ///< The turning is controlled by the radius
                            N2kTM_Unavailable=7                 ///< unavailable
                          };
/*************************************************************************//**
 * \enum tN2kMOBStatus
 * \brief Enumeration of ManOverBoard state according to PGN127233
 */
enum tN2kMOBStatus {
                            MOBEmitterActivated=0,                ///< MOB emitter has triggered the status
                            ManualOnBoardMOBButtonActivation=1,   ///< MOB was triggered manually
                            TestMode=2,                           ///< MOB in test mode
                            MOBNotActive=3                        ///< MOB not active
                          };
/*************************************************************************//**
 * \enum tN2kMOBPositionSource
 * \brief Enumeration of ManOverBoard position source according to PGN127233
 */
enum tN2kMOBPositionSource {
                           PositionEstimatedByVessel=0,           ///< MOB position is estimated by the vessel
                           PositionReportedByMOBEmitter=1,        ///< MOB position is reported by MOB emitter
                          };
/*************************************************************************//**
 * \enum tN2kMOBEmitterBatteryStatus
 * \brief Enumeration of MOB emitter battery status  according to PGN127233
 */
enum tN2kMOBEmitterBatteryStatus {
                           Good=0,    ///< Battery status of the MOB emitter is good
                           Low=1,     ///< Battery status of the MOB emitter is low
                          };

//*****************************************************************************
// Aliases for N2K standard types.
//*****************************************************************************

/** \brief Alias easier for humans to read for \ref tN2kDD206*/
using tN2kEngineDiscreteStatus1 = tN2kDD206;
/** \brief Alias easier for humans to read for \ref tN2kDD223*/
using tN2kEngineDiscreteStatus2 = tN2kDD223;

/** \brief Alias easier for humans to read for \ref tN2kDD002*/
using tN2kGenericStatusPair = tN2kDD002;
			    // Enum type members:
                  // N2kDD002_No=0
                  // N2kDD002_Yes=1
                  // N2kDD002_Error=2
                  // N2kDD002_Unavailable=3
                  // N2kDD002_Off=N2kDD002_No
                  // N2kDD002_Disabled=N2kDD002_No
                  // N2kDD002_Reset=N2kDD002_No
                  // N2kDD002_0=N2kDD002_No
                  // N2kDD002_On=N2kDD002_Yes
                  // N2kDD002_Enabled=N2kDD002_Yes
                  // N2kDD002_Set=N2kDD002_Yes
                  // N2kDD002_1=N2kDD002_Yes
                  // N2kDD002_Unknown=N2kDD002_Unavailable                         

/** \brief Alias easier for humans to read for \ref tN2kDD025*/
using tN2kDataMode = tN2kDD025;
            // Enum type members:
                  // N2kDD025_Autonomous=0,
                  // N2kDD025_Differential=1,
                  // N2kDD025_Estimated=2,
                  // N2kDD025_Simulator=3,
                  // N2kDD025_Manual=4,
                  // N2kDD025_Error=0xe,
                  // N2kDD025_Unavailable=0xf

/** \brief Alias easier for humans to read for \ref tN2kDD072*/
using tN2kRangeResidualMode = tN2kDD072;
                  // N2kDD072_RangeResidualsWereUsedToCalculateData=0,
                  // N2kDD072_RangeResidualsWereCalculatedAfterPosition=1,
                  // N2kDD072_Error=2,
                  // N2kDD072_Unavailable=3

/** \brief Alias easier for humans to read for \ref tN2kDD124*/
using tN2kPRNUsageStatus = tN2kDD124;                           
                  // N2kDD124_NotTracked=0,
                  // N2kDD124_TrackedButNotUsedInSolution=1,
                  // N2kDD124_UsedInSolutionWithoutDifferentialCorrections=2,
                  // N2kDD124_DifferentialCorrectionsAvailable=3,
                  // N2kDD124_TrackedWithDifferentialCorrections=4,
                  // N2kDD124_UsedWithDifferentialCorrections=5,
                  // N2kDD124_Error=14,
                  // N2kDD124_Unavailable=15,

/** \brief Alias easier for humans to read for \ref tN2kDD305*/
using tN2kAISAtoNType = tN2kDD305;
			    // Enum type members:
                // N2kAISAtoN_not_specified=0,
                // N2kAISAtoN_reference_point=1,
                // N2kAISAtoN_RACON=2,
                // N2kAISAtoN_fixed_structure=3,
                // N2kAISAtoN_emergency_wreck_marking_buoy=4,
                // N2kAISAtoN_light_without_sectors=5,
                // N2kAISAtoN_light_with_sectors=6,
                // N2kAISAtoN_leading_light_front=7,
                // N2kAISAtoN_leading_light_rear=8,
                // N2kAISAtoN_beacon_cardinal_N=9,
                // N2kAISAtoN_beacon_cardinal_E=10,
                // N2kAISAtoN_beacon_cardinal_S=11,
                // N2kAISAtoN_beacon_cardinal_W=12,
                // N2kAISAtoN_beacon_port_hand=13,
                // N2kAISAtoN_beacon_starboard_hand=14,
                // N2kAISAtoN_beacon_preferred_ch_port_hand=15,
                // N2kAISAtoN_beacon_preferred_ch_starboard_hand=16,
                // N2kAISAtoN_beacon_isolated_danger=17,
                // N2kAISAtoN_beacon_safe_water=18,
                // N2kAISAtoN_beacon_special_mark=19,
                // N2kAISAtoN_cardinal_mark_N=20,
                // N2kAISAtoN_cardinal_mark_E=21,
                // N2kAISAtoN_cardinal_mark_S=22,
                // N2kAISAtoN_cardinal_mark_W=23,
                // N2kAISAtoN_port_hand_mark=24,
                // N2kAISAtoN_starboard_hand_mark=25,
                // N2kAISAtoN_preferred_channel_port_hand=26,
                // N2kAISAtoN_preferred_channel_starboard_hand=27,
                // N2kAISAtoN_isolated_danger=28,
                // N2kAISAtoN_safe_water=29,
                // N2kAISAtoN_special_mark=30,
                // N2kAISAtoN_light_vessel_lanby_rigs=31,

/** \brief Alias easier for humans to read for \ref tN2kDD374*/
using tN2kDelaySource = tN2kDD374;
    //        N2kDD374_UserDefined=0,
    //        N2kDD374_MinimumDefined=1,
    //        N2kDD374_FactoryDefault=2,
    //        N2kDD374_MaximumSupported=3,
    //        N2kDD374_OutOfRange=14,
    //        N2kDD374_DataNotAvailable=15

/** \brief Alias easier for humans to read for \ref tN2kDD471*/
using tN2kThrusterMotorEvents = tN2kDD471;
	// .Event.MotorOverTemperatureCutout = 0 | 1
	// .Event.MotorOverCurrentCutout = 0 | 1
	// .Event.LowOilLevelWarning = 0 | 1
	// .Event.OilOverTemperatureWarning = 0 | 1
	// .Event.ControllerUnderVoltageCutout = 0 | 1
	// .Event.ManufacturerDefined = 0 | 1
	// .Event.DataNotAvailable = 0 | 1

/** \brief Alias easier for humans to read for \ref tN2kDD473*/
using tN2kThrusterDirectionControl = tN2kDD473;
	// N2kDD473_OFF=0
	// N2kDD473_ThrusterReady=1
	// N2kDD473_ThrusterToPORT=2
	// N2kDD473_ThrusterToSTARBOARD=3

/** \brief Alias easier for humans to read for \ref tN2kDD474*/
using tN2kThrusterRetraction = tN2kDD474;
	// N2kDD474_OFF=0
	// N2kDD474_Extend=1
	// N2kDD474_Retract=2

/** \brief Alias easier for humans to read for \ref tN2kDD475*/
using tN2kThrusterControlEvents = tN2kDD475;
	// .Event.AnotherDeviceControllingThruster = 0 | 1
	// .Event.BoatSpeedTooFast = 0 | 1

/** \brief Alias easier for humans to read for \ref tN2kDD477*/
using tN2kWindlassMonitoringEvents = tN2kDD477;
			    // Union type with fields:
                            // .Event.ControllerUnderVoltageCutout = 0 | 1
                            // .Event.ControllerOverCurrentCutout = 0 | 1
                            // .Event.ControllerOverTemperatureCutout = 0 | 1
 
/** \brief Alias easier for humans to read for \ref tN2kDD478*/
using tN2kWindlassControlEvents = tN2kDD478;
			    // Union type with field:
                            // .Event.AnotherDeviceControllingWindlass = 0 | 1

/** \brief Alias easier for humans to read for \ref tN2kDD480*/
using tN2kWindlassMotionStates = tN2kDD480;
			    // Enum type members:
                            // N2kDD480_WindlassStopped=0
                            // N2kDD480_DeploymentOccurring=1
                            // N2kDD480_RetrievalOccurring=2
                            // N2kDD480_Unavailable=3

/** \brief Alias easier for humans to read for \ref tN2kDD481*/
using tN2kRodeTypeStates = tN2kDD481;
			    // Enum type members:
                            // N2kDD481_ChainPresentlyDetected=0
                            // N2kDD481_RopePresentlyDetected=1
                            // N2kDD481_Error=2
                            // N2kDD481_Unavailable=3

/** \brief Alias easier for humans to read for \ref tN2kDD482*/
using tN2kAnchorDockingStates = tN2kDD482;
			    // Enum type members:
                            // N2kDD482_NotDocked=0
                            // N2kDD482_FullyDocked=1
                            // N2kDD482_Error=2
                            // N2kDD482_DataNotAvailable=3

/** \brief Alias easier for humans to read for \ref tN2kDD483*/
using tN2kWindlassOperatingEvents = tN2kDD483;
			    // Union type with fields:
                            // .Event.SystemError =  0 | 1
                            // .Event.SensorError = 0 | 1
                            // .Event.NoWindlassMotionDetected = 0 | 1
                            // .Event.RetrievalDockingDistanceReached = 0 | 1
                            // .Event.EndOfRodeReached = 0 | 1

/** \brief Alias easier for humans to read for \ref tN2kDD484*/
using tN2kWindlassDirectionControl = tN2kDD484;
			    // Enum type members:
                            // N2kDD484_Off=0
                            // N2kDD484_Down=1
                            // N2kDD484_Up=2
                            // N2kDD484_Reserved=3

/** \brief Alias easier for humans to read for \ref tN2kDD487*/
using tN2kMotorPowerType = tN2kDD487;
	// N2kDD487_12VDC=0
	// N2kDD487_24VDC=1
	// N2kDD487_48VDC=2
	// N2kDD487_12VAC=3
	// N2kDD487_Hydraulic=4

/** \brief Alias easier for humans to read for \ref tN2kDD488*/
using tN2kSpeedType = tN2kDD488;
			    // Enum type members:
                            // N2kDD488_SingleSpeed=0
                            // N2kDD488_DualSpeed=1
                            // N2kDD488_ProportionalSpeed=2
                            // N2kDD488_DataNotAvailable=3

#endif

/** @} */
