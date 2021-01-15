/*
N2kMaretron.cpp

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
*/

#include "N2kTypes.h"
#include "N2kMaretron.h"
#include <string.h>

#define MaretronProprietary 0x9889 // Maretron 137 + reserved + industry code=marine


//*****************************************************************************
// Temperature High Range [MARETRON TMP100]
// Temperatures should be in Kelvin
void SetN2kMaretronPGN130823(tN2kMsg &N2kMsg, unsigned char SID, unsigned char TempInstance, tN2kTempSource TempSource,
                     double ActualTemperature, double SetTemperature) {
    N2kMsg.SetPGN(130823L);
    N2kMsg.Priority=5;
    N2kMsg.Add2ByteUInt(MaretronProprietary);
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((unsigned char)TempInstance);
    N2kMsg.AddByte((unsigned char)TempSource);
    N2kMsg.Add2ByteUDouble(ActualTemperature,0.1);
    N2kMsg.Add2ByteUDouble(SetTemperature,0.1);
}

bool ParseN2kMaretronPGN130823(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &TempInstance, tN2kTempSource &TempSource,
                     double &ActualTemperature, double &SetTemperature) {
  if (N2kMsg.PGN!=130823L) return false;
  int Index=0;
  if (N2kMsg.Get2ByteUInt(Index)!=MaretronProprietary ) return false;
  SID=N2kMsg.GetByte(Index);
  TempInstance=N2kMsg.GetByte(Index);
  TempSource=(tN2kTempSource)(N2kMsg.GetByte(Index));
  ActualTemperature=N2kMsg.Get2ByteUDouble(0.1,Index);
  SetTemperature=N2kMsg.Get2ByteUDouble(0.1,Index);
  return true;
}

//*****************************************************************************
// Fluid Flow Rate [MARETRON FFM100]
// Flow unit is 1 lt/hour
void SetN2kMaretronPGN65286(tN2kMsg &N2kMsg, unsigned char SID, unsigned char FlowRateInstance, tN2kFluidType FluidType,
                     double FluidFlowRate) {
    N2kMsg.SetPGN(65286L);
    N2kMsg.Priority=5;
    N2kMsg.Add2ByteUInt(MaretronProprietary);
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((unsigned char)FlowRateInstance);
    N2kMsg.AddByte((unsigned char)FluidType);
    N2kMsg.Add3ByteDouble(FluidFlowRate,0.1);
}

bool ParseN2kMaretronPGN65286(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &FlowRateInstance, tN2kFluidType &FluidType,
                     double &FluidFlowRate) {
  if (N2kMsg.PGN!=65286L) return false;
  int Index=0;
  if (N2kMsg.Get2ByteUInt(Index)!=MaretronProprietary ) return false;
  SID=N2kMsg.GetByte(Index);
  FlowRateInstance=N2kMsg.GetByte(Index);
  FluidType=(tN2kFluidType)(N2kMsg.GetByte(Index));
  FluidFlowRate=N2kMsg.Get3ByteDouble(0.1,Index);
  return true;
}

//*****************************************************************************
// Trip Volume [MARETRON FFM100]
// Volume unit is 1 litre
void SetN2kMaretronPGN65287(tN2kMsg &N2kMsg, unsigned char SID, unsigned char VolumeInstance, tN2kFluidType FluidType,
                     double TripVolume) {
    N2kMsg.SetPGN(65287L);
    N2kMsg.Priority=5;
    N2kMsg.Add2ByteUInt(MaretronProprietary);
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte((unsigned char)VolumeInstance);
    N2kMsg.AddByte((unsigned char)FluidType);
    N2kMsg.Add3ByteDouble(TripVolume,1);
}

bool ParseN2kMaretronPGN65287(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &VolumeInstance, tN2kFluidType &FluidType,
                     double &TripVolume) {
  if (N2kMsg.PGN!=65287L) return false;
  int Index=0;
  if (N2kMsg.Get2ByteUInt(Index)!=MaretronProprietary ) return false;
  SID=N2kMsg.GetByte(Index);
  VolumeInstance=N2kMsg.GetByte(Index);
  FluidType=(tN2kFluidType)(N2kMsg.GetByte(Index));
  TripVolume=N2kMsg.Get3ByteDouble(1,Index);
  return true;
}