/*
N2kGroupFunctionDefaultHandlers.cpp

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

#include "N2kGroupFunctionDefaultHandlers.h"
#include "NMEA2000.h"

#define N2kPGN60928_UniqueNumber_field 1
#define N2kPGN60928_ManufacturerCode_field 2
#define N2kPGN60928_DeviceInstanceLower_field 3
#define N2kPGN60928_DeviceInstanceUpper_field 4
#define N2kPGN60928_DeviceFunction_field 5
#define N2kPGN60928_Reserved_field 6
#define N2kPGN60928_DeviceClass_field 7
#define N2kPGN60928_SystemInstance_field 8
#define N2kPGN60928_InsdustryGroup_field 9
#define N2kPGN60928_ISOSelfConfigurable_field 10

template <typename T> void MatchRequestField(T FieldVal, T MatchVal, T Mask, bool &Match, tN2kGroupFunctionParameterErrorCode &ErrorCode)
{
  if ( (FieldVal&Mask)!=MatchVal ) {
    ErrorCode=N2kgfpec_RequestOrCommandParameterOutOfRange;
    Match=false;
  } else ErrorCode=N2kgfpec_Acknowledge;
}

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
bool tN2kGroupFunctionHandlerForPGN60928::HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t /*TransmissionIntervalOffset*/, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=(TransmissionInterval==0xFFFFFFFF?N2kgfTPec_Acknowledge:N2kgfTPec_TransmitIntervalOrPriorityNotSupported);
  bool MatchFilter=true;
  
  if ( NumberOfParameterPairs>0 ) { // We need to filter accroding to fiels
    int i;
    int Index;
    uint8_t field;
    tNMEA2000::tDeviceInformation DI=pNMEA2000->GetDeviceInformation(iDev);
    tN2kMsg N2kRMsg;
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField=false;
    
    // Start to build response
    SetStartAcknowledge(N2kRMsg,PGN,
                        N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                        pec,
                        NumberOfParameterPairs);
    N2kRMsg.Destination=N2kMsg.Source;
    
    StartParseRequestPairParameters(N2kMsg,Index);
    // Next read new field values. Note that if message is not broadcast, we need to parse all fields allways.
    for (i=0; i<NumberOfParameterPairs && (MatchFilter || !tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if ( !FoundInvalidField) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case N2kPGN60928_UniqueNumber_field: 
            MatchRequestField(N2kMsg.Get4ByteUInt(Index),DI.GetUniqueNumber(),(uint32_t)0x001fffff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_ManufacturerCode_field: 
            MatchRequestField(N2kMsg.Get2ByteUInt(Index),DI.GetManufacturerCode(),(uint16_t)0x07ff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_DeviceInstanceLower_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetDeviceInstanceLower(),(uint8_t)0x07,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_DeviceInstanceUpper_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetDeviceInstanceUpper(),(uint8_t)0x1f,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_DeviceFunction_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetDeviceFunction(),(uint8_t)0xff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_Reserved_field: 
            N2kMsg.GetByte(Index);
            FieldErrorCode=N2kgfpec_Acknowledge;
            break;
          case N2kPGN60928_DeviceClass_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetDeviceClass(),(uint8_t)0x7f,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_SystemInstance_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetSystemInstance(),(uint8_t)0x0f,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_InsdustryGroup_field: 
            MatchRequestField(N2kMsg.GetByte(Index),DI.GetIndustryGroup(),(uint8_t)0x07,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN60928_ISOSelfConfigurable_field: 
            N2kMsg.GetByte(Index);
            FieldErrorCode=N2kgfpec_Acknowledge;
            break;
          default:
            FieldErrorCode=N2kgfpec_InvalidRequestOrCommandParameterField;
            MatchFilter=false;
            FoundInvalidField=true;
        }
      } else {
        // If there is any invalid field, we can not parse others, since we do not
        // know right data length. So fo rest of the fields we can only send response below.
        FieldErrorCode=N2kgfpec_TemporarilyUnableToComply;
      }
      AddAcknowledgeParameter(N2kRMsg,i,FieldErrorCode);
    }

    // Send Acknowledge, if request was not broadcast and it did not match   
    if (!MatchFilter && !tNMEA2000::IsBroadcast(N2kMsg.Destination)) { 
      pNMEA2000->SendMsg(N2kRMsg,iDev); 
    }
  }
  
  if (MatchFilter) {
    // SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,PGN,N2kgfPGNec_Acknowledge,pec); // It was not clear should we also respond with acknowledge
    pNMEA2000->SendIsoAddressClaim(0xff,iDev);
  }
  
  return true;
}

//*****************************************************************************
// Command group function for 60928 can be used to set Device Instance Lower, Device Instance Upper and System Instance
// values on device name. They all are in padded to one byte. 
bool tN2kGroupFunctionHandlerForPGN60928::HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev) {
  int i;
  int Index;
  uint8_t field;
  uint8_t DILower=0xff;
  uint8_t DIUpper=0xff;
  uint8_t SI=0xff;
  bool HasInvalidField=false;
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=N2kgfTPec_Acknowledge;
  tN2kMsg N2kRMsg;
  
    if (PrioritySetting!=8) pec=N2kgfTPec_TransmitIntervalOrPriorityNotSupported;
    StartParseCommandPairParameters(N2kMsg,Index);
    // Next read new field values
    for (i=0; i<NumberOfParameterPairs; i++) {
      field=N2kMsg.GetByte(Index);
      switch (field) {
        case N2kPGN60928_DeviceInstanceLower_field: 
          DILower=N2kMsg.GetByte(Index) & 0x7;
          break;
        case N2kPGN60928_DeviceInstanceUpper_field: 
          DIUpper=N2kMsg.GetByte(Index) & 0x1f;
          break;
        case N2kPGN60928_SystemInstance_field: 
          SI=N2kMsg.GetByte(Index) & 0x0f;
          break;
        default: 
          HasInvalidField=true;
      }
    }
    
    // Now build response
    SetStartAcknowledge(N2kRMsg,PGN,
                        N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                        pec,
                        (HasInvalidField?NumberOfParameterPairs:0));
    
    if ( HasInvalidField ) {
    // If there were invalid field, we respond all fields
      StartParseCommandPairParameters(N2kMsg,Index);
      for (i=0; i<NumberOfParameterPairs; i++) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case N2kPGN60928_DeviceInstanceLower_field: 
          case N2kPGN60928_DeviceInstanceUpper_field: 
          case N2kPGN60928_SystemInstance_field: 
            AddAcknowledgeParameter(N2kRMsg,i,N2kgfpec_Acknowledge);
            break;
          default: 
            AddAcknowledgeParameter(N2kRMsg,i,N2kgfpec_InvalidRequestOrCommandParameterField);
        }
      }
    }
    
    pNMEA2000->SendMsg(N2kRMsg,iDev);
    pNMEA2000->SetDeviceInformationInstances(DILower,DIUpper,SI,iDev);
    
    return true;
}  

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)    
//*****************************************************************************
// See document https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf for
// Heartbeat settings
bool tN2kGroupFunctionHandlerForPGN126993::HandleRequest(const tN2kMsg &N2kMsg, 
                               uint32_t TransmissionInterval, 
                               uint16_t /*TransmissionIntervalOffset*/, 
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  if ( NumberOfParameterPairs==0 ) { // According to doc, there should not be any parameter pairs defined
    pNMEA2000->SetHeartbeatInterval(TransmissionInterval,false,iDev);
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,PGN,N2kgfPGNec_Acknowledge,N2kgfTPec_Acknowledge);
    return true;
  } else {
    SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,PGN,
                    N2kgfPGNec_Acknowledge,
                    N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                    NumberOfParameterPairs, N2kgfpec_RequestOrCommandNotSupported);
    return true;
  }
}
#endif

