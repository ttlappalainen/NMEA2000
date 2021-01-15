/*
N2kGroupFunctionDefaultHandlers.h

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

*/

/*
  The file contains default group function handler classes for PGN 60928, 126993.
  These handlers are mandatory for NMEA 2000 certified devices.
*/

#ifndef _N2kGroupFunctionDefaultHandlers_H_
#define _N2kGroupFunctionDefaultHandlers_H_

#include "NMEA2000_CompilerDefns.h"
#include "N2kGroupFunction.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
class tN2kGroupFunctionHandlerForPGN60928 : public tN2kGroupFunctionHandler {
  protected:
    virtual bool HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t TransmissionIntervalOffset, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
    virtual bool HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev);
  public:
    tN2kGroupFunctionHandlerForPGN60928(tNMEA2000 *_pNMEA2000) : tN2kGroupFunctionHandler(_pNMEA2000,60928L) {}
};

//*****************************************************************************
// This is mandatory.
class tN2kGroupFunctionHandlerForPGN126464 : public tN2kGroupFunctionHandler {
  protected:
    virtual bool HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t TransmissionIntervalOffset, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
  public:
    tN2kGroupFunctionHandlerForPGN126464(tNMEA2000 *_pNMEA2000) : tN2kGroupFunctionHandler(_pNMEA2000,126464L) {}
};

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)    
//*****************************************************************************
// See document https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf for
// Heartbeat settings
class tN2kGroupFunctionHandlerForPGN126993 : public tN2kGroupFunctionHandler {
  protected:
    virtual bool HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t TransmissionIntervalOffset, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
  public:
    tN2kGroupFunctionHandlerForPGN126993(tNMEA2000 *_pNMEA2000) : tN2kGroupFunctionHandler(_pNMEA2000,126993L) {}
};
#endif

//*****************************************************************************
// This is not mandatory, but prefered
class tN2kGroupFunctionHandlerForPGN126996 : public tN2kGroupFunctionHandler {
  protected:
    virtual bool HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t TransmissionIntervalOffset, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
  public:
    tN2kGroupFunctionHandlerForPGN126996(tNMEA2000 *_pNMEA2000) : tN2kGroupFunctionHandler(_pNMEA2000,126996L) {}
};

//*****************************************************************************
// This is not mandatory, but prefered
class tN2kGroupFunctionHandlerForPGN126998 : public tN2kGroupFunctionHandler {
  protected:
    virtual bool HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t TransmissionIntervalOffset, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
    virtual bool HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev);
  public:
    tN2kGroupFunctionHandlerForPGN126998(tNMEA2000 *_pNMEA2000) : tN2kGroupFunctionHandler(_pNMEA2000,126998L) {}
};

#endif
#endif
