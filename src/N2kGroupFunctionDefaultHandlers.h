/* 
 * N2kGroupFunctionDefaultHandlers.h
 *
 * The MIT License
 *
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
 * \file  N2kGroupFunctionDefaultHandlers.h
 * \brief The file contains default group function handler classes
 * 
 * The file contains default group function handler classes for PGN 60928, 
 * 126464, 126993, 126996 and 126998. Some of these handlers are mandatory 
 * for NMEA 2000 certified devices.
 * 
 */

#ifndef _N2kGroupFunctionDefaultHandlers_H_
#define _N2kGroupFunctionDefaultHandlers_H_

#include "NMEA2000_CompilerDefns.h"
#include "N2kGroupFunction.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

/************************************************************************//**
 * \class   tN2kGroupFunctionHandlerForPGN60928
 * \brief   Default Group Function Handler for PGN 60928 - "ISO Address Claim"
 * \ingroup group_coreSupplementary
 * 
 * For requirements for handling Group function request for PGN 60928 - 
 * "ISO Address Claim" 
 * 
 *  \note This handler is mandatory for NMEA 2000 certified devices.
 * 
 * This network management message is used to claim a network address and
 * to respond with device information (NAME) requested by the ISO Request
 * or Complex Request Group Function. This PGN contains several fields that
 * are Request Parameters that can be used to control the expected response
 * to requests for this PGN.
 * 
 * [See document] (https://web.archive.org/web/20150910070107/http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf)
 *
 */
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


/************************************************************************//**
 * \class   tN2kGroupFunctionHandlerForPGN126464
 * \brief   Default Group Function Handler for PGN 126464 - 
 *          "PGN List - Transmit/Receive PGNs group function"
 * \ingroup group_coreSupplementary
 * 
 *  \note This handler is mandatory for NMEA 2000 certified devices.
 * 
 * The PGN List group function type is defined by the first field. The 
 * message will be either a Transmit PGNs or a Receive PGNs group function
 * that identifies the PGNs transmitted from or received by a node. 
 * 
 */
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

/************************************************************************//**
 * \class tN2kGroupFunctionHandlerForPGN126993
 * \brief Default Group Function Handler for PGN 126993 - "Heartbeat"
 * \ingroup group_coreSupplementary
 * 
 * 
 * This PGN shall be transmitted by all NMEA devices.
 * Reception of this PGN confirms that a device is still present on the 
 * network. Reception of this PGN may also be used to maintain an address
 * to NAME association table within the receiving device.
 * 
 * [See document](https://web.archive.org/web/20170609023206/https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf)
 * 
 */
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

/************************************************************************//**
 * \class tN2kGroupFunctionHandlerForPGN126996
 * \brief Default Group Function Handler for PGN 126996 - 
 *        Product Information
 * \ingroup group_coreSupplementary
 * 
 * Provides product information onto the network that could be important
 * for determining quality of data coming from this product.
 * 
 * \note This is not mandatory, but preferred
 */
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

/************************************************************************//**
 * \class tN2kGroupFunctionHandlerForPGN126998
 * \brief Default Group Function Handler for PGN 126998 - 
 *        "Configuration Information"
 * \ingroup group_coreSupplementary
 * 
 * Free-form alphanumeric fields describing the installation (e.g., starboard 
 * engine room location) of the device and installation notes (e.g.,
 * calibration data).
 * 
 * \note This is not mandatory, but preferred
 */
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
