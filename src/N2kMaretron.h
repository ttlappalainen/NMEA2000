/*
N2kMaretron.h

Copyright (c) 2019-2021 Vassilis Bourdakis,
                        Timo Lappalainen, Kave Oy, www.kave.fi, 

Authors  Vassilis Bourdakis
         Timo Lappalainen          

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

This is collection of functions for handling NMEA2000 bus Maretron messages.
Library contains functions to e.g. create message named with PGN like SetN2kPGN130823 and
inline alias for them like SetN2kLatLonRapid.

Each Setxxx funtion sets related message PGN and its default priority. So if you want to change
priority on function, you have to do it after Setxxx call.

If you do not send any Maretron messages to NMEA2000 bus, you do not need this library.
*/

#ifndef _N2kMaretron_H_
#define _N2kMaretron_H_

#include "N2kMsg.h"
#include <stdint.h>


//*****************************************************************************
// Temperature High Range [as used by MARETRON TMP100]
// Temperatures should be in Kelvin
// Input:
//  - SID                   Sequence ID.
//  - TempInstance          This should be unique at least on one device. May be best to have it unique over all devices sending this PGN.
//  - TempSource            see tN2kTempSource
//  - ActualTemperature     Temperature in K. Use function CToKelvin, if you want to use °C.
//  - SetTemperature        Set temperature in K. Use function CToKelvin, if you want to use °C. This is meaningful for temperatures
//                          which can be controlled like cabin, freezer, refrigeration temperature. 
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kMaretronPGN130823(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA);

inline void SetN2kMaretronTempHR(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature=N2kDoubleNA) {
  SetN2kMaretronPGN130823(N2kMsg,SID,TempInstance,TempSource,ActualTemperature,SetTemperature);
}

bool ParseN2kMaretronPGN130823(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature);
inline bool ParseN2kMaretronTempHR(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  return ParseN2kMaretronPGN130823(N2kMsg, SID, TempInstance, TempSource, ActualTemperature, SetTemperature);
}

//*****************************************************************************
// Fluid Flow Rate [as used by MARETRON FFM100]
// Flow Rate should be in lt/hour
// Input:
//  - SID                   Sequence ID.
//  - FlowRateInstance      This should be unique at least on one device. May be best to have it unique over all devices sending this PGN.
//  - FluidType             see tN2kFluidType [possible values for this field include Fuel, Fresh Water, Waste Water, Live Well, Oil, and Black Water]
//  - FluidFlowRate         This field is used to indicate the rate of fluid flow in units litres/hour.
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kMaretronPGN65286(tN2kMsg &N2kMsg, unsigned char SID, unsigned char FlowRateInstance, tN2kFluidType FluidType,
                     double FluidFlowRate);

inline void SetN2kMaretronFluidFR(tN2kMsg &N2kMsg, unsigned char SID, unsigned char FlowRateInstance, tN2kFluidType FluidType,
                     double FluidFlowRate) {
  SetN2kMaretronPGN65286(N2kMsg,SID,FlowRateInstance,FluidType,FluidFlowRate);
}

bool ParseN2kMaretronPGN65286(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &FlowRateInstance, tN2kFluidType &FluidType,
                     double &FluidFlowRate);
inline bool ParseN2kMaretronFluidFR(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &FlowRateInstance, tN2kFluidType &FluidType,
                     double &FluidFlowRate) {
  return ParseN2kMaretronPGN65286(N2kMsg, SID, FlowRateInstance, FluidType, FluidFlowRate);
}

//*****************************************************************************
// Trip Volume [as used by MARETRON FFM100]
// Volume should be in litres
// Input:
//  - SID                   Sequence ID.
//  - VolumeInstance        This should be unique at least on one device. May be best to have it unique over all devices sending this PGN.
//  - FluidType             see tN2kFluidType [possible values for this field include Fuel, Fresh Water, Waste Water, Live Well, Oil, and Black Water]
//  - TripVolume            This field is used to indicate the trip volume in units of litres.
// Output:
//  - N2kMsg                NMEA2000 message ready to be send.
void SetN2kMaretronPGN65287(tN2kMsg &N2kMsg, unsigned char SID, unsigned char VolumeInstance, tN2kFluidType FluidType,
                     double TripVolume);

inline void SetN2kMaretronTripVolume(tN2kMsg &N2kMsg, unsigned char SID, unsigned char VolumeInstance, tN2kFluidType FluidType,
                     double TripVolume) {
  SetN2kMaretronPGN65287(N2kMsg,SID,VolumeInstance,FluidType,TripVolume);
}

bool ParseN2kMaretronPGN65287(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &VolumeInstance, tN2kFluidType &FluidType,
                     double &TripVolume);
inline bool ParseN2kMaretronTripVolume(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &VolumeInstance, tN2kFluidType &FluidType,
                     double &TripVolume) {
  return ParseN2kMaretronPGN65287(N2kMsg, SID, VolumeInstance, FluidType, TripVolume);
}

#endif

