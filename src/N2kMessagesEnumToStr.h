/*
N2kMessagesEnumToStr.h

Copyright (c) 2015-2021 Timo Lappalainen, Kave Oy, www.kave.fi

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

This is collection of functions for handling NMEA2000 bus messages.
Library contains functions to convert enums in N2kMessages.h to
const char *

If you do not need enums in clear text, you do not need this library.
*/

#ifndef _N2kMessagesEnumToStr_H_
#define _N2kMessagesEnumToStr_H_

#include "N2kMessages.h"

const char *N2kEnumTypeEmpty="";

template<typename T> void PrintN2kEnumType(T a, Stream *OutputStream, bool addLF=true) {
  const char *str=N2kEnumTypeToStr(a);
  if (str[0] != '\0') {
    if (addLF) { OutputStream->println(str); } else { OutputStream->print(str); }
  } else {
    OutputStream->print(F("unknown (")); OutputStream->print(a); OutputStream->println(F(")"));
  }
}

#define MakeN2kEnumTypeToStrFunc(enumType,strs) \
const char * N2kEnumTypeToStr(enumType enumVal) { \
  if ( enumVal<(sizeof(strs)/sizeof(const char *)) ) { \
    return strs[enumVal]; \
  } else { return N2kEnumTypeEmpty; }\
}

//*****************************************************************************
const char* tN2kHeadingReferenceStrs[] = { "true", "magnetic" };
MakeN2kEnumTypeToStrFunc(tN2kHeadingReference,tN2kHeadingReferenceStrs)

const char* tN2kTimeSourceStrs[] = { "GPS", "GLONASS", "radio station", "local cesium clock", "local rubidium clock", "local crystal clock" };
MakeN2kEnumTypeToStrFunc(tN2kTimeSource,tN2kTimeSourceStrs)

const char* tN2kGNSStypeStrs[] = { "GPS", "GLONASS", "GPS+GLONASS", "GPS+SBAS/WAAS", "GPS+SBAS/WAAS+GLONASS", "Chayka", "integrated", "surveyed", "Galileo" };
MakeN2kEnumTypeToStrFunc(tN2kGNSStype,tN2kGNSStypeStrs)

const char* tN2kGNSSmethodStrs[] = { "no GNSS", "GNSS fix", "DGNSS", "precise GNSS" };
MakeN2kEnumTypeToStrFunc(tN2kGNSSmethod,tN2kGNSSmethodStrs)

const char* tN2kTempSourceStrs[] = { "sea", "outside", "inside", "engine room", "main cabin", "live well", "bait well", "refridgeration",
                                     "heating system", "dew point", "apparent wind chill", "theoretical wind chill", "heat index", "freezer", "exhaust gas"};
MakeN2kEnumTypeToStrFunc(tN2kTempSource,tN2kTempSourceStrs)

const char* tN2kBatTypeStrs[] = { "flooded", "gel", "AGM"};
MakeN2kEnumTypeToStrFunc(tN2kBatType,tN2kBatTypeStrs)

const char* tN2kBatEqSupportStrs[] = { "no", "yes", "error"};
MakeN2kEnumTypeToStrFunc(tN2kBatEqSupport,tN2kBatEqSupportStrs)

const char* tN2kBatNomVoltStrs[] = { "6V", "12V", "24V", "32V", "62V", "42V", "48V"};
MakeN2kEnumTypeToStrFunc(tN2kBatNomVolt,tN2kBatNomVoltStrs)

const char* tN2kBatChemStrs[] = { "lead acid", "LiIon", "NiCad", "NiMh"};
MakeN2kEnumTypeToStrFunc(tN2kBatChem,tN2kBatChemStrs)

const char* tN2kDCTypeStrs[] = { "battery", "altenator", "converter", "solar cell", "wind generator"};
MakeN2kEnumTypeToStrFunc(tN2kDCType,tN2kDCTypeStrs)

const char* tN2kTransmissionGearStrs[] = { "forward", "neutral", "reverse", "unknown"};
MakeN2kEnumTypeToStrFunc(tN2kTransmissionGear,tN2kTransmissionGearStrs)

const char* tN2kOnOffStrs[] = { "0", "1", "err", "NA" };
MakeN2kEnumTypeToStrFunc(tN2kOnOff,tN2kOnOffStrs)

const char* tN2kPressureStrs[] = { "atmospheric", "water", "steam", "compressed air", "hydraulic", "filter", "altimete setting", "oil", "fuel" };
MakeN2kEnumTypeToStrFunc(tN2kPressureSource,tN2kPressureStrs)

const char* tN2kHumidityStrs[] = { "inside", "outside" };
MakeN2kEnumTypeToStrFunc(tN2kHumiditySource,tN2kHumidityStrs)

const char* tN2kRudderDirectionOrderStrs[] = { "no direction order", "move to starboard", "move to port" };
MakeN2kEnumTypeToStrFunc(tN2kRudderDirectionOrder,tN2kRudderDirectionOrderStrs)

const char* tN2kSpeedWaterReferenceTypeStrs[] = {"Paddle wheel","Pitot tube","Doppler","Correlation (ultra sound)","Electro Magnetic"};
MakeN2kEnumTypeToStrFunc(tN2kSpeedWaterReferenceType,tN2kSpeedWaterReferenceTypeStrs);

const char* tN2kMagneticVariationStrs[] = { "Manual","Automatic Chart","Automatic Table","Automatic Calculation","WMM 2000","WMM 2005","WMM 2010","WMM 2015","WMM 2020" };
MakeN2kEnumTypeToStrFunc(tN2kMagneticVariation, tN2kSpeedWaterReferenceTypeStrs);

#endif
