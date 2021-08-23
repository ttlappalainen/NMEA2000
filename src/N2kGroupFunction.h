/*
N2kGroupFunction.h

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

#ifndef _N2kGroupFunction_H_
#define _N2kGroupFunction_H_

#ifndef ARDUINO
#include <cstring>
#endif
#include "NMEA2000_CompilerDefns.h"
#include "N2kMsg.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

enum tN2kGroupFunctionCode {
                            N2kgfc_Request=0,
                            N2kgfc_Command=1,
                            N2kgfc_Acknowledge=2,
                            N2kgfc_Read=3,
                            N2kgfc_ReadReply=4,
                            N2kgfc_Write=5,
                            N2kgfc_WriteReply=6
                          };

enum tN2kGroupFunctionPGNErrorCode {
                            N2kgfPGNec_Acknowledge=0,
                            N2kgfPGNec_PGNNotSupported=1,
                            N2kgfPGNec_PGNTemporarilyNotAvailable=2,
                            N2kgfPGNec_AccessDenied=3,
                            N2kgfPGNec_RequestOrCommandNotSupported=4,
                            N2kgfPGNec_DefinerTagNotSupported=5,
                            N2kgfPGNec_ReadOrWriteNotSupported=6
                          };

enum tN2kGroupFunctionTransmissionOrPriorityErrorCode {
                            N2kgfTPec_Acknowledge=0,
                            N2kgfTPec_TransmitIntervalOrPriorityNotSupported=1,
                            N2kgfTPec_TransmitIntervalIsLessThanMeasurementInterval=2,
                            N2kgfTPec_AccessDenied=3,
                            N2kgfTPec_RequestNotSupported=4
                          };

enum tN2kGroupFunctionParameterErrorCode {
                            N2kgfpec_Acknowledge=0,
                            N2kgfpec_InvalidRequestOrCommandParameterField=1,
                            N2kgfpec_TemporarilyUnableToComply=2,
                            N2kgfpec_RequestOrCommandParameterOutOfRange=3,
                            N2kgfpec_AccessDenied=4,
                            N2kgfpec_RequestOrCommandNotSupported=5,
                            N2kgfpec_ReadOrWriteIsNotSupported=6
                          };

class tNMEA2000;

class tN2kGroupFunctionHandler {
  public:
    template <typename T> void MatchRequestField(T FieldVal, T MatchVal, T Mask, bool &Match, tN2kGroupFunctionParameterErrorCode &ErrorCode)
    {
      if ( (FieldVal&Mask)!=MatchVal ) {
        ErrorCode=N2kgfpec_RequestOrCommandParameterOutOfRange;
        Match=false;
      } else ErrorCode=N2kgfpec_Acknowledge;
    }

    void MatchRequestField(const char * FieldVal, const char * MatchVal, bool &Match, tN2kGroupFunctionParameterErrorCode &ErrorCode)
    {
      Match&=(strcmp(FieldVal,MatchVal)==0);
      ErrorCode = ( Match ? N2kgfpec_Acknowledge : N2kgfpec_RequestOrCommandParameterOutOfRange );
    }

  private:
    tN2kGroupFunctionHandler *pNext;
    friend class tNMEA2000;

  protected:
    unsigned long PGN;
    bool Proprietary;
    tNMEA2000 *pNMEA2000;

  protected:
    // This is default handler for Complex Request transmission interval setting. Overwrite it, if your PGN will support
    // changing interval.
    virtual tN2kGroupFunctionTransmissionOrPriorityErrorCode GetRequestGroupFunctionTransmissionOrPriorityErrorCode(uint32_t TransmissionInterval);

    virtual bool HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t TransmissionIntervalOffset,
                               uint8_t  NumberOfParameterPairs,
                               int iDev);

    virtual bool HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev);

    // This function handles Acknowledge group function, whis is response for Request, Command, ReadFields or WriteFields
    // group function. So you need to override this, if your device will send one of those commands.
    virtual bool HandleAcknowledge(const tN2kMsg &N2kMsg,
                                   tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                   tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                   uint8_t NumberOfParameterPairs,
                                   int iDev);

    virtual bool HandleReadFields(const tN2kMsg &N2kMsg,
                                  uint16_t ManufacturerCode, // This will be set to 0xffff for non-propprietary PNGs
                                  uint8_t IndustryGroup, // This will be set to 0xff for non-propprietary PNGs
                                  uint8_t UniqueID,
                                  uint8_t NumberOfSelectionPairs,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev);
    // Under construction
    virtual bool HandleReadFieldsReply(const tN2kMsg &N2kMsg,int iDev);
    virtual bool HandleWriteFields(const tN2kMsg &N2kMsg,
                                  uint16_t ManufacturerCode, // This will be set to 0xffff for non-propprietary PNGs
                                  uint8_t IndustryGroup, // This will be set to 0xff for non-propprietary PNGs
                                  uint8_t UniqueID,
                                  uint8_t NumberOfSelectionPairs,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev);
    // Under construction
    virtual bool HandleWriteFieldsReply(const tN2kMsg &N2kMsg,int iDev);

  public:
    tN2kGroupFunctionHandler(tNMEA2000 *_pNMEA2000, unsigned long _PGN);

    virtual bool Handle(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev);

    // Static functions
    static unsigned long GetPGNForGroupFunction(const tN2kMsg &N2kMsg);

    // Parse group function code and PGN for it
    static bool Parse(const tN2kMsg &N2kMsg,
                            tN2kGroupFunctionCode &GroupFunctionCode,
                            unsigned long &PGNForGroupFunction);

    static bool ParseRequestParams(const tN2kMsg &N2kMsg,
                               uint32_t &TransmissionInterval,
                               uint16_t &TransmissionIntervalOffset,
                               uint8_t  &NumberOfParameterPairs);

    static bool StartParseRequestPairParameters(const tN2kMsg &N2kMsg, int &Index);

    static bool ParseCommandParams(const tN2kMsg &N2kMsg,
                               uint8_t &PrioritySetting,
                               uint8_t &NumberOfParameterPairs);

    static bool StartParseCommandPairParameters(const tN2kMsg &N2kMsg, int &Index);

    static bool ParseAcknowledgeParams(const tN2kMsg &N2kMsg,
                               tN2kGroupFunctionPGNErrorCode &PGNErrorCode,
                               tN2kGroupFunctionTransmissionOrPriorityErrorCode &TransmissionOrPriorityErrorCode,
                               uint8_t &NumberOfParameterPairs);

    static bool StartParseReadOrWriteParameters(const tN2kMsg &N2kMsg, bool Proprietary, int &Index);

    static bool ParseReadOrWriteParams(const tN2kMsg &N2kMsg,
                               uint16_t &ManufacturerCode,
                               uint8_t &IndustryGroup,
                               uint8_t &UniqueID,
                               uint8_t &NumberOfSelectionPairs,
                               uint8_t &NumberOfParameterPairs,
                               bool Proprietary=false);

    static void SetStartReadReply(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                               uint16_t ManufacturerCode,
                               uint8_t IndustryGroup,
                               uint8_t UniqueID,
                               uint8_t NumberOfSelectionPairs,
                               uint8_t NumberOfParameterPairs,
                               bool Proprietary);

    static void SetStartWriteReply(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                               uint16_t ManufacturerCode,
                               uint8_t IndustryGroup,
                               uint8_t UniqueID,
                               uint8_t NumberOfSelectionPairs,
                               uint8_t NumberOfParameterPairs,
                               bool Proprietary);

    static void SetStartAcknowledge(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs=0);
    static void ChangePNGErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionPGNErrorCode PGNErrorCode);
    static void ChangeTransmissionOrPriorityErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode);
    static void AddAcknowledgeParameter(tN2kMsg &N2kMsg,
                                         uint8_t ParameterPairIndex,
                                         tN2kGroupFunctionParameterErrorCode ErrorCode=N2kgfpec_ReadOrWriteIsNotSupported);

    static void SendAcknowledge(tNMEA2000 *pNMEA2000, unsigned char Destination, int iDev, unsigned long PGN,
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs=0,
                                         tN2kGroupFunctionParameterErrorCode ParameterErrorCodeForAll=N2kgfpec_Acknowledge);

};

#endif

#endif
