/*
N2kGroupFunction.cpp

Copyright (c) 2015-2017 Timo Lappalainen, Kave Oy, www.kave.fi

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


#include "N2kGroupFunction.h"
#include "NMEA2000.h"


//*****************************************************************************
tN2kGroupFunctionHandler::tN2kGroupFunctionHandler(tNMEA2000 *_pNMEA2000, unsigned long _PGN)  {
  pNext=0;
  pNMEA2000=_pNMEA2000;
  PGN=_PGN;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::Handle(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev) {
  if ( PGN!=PGNForGroupFunction && PGN!=0 ) return false;
  
  bool handled=false;
  uint8_t NumberOfParameterPairs;
  
  switch (GroupFunctionCode) {
    case N2kgfc_Request:
      uint32_t TransmissionInterval;
      uint16_t TransmissionIntervalOffset;
      
        if (ParseRequestParams(N2kMsg,TransmissionInterval,TransmissionIntervalOffset,NumberOfParameterPairs)) {
          handled=HandleRequest(N2kMsg,TransmissionInterval,TransmissionIntervalOffset,NumberOfParameterPairs,iDev);
        }
      break;
    case N2kgfc_Command:
      uint8_t PrioritySetting;
      
        if (ParseCommandParams(N2kMsg,PrioritySetting,NumberOfParameterPairs)) {
          handled=HandleCommand(N2kMsg,PrioritySetting,NumberOfParameterPairs,iDev);
        }
      break;
    case N2kgfc_Acknowledge:
      tN2kGroupFunctionPGNErrorCode PGNErrorCode;
      tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode;
      
        if (ParseAcknowledgeParams(N2kMsg,PGNErrorCode,TransmissionOrPriorityErrorCode,NumberOfParameterPairs)) {
          handled=HandleAcknowledge(N2kMsg,PGNErrorCode,TransmissionOrPriorityErrorCode,NumberOfParameterPairs,iDev);
        }
      break;
    case N2kgfc_Read:
      break;
    case N2kgfc_ReadReply:
      break;
    case N2kgfc_Write:
      break;
    case N2kgfc_WriteReply:
      break;

  }

  return handled;  
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t /*TransmissionInterval*/, 
                               uint16_t /*TransmissionIntervalOffset*/, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  
    // As default we respond with not supported.
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,GetPGNForGroupFunction(N2kMsg),
                    N2kgfPGNec_RequestOrCommandNotSupported,
                    N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                    NumberOfParameterPairs, N2kgfpec_RequestOrCommandNotSupported);
                    
    return true;  
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleCommand(const tN2kMsg &N2kMsg, uint8_t /*PrioritySetting*/, uint8_t  NumberOfParameterPairs, int iDev) {

    // As default we respond with not supported.
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,GetPGNForGroupFunction(N2kMsg),
                    N2kgfPGNec_RequestOrCommandNotSupported,
                    N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                    NumberOfParameterPairs, N2kgfpec_RequestOrCommandNotSupported);
                    
    return true;  
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleAcknowledge(const tN2kMsg &/*N2kMsg*/, 
                                   tN2kGroupFunctionPGNErrorCode /*PGNErrorCode*/,
                                   tN2kGroupFunctionTransmissionOrPriorityErrorCode /*TransmissionOrPriorityErrorCode*/,
                                   uint8_t /*ParameterCount*/,
                                   int /*iDev*/) {
    // As default, simply return true meaning that received acknowledge has been handled.
    return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleReadFields(const tN2kMsg &N2kMsg,
                                  uint16_t /*ManufacturerCode*/, // This will be set to 0xffff for non-propprietary PNGs
                                  uint8_t /*IndustryGroup*/, // This will be set to 0xff for non-propprietary PNGs 
                                  uint8_t /*UniqueID*/,
                                  uint8_t /*NumberOfSelectionPairs*/,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev) {

    // As default we respond with not supported.
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,GetPGNForGroupFunction(N2kMsg),
                    N2kgfPGNec_ReadOrWriteNotSupported,
                    N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                    NumberOfParameterPairs, N2kgfpec_ReadOrWriteIsNotSupported);
    return true;
}
                                  
//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleReadFieldsReply(const tN2kMsg &/*N2kMsg*/,int /*iDev*/) {
    return true;
}


//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleWriteFields(const tN2kMsg &N2kMsg,
                                  uint16_t /*ManufacturerCode*/, // This will be set to 0xffff for non-propprietary PNGs
                                  uint8_t /*IndustryGroup*/, // This will be set to 0xff for non-propprietary PNGs 
                                  uint8_t /*UniqueID*/,
                                  uint8_t /*NumberOfSelectionPairs*/,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev) {
    // As default we respond with not supported.
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,GetPGNForGroupFunction(N2kMsg),
                    N2kgfPGNec_ReadOrWriteNotSupported,
                    N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                    NumberOfParameterPairs, N2kgfpec_ReadOrWriteIsNotSupported);
                    
    return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::HandleWriteFieldsReply(const tN2kMsg &/*N2kMsg*/,int /*iDev*/) {
    return true;
}

// Static functions for PGN 126208 handling
//*****************************************************************************
unsigned long tN2kGroupFunctionHandler::GetPGNForGroupFunction(const tN2kMsg &N2kMsg) {
  int Index=1;
  return N2kMsg.Get3ByteUInt(Index);
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::Parse(const tN2kMsg &N2kMsg, 
                            tN2kGroupFunctionCode &GroupFunctionCode, 
                            unsigned long &PGNForGroupFunction) {
  if (N2kMsg.PGN!=126208L) return false;

  GroupFunctionCode=(tN2kGroupFunctionCode)(N2kMsg.Data[0]);
  PGNForGroupFunction=GetPGNForGroupFunction(N2kMsg);
  
  return true;
}

#define N2kgf_OffsetToParams 4
#define N2kgf_OffsetToRequestPairParameters 11
#define N2kgf_OffsetToCommandPairParameters 6

//*****************************************************************************
bool tN2kGroupFunctionHandler::ParseRequestParams(const tN2kMsg &N2kMsg, 
                               uint32_t &TransmissionInterval, 
                               uint16_t &TransmissionIntervalOffset, 
                               uint8_t  &NumberOfParameterPairs) {
  if (N2kMsg.PGN!=126208L) return false;
  int Index=N2kgf_OffsetToParams;
  TransmissionInterval=N2kMsg.Get4ByteUInt(Index);
  TransmissionIntervalOffset=N2kMsg.Get2ByteUInt(Index);
  NumberOfParameterPairs=N2kMsg.GetByte(Index);
  
  return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::StartParseRequestPairParameters(const tN2kMsg &N2kMsg, int &Index) {
  if (N2kMsg.PGN!=126208L) return false;
  Index=N2kgf_OffsetToRequestPairParameters;
  
  return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::ParseCommandParams(const tN2kMsg &N2kMsg, 
                               uint8_t &PrioritySetting,
                               uint8_t &NumberOfParameterPairs) {
  if (N2kMsg.PGN!=126208L) return false;
  int Index=N2kgf_OffsetToParams;
  PrioritySetting=N2kMsg.GetByte(Index);
  NumberOfParameterPairs=N2kMsg.GetByte(Index);
  
  return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::StartParseCommandPairParameters(const tN2kMsg &N2kMsg, int &Index) {
  if (N2kMsg.PGN!=126208L) return false;
  Index=N2kgf_OffsetToCommandPairParameters;
  
  return true;
}

//*****************************************************************************
bool tN2kGroupFunctionHandler::ParseAcknowledgeParams(const tN2kMsg &N2kMsg, 
                               tN2kGroupFunctionPGNErrorCode &PGNErrorCode,
                               tN2kGroupFunctionTransmissionOrPriorityErrorCode &TransmissionOrPriorityErrorCode,
                               uint8_t &NumberOfParameterPairs) {
  PGNErrorCode=N2kgfPGNec_PGNNotSupported;
  TransmissionOrPriorityErrorCode=N2kgfTPec_TransmitIntervalOrPriorityNotSupported;
  NumberOfParameterPairs=0;                               
  if (N2kMsg.PGN!=126208L) return false;
  int Index=N2kgf_OffsetToParams;
  uint8_t b=N2kMsg.GetByte(Index);
  PGNErrorCode=(tN2kGroupFunctionPGNErrorCode)(b&0x0f);
  TransmissionOrPriorityErrorCode=(tN2kGroupFunctionTransmissionOrPriorityErrorCode)(b>>4);
  
  return true;
}

//*****************************************************************************
void tN2kGroupFunctionHandler::SetStartAcknowledge(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN, 
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs) {
	N2kMsg.SetPGN(126208L);
	N2kMsg.Priority=7;
  N2kMsg.Destination=Destination;
	N2kMsg.AddByte(N2kgfc_Acknowledge);
  N2kMsg.Add3ByteInt(PGN);
  N2kMsg.AddByte(PGNErrorCode | TransmissionOrPriorityErrorCode<<4);
  N2kMsg.AddByte(NumberOfParameterPairs);
}  

//*****************************************************************************
void tN2kGroupFunctionHandler::AddAcknowledgeParameter(tN2kMsg &N2kMsg, 
                                         uint8_t ParameterPairIndex, 
                                         tN2kGroupFunctionParameterErrorCode ErrorCode) {
  if ( ((ParameterPairIndex % 2) == 0) && (N2kMsg.DataLen>0) ) {
    N2kMsg.AddByte((uint8_t)(ErrorCode) | 0x0f << 4);
  } else {
    // Should actually add AddBits to N2kMsg
    N2kMsg.Data[N2kMsg.DataLen-1]=(N2kMsg.Data[N2kMsg.DataLen-1] & 0x0f) | ((uint8_t)(ErrorCode) << 4);
  }  
}

//*****************************************************************************
void tN2kGroupFunctionHandler::SendAcknowledge(tNMEA2000 *pNMEA2000, unsigned char Destination, int iDev, unsigned long PGN, 
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs,
                                         tN2kGroupFunctionParameterErrorCode ParameterErrorCodeForAll) {
  tN2kMsg N2kRMsg;
  
    // As default we respond with not supported.
    SetStartAcknowledge(N2kRMsg,Destination,PGN,
                        PGNErrorCode,
                        TransmissionOrPriorityErrorCode,
                        NumberOfParameterPairs);
    for (uint8_t ParamIndex=0; ParamIndex<NumberOfParameterPairs;ParamIndex++) {
      AddAcknowledgeParameter(N2kRMsg,ParamIndex,ParameterErrorCodeForAll);
    }
    pNMEA2000->SendMsg(N2kRMsg,iDev);
}
