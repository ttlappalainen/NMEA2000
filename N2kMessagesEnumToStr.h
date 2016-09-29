/* 
N2kMessagesEnumToStr.h

2015-2016 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen

  This library is free software; you can redistribute it and/or
  modify it how ever you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  NO WARRANTY AT ALL IN ANY CASE!
  
This is collection of functions for handling NMEA2000 bus messages. 
Library contains functions to convert enums in N2kMessages.h to
const char *

If you do not need enums in clear text, you do not need this library. 
*/

#ifndef _N2kMessagesEnumToStr_H_
#define _N2kMessagesEnumToStr_H_

#include <N2kMessages.h>

const char *N2kEnumTypeEmpty="";

template<typename T> void PrintN2kEnumType(T a, Stream *OutputStream, bool addLF=true) {
  const char *str=N2kEnumTypeToStr(a);
  if (str[0] != '\0') {
    if (addLF) { OutputStream->println(str); } else { OutputStream->print(str); }
  } else {
    OutputStream->print("unknown ("); OutputStream->print(a); OutputStream->println(")");
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
                                     "heating system", "dew point", "apparent wind chill", "theoretical wind chill", "heat index", "freezer"};
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

#endif
