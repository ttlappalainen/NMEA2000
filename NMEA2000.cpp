/* 
NMEA2000.cpp

2015-2016 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA
*/

#include <NMEA2000.h> 

#define N2kAddressClaimTimeout 250

unsigned long SingleFrameSystemMessages[]={59392L /*ISO Acknowledgement*/, 59904L /*ISO Request*/, 60928L /*ISO Address Claim*/,
                                       0};
unsigned long FastPacketSystemMessages[]={126208L, 
                                       0};
unsigned long DefSingleFrameMessages[]={126992L, 127245L, 127250L, 127488L, 127505L, 127508L, 128259L, 128267L , 129026L, 130311L,
                                       0};
unsigned long DefFastPacketMessages[]={126996L,126998L,127489L,127506L,128275L,129029L,
                                      0};

//*****************************************************************************
void SetCharBuf(const char *str, int MaxLen, char *buf) {
  int i=0;
  for (; i<MaxLen-1 && str[i]!=0; i++) buf[i]=str[i];
  for (; i<MaxLen; i++) buf[i]=0;
  buf[MaxLen-1]=0; // Force null ternimation
}
 
void SetMsgBuf(const unsigned long *msgs, int MaxLen, unsigned long* buf) {
  for (int i=0; i<MaxLen-1 && msgs[i]!=0; i++) buf[i]=msgs[i];
  buf[MaxLen]=0;
}
 
//*****************************************************************************
tNMEA2000::tNMEA2000() {
  SetMsgBuf(DefSingleFrameMessages,Max_N2kSingleFrameMessages_len,SingleFrameMessages);
  SetMsgBuf(DefFastPacketMessages,Max_N2kFastPacketMessages_len,FastPacketMessages);
  MsgHandler=0;
  ForwardStream=&Serial;
  N2kSource[0]=0;
  DeviceReady=false;
  AddressClaimStarted=0;
  AddressChanged=false;
  dbMode=dm_None;
  N2kMode=N2km_ListenOnly;
  ForwardType=fwdt_Actisense;
  ForwardMode=0;
  EnableForward();
  SetForwardSystemMessages();
  SetForwardOwnMessages();
  for (int i=0; i<MaxN2kCANMsgs; i++) N2kCANMsgBuf[i].FreeMessage();
  SetProductInformation("00000001", // Manufacturer's Model serial code
                        666,  // Manufacturer's product code
                        "Arduino N2k->PC", // Manufacturer's  Model ID
                        "1.0.0.0", // Manufacturer's software version code
                        "1.0.0", // Manufacturer's Model version
                        1, // Load equivalency x * 50mA
                        1310, // NMEA2000 version used
                        0 // Sertification level ??
                        );
  SetDeviceInformation(1, // 21 bit resolution, max 2097151. Each device from same manufacturer should have unique number.
                       130, // PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                       25, // Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                       2046,  // Maximum 2046. See the list of codes on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                       4  // Marine
                       );
  DeviceCount=1;
}

//*****************************************************************************
void tNMEA2000::SetProductInformation(const char *_ModelSerialCode,
                                      unsigned int _ProductCode,
                                      const char *_ModelID, 
                                      const char *_SwCode, 
                                      const char *_ModelVersion,
                                      unsigned char _LoadEquivalency,
                                      unsigned int _N2kVersion, 
                                      unsigned char _SertificationLevel) {
   if (_N2kVersion!=0xffff) N2kVersion=_N2kVersion;
   if (_ProductCode!=0xffff) ProductCode=_ProductCode;
   if (_ModelID) SetCharBuf(_ModelID,sizeof(N2kModelID),N2kModelID); 
   if (_SwCode) SetCharBuf(_SwCode,sizeof(N2kSwCode),N2kSwCode); 
   if (_ModelVersion) SetCharBuf(_ModelVersion,sizeof(N2kModelVersion),N2kModelVersion); 
   if (_ModelSerialCode) SetCharBuf(_ModelSerialCode,sizeof(N2kModelSerialCode),N2kModelSerialCode); 
   if (_SertificationLevel!=0xff) SertificationLevel=_SertificationLevel;
   if (_LoadEquivalency!=0xff) LoadEquivalency=_LoadEquivalency;
}

//*****************************************************************************
void tNMEA2000::SetDeviceInformation(unsigned long _UniqueNumber,
                                     unsigned char _DeviceFunction,
                                     unsigned char _DeviceClass,
                                     int _ManufacturerCode,
                                     unsigned char _IndustryGroup
                                     ) {
   if (_ManufacturerCode!=0xffff) DeviceInformation[0].SetManufacturerCode(_ManufacturerCode);
   if (_UniqueNumber!=0xffffffff) DeviceInformation[0].SetUniqueNumber(_UniqueNumber);
   if (_DeviceFunction!=0xff) DeviceInformation[0].SetDeviceFunction(_DeviceFunction);
   if (_DeviceClass!=0xff) DeviceInformation[0].SetDeviceClass(_DeviceClass);
   if (_IndustryGroup!=0xff) DeviceInformation[0].SetIndustryGroup(_IndustryGroup);
}

//*****************************************************************************
void tNMEA2000::SetMode(tN2kMode _N2kMode, unsigned long _N2kSource) {
  N2kMode=_N2kMode;
  N2kSource[0]=_N2kSource;
  AddressChanged=false;
}

//*****************************************************************************
bool tNMEA2000::Open() {
  if (!DeviceReady) {
    DeviceReady=CANOpen();

    if ( ForwardType==tNMEA2000::fwdt_Text) { 
      if ( DeviceReady ) { ForwardStream->println("CAN device ready"); } else { ForwardStream->println("CAN device failed to open"); }
    }

    delay(200);
    if ((DeviceReady || dbMode!=dm_None) && (N2kMode!=N2km_ListenOnly) && (N2kMode!=N2km_SendOnly) && (N2kMode!=N2km_ListenAndSend) ) { // Start address claim automatically
      AddressClaimStarted=0;
      if ( ForwardType==tNMEA2000::fwdt_Text) ForwardStream->println("Start address claim");
      SendIsoAddressClaim(0xff,0);
      AddressClaimStarted=millis();
    }
  } 
  
  return DeviceReady;
}

//*****************************************************************************
void CanIdToN2k(unsigned long id, unsigned char &prio, unsigned long &pgn, unsigned char &src, unsigned char &dst) {
  unsigned char PF = (unsigned char) (id >> 16);
  unsigned char PS = (unsigned char) (id >> 8);
  unsigned char DP = (unsigned char) (id >> 24) & 1;

    src = (unsigned char) id >> 0;
    prio = (unsigned char) ((id >> 26) & 0x7);

    if (PF < 240) {
      /* PDU1 format, the PS contains the destination address */
        dst = PS;
        pgn = (((unsigned long)DP) << 16) | (((unsigned long)PF) << 8);
    } else {
      /* PDU2 format, the destination is implied global and the PGN is extended */
        dst = 0xff;
        pgn = (((unsigned long)DP) << 16) | (((unsigned long)PF) << 8) | (unsigned long)PS;
    }
}

//*****************************************************************************
unsigned long N2ktoCanID(unsigned char priority, unsigned long PGN, unsigned long Source, unsigned char Destination) {
  unsigned char PF = (unsigned char) (PGN >> 8);

  if (PF < 240) {  // PDU1 format
     return ( ((unsigned long)(priority & 0x7))<<26 | PGN<<8 | ((unsigned long)Destination)<<8 | (unsigned long)Source);
  } else { // PDU2 format
     return ( ((unsigned long)(priority & 0x7))<<26 | PGN<<8 | (unsigned long)Source);
  }
}

//*****************************************************************************
//bool tNMEA2000::CANSendMessage(unsigned long id, unsigned char len, const unsigned char *buf) {
//  return (N2kCAN.sendMsgBuf(id, 1, len, buf)==CAN_OK); 
//}

//*****************************************************************************
// Sends message to N2k bus
// 
bool tNMEA2000::SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex) {
//void tNMEA2000::SendMsg(unsigned long priority, unsigned long PGN, unsigned long Source, unsigned long Destination, INT8U len, const INT8U *pData) {
  static int SendOrder=0;
  bool result=false;
  
  if ( DeviceIndex>=DeviceCount) return result;
  N2kMsg.CheckDestination();
  if (DeviceIndex>=0) N2kMsg.ForceSource(N2kSource[DeviceIndex]);
  
  if (N2kMsg.Source>253) return false; // CAN bus address range is 0-253.
  
  unsigned long canId=N2ktoCanID(N2kMsg.Priority,N2kMsg.PGN,N2kMsg.Source, N2kMsg.Destination);
  
  if (N2kMode==N2km_ListenOnly) return false; // Do not send anything on listen only mode
  
  if (N2kMsg.PGN==0) return false;
  
  switch (dbMode) {
    case dm_None:
//      ForwardStream->print("Send PGN:"); ForwardStream->println(N2kMsg.PGN);
//      ForwardStream->print("Can ID:"); ForwardStream->println(canId);
      if (!Open()) return false;  // Can not do much
      if ( (AddressClaimStarted!=0) && (AddressClaimStarted+N2kAddressClaimTimeout>millis()) ) return false; // Do not send during address claiming
      if (N2kMsg.DataLen<=8) { // We can send single frame
          result=CANSendFrame(canId, N2kMsg.DataLen, N2kMsg.Data); 
          if (!result && ForwardType==tNMEA2000::fwdt_Text) { ForwardStream->print("PGN "); ForwardStream->print(N2kMsg.PGN); ForwardStream->println(" send failed"); }
      } else { // Send it as fast packet in multiple frames
        unsigned char temp[8]; // {0,0,0,0,0,0,0,0};
        int cur=0;
        int frames=(N2kMsg.DataLen-6-1)/7+1+1;
        int Order=SendOrder<<5;
        result=true;
        for(int i = 0; i<frames && result; i++){
            temp[0] = i|Order; //frame counter
            if (i==0) {
                temp[1] = N2kMsg.DataLen; //total bytes in fast packet
                //send the first 6 bytes
                for (int j = 2; j<8; j++) { 
                     temp[j]=N2kMsg.Data[cur];
                     cur++;   
                 }
            } else {
                 int j=1;
                 //send the next 7 data bytes
                 for (; j<8 && cur<N2kMsg.DataLen; j++) { 
                     temp[j]=N2kMsg.Data[cur];
                     cur++;   
                 }
                 for (; j<8; j++) { 
                     temp[j]=0;
                 }
            }
            // delay(3);
            result=CANSendFrame(canId, 8, temp, true);
            if (!result && ForwardType==tNMEA2000::fwdt_Text) { ForwardStream->print("PGN "); ForwardStream->print(N2kMsg.PGN); ForwardStream->println(" send failed"); }
        }
        SendOrder++; if (SendOrder>7) SendOrder=0;
      };
      if ( ForwardOwnMessages() ) ForwardMessage(N2kMsg);
      break;
    case dm_ClearText:
      result=true;
      N2kMsg.Print(ForwardStream);
      break;
    case dm_Actisense:
      result=true;
      N2kMsg.SendInActisenseFormat(ForwardStream);
      break;
  }
  
  return result;
}

//*****************************************************************************
void tNMEA2000::SetDebugMode(tDebugMode _dbMode) {
  dbMode=_dbMode;
}

//*****************************************************************************
bool tNMEA2000::CheckKnownMessage(unsigned long PGN, bool &SystemMessage, bool &FastPacket) {
  int i;
//    return true;
    FastPacket=false;
    if ( PGN==0 ) { SystemMessage=false; return false; }  // Unknown
    
    // First check system messages
    SystemMessage=true;
    for (i=0; SingleFrameSystemMessages[i]!=PGN && SingleFrameSystemMessages[i]!=0; i++);
    if (SingleFrameSystemMessages[i]==PGN) return true;
    
    for (i=0; FastPacketSystemMessages[i]!=PGN && FastPacketSystemMessages[i]!=0; i++);
    if (FastPacketSystemMessages[i]==PGN) {
      FastPacket=true;
      return true;
    }

    // It was not system message, so check other messages
    SystemMessage=false;
    for (i=0; SingleFrameMessages[i]!=PGN && SingleFrameMessages[i]!=0; i++);
    if (SingleFrameMessages[i]==PGN) return true;
    
    for (i=0; FastPacketMessages[i]!=PGN && FastPacketMessages[i]!=0; i++);
    if (FastPacketMessages[i]==PGN) {
      FastPacket=true;
      return true;
    }
    
    return false;
}

//*****************************************************************************
int tNMEA2000::SetN2kCANBufMsg(unsigned long canId, unsigned char len, unsigned char *buf) {
  unsigned char Priority;
  unsigned long PGN;
  unsigned long OldestMsgTime,CurTime;
  int OldestIndex;
  unsigned char Source;
  unsigned char Destination;
  bool FastPacket;
  bool SystemMessage;
  int i;
  int result=-1;

    CanIdToN2k(canId,Priority,PGN,Source,Destination);
    if ( (CheckKnownMessage(PGN,SystemMessage,FastPacket)? true: !ForwardOnlyKnownMessages()) ) {
      if (FastPacket && ((buf[0] & 0x1F)>0) ) { // Not first frame
//    Serial.print("New frame="); Serial.print(PGN); Serial.print(" frame="); Serial.print(buf[0],HEX); Serial.print("\r\n");
        // Find previous slot for this PGN
        for (i=0; i<MaxN2kCANMsgs && !(N2kCANMsgBuf[i].N2kMsg.PGN==PGN && N2kCANMsgBuf[i].N2kMsg.Source==Source); i++);
        if (i==MaxN2kCANMsgs) return result; // we did not find start for this message, so just skip it.
        if (N2kCANMsgBuf[i].LastFrame+1 != buf[0]) { // We have lost frame, so free this
          N2kCANMsgBuf[i].FreeMessage();
        }
        N2kCANMsgBuf[i].LastFrame=buf[0];
        for (int j=1; j<len; j++, N2kCANMsgBuf[i].CopiedLen++) {
          N2kCANMsgBuf[i].N2kMsg.Data[N2kCANMsgBuf[i].CopiedLen]=buf[j];
        }
      } else { // Handle first frame
        for (i=0, CurTime=OldestMsgTime=millis(), OldestIndex=MaxN2kCANMsgs; 
             i<MaxN2kCANMsgs && !(N2kCANMsgBuf[i].FreeMsg || (N2kCANMsgBuf[i].N2kMsg.PGN==PGN && N2kCANMsgBuf[i].N2kMsg.Source==Source));
             i++) { // Find free message place
          if (N2kCANMsgBuf[i].N2kMsg.MsgTime<OldestMsgTime) {
            OldestIndex=i;
            OldestMsgTime=N2kCANMsgBuf[i].N2kMsg.MsgTime;            
          }
        }
        if ( i==MaxN2kCANMsgs && OldestMsgTime+Max_N2kMsgBuf_Time<CurTime) i=OldestIndex; // Use the old one, which has timed out
        if (i==MaxN2kCANMsgs) return result; // we did not find free place, so skip this
        N2kCANMsgBuf[i].FreeMsg=false;
        N2kCANMsgBuf[i].SystemMessage=SystemMessage;
        N2kCANMsgBuf[i].N2kMsg.Init(Priority,PGN,Source,Destination);
        N2kCANMsgBuf[i].CopiedLen=0;
        if (FastPacket) {
//    Serial.print("First frame="); Serial.print(PGN);  Serial.print("\r\n");
          N2kCANMsgBuf[i].LastFrame=buf[0];
          N2kCANMsgBuf[i].N2kMsg.DataLen=buf[1];
          for (int j=2; j<len; j++, N2kCANMsgBuf[i].CopiedLen++) {
            N2kCANMsgBuf[i].N2kMsg.Data[N2kCANMsgBuf[i].CopiedLen]=buf[j];
          }
        } else {
//    Serial.print("Single frame="); Serial.print(PGN); Serial.print("\r\n");
          N2kCANMsgBuf[i].LastFrame=0;
          N2kCANMsgBuf[i].N2kMsg.DataLen=len;
          for (int j=0; j<len; j++, N2kCANMsgBuf[i].CopiedLen++) {
            N2kCANMsgBuf[i].N2kMsg.Data[N2kCANMsgBuf[i].CopiedLen]=buf[j];
          }
        }
      }
      N2kCANMsgBuf[i].Ready=(N2kCANMsgBuf[i].CopiedLen>=N2kCANMsgBuf[i].N2kMsg.DataLen);
      result=(N2kCANMsgBuf[i].Ready?i:-1); // If packet is ready, return index to it
    }
    
    return result;
}

//*****************************************************************************
 int tNMEA2000::FindSourceDeviceIndex(unsigned char Source) {
   int result=-1;
   int i;
   
     for (i=0; i<DeviceCount && N2kSource[i]!=Source; i++);
     if (i<DeviceCount) result=i;
     
     return result;
 }
 
//*****************************************************************************
bool tNMEA2000::IsMySource(unsigned char Source) {
    return (FindSourceDeviceIndex(Source)!=-1);
}

//*****************************************************************************
void tNMEA2000::ForwardMessage(const tN2kMsg &N2kMsg) {
  if ( !ForwardEnabled() || ( !( ForwardOwnMessages() && IsMySource(N2kMsg.Source) ) && N2kMode==N2km_NodeOnly ) ) return; 

  switch (ForwardType) {
    case fwdt_Actisense: 
      N2kMsg.SendInActisenseFormat(ForwardStream);
      break;
    case fwdt_Text:
      N2kMsg.Print(ForwardStream);
      break;
  }
}

//*****************************************************************************
void tNMEA2000::SendIsoAddressClaim(unsigned char Destination, int DeviceIndex) {
  if ( DeviceIndex<0 || DeviceIndex>=DeviceCount) return;
  tN2kMsg RespondMsg(N2kSource[DeviceIndex]);

    RespondMsg.Destination=Destination;
    SetN2kISOAddressClaim(RespondMsg,DeviceInformation[DeviceIndex].GetName());
    SendMsg(RespondMsg,DeviceIndex);
}

//*****************************************************************************
void tNMEA2000::SendProductInformation(int DeviceIndex) {
  if ( DeviceIndex<0 || DeviceIndex>=DeviceCount) return;
  tN2kMsg RespondMsg(N2kSource[DeviceIndex]);

    SetN2kProductInformation(RespondMsg,N2kVersion,ProductCode,N2kModelID,N2kSwCode,N2kModelVersion,N2kModelSerialCode,SertificationLevel,LoadEquivalency);
    SendMsg(RespondMsg,DeviceIndex);
}

//*****************************************************************************
void tNMEA2000::SendConfigurationInformation(int DeviceIndex) {
  if ( DeviceIndex<0 || DeviceIndex>=DeviceCount) return;
  tN2kMsg RespondMsg(N2kSource[DeviceIndex]);

    // Curretly simply respond with NAK
    SetN2kPGNISOAcknowledgement(RespondMsg,1,0xff,126998L);
    SendMsg(RespondMsg,DeviceIndex);
}

//*****************************************************************************
void tNMEA2000::HandleISORequest(const tN2kMsg &N2kMsg) {
  unsigned long RequestedPGN;
  unsigned char Destination=0xff;
  int iDev=FindSourceDeviceIndex(N2kMsg.Destination);
  
    if ( N2kMsg.Destination!=0xff && iDev==-1) return; // if destination is not for us, we do nothing
    
    if (iDev!=-1) Destination=N2kMsg.Source; // Respond to caller
    
    ParseN2kPGNISORequest(N2kMsg,RequestedPGN);
    switch (RequestedPGN) { 
      case 60928L: /*ISO Address Claim*/  // Someone is asking others to claim their addresses
        SendIsoAddressClaim(Destination,iDev);
        break;
      case 126996L: /* Product information */
        SendProductInformation(iDev);
        break;
      case 126998L: /* Configuration information */
        SendConfigurationInformation(iDev);
        break;
       // ToDo: Write default handler so that user can add their own.
    }
}

//*****************************************************************************
void tNMEA2000::HandleISOAddressClaim(const tN2kMsg &N2kMsg) {
  int iDev=FindSourceDeviceIndex(N2kMsg.Source);
  if ( iDev==-1 ) return; // if the address is not same as we have, we do nothing
  
  uint64_t *CallerName;

    CallerName=(uint64_t *)(N2kMsg.Data);
    
    if (DeviceInformation[iDev].GetName()<*CallerName) { // We can keep our address, so just reclaim it
      SendIsoAddressClaim(0xff,iDev);
    } else { // we have to try an other address
      if (DeviceInformation[iDev].GetName()==*CallerName) { 
        // If the name is same, then the first instance will get claim and change its address. 
        // This should not happen, if user takes care of setting unique ID for device information.
        // If he does not there is no problem with this class, but e.g. Garmin gets crazy.
        // Try to solve situation by changing our device instance. 
        DeviceInformation[iDev].SetDeviceInstance(DeviceInformation[iDev].GetDeviceInstance()+1);
      }
      GetNextAddress(iDev);
      AddressChanged=true;
      AddressClaimStarted=0;
      SendIsoAddressClaim(0xff,iDev);
      AddressClaimStarted=millis();
    }
}

//*****************************************************************************
bool tNMEA2000::ReadResetAddressChanged() {
  bool result=AddressChanged;
  
  AddressChanged=false;
  return result;
}

//*****************************************************************************
void tNMEA2000::GetNextAddress(int DeviceIndex) {
  bool FoundSame=false;
  // Currently simply add address
  // Note that 253 is the last source. We do not send data if address is higher than that.
  do {
    if (N2kSource[DeviceIndex]<255) N2kSource[DeviceIndex]++; 
    
    // Check that we do not have same on our list
    for (int i=0; i<DeviceCount && !FoundSame; i++) {
      if (i!=DeviceIndex) FoundSame=(N2kSource[DeviceIndex]==N2kSource[i]);
    }
  } while (FoundSame);
}

//*****************************************************************************
bool tNMEA2000::HandleReceivedSystemMessage(int MsgIndex) {
  bool result=false;

    if ( N2kMode==N2km_SendOnly || N2kMode==N2km_ListenAndSend ) return result;
    
    if ( N2kCANMsgBuf[MsgIndex].SystemMessage ) {
      if ( ForwardSystemMessages() ) ForwardMessage(N2kCANMsgBuf[MsgIndex].N2kMsg);
      if ( N2kMode!=N2km_ListenOnly ) { // Note that in listen only mode we will not inform us to the bus
        switch (N2kCANMsgBuf[MsgIndex].N2kMsg.PGN) {
          case 59392L: /*ISO Acknowledgement*/
            break;
          case 59904L: /*ISO Request*/
            HandleISORequest(N2kCANMsgBuf[MsgIndex].N2kMsg);
            break;
          case 60928L: /*ISO Address Claim*/
            HandleISOAddressClaim(N2kCANMsgBuf[MsgIndex].N2kMsg);
            break;
        }
      }
      result=true;
    }
  
  return result;
}

//*****************************************************************************
void tNMEA2000::ParseMessages() {
    unsigned long canId;
    unsigned char len = 0;
    unsigned char buf[8];
    int MsgIndex;
//    tN2kMsg N2kMsg;
  
    if (!Open()) return;  // Can not do much
    
    if (CANGetFrame(canId,len,buf) ) {           // check if data coming
//        ForwardStream->print("Can ID:"); ForwardStream->print(canId); ForwardStream->print(" len:"); ForwardStream->print(len); ForwardStream->print(" data:"); PrintBuf(ForwardStream,len,buf); ForwardStream->println("\r\n");
        MsgIndex=SetN2kCANBufMsg(canId,len,buf);
        if (MsgIndex>=0) {
          if ( !HandleReceivedSystemMessage(MsgIndex) ) {
//            Serial.println(MsgIndex);
            ForwardMessage(N2kCANMsgBuf[MsgIndex].N2kMsg);
          }
//          N2kCANMsgBuf[MsgIndex].N2kMsg.Print(Serial);
          if ( MsgHandler!=0 ) MsgHandler(N2kCANMsgBuf[MsgIndex].N2kMsg);
          N2kCANMsgBuf[MsgIndex].FreeMessage();
//          Serial.print(MsgIndex); Serial.print("\r\n");
        }
    }
}

//*****************************************************************************
void tNMEA2000::SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg)) {
  MsgHandler=_MsgHandler;
}

//*****************************************************************************
/// ISO Acknowledgement
void SetN2kPGN59392(tN2kMsg &N2kMsg, unsigned char Control, unsigned char GroupFunction, unsigned long PGN) {
    N2kMsg.SetPGN(59392L);
    N2kMsg.Priority=6;

    N2kMsg.AddByte(Control);
    N2kMsg.AddByte(GroupFunction);
    N2kMsg.AddByte(0xff); // Reserved
    N2kMsg.AddByte(0xff); // Reserved
    N2kMsg.AddByte(0xff); // Reserved
    N2kMsg.AddByte(PGN & 0xff); PGN>>=8;
    N2kMsg.AddByte(PGN & 0xff); PGN>>=8;
    N2kMsg.AddByte(PGN & 0xff); PGN>>=8;
}

//*****************************************************************************
// ISO Address Claim 
void SetN2kPGN60928(tN2kMsg &N2kMsg, unsigned long UniqueNumber, int ManufacturerCode,
                   unsigned char DeviceFunction, unsigned char DeviceClass,
                   unsigned char DeviceInstance, unsigned char SystemInstance, unsigned char IndustryGroup
                   ) {
    N2kMsg.SetPGN(60928L);
    N2kMsg.Priority=6;
    
    N2kMsg.Add4ByteUInt((UniqueNumber&0x1FFFFF) | ((unsigned long)(ManufacturerCode&0x7ff))<<21);
    N2kMsg.AddByte(DeviceInstance);
    N2kMsg.AddByte(DeviceFunction);
    N2kMsg.AddByte((DeviceClass&0x7f)<<1);
    N2kMsg.AddByte( 0x80 | ((IndustryGroup&0x7)<<4) | (SystemInstance&0x0f) );
}

//*****************************************************************************
// ISO Address Claim 
void SetN2kPGN60928(tN2kMsg &N2kMsg, uint64_t Name) {
    N2kMsg.SetPGN(60928L);
    N2kMsg.Priority=6;
    
    N2kMsg.AddUInt64(Name);
}

//*****************************************************************************
// Product Information
void SetN2kPGN126996(tN2kMsg &N2kMsg, unsigned int N2kVersion, unsigned int ProductCode,
                     const char *ModelID, const char *SwCode, 
                     const char *ModelVersion, const char *ModelSerialCode,
                     unsigned char SertificationLevel, unsigned char LoadEquivalency) {

    N2kMsg.SetPGN(126996L);
    N2kMsg.Priority=6;
    N2kMsg.Add2ByteInt(N2kVersion);
    N2kMsg.Add2ByteInt(ProductCode);
    N2kMsg.AddStr(ModelID, Max_N2kModelID_len);
    N2kMsg.AddStr(SwCode, Max_N2kSwCode_len);
    N2kMsg.AddStr(ModelVersion, Max_N2kModelVersion_len);
    N2kMsg.AddStr(ModelSerialCode, Max_N2kModelSerialCode_len);
    N2kMsg.AddByte(SertificationLevel);
    N2kMsg.AddByte(LoadEquivalency);
}

//*****************************************************************************
// Iso request
bool ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN) {
  int result=(N2kMsg.DataLen==3);
  RequestedPGN=0;
  
  if (result) {
    RequestedPGN=((unsigned long)N2kMsg.Data[2])<<16 | ((unsigned long)N2kMsg.Data[1]) << 8 | ((unsigned long)N2kMsg.Data[0]);
  }
  
  return result;
}

