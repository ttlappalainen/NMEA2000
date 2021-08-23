/*
N2kGroupFunctionDefaultHandlers.cpp

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

#include <string.h>
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
#define N2kPGN60928_IndustryGroup_field 9
#define N2kPGN60928_ISOSelfConfigurable_field 10

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
bool tN2kGroupFunctionHandlerForPGN60928::HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t /*TransmissionIntervalOffset*/,
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
  bool MatchFilter=true;
  tN2kMsg N2kRMsg;

  // Start to build response
  SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                      N2kgfPGNec_Acknowledge,  // Always acknoledge for mandatory PGN
                      pec,
                      NumberOfParameterPairs);
  N2kRMsg.Destination=N2kMsg.Source;

  if ( NumberOfParameterPairs>0 ) { // We need to filter accroding to fiels
    int i;
    int Index;
    uint8_t field;
    tNMEA2000::tDeviceInformation DI=pNMEA2000->GetDeviceInformation(iDev);
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField=false;

    StartParseRequestPairParameters(N2kMsg,Index);
    // Next read new field values. Note that if message is not broadcast, we need to parse all fields always.
    for (i=0; i<NumberOfParameterPairs && (MatchFilter || !tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if ( !FoundInvalidField) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case N2kPGN60928_UniqueNumber_field:
            MatchRequestField(N2kMsg.Get4ByteUInt(Index),(uint32_t)DI.GetUniqueNumber(),(uint32_t)0x001fffff,MatchFilter,FieldErrorCode);
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
          case N2kPGN60928_IndustryGroup_field:
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
  }

  // Send Acknowledge, if request was not broadcast and it did not match
  if ( (!MatchFilter || pec!=N2kgfTPec_Acknowledge) && !tNMEA2000::IsBroadcast(N2kMsg.Destination)) {
    pNMEA2000->SendMsg(N2kRMsg,iDev);
  }

  if (MatchFilter) {
    // SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,PGN,N2kgfPGNec_Acknowledge,pec); // It was not clear should we also respond with acknowledge
    // Send delayed so that ack will be sent first.
    pNMEA2000->SendIsoAddressClaim(0xff,iDev,2);
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
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=N2kgfTPec_Acknowledge;
  tN2kGroupFunctionParameterErrorCode PARec;
  tN2kMsg N2kRMsg;

    SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                        N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                        pec,
                        NumberOfParameterPairs);

    if (PrioritySetting!=8) pec=N2kgfTPec_TransmitIntervalOrPriorityNotSupported;
    StartParseCommandPairParameters(N2kMsg,Index);
    // Next read new field values
    for (i=0; i<NumberOfParameterPairs; i++) {
      field=N2kMsg.GetByte(Index);
      PARec=N2kgfpec_Acknowledge;
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
          PARec=N2kgfpec_InvalidRequestOrCommandParameterField;
      }

      AddAcknowledgeParameter(N2kRMsg,i,PARec);
    }

    pNMEA2000->SendMsg(N2kRMsg,iDev);
    pNMEA2000->SetDeviceInformationInstances(DILower,DIUpper,SI,iDev);

    return true;
}

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
bool tN2kGroupFunctionHandlerForPGN126464::HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t /*TransmissionIntervalOffset*/,
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
  bool MatchFilter=true;
  uint8_t RespondTxRx=0xff;
  tN2kMsg N2kRMsg;

  // Start to build response
  SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                      N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                      pec,
                      NumberOfParameterPairs);
  N2kRMsg.Destination=N2kMsg.Source;

  if ( NumberOfParameterPairs>0 ) { // We need to filter accroding to fiels
    int i;
    int Index;
    uint8_t field;
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField=false;

    StartParseRequestPairParameters(N2kMsg,Index);
    // Next read new field values. Note that if message is not broadcast, we need to parse all fields always.
    for (i=0; i<NumberOfParameterPairs && (MatchFilter || !tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if ( !FoundInvalidField) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case 1: // Selection of Tx or Rx
            RespondTxRx=N2kMsg.GetByte(Index);
            if ( RespondTxRx==N2kpgnl_transmit || RespondTxRx==N2kpgnl_receive ) {
              FieldErrorCode=N2kgfpec_Acknowledge;
            } else {
              FieldErrorCode=N2kgfpec_RequestOrCommandParameterOutOfRange;
              MatchFilter=false;
            }
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
  }

  // Send Acknowledge, if request was not broadcast and it did not match
  if ( (!MatchFilter || pec!=N2kgfTPec_Acknowledge) && !tNMEA2000::IsBroadcast(N2kMsg.Destination)) {
    pNMEA2000->SendMsg(N2kRMsg,iDev);
  }

  if (MatchFilter) {
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    unsigned char dest=N2kMsg.Source;
    if ( N2kMsg.IsTPMessage() && tNMEA2000::IsBroadcast(N2kMsg.Destination) ) dest=N2kMsg.Destination;
    if ( (RespondTxRx==N2kpgnl_transmit) || (RespondTxRx==0xff) ) pNMEA2000->SendTxPGNList(dest,iDev,N2kMsg.IsTPMessage());
    if ( (RespondTxRx==N2kpgnl_receive) || (RespondTxRx==0xff) ) pNMEA2000->SendRxPGNList(dest,iDev,N2kMsg.IsTPMessage());
#else
    if ( (RespondTxRx==N2kpgnl_transmit) || (RespondTxRx==0xff) ) pNMEA2000->SendTxPGNList(N2kMsg.Source,iDev);
    if ( (RespondTxRx==N2kpgnl_receive) || (RespondTxRx==0xff) ) pNMEA2000->SendRxPGNList(N2kMsg.Source,iDev);
#endif
  }

  return true;
}

#define N2kPGN126996_NMEA2000DatabaseVersion_field 1
#define N2kPGN126996_NMEAManufacturersProductCode_field 2
#define N2kPGN126996_ManufacturersModelID_field 3
#define N2kPGN126996_ManufacturersSoftwareVersionCode_field 4
#define N2kPGN126996_ManufacturersModelVersion_field 5
#define N2kPGN126996_ManufacturersModelSerialCode_field 6
#define N2kPGN126996_NMEA2000CertificationLevel_field 7
#define N2kPGN126996_LoadEquivalency_field 8

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
bool tN2kGroupFunctionHandlerForPGN126996::HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t /*TransmissionIntervalOffset*/,
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
  bool MatchFilter=true;
  tN2kMsg N2kRMsg;

  // Start to build response
  SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                      N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                      pec,
                      NumberOfParameterPairs);
  N2kRMsg.Destination=N2kMsg.Source;

  if ( NumberOfParameterPairs>0 ) { // We need to filter accroding to fiels
    int i;
    int Index;
    uint8_t field;
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField=false;
    size_t strSize=Max_N2kProductInfoStrLen;
    char Query[strSize];
    char CurVal[strSize];

    StartParseRequestPairParameters(N2kMsg,Index);
    // Next read new field values. Note that if message is not broadcast, we need to parse all fields always.
    for (i=0; i<NumberOfParameterPairs && (MatchFilter || !tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if ( !FoundInvalidField) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case N2kPGN126996_NMEA2000DatabaseVersion_field:
            MatchRequestField(N2kMsg.Get2ByteUInt(Index),(uint16_t)pNMEA2000->GetN2kVersion(iDev),(uint16_t)0xffff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_NMEAManufacturersProductCode_field:
            MatchRequestField(N2kMsg.Get2ByteUInt(Index),(uint16_t)pNMEA2000->GetProductCode(iDev),(uint16_t)0xffff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_ManufacturersModelID_field:
            N2kMsg.GetStr(strSize,Query,Max_N2kModelID_len,0xff,Index);
            pNMEA2000->GetModelID(CurVal,strSize,iDev);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_ManufacturersSoftwareVersionCode_field:
            N2kMsg.GetStr(strSize,Query,Max_N2kSwCode_len,0xff,Index);
            pNMEA2000->GetModelID(CurVal,strSize,iDev);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_ManufacturersModelVersion_field:
            N2kMsg.GetStr(strSize,Query,Max_N2kModelVersion_len,0xff,Index);
            pNMEA2000->GetModelID(CurVal,strSize,iDev);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_ManufacturersModelSerialCode_field:
            N2kMsg.GetStr(strSize,Query,Max_N2kModelSerialCode_len,0xff,Index);
            pNMEA2000->GetModelID(CurVal,strSize,iDev);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_NMEA2000CertificationLevel_field:
            MatchRequestField(N2kMsg.GetByte(Index),(uint8_t)pNMEA2000->GetCertificationLevel(iDev),(uint8_t)0xff,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126996_LoadEquivalency_field:
            MatchRequestField(N2kMsg.GetByte(Index),(uint8_t)pNMEA2000->GetLoadEquivalency(iDev),(uint8_t)0xff,MatchFilter,FieldErrorCode);
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
  }

  // Send Acknowledge, if request was not broadcast and it did not match
  if ( (!MatchFilter || pec!=N2kgfTPec_Acknowledge) && !tNMEA2000::IsBroadcast(N2kMsg.Destination)) {
    pNMEA2000->SendMsg(N2kRMsg,iDev);
  }

  if (MatchFilter) {
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    unsigned char dest=N2kMsg.Source;
    if ( N2kMsg.IsTPMessage() && tNMEA2000::IsBroadcast(N2kMsg.Destination) ) dest=N2kMsg.Destination;
    pNMEA2000->SendProductInformation(dest,iDev,N2kMsg.IsTPMessage());
#else
    pNMEA2000->SendProductInformation(iDev);
#endif
  }

  return true;
}

#define N2kPGN126998_InstallationDescription1_field 1
#define N2kPGN126998_InstallationDescription2_field 2
#define N2kPGN126998_ManufacturerInformation_field 3

//*****************************************************************************
// See document http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf
// For requirements for handling Group function request for PGN 60928
bool tN2kGroupFunctionHandlerForPGN126998::HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t /*TransmissionIntervalOffset*/,
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
  bool MatchFilter=true;
  tN2kMsg N2kRMsg;

  // Start to build response
  SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                      N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                      pec,
                      NumberOfParameterPairs);
  N2kRMsg.Destination=N2kMsg.Source;

  if ( NumberOfParameterPairs>0 ) { // We need to filter accroding to fiels
    int i;
    int Index;
    uint8_t field;
    tN2kGroupFunctionParameterErrorCode FieldErrorCode;
    bool FoundInvalidField=false;
    char Query[Max_N2kConfigurationInfoField_len];
    char CurVal[Max_N2kConfigurationInfoField_len];
    size_t QueryStrSize;

    StartParseRequestPairParameters(N2kMsg,Index);
    // Next read new field values. Note that if message is not broadcast, we need to parse all fields always.
    for (i=0; i<NumberOfParameterPairs && (MatchFilter || !tNMEA2000::IsBroadcast(N2kMsg.Destination)); i++) {
      if ( !FoundInvalidField) {
        field=N2kMsg.GetByte(Index);
        switch (field) {
          case N2kPGN126998_InstallationDescription1_field:
            QueryStrSize=Max_N2kConfigurationInfoField_len;
            N2kMsg.GetVarStr(QueryStrSize,Query,Index);
            pNMEA2000->GetInstallationDescription1(CurVal,Max_N2kConfigurationInfoField_len);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126998_InstallationDescription2_field:
            QueryStrSize=Max_N2kConfigurationInfoField_len;
            N2kMsg.GetVarStr(QueryStrSize,Query,Index);
            pNMEA2000->GetInstallationDescription2(CurVal,Max_N2kConfigurationInfoField_len);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
            break;
          case N2kPGN126998_ManufacturerInformation_field:
            QueryStrSize=Max_N2kConfigurationInfoField_len;
            N2kMsg.GetVarStr(QueryStrSize,Query,Index);
            pNMEA2000->GetManufacturerInformation(CurVal,Max_N2kConfigurationInfoField_len);
            MatchRequestField(Query,CurVal,MatchFilter,FieldErrorCode);
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

  }

  // Send Acknowledge, if request was not broadcast and it did not match
  if ( (!MatchFilter || pec!=N2kgfTPec_Acknowledge) && !tNMEA2000::IsBroadcast(N2kMsg.Destination)) {
    pNMEA2000->SendMsg(N2kRMsg,iDev);
  }

  if (MatchFilter) {
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    unsigned char dest=N2kMsg.Source;
    if ( N2kMsg.IsTPMessage() && tNMEA2000::IsBroadcast(N2kMsg.Destination) ) dest=N2kMsg.Destination;
    pNMEA2000->SendConfigurationInformation(dest,iDev,N2kMsg.IsTPMessage());
#else
    pNMEA2000->SendConfigurationInformation(iDev);
#endif
  }

  return true;
}

//*****************************************************************************
// Command group function for 126998 can be used to set installation description 1 and 2 fields
bool tN2kGroupFunctionHandlerForPGN126998::HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev) {
  int i;
  int Index;
  uint8_t field;
  size_t InstallationDescriptionSize;
  char InstallationDescription[Max_N2kConfigurationInfoField_len];
  tN2kGroupFunctionTransmissionOrPriorityErrorCode pec=N2kgfTPec_Acknowledge;
  tN2kMsg N2kRMsg;

 		if (PrioritySetting != 0x08 || PrioritySetting != 0x0f || PrioritySetting != 0x09) pec = N2kgfTPec_TransmitIntervalOrPriorityNotSupported;

    SetStartAcknowledge(N2kRMsg,N2kMsg.Source,PGN,
                        N2kgfPGNec_Acknowledge,  // What we actually should response as PGN error, if we have invalid field?
                        pec,
                        NumberOfParameterPairs);

    StartParseCommandPairParameters(N2kMsg,Index);
    // Next read new field values
    for (i=0; i<NumberOfParameterPairs; i++) {
      field=N2kMsg.GetByte(Index);
      switch (field) {
        case 1: // Installation description 1
          InstallationDescriptionSize=Max_N2kConfigurationInfoField_len;
          N2kMsg.GetVarStr(InstallationDescriptionSize,InstallationDescription,Index);
          pNMEA2000->SetInstallationDescription1(InstallationDescription);
          AddAcknowledgeParameter(N2kRMsg,i,N2kgfpec_Acknowledge);
          break;
        case 2:  // Installation description 2
          InstallationDescriptionSize=Max_N2kConfigurationInfoField_len;
          N2kMsg.GetVarStr(InstallationDescriptionSize,InstallationDescription,Index);
          pNMEA2000->SetInstallationDescription2(InstallationDescription);
          AddAcknowledgeParameter(N2kRMsg,i,N2kgfpec_Acknowledge);
          break;
        default:
          AddAcknowledgeParameter(N2kRMsg,i,N2kgfpec_InvalidRequestOrCommandParameterField);
      }
    }

    pNMEA2000->SendMsg(N2kRMsg,iDev);

    return true;
}

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
//*****************************************************************************
// See document https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf for
// Heartbeat settings
bool tN2kGroupFunctionHandlerForPGN126993::HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t TransmissionIntervalOffset,
                               uint8_t  NumberOfParameterPairs,
                               int iDev) {
	tN2kGroupFunctionTransmissionOrPriorityErrorCode pec = GetRequestGroupFunctionTransmissionOrPriorityErrorCode(TransmissionInterval);
	if ( TransmissionInterval >= 1000 && TransmissionInterval <= 60000 ) pec = N2kgfTPec_Acknowledge;
  if ( NumberOfParameterPairs==0 ) { // According to doc, there should not be any parameter pairs defined
    if ( TransmissionInterval==0xffffffff && TransmissionIntervalOffset==0xffff ) {
      return tN2kGroupFunctionHandler::HandleRequest(N2kMsg,TransmissionInterval,TransmissionIntervalOffset,NumberOfParameterPairs,iDev);
    }
    if ( pec==N2kgfTPec_Acknowledge ) {
      pNMEA2000->SetHeartbeatInterval(TransmissionInterval,false,iDev);
      pNMEA2000->SendHeartbeat(iDev);
    } else {
			if (!tNMEA2000::IsBroadcast(N2kMsg.Destination)) {
				SendAcknowledge(pNMEA2000, N2kMsg.Source, iDev, PGN, N2kgfPGNec_Acknowledge, pec);
			}
    }
  } else {
    if ( !tNMEA2000::IsBroadcast(N2kMsg.Destination) ) {
      SendAcknowledge(pNMEA2000,N2kMsg.Source,iDev,PGN,
                      N2kgfPGNec_Acknowledge,
                      N2kgfTPec_TransmitIntervalOrPriorityNotSupported,
                      NumberOfParameterPairs, N2kgfpec_RequestOrCommandNotSupported);
    }
  }
  return true;
}
#endif

#endif
