/*
 * N2kMessagesEnumToStr.h

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
 * \file    N2kMessagesEnumToStr.h
 * \brief   This File contains functions to convert enums from N2kMessages.h 
 *          to const char
 * 
 * This is collection of functions for handling enums of NMEA2000 bus messages.
 * There are basically 2 functions in the library to convert enums 
 * derived from N2kMessages to const char *:
 * 
 * #### const char * N2kEnumTypeToStr(enumType enumVal)
 * 
 *  This functions returns a const char * value thats representing the given
 *  enum Value. By detecting the enumType, the right text output is
 *  chosen.
 * 
 * #### void PrintN2kEnumType (T a, Stream *OutputStream, bool addLF=true)
 *  
 *  This prints the corresponding text output to the given output stream.
 * 
 * \note If you do not need enums in clear text, you do not need this library.
 * 
 * ********************************************************************/

#ifndef _N2kMessagesEnumToStr_H_
#define _N2kMessagesEnumToStr_H_

#include "N2kMessages.h"

/************************************************************************//**
 * \brief Default value if no string representation is found
 */
const char *N2kEnumTypeEmpty="";

/************************************************************************//**
 * \brief Print function for the enum values
 * 
 * This function template generation an text output to the defined stream. 
 * Corresponding to the type of the enum value, the correct function (and
 * therefore output text) is chosen.
 * 
 * @tparam T            Type of the enum
 * @param a             enum value
 * @param OutputStream  stream handler for the output
 * @param addLF         add a line feed (default = true)
 */
template<typename T> void PrintN2kEnumType(T a, Stream *OutputStream, bool addLF=true) {
  const char *str=N2kEnumTypeToStr(a);
  if (str[0] != '\0') {
    if (addLF) { OutputStream->println(str); } else { OutputStream->print(str); }
  } else {
    OutputStream->print(F("unknown (")); OutputStream->print(a); OutputStream->println(F(")"));
  }
}
/************************************************************************//**
 * \brief Macro function for converting various N2k Enums to a const char
 * 
 * This generic macro defines a way to use a specific version of
 * **N2kEnumTypeToStr(enumType enumVal)** depending on which enumType is
 * given as parameter.
 * 
 * In NMEA2000 messages there are several enumerations defined and used
 * for the states of different systems. E.g tN2kBatType holds all the
 * available battery types like "flooded", "gel", "AGM". If you need 
 * the "real" values of these enums written to a string 
 * (e.g. for debugging), you can use these functions.
 * 
 * 
 * ### Usage of the function N2kEnumTypeToStr(enumType enumVal):
 * 
 * ```
 * tN2kBatType batCem = N2kDCbt_Gel;
 * const char* tmp;
 * tmp = N2kEnumTypeToStr(batCem);
 *
 * Serial.print(tmp);     // Output to serial =>  Gel
 * ```
 *
 */
#define MakeN2kEnumTypeToStrFunc(enumType,strs) \
const char * N2kEnumTypeToStr(enumType enumVal) { \
  if ( enumVal<(sizeof(strs)/sizeof(const char *)) ) { \
    return strs[enumVal]; \
  } else { return N2kEnumTypeEmpty; }\
}

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kHeadingReference
 */
const char* tN2kHeadingReferenceStrs[] = { "true", "magnetic" };

/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kHeadingReference to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kHeadingReference,tN2kHeadingReferenceStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kTimeSource
 */
const char* tN2kTimeSourceStrs[] = { "GPS", "GLONASS", "radio station", "local cesium clock", "local rubidium clock", "local crystal clock" };

/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kTimeSource to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kTimeSource,tN2kTimeSourceStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kGNSStype
 */
const char* tN2kGNSStypeStrs[] = { "GPS", "GLONASS", "GPS+GLONASS", "GPS+SBAS/WAAS", "GPS+SBAS/WAAS+GLONASS", "Chayka", "integrated", "surveyed", "Galileo" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kTimeSource to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kGNSStype,tN2kGNSStypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kGNSSmethod
 */
const char* tN2kGNSSmethodStrs[] = { "no GNSS", "GNSS fix", "DGNSS", "precise GNSS" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kGNSSmethod to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kGNSSmethod,tN2kGNSSmethodStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kFluidType
 */
const char* tN2kFluidTypeStrs[] = { "Fuel", "Water", "Gray water", "Live well", "Oil", "Black water", "Gasoline Fuel", 
                                    "Reserved", "Reserved",  "Reserved",  "Reserved",  "Reserved",  "Reserved",  "Reserved",
                                    "Error", "Unavailable",};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kFluidType to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kFluidType,tN2kFluidTypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kTempSource
 */
const char* tN2kTempSourceStrs[] = { "sea", "outside", "inside", "engine room", "main cabin", "live well", "bait well", "refridgeration",
                                     "heating system", "dew point", "apparent wind chill", "theoretical wind chill", "heat index", "freezer", "exhaust gas"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kTempSource to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kTempSource,tN2kTempSourceStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kBatType
 */
const char* tN2kBatTypeStrs[] = { "flooded", "gel", "AGM"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kBatType to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kBatType,tN2kBatTypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kBatEqSupport
 */
const char* tN2kBatEqSupportStrs[] = { "no", "yes", "error"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kBatEqSupport to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kBatEqSupport,tN2kBatEqSupportStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kBatNomVolt
 */
const char* tN2kBatNomVoltStrs[] = { "6V", "12V", "24V", "32V", "62V", "42V", "48V"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kBatNomVolt to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kBatNomVolt,tN2kBatNomVoltStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kBatChem
 */
const char* tN2kBatChemStrs[] = { "lead acid", "LiIon", "NiCad", "NiMh"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kBatChem to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kBatChem,tN2kBatChemStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kDCType
 */
const char* tN2kDCTypeStrs[] = { "battery", "altenator", "converter", "solar cell", "wind generator"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kDCType to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kDCType,tN2kDCTypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kTransmissionGear
 */
const char* tN2kTransmissionGearStrs[] = { "forward", "neutral", "reverse", "unknown"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kTransmissionGear to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kTransmissionGear,tN2kTransmissionGearStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kOnOff
 */
const char* tN2kOnOffStrs[] = { "0", "1", "err", "NA" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kOnOff to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kOnOff,tN2kOnOffStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kPressureSource
 */
const char* tN2kPressureStrs[] = { "atmospheric", "water", "steam", "compressed air", "hydraulic", "filter", "altimete setting", "oil", "fuel" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kPressureSource to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kPressureSource,tN2kPressureStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kHumiditySource
 */
const char* tN2kHumidityStrs[] = { "inside", "outside" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kHumiditySource to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kHumiditySource,tN2kHumidityStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kRudderDirectionOrder
 */
const char* tN2kRudderDirectionOrderStrs[] = { "no direction order", "move to starboard", "move to port" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kRudderDirectionOrder to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kRudderDirectionOrder,tN2kRudderDirectionOrderStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kSpeedWaterReferenceType
 */
const char* tN2kSpeedWaterReferenceTypeStrs[] = {"Paddle wheel","Pitot tube","Doppler","Correlation (ultra sound)","Electro Magnetic"};
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kSpeedWaterReferenceType to a
 * const char
 */
MakeN2kEnumTypeToStrFunc(tN2kSpeedWaterReferenceType,tN2kSpeedWaterReferenceTypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kMagneticVariation
 */
const char* tN2kMagneticVariationStrs[] = { "Manual","Automatic Chart","Automatic Table","Automatic Calculation","WMM 2000","WMM 2005","WMM 2010","WMM 2015","WMM 2020" };
/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kMagneticVariation to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kMagneticVariation, tN2kSpeedWaterReferenceTypeStrs);

/************************************************************************//**
 * \brief string representation of the N2k enum tN2kXTEModeStrs
 */
const char* const tN2kXTEModeStrs[] = { "Autonomous", "Differential", "Estimated", "Simulator" };

/************************************************************************//**
 * \brief Converting the N2k enum \ref tN2kXTEModeStrs to a const char
 */
MakeN2kEnumTypeToStrFunc(tN2kXTEMode, tN2kXTEModeStrs);

#endif
