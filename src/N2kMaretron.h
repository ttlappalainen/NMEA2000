/*
 * N2kMaretron.h
 * 
 * Copyright (c) 2019-2024 Vassilis Bourdakis,
 *                        Timo Lappalainen, Kave Oy, www.kave.fi, 
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

/**************************************************************************//**
 * \file   N2kMaretron.h
 * \brief  Collection of functions for handling NMEA2000 bus Maretron messages
 *
 * This is collection of functions for handling NMEA2000 bus Maretron
 * messages. Library contains functions to e.g. create message named with
 * PGN like SetN2kPGN130823 and inline alias for them like SetN2kLatLonRapid.
 * Each Setxxx funtion sets related message PGN and its default priority. So if
 * you want to change priority on function, you have to do it after Setxxx call.
 * 
 * If you do not send any Maretron messages to NMEA2000 bus, you do not need
 * this library.
 * 
 * \note  These are proprietary Message Numbers created and used by Maretron!
 *        see [Maretron Website] (https://www.maretron.com/)
 * 
 * \authors Vassilis Bourdakis, Timo Lappalainen
*/

#ifndef _N2kMaretron_H_
#define _N2kMaretron_H_

#include "N2kMsg.h"
#include "N2kTypes.h"
#include <stdint.h>


/************************************************************************//**
 * \brief Setting up PGN 130823 for Maretron Message "Temperature High Range"
 * 
 * This Temperature High Range Message is used e.g. by MARETRON TMP100.
 * The wide range is required for very high temperature values
 * (e.g. Exhaust Gas Temperatures). 
 * see [Maretron TMP100] (https://www.maretron.com/products/tmp100.php)
 * 
 * \note Temperatures should be in Kelvin
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                        \n
 *                        The sequence identifier field is used to tie different PGNs data together to same 
 *                        sampling or calculation time.
 * \param TempInstance    This should be unique at least on one device. May be
 *                        best to have it unique over all devices sending
 *                        this PGN.
 * \param TempSource      Source of the temperature, see \ref tN2kTempSource
 * \param ActualTemperature   Temperature in K. Use function CToKelvin, if you
 *                            want to use °C.
 * \param SetTemperature      Set temperature in K. Use function CToKelvin, if
 *                            you want to use °C. This is meaningful for
 *                            temperatures which can be controlled like cabin,
 *                            freezer, refrigeration temperature.
 */
void SetN2kMaretronPGN130823(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA);

/************************************************************************//**
 * \brief Setting up Maretron Message "Temperature High Range" - PGN 130823
 * 
 * Alias of PGN 130823. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kMaretronPGN130823
 * 
 */
inline void SetN2kMaretronTempHR(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA) {
  SetN2kMaretronPGN130823(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature);
}

/************************************************************************//**
 * \brief Parsing the content of Maretron Message PGN 130823
 *        "Temperature High Range"
 * 
 * This Temperature High Range Message is used e.g. by MARETRON TMP100.
 * The wide range is required for very high temperature values
 * (e.g. Exhaust Gas Temperatures).
 * see [Maretron TMP100] (https://www.maretron.com/products/tmp100.php)
 * 
 * \note Temperatures should be in Kelvin
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param TempInstance    This should be unique at least on one device. May be
 *                        best to have it unique over all devices sending
 *                        this PGN.
 * \param TempSource      Source of the temperature, see \ref tN2kTempSource
 * \param ActualTemperature   Temperature in K. Use function CToKelvin, if you
 *                            want to use °C.
 * \param SetTemperature      Set temperature in K. Use function CToKelvin, if
 *                            you want to use °C. This is meaningful for
 *                            temperatures which can be controlled like cabin,
 *                            freezer, refrigeration temperature.
 *
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kMaretronPGN130823(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature);

/************************************************************************//**
 * \brief Parsing the content of Maretron Message "Temperature High Range" - 
 * PGN 130823
 * 
 * Alias of PGN 130823. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kMaretronPGN130823
 * 
 */
inline bool ParseN2kMaretronTempHR(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  return ParseN2kMaretronPGN130823(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature);
}

/************************************************************************//**
 * \brief Setting up PGN 65286 for Maretron Message "Fluid Flow Rate"
 * 
 * This Fluid Flow Rate Message is used e.g. by MARETRON FFM100. With this
 * fuel flow rate it is possible to monitor Diesel Fuel , Gasoline/Petrol 
 * Fuel Flow and Cooling Water Flow.
 * 
 * see [Maretron FFM100] (https://www.maretron.com/products/ffm100.php)
 * 
 * \note Flow Rate should be in lt/hour
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                        \n
 *                        The sequence identifier field is used to tie different PGNs data together to same 
 *                        sampling or calculation time.
 * \param FlowRateInstance  This should be unique at least on one device. May
 *                          be best to have it unique over all devices sending
 *                          this PGN.
 * \param FluidType         Type of Fluid, see \ref tN2kFluidType [possible
 *                          values for this field include Fuel, Fresh Water,
 *                          Waste Water, Live Well, Oil, and Black Water]
 * \param FluidFlowRate     This field is used to indicate the rate of fluid
 *                          flow in units litres/hour.
 * 
 */
void SetN2kMaretronPGN65286(tN2kMsg &N2kMsg, unsigned char SID, unsigned char FlowRateInstance, tN2kFluidType FluidType,
                     double FluidFlowRate);

/************************************************************************//**
 * \brief Setting up Maretron Message "Fluid Flow Rate" - PGN 65286
 * 
 * Alias of PGN 65286. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kMaretronPGN65286
 * 
 */
inline void SetN2kMaretronFluidFR(tN2kMsg &N2kMsg, unsigned char SID, unsigned char FlowRateInstance, tN2kFluidType FluidType,
                     double FluidFlowRate) {
  SetN2kMaretronPGN65286(N2kMsg,SID,FlowRateInstance,FluidType,FluidFlowRate);
}

/************************************************************************//**
 * \brief Parsing the content of Maretron Message PGN 65286 "Fluid Flow Rate"
 * 
 * This Fluid Flow Rate Message is used e.g. by MARETRON FFM100. With this
 * fuel flow rate it is possible to monitor Diesel Fuel , Gasoline/Petrol 
 * Fuel Flow and Cooling Water Flow.
 * 
 * see [Maretron FFM100] (https://www.maretron.com/products/ffm100.php)
 * 
 * \note Flow Rate should be in lt/hour
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param FlowRateInstance  This should be unique at least on one device. May
 *                          be best to have it unique over all devices sending
 *                          this PGN.
 * \param FluidType         Type of Fluid, see \ref tN2kFluidType [possible
 *                          values for this field include Fuel, Fresh Water,
 *                          Waste Water, Live Well, Oil, and Black Water]
 * \param FluidFlowRate     This field is used to indicate the rate of fluid
 *                          flow in units litres/hour.
 *
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 *
 */
bool ParseN2kMaretronPGN65286(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &FlowRateInstance, tN2kFluidType &FluidType,
                     double &FluidFlowRate);

/************************************************************************//**
 * \brief Parsing the content of Maretron Message "Fluid Flow Rate" - PGN 65286
 * 
 * Alias of PGN 65286. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kMaretronPGN65286
 * 
 */
inline bool ParseN2kMaretronFluidFR(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &FlowRateInstance, tN2kFluidType &FluidType,
                     double &FluidFlowRate) {
  return ParseN2kMaretronPGN65286(N2kMsg, SID, FlowRateInstance, FluidType, FluidFlowRate);
}

/************************************************************************//**
 * \brief Setting up PGN 65287 for Maretron Message "Trip Volume"
 * 
 * This Trip Volume Message is used e.g. by MARETRON FFM100. With this
 * Trip Volume it is possible to monitor Diesel Fuel , Gasoline/Petrol 
 * Fuel Flow and Cooling Water Flow.
 * 
 * see [Maretron FFM100] (https://www.maretron.com/products/ffm100.php)
 * 
 * \note Volume should be in litres
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence identifier. In most cases you can use just 0xff for SID. See \ref secRefTermSID. \n
 *                        \n
 *                        The sequence identifier field is used to tie different PGNs data together to same 
 *                        sampling or calculation time.
 * \param VolumeInstance  This should be unique at least on one device. May
 *                        be best to have it unique over all devices sending
 *                        this PGN.
 * \param FluidType       Type of Fluid, see \ref tN2kFluidType [possible
 *                        values for this field include Fuel, Fresh Water,
 *                        Waste Water, Live Well, Oil, and Black Water]
 * \param TripVolume      This field is used to indicate the trip volume
 *                        units of litres.
 */
void SetN2kMaretronPGN65287(tN2kMsg &N2kMsg, unsigned char SID, unsigned char VolumeInstance, tN2kFluidType FluidType,
                     double TripVolume);

/************************************************************************//**
 * \brief Setting up Maretron Message "Trip Volume" - PGN 65287
 * 
 * Alias of PGN 65287. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref SetN2kMaretronPGN65287
 * 
 */
inline void SetN2kMaretronTripVolume(tN2kMsg &N2kMsg, unsigned char SID, unsigned char VolumeInstance, tN2kFluidType FluidType,
                     double TripVolume) {
  SetN2kMaretronPGN65287(N2kMsg,SID,VolumeInstance,FluidType,TripVolume);
}

/************************************************************************//**
 * \brief Parsing the content of Maretron Message PGN 65287 "Trip Volume"
 * 
 * This Trip Volume Message is used e.g. by MARETRON FFM100. With this
 * Trip Volume it is possible to monitor Diesel Fuel , Gasoline/Petrol 
 * Fuel Flow and Cooling Water Flow.
 * 
 * see [Maretron FFM100] (https://www.maretron.com/products/ffm100.php)
 * 
 * \note Volume should be in litres
 * 
 * \param N2kMsg          Reference to a N2kMsg Object, 
 *                        Output: NMEA2000 message ready to be send.
 * \param SID             Sequence ID. Normally you can just forget its value. See \ref secRefTermSID.
 * \param VolumeInstance  This should be unique at least on one device. May
 *                        be best to have it unique over all devices sending
 *                        this PGN.
 * \param FluidType       Type of Fluid, see \ref tN2kFluidType [possible
 *                        values for this field include Fuel, Fresh Water,
 *                        Waste Water, Live Well, Oil, and Black Water]
 * \param TripVolume      This field is used to indicate the trip volume
 *                        units of litres.
 *
 * \return true     Parsing of PGN Message successful
 * \return false    Parsing of PGN Message aborted
 */
bool ParseN2kMaretronPGN65287(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &VolumeInstance, tN2kFluidType &FluidType,
                     double &TripVolume);

/************************************************************************//**
 * \brief Parsing the content of Maretron Message "Trip Volume" - PGN 65287
 * 
 * Alias of PGN 65287. This alias was introduced to improve the readability
 * of the source code. See parameter details on \ref ParseN2kMaretronPGN65287
 * 
 */
inline bool ParseN2kMaretronTripVolume(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &VolumeInstance, tN2kFluidType &FluidType,
                     double &TripVolume) {
  return ParseN2kMaretronPGN65287(N2kMsg, SID, VolumeInstance, FluidType, TripVolume);
}

#endif

