/*
N2kTypes.h

Copyright (c) 2019 Timo Lappalainen, Kave Oy, www.kave.fi

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

NMEA2000 type definitions.
*/

#ifndef _N2kTypes_H_
#define _N2kTypes_H_

enum tN2kHeadingReference {
                            N2khr_true=0,
                            N2khr_magnetic=1,
                            N2khr_error=2,
                            N2khr_Unavailable=3
                          };
enum tN2kDistanceCalculationType {
                            N2kdct_GreatCircle=0,
                            N2kdct_RhumbLine=1
                          };
enum tN2kXTEMode  {
                            N2kxtem_Autonomous=0,
                            N2kxtem_Differential=1,
                            N2kxtem_Estimated=2,
                            N2kxtem_Simulator=3,
                            N2kxtem_Manual=4
                          };
enum tN2kGNSStype {
                            N2kGNSSt_GPS=0,
                            N2kGNSSt_GLONASS=1,
                            N2kGNSSt_GPSGLONASS=2,
                            N2kGNSSt_GPSSBASWAAS=3,
                            N2kGNSSt_GPSSBASWAASGLONASS=4,
                            N2kGNSSt_Chayka=5,
                            N2kGNSSt_integrated=6,
                            N2kGNSSt_surveyed=7,
                            N2kGNSSt_Galileo=8
                          };
enum tN2kGNSSmethod {
                            N2kGNSSm_noGNSS=0,
                            N2kGNSSm_GNSSfix=1,
                            N2kGNSSm_DGNSS=2,
                            N2kGNSSm_PreciseGNSS=3,
                            N2kGNSSm_RTKFixed=4,
                            N2kGNSSm_RTKFloat=5,
                            N2kGNSSm_Error=14,
                            N2kGNSSm_Unavailable=15
                          };

enum tN2kGNSSDOPmode {
                            N2kGNSSdm_1D=0,
                            N2kGNSSdm_2D=1,
                            N2kGNSSdm_3D=2,
                            N2kGNSSdm_Auto=3,
                            N2kGNSSdm_Reserved=4,
                            N2kGNSSdm_Reserved2=5,
                            N2kGNSSdm_Error=6,
                            N2kGNSSdm_Unavailable=7
                          };

enum tN2kTempSource {
                            N2kts_SeaTemperature=0,
                            N2kts_OutsideTemperature=1,
                            N2kts_InsideTemperature=2,
                            N2kts_EngineRoomTemperature=3,
                            N2kts_MainCabinTemperature=4,
                            N2kts_LiveWellTemperature=5,
                            N2kts_BaitWellTemperature=6,
                            N2kts_RefridgerationTemperature=7,
                            N2kts_HeatingSystemTemperature=8,
                            N2kts_DewPointTemperature=9,
                            N2kts_ApparentWindChillTemperature=10,
                            N2kts_TheoreticalWindChillTemperature=11,
                            N2kts_HeatIndexTemperature=12,
                            N2kts_FreezerTemperature=13,
                            N2kts_ExhaustGasTemperature=14
                          };

enum tN2kHumiditySource {
                            N2khs_InsideHumidity=0,
                            N2khs_OutsideHumidity=1,
                            N2khs_Undef=0xff
                          };

enum tN2kPressureSource {
                            N2kps_Atmospheric = 0,
                            N2kps_Water = 1,
                            N2kps_Steam = 2,
                            N2kps_CompressedAir = 3,
                            N2kps_Hydraulic = 4
                          };

enum tN2kTimeSource {
                            N2ktimes_GPS=0,
                            N2ktimes_GLONASS=1,
                            N2ktimes_RadioStation=2,
                            N2ktimes_LocalCesiumClock=3,
                            N2ktimes_LocalRubidiumClock=4,
                            N2ktimes_LocalCrystalClock=5
                          };

enum tN2kFluidType {
                            N2kft_Fuel=0,
                            N2kft_Water=1,
                            N2kft_GrayWater=2,
                            N2kft_LiveWell=3,
                            N2kft_Oil=4,
                            N2kft_BlackWater=5,
                            N2kft_FuelGasoline=6,
                            N2kft_Error=14,
                            N2kft_Unavailable=15
                          };

enum tN2kWindReference {
  // Details found on page 12 of https://www.rocktheboatmarinestereo.com/specs/MSNRX200I.pdf
                            // Theoretical Wind (ground referenced, referenced to True North; calculated using COG/SOG)
                            N2kWind_True_North=0,
                            // Theoretical Wind (ground referenced, referenced to Magnetic North; calculated using COG/SOG)
                            N2kWind_Magnetic=1,
                            // Apparent Wind (relative to the vessel centerline)
                            N2kWind_Apparent=2,
                            // Deprecated - We had the typo in older version of the library
                            N2kWind_Apprent=2,
                            // Theoretical (Calculated to Centerline of the vessel, referenced to ground; calculated using COG/SOG)
                            N2kWind_True_boat=3,
                            // Theoretical (Calculated to Centerline of the vessel, referenced to water; calculated using Heading/Speed through Water)
                            N2kWind_True_water=4,
                            N2kWind_Error=6,
                            N2kWind_Unavailable=7
                          };

enum tN2kSpeedWaterReferenceType {
                            N2kSWRT_Paddle_wheel=0,
                            N2kSWRT_Pitot_tube=1,
                            N2kSWRT_Doppler_log=2,
                            N2kSWRT_Ultra_Sound=3,
                            N2kSWRT_Electro_magnetic=4,
                            N2kSWRT_Error=254,
                            N2kSWRT_Unavailable=255
                          };

enum tN2kRudderDirectionOrder {
                            N2kRDO_NoDirectionOrder=0,
                            N2kRDO_MoveToStarboard=1,
                            N2kRDO_MoveToPort=2,
                            N2kRDO_Unavailable=7
                          };

enum tN2kDCType {
                            N2kDCt_Battery=0,
                            N2kDCt_Alternator=1,
                            N2kDCt_Converter=2,
                            N2kDCt_SolarCell=3,
                            N2kDCt_WindGenerator=4
                          };

enum tN2kBatType  {
                            N2kDCbt_Flooded=0,
                            N2kDCbt_Gel=1,
                            N2kDCbt_AGM=2
                          };

enum tN2kBatEqSupport  {
                            N2kDCES_No=0,  // No, Off, Disabled
                            N2kDCES_Yes=1, // Yes, On, Enabled
                            N2kDCES_Error=2, // Error
                            N2kDCES_Unavailable=3 // Unavailable
                          };

enum tN2kBatChem {
                            N2kDCbc_LeadAcid=0,
                            N2kDCbc_LiIon=1,
                            N2kDCbc_NiCad=2,
                            N2kDCbc_NiMh=3
                          };

enum tN2kBatNomVolt {
                            N2kDCbnv_6v=0,
                            N2kDCbnv_12v=1,
                            N2kDCbnv_24v=2,
                            N2kDCbnv_32v=3,
                            N2kDCbnv_62v=4,
                            N2kDCbnv_42v=5,
                            N2kDCbnv_48v=6
                          };

enum tN2kTransmissionGear {
                            N2kTG_Forward=0,
                            N2kTG_Neutral=1,
                            N2kTG_Reverse=2,
                            N2kTG_Unknown=3,
                          };

enum tN2kAISRepeat {
                            N2kaisr_Initial=0,
                            N2kaisr_First=1,
                            N2kaisr_Second=2,
                            N2kaisr_Final=3,
                          };

enum tN2kAISVersion {
                            N2kaisv_ITU_R_M_1371_1=0,
                            N2kaisv_ITU_R_M_1371_3=1,
                          };

enum tN2kAISTranceiverInfo {
                            N2kaisti_Channel_A_VDL_reception=0,
                            N2kaisti_Channel_B_VDL_reception=1,
                            N2kaisti_Channel_A_VDL_transmission=2,
                            N2kaisti_Channel_B_VDL_transmission=3,
                            N2kaisti_Own_information_not_broadcast=4,
                            N2kaisti_Reserved=5
                          };

enum tN2kAISNavStatus {
                            N2kaisns_Under_Way_Motoring=0,
                            N2kaisns_At_Anchor=1,
                            N2kaisns_Not_Under_Command=2,
                            N2kaisns_Restricted_Manoeuverability=3,
                            N2kaisns_Constrained_By_Draught=4,
                            N2kaisns_Moored=5,
                            N2kaisns_Aground=6,
                            N2kaisns_Fishing=7,
                            N2kaisns_Under_Way_Sailing=8,
                            N2kaisns_Hazardous_Material_High_Speed=9,
                            N2kaisns_Hazardous_Material_Wing_In_Ground=10,
                            N2kaisns_AIS_SART=14,
                          };

enum tN2kAISDTE {
                            N2kaisdte_Ready=0,
                            N2kaisdte_NotReady=1,
                          };

enum tN2kAISUnit {
                            N2kaisunit_ClassB_SOTDMA=0,
                            N2kaisunit_ClassB_CS=1,
                          };

enum tN2kAISMode {
                            N2kaismode_Autonomous=0,
                            N2kaismode_Assigned=1,
                          };
enum tN2kMagneticVariation {
                            N2kmagvar_Manual=0,
                            N2kmagvar_Chart=1,
                            N2kmagvar_Table=2,
                            N2kmagvar_Calc=3,
                            N2kmagvar_WMM2000=4,
                            N2kmagvar_WMM2005=5,
                            N2kmagvar_WMM2010=6,
                            N2kmagvar_WMM2015=7,
                            N2kmagvar_WMM2020=8,
                          };

enum tN2kOnOff  {
                            N2kOnOff_Off=0,  // No, Off, Disabled
                            N2kOnOff_On=1, // Yes, On, Enabled
                            N2kOnOff_Error=2, // Error
                            N2kOnOff_Unavailable=3 // Unavailable
                          };

enum tN2kChargeState  {
                            N2kCS_Not_Charging=0,
                            N2kCS_Bulk=1,
                            N2kCS_Absorption=2,
                            N2kCS_Overcharge=3,
                            N2kCS_Equalise=4,
                            N2kCS_Float=5,
                            N2kCS_No_Float=6,
                            N2kCS_Constant_VI=7,
                            N2kCS_Disabled=8,
                            N2kCS_Fault=9,
                            N2kCS_Unavailable=15
                          };

enum tN2kChargerMode {
                            N2kCM_Standalone=0,
                            N2kCM_Primary=1,
                            N2kCM_Secondary=2,
                            N2kCM_Echo=3,
                            N2kCM_Unavailable=15
                          };

enum tN2kSteeringMode {
                            N2kSM_MainSteering=0,
                            N2kSM_NonFollowUpDevice=1,
                            N2kSM_FollowUpDevice=2,
                            N2kSM_HeadingControlStandalone=3,
                            N2kSM_HeadingControl=4,
                            N2kSM_TrackControl=5,
                            N2kSM_Unavailable=7
                          };
                          
enum tN2kTurnMode {
                            N2kTM_RudderLimitControlled=0,
                            N2kTM_TurnRateControlled=1,
                            N2kTM_RadiusControlled=2,
                            N2kTM_Unavailable=7
                          };

#endif

