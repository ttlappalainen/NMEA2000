/*
NMEA2000.cpp

Copyright (c) 2015-2018 Timo Lappalainen, Kave Oy, www.kave.fi

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

#include "NMEA2000.h"
#include "N2kDef.h"
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
#include "N2kGroupFunctionDefaultHandlers.h"
#endif
#include <string.h>
#include <stdlib.h>

#define DebugStream Serial

// #define NMEA2000_FRAME_IN_DEBUG
// #define NMEA2000_FRAME_OUT_DEBUG
// #define NMEA2000_MSG_DEBUG
// #define NMEA2000_BUF_DEBUG
// #define NMEA2000_DEBUG

#if defined(NMEA2000_FRAME_IN_DEBUG)
# define N2kFrameInDbg(fmt, args...)     DebugStream.print (fmt , ## args)
# define N2kFrameInDbgln(fmt, args...)   DebugStream.println (fmt , ## args)
#else
# define N2kFrameInDbg(fmt, args...)
# define N2kFrameInDbgln(fmt, args...)
#endif

#if defined(NMEA2000_FRAME_OUT_DEBUG)
# define N2kFrameOutDbg(fmt, args...)     DebugStream.print (fmt , ## args)
# define N2kFrameOutDbgln(fmt, args...)   DebugStream.println (fmt , ## args)
#else
# define N2kFrameOutDbg(fmt, args...)
# define N2kFrameOutDbgln(fmt, args...)
#endif

#if defined(NMEA2000_MSG_DEBUG)
# define N2kMsgDbg(fmt, args...)     DebugStream.print (fmt , ## args)
# define N2kMsgDbgln(fmt, args...)   DebugStream.println (fmt , ## args)
#else
# define N2kMsgDbg(fmt, args...)
# define N2kMsgDbgln(fmt, args...)
#endif

#if defined(NMEA2000_BUF_DEBUG)
# define DbgPrintBuf(len, buf, addln)     PrintBuf(&DebugStream, len, buf, addln)
#else
# define DbgPrintBuf(len, buf, addln)
#endif

#if defined(NMEA2000_DEBUG)
# define N2kDbg(fmt, args...)     DebugStream.print (fmt , ## args)
# define N2kDbgln(fmt, args...)   DebugStream.println (fmt , ## args)
#else
# define N2kDbg(fmt, args...)
# define N2kDbgln(fmt, args...)
#endif

// #define NMEA2000_MEMORY_TEST 1

#if defined(NMEA2000_MEMORY_TEST)
#include <MemoryFree.h>

void N2kPrintFreeMemory(const char *Source) {
    Serial.print(Source);
    Serial.print(", free memory=");
    Serial.println(freeMemory());
}
#else
#define N2kPrintFreeMemory(a)
#endif

#define N2kAddressClaimTimeout 250

#define MaxHeartbeatInterval 655320UL

#define TP_MAX_FRAMES 5  // Max frames, which can be received at time
#define TP_CM 60416L /* Multi packet connection management, TP.CM */
#define TP_DT 60160L /* Multi packet data transfer, TP.DT */
#define TP_CM_BAM    32
#define TP_CM_RTS    16
#define TP_CM_CTS    17
#define TP_CM_ACK    19
#define TP_CM_Abort 255

#define TP_CM_AbortBusy 1	// Already in one or more connection managed sessions and cannot support another.
#define TP_CM_AbortNoResources 2	// System resources were needed for another task so this connection managed session was terminated.
#define TP_CM_AbortTimeout 3 //	A timeout occurred and this is the connection abort to close the session.


const unsigned long DefTransmitMessages[] PROGMEM = {
                                        59392L, /* ISO Acknowledgement */
                                        59904L, /* ISO Request */
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
                                         TP_DT, /* Multi packet data transfer, TP.DT */
                                         TP_CM, /* Multi packet connection management, TP.CM */
#endif
                                        60928L, /* ISO Address Claim */
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
                                       126208L, /* NMEA Request/Command/Acknowledge group function */
#endif
                                       126464L, /* PGN List (Transmit and Receive) */
#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
                                       126993L, /* Heartbeat */
#endif
                                       126996L, /* Product information */
                                       126998L, /* Configuration information */
                                       0};
const unsigned long DefReceiveMessages[] PROGMEM = {
                                        59392L, /* ISO Acknowledgement */
                                        59904L, /* ISO Request */
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
                                         TP_DT, /* Multi packet data transfer, TP.DT */
                                         TP_CM, /* Multi packet connection management, TP.CM */
#endif
                                        60928L, /* ISO Address Claim */
                                        65240L, /* Commanded Address */
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
                                       126208L, /* NMEA Request/Command/Acknowledge group function */
#endif
                                       0};
                                       
bool IsSingleFrameSystemMessage(unsigned long PGN) {
                                  switch (PGN) {
                                      case  59392L: /* ISO Acknowledgement */
                                      case   TP_DT: /* Multi packet data transfer, TP.DT */
                                      case   TP_CM: /* Multi packet connection management, TP.CM */
                                      case  59904L: /* ISO Request */
                                      case  60928L: /* ISO Address Claim */
                                      return true;
                                  }
                                  return false;
}

bool IsFastPacketSystemMessage(unsigned long PGN) {
                                  switch (PGN) {
                                      case  65240L: /* Commanded Address*/
                                      case 126208L: /* NMEA Request/Command/Acknowledge group function */
                                      return true;
                                  }
                                  return false;
}

bool IsDefaultSingleFrameMessage(unsigned long PGN) {
                                  switch (PGN) {
                                      case 126992L: // System date/time
                                      case 126993L: /* Heartbeat */
                                      case 127245L: // Rudder
                                      case 127250L: // Vessel Heading
                                      case 127251L: // Rate of Turn
                                      case 127257L: // Attitude
                                      case 127488L: // Engine parameters rapid
                                      case 127493L: // Transmission parameters: dynamic
                                      case 127501L: // Binary status report
                                      case 127505L: // Fluid level
                                      case 127508L: // Battery Status
                                      case 127513L: // Battery Configuration Status
                                      case 128259L: // Boat speed
                                      case 128267L: // Water depth
                                      case 129025L: // Lat/lon rapid
                                      case 129026L: // COG SOG rapid
                                      case 129283L: // Cross Track Error
                                      case 130306L: // Wind Speed
                                      case 130310L: // Outside Environmental parameters
                                      case 130311L: // Environmental parameters
                                      case 130312L: // Temperature
                                      case 130314L: // Pressure
                                      case 130316L: // Temperature extended range
                                      case 130576L: // Small Craft Status (Trim Tab position)
                                      return true;
                                  }
                                  return false;
}

bool IsMandatoryFastPacketMessage(unsigned long PGN) {
                                  switch (PGN) {
                                      case 126464L: /* PGN List (Transmit and Receive) */
                                      case 126996L: /* Product information */
                                      case 126998L: /* Configuration information */
                                      return true;
                                  }
                                  return false;
}

bool IsDefaultFastPacketMessage(unsigned long PGN) {
                                  switch (PGN) {
                                      case 127237L: /* Heading/Track control */
                                      case 127489L: /* Engine parameters dynamic */
                                      case 127506L: /* DC Detailed status */
                                      case 128275L: /* Distance log */
                                      case 129029L: /* GNSS Position Data */
                                      case 129038L: /*AIS Class A Position Report*/
                                      case 129039L: /*AIS Class B Position Report*/
                                      case 129284L: // Navigation info
                                      case 129285L: // Waypoint list
                                      case 129540L: /* GNSS Sats in View */
                                      case 129794L: /*AIS Class A Static data*/
                                      case 129802L: /*AIS Safety Related Broadcast Message*/
                                      case 129809L: /*AIS Class B Static Data: Part A*/
                                      case 129810L: /*AIS Class B Static Data Part B*/
                                      case 130074L: // Waypoint list
                                      return true;
                                  }
                                  return false;
}

const tNMEA2000::tProductInformation DefProductInformation PROGMEM = {
                                       2101,               // N2kVersion
                                       666,                // ProductCode
                                       "Arduino N2k->PC",  //N2kModelID
                                       "1.0.0.0",          //N2kSwCode
                                       "1.0.0",    // N2kModelVersion
                                       "00000001", // N2kModelSerialCode
                                       0, // CertificationLevel
                                       1 // LoadEquivalency
                                      };

const char DefManufacturerInformation [] PROGMEM = "NMEA2000 library, https://github.com/ttlappalainen/NMEA2000";
const char DefInstallationDescription1 [] PROGMEM = "";
const char DefInstallationDescription2 [] PROGMEM = "";

//*****************************************************************************
void tNMEA2000::tProductInformation::Clear() {
  memset(this,0,sizeof(tProductInformation));
}

//*****************************************************************************
bool tNMEA2000::tProductInformation::IsSame(const tProductInformation &Other) {
  return memcmp(this,&Other,sizeof(tProductInformation))==0;
}

//*****************************************************************************
void tNMEA2000::ClearCharBuf(size_t MaxLen, char *buf) {
  if ( buf==0 ) return;
  size_t i=0;
  for (; i<MaxLen; i++) buf[i]=0;
}

//*****************************************************************************
void tNMEA2000::SetCharBuf(const char *str, size_t MaxLen, char *buf) {
  if ( buf==0 || MaxLen==0 ) return; // nothing to do for 0 buffer
  if ( str==0 ) { buf[0]=0; return; }
  size_t i=0;
  for (; i<MaxLen-1 && str[i]!=0; i++) buf[i]=str[i];
  for (; i<MaxLen; i++) buf[i]=0;
  buf[MaxLen-1]=0; // Force null termination
}

//*****************************************************************************
void tNMEA2000::ClearSetCharBuf(const char *str, size_t MaxLen, char *buf) {
  ClearCharBuf(MaxLen,buf);
  if (str) SetCharBuf(str,MaxLen,buf);
}

//*****************************************************************************
tNMEA2000::tNMEA2000() {

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
  pGroupFunctionHandlers=0;
#endif
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
  InstallationDescriptionChanged=false;
#endif
  ForwardStream=0;

  for (int i=0; i<N2kMessageGroups; i++) {SingleFrameMessages[i]=0; FastPacketMessages[i]=0;}

  N2kCANMsgBuf=0;
  MaxN2kCANMsgs=0;

  MaxCANSendFrames=40;
  MaxCANReceiveFrames=0; // Use driver default
  CANSendFrameBuf=0;

  MsgHandler=0;
  MsgHandlers=0;
  ISORqstHandler=0;

  DeviceReady=false;
  AddressChanged=false;
  DeviceInformationChanged=false;
  dbMode=dm_None;
  N2kMode=N2km_ListenOnly;
  ForwardType=fwdt_Actisense;
  ForwardMode=0;
  EnableForward();
  SetForwardSystemMessages();
  SetForwardOwnMessages();
  LocalConfigurationInformationData=0;
  SetProgmemConfigurationInformation(DefManufacturerInformation,
                                     DefInstallationDescription1,
                                     DefInstallationDescription2);
  Devices=0;
  DeviceCount=1;
}

//*****************************************************************************
void tNMEA2000::SetDeviceCount(const uint8_t _DeviceCount) {
  // Note that we can set this only before any initialization. Limit count to 10.
  if ( Devices==0 && _DeviceCount>=1 && _DeviceCount<10 ) DeviceCount=_DeviceCount;
}

//*****************************************************************************
void tNMEA2000::InitDevices() {
  if ( Devices==0 ) {
    N2kDbgln("Init devices");
    Devices=new tInternalDevice[DeviceCount];
    MaxCANSendFrames*=DeviceCount; // We need bigger buffer for sending all information
//    for (int i=0; i<DeviceCount; i++) Devices[i].tDevice();
    // We set default device information here.
    Devices[0].LocalProductInformation=0;
    Devices[0].ProductInformation=&DefProductInformation;
    for ( int i=0; i<DeviceCount; i++) { // Initialize all devices with some value
      SetDeviceInformation(1+i, // 21 bit resolution, max 2097151. Each device from same manufacturer should have unique number.
                           130, // PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                           25, // Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                           2046,  // Maximum 2046. See the list of codes on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                           4,  // Marine
                           i
                          );
      if ( i>0 ) {
        Devices[i].LocalProductInformation=0;
        Devices[i].ProductInformation=0;
      }
    }
  }
}

//*****************************************************************************
void tNMEA2000::SetProductInformation(const tProductInformation *_ProductInformation, int iDev) {
  if ( !IsValidDevice(iDev) ) return;
  InitDevices();
  Devices[iDev].ProductInformation=_ProductInformation;
  if (Devices[iDev].ProductInformation==0) Devices[iDev].ProductInformation=Devices[iDev].LocalProductInformation;
  if (Devices[iDev].ProductInformation==0) Devices[iDev].ProductInformation=&DefProductInformation;
}

//*****************************************************************************
void tNMEA2000::SetProductInformation(const char *_ModelSerialCode,
                                      unsigned short _ProductCode,
                                      const char *_ModelID,
                                      const char *_SwCode,
                                      const char *_ModelVersion,
                                      unsigned char _LoadEquivalency,
                                      unsigned short _N2kVersion,
                                      unsigned char _CertificationLevel,
                                      int iDev) {
  if ( !IsValidDevice(iDev) ) return;
  InitDevices();
  if (Devices[iDev].LocalProductInformation==0) {
    Devices[iDev].LocalProductInformation=new tProductInformation();
  }
  Devices[iDev].ProductInformation=Devices[iDev].LocalProductInformation;
  Devices[iDev].LocalProductInformation->Set(_ModelSerialCode,_ProductCode,_ModelID,_SwCode,_ModelVersion,_LoadEquivalency,_N2kVersion,_CertificationLevel);
}

//*****************************************************************************
void tNMEA2000::SetConfigurationInformation(const char *ManufacturerInformation,
                                            const char *InstallationDescription1,
                                            const char *InstallationDescription2) {
  if ( LocalConfigurationInformationData!=0 ) free(LocalConfigurationInformationData); // This happens on second call, which is not good.
  LocalConfigurationInformationData=0;

  size_t ManInfoLen=(ManufacturerInformation?strlen(ManufacturerInformation)+1:0);
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
  size_t InstDesc1Len=Max_N2kConfigurationInfoField_len;
  size_t InstDesc2Len=Max_N2kConfigurationInfoField_len;
#else
  size_t InstDesc1Len=(InstallationDescription1?strlen(InstallationDescription1)+1:0);
  size_t InstDesc2Len=(InstallationDescription2?strlen(InstallationDescription2)+1:0);
#endif

  if ( ManInfoLen>Max_N2kConfigurationInfoField_len ) ManInfoLen=Max_N2kConfigurationInfoField_len;
  if ( InstDesc1Len>Max_N2kConfigurationInfoField_len ) InstDesc1Len=Max_N2kConfigurationInfoField_len;
  if ( InstDesc2Len>Max_N2kConfigurationInfoField_len ) InstDesc2Len=Max_N2kConfigurationInfoField_len;

  size_t TotalSize=ManInfoLen+InstDesc1Len+InstDesc2Len;
  void *mem=(TotalSize>0?malloc(TotalSize):0);

  LocalConfigurationInformationData=(char*)mem;
  char *Info=LocalConfigurationInformationData;

  SetCharBuf(InstallationDescription1,InstDesc1Len,Info);
  ConfigurationInformation.InstallationDescription1=(InstallationDescription1?Info:0);
  Info+=InstDesc1Len;

  SetCharBuf(InstallationDescription2,InstDesc2Len,Info);
  ConfigurationInformation.InstallationDescription2=(InstallationDescription2?Info:0);
  Info+=InstDesc2Len;

  SetCharBuf(ManufacturerInformation,ManInfoLen,Info);
  ConfigurationInformation.ManufacturerInformation=(ManufacturerInformation?Info:0);
}

//*****************************************************************************
void tNMEA2000::SetProgmemConfigurationInformation(const char *ManufacturerInformation,
                                            const char *InstallationDescription1,
                                            const char *InstallationDescription2) {
  if ( LocalConfigurationInformationData!=0 ) free(LocalConfigurationInformationData); // This happens on second call, which is not good.
  LocalConfigurationInformationData=0;
  ConfigurationInformation.ManufacturerInformation=ManufacturerInformation;
  ConfigurationInformation.InstallationDescription1=InstallationDescription1;
  ConfigurationInformation.InstallationDescription2=InstallationDescription2;
}

//*****************************************************************************
size_t tNMEA2000::GetFastPacketTxPGNCount(int iDev) {
  if ( !IsValidDevice(iDev) ) return 0;
  unsigned long ListPGN;
  size_t FPTxPGNCount=0;
  
  for (int i=0; (ListPGN=pgm_read_dword(&DefTransmitMessages[i]))!=0; i++) {
    if ( IsFastPacketPGN(ListPGN) ) FPTxPGNCount++;
  }
  if (Devices[iDev].TransmitMessages!=0) {
    for (int i=0; (ListPGN=pgm_read_dword(&Devices[iDev].TransmitMessages[i]))!=0; i++) {
      if ( IsFastPacketPGN(ListPGN) ) FPTxPGNCount++;
    }
  }
  
  return FPTxPGNCount;
}

//*****************************************************************************
int tNMEA2000::GetSequenceCounter(unsigned long PGN, int iDev) {
  if ( !IsValidDevice(iDev) ) return 0;
  
  if ( Devices[iDev].PGNSequenceCounters==0 ) { // Sequence counters has not yet been initialized
    Devices[iDev].MaxPGNSequenceCounters=GetFastPacketTxPGNCount(iDev)+1; // Reserve 1 for undefined PGNs
    Devices[iDev].PGNSequenceCounters=new unsigned long[Devices[iDev].MaxPGNSequenceCounters];
    for ( size_t i=0; i<Devices[iDev].MaxPGNSequenceCounters; i++ ) Devices[iDev].PGNSequenceCounters[i]=0;
  }
  if ( Devices[iDev].PGNSequenceCounters==0 ) return 0; // Should not be. Only in case of memory allocation problem.
  size_t last=Devices[iDev].MaxPGNSequenceCounters-1;
  unsigned long sc;
  for ( size_t i=0; i<last; i++ ) {
    if ( Devices[iDev].PGNSequenceCounters[i]==0 ) { // Empty place, use this
      Devices[iDev].PGNSequenceCounters[i]=PGN;
      return 0; // Start from sequence 0
    }
    if ( (Devices[iDev].PGNSequenceCounters[i]&0x00ffffff) == PGN ) { // Found counter, use it
      sc=Devices[iDev].PGNSequenceCounters[i]>>24;
      sc++; if (sc>7) sc=0; // Get next counter
      Devices[iDev].PGNSequenceCounters[i]=PGN | (sc << 24);
      return sc;
    }
  }
  // PGN counter not found, so use common
  sc=Devices[iDev].PGNSequenceCounters[last];
  sc++; if (sc>7) sc=0;
  Devices[iDev].PGNSequenceCounters[last]=sc;
  return sc;
}

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

//*****************************************************************************
void CopyProgmemString(const char *str, size_t MaxLen, char *buf) {
  if ( buf==0 || MaxLen==0 ) return; // nothing to do for 0 buffer
  if ( str==0 ) { buf[0]=0; return; }
  size_t i=0;
  char c;
  for (; i<MaxLen-1 && (c=pgm_read_byte(&(str[i])))!=0; i++) buf[i]=c;
  for (; i<MaxLen; i++) buf[i]=0;
  buf[MaxLen-1]=0; // Force null termination
}

//*****************************************************************************
bool tNMEA2000::IsTxPGN(unsigned long PGN, int iDev) {
  if ( !IsValidDevice(iDev) ) return false;
  unsigned long ListPGN;
  
  for (int i=0; (ListPGN=pgm_read_dword(&DefTransmitMessages[i]))!=0; i++) {
    if ( ListPGN==PGN ) return true;
  }
  if (Devices[iDev].TransmitMessages!=0) {
    for (int i=0; (ListPGN=pgm_read_dword(&Devices[iDev].TransmitMessages[i]))!=0; i++) {
      if ( ListPGN==PGN ) return true;
    }
  }
  return false;
}

//*****************************************************************************
const tNMEA2000::tProductInformation * tNMEA2000::GetProductInformation(int iDev, bool &IsProgMem) const {
  if ( !IsValidDevice(iDev) ) return 0;
  int iPIDev=iDev;

    if ( Devices[iPIDev].ProductInformation==0 ) iPIDev=0; // Use first device product information
    if ( Devices[iPIDev].ProductInformation==0 ) return 0; // Can not do anything.
    IsProgMem = (Devices[iPIDev].ProductInformation!=Devices[iPIDev].LocalProductInformation );
    return Devices[iPIDev].ProductInformation;
}

//*****************************************************************************
unsigned short tNMEA2000::GetN2kVersion(int iDev) const {
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return 0;

  if ( !IsProgMem ) {
    return ProductInformation->N2kVersion;
  } else {
    return pgm_read_word(&ProductInformation->N2kVersion);
  }
}

//*****************************************************************************
unsigned short tNMEA2000::GetProductCode(int iDev) const {
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return 0;

  if ( !IsProgMem ) {
    return ProductInformation->ProductCode;
  } else {
    return pgm_read_word(&ProductInformation->ProductCode);
  }
}

//*****************************************************************************
void tNMEA2000::GetModelID(char *buf, size_t max_len, int iDev) const {
  if ( max_len==0 ) return;
  buf[0]=0;
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return;

  if ( !IsProgMem ) {
    SetCharBuf(ProductInformation->N2kModelID,max_len,buf);
  } else {
    CopyProgmemString(ProductInformation->N2kModelID,max_len,buf);
  }
}

//*****************************************************************************
void tNMEA2000::GetSwCode(char *buf, size_t max_len, int iDev) const {
  if ( max_len==0 ) return;
  buf[0]=0;
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return;

  if ( !IsProgMem ) {
    SetCharBuf(ProductInformation->N2kSwCode,max_len,buf);
  } else {
    CopyProgmemString(ProductInformation->N2kSwCode,max_len,buf);
  }
}

//*****************************************************************************
void tNMEA2000::GetModelVersion(char *buf, size_t max_len, int iDev) const {
  if ( max_len==0 ) return;
  buf[0]=0;
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return;

  if ( !IsProgMem ) {
    SetCharBuf(ProductInformation->N2kModelVersion,max_len,buf);
  } else {
    CopyProgmemString(ProductInformation->N2kModelVersion,max_len,buf);
  }
}

//*****************************************************************************
void tNMEA2000::GetModelSerialCode(char *buf, size_t max_len, int iDev) const {
  if ( max_len==0 ) return;
  buf[0]=0;
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return;

  if ( !IsProgMem ) {
    SetCharBuf(ProductInformation->N2kModelSerialCode,max_len,buf);
  } else {
    CopyProgmemString(ProductInformation->N2kModelSerialCode,max_len,buf);
  }
}

//*****************************************************************************
unsigned char tNMEA2000::GetCertificationLevel(int iDev) const {
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return 0;

  if ( !IsProgMem ) {
    return ProductInformation->CertificationLevel;
  } else {
    return pgm_read_byte(&ProductInformation->CertificationLevel);
  }
}

//*****************************************************************************
unsigned char tNMEA2000::GetLoadEquivalency(int iDev) const {
  bool IsProgMem;
  const tProductInformation *ProductInformation=GetProductInformation(iDev,IsProgMem);

  if ( ProductInformation==0 ) return 0;

  if ( !IsProgMem ) {
    return ProductInformation->LoadEquivalency;
  } else {
    return pgm_read_byte(&ProductInformation->LoadEquivalency);
  }
}

//*****************************************************************************
void tNMEA2000::CopyProgmemConfigurationInformationToLocal() {
  if ( LocalConfigurationInformationData==0 ) {
    char Buf[Max_N2kConfigurationInfoField_len];
    const char *ID1=ConfigurationInformation.InstallationDescription1;
    const char *ID2=ConfigurationInformation.InstallationDescription2;
    CopyProgmemString(ConfigurationInformation.ManufacturerInformation,Max_N2kConfigurationInfoField_len,Buf);
    SetConfigurationInformation(Buf);
    CopyProgmemString(ID1,Max_N2kConfigurationInfoField_len,Buf);
    SetInstallationDescription1(Buf);
    CopyProgmemString(ID2,Max_N2kConfigurationInfoField_len,Buf);
    SetInstallationDescription2(Buf);
  }
}

//*****************************************************************************
void tNMEA2000::SetInstallationDescription1(const char *InstallationDescription1) {
  CopyProgmemConfigurationInformationToLocal();
  // Get pointer to local InstallationDescription1, which is after 
  char *Info=LocalConfigurationInformationData;
  SetCharBuf(InstallationDescription1,Max_N2kConfigurationInfoField_len,Info);
  ConfigurationInformation.InstallationDescription1=(InstallationDescription1?Info:0);
  InstallationDescriptionChanged=true;
}

//*****************************************************************************
void tNMEA2000::SetInstallationDescription2(const char *InstallationDescription2) {
  CopyProgmemConfigurationInformationToLocal();
  char *Info=LocalConfigurationInformationData+Max_N2kConfigurationInfoField_len;
  SetCharBuf(InstallationDescription2,Max_N2kConfigurationInfoField_len,Info);
  ConfigurationInformation.InstallationDescription2=(InstallationDescription2?Info:0);
  InstallationDescriptionChanged=true;
}

//*****************************************************************************
void tNMEA2000::GetInstallationDescription1(char *buf, size_t max_len) {
  if ( LocalConfigurationInformationData!=0 ) {
    SetCharBuf(ConfigurationInformation.InstallationDescription1,max_len,buf);
  } else {
    CopyProgmemString(ConfigurationInformation.InstallationDescription1,max_len,buf);
  }
}

//*****************************************************************************
void tNMEA2000::GetInstallationDescription2(char *buf, size_t max_len) {
  if ( LocalConfigurationInformationData!=0 ) {
    SetCharBuf(ConfigurationInformation.InstallationDescription2,max_len,buf);
  } else {
    CopyProgmemString(ConfigurationInformation.InstallationDescription2,max_len,buf);
  }
}

//*****************************************************************************
void tNMEA2000::GetManufacturerInformation(char *buf, size_t max_len) {
  if ( LocalConfigurationInformationData!=0 ) {
    SetCharBuf(ConfigurationInformation.ManufacturerInformation,max_len,buf);
  } else {
    CopyProgmemString(ConfigurationInformation.ManufacturerInformation,max_len,buf);
  }
}
 
//*****************************************************************************
bool tNMEA2000::ReadResetInstallationDescriptionChanged() {
  bool result=InstallationDescriptionChanged;
  
  InstallationDescriptionChanged=false;
  return result;
}

#endif

//*****************************************************************************
void tNMEA2000::SetDeviceInformation(unsigned long _UniqueNumber,
                                     unsigned char _DeviceFunction,
                                     unsigned char _DeviceClass,
                                     uint16_t _ManufacturerCode,
                                     unsigned char _IndustryGroup,
                                     int iDev
                                     ) {
   if ( !IsValidDevice(iDev) ) return;
   InitDevices();
   if (_ManufacturerCode!=0xffff) Devices[iDev].DeviceInformation.SetManufacturerCode(_ManufacturerCode);
   if (_UniqueNumber!=0xffffffff) Devices[iDev].DeviceInformation.SetUniqueNumber(_UniqueNumber);
   if (_DeviceFunction!=0xff) Devices[iDev].DeviceInformation.SetDeviceFunction(_DeviceFunction);
   if (_DeviceClass!=0xff) Devices[iDev].DeviceInformation.SetDeviceClass(_DeviceClass);
   if (_IndustryGroup!=0xff) Devices[iDev].DeviceInformation.SetIndustryGroup(_IndustryGroup);
}

//*****************************************************************************
void tNMEA2000::SetDeviceInformationInstances(
                              uint8_t _DeviceInstanceLower,
                              uint8_t _DeviceInstanceUpper,
                              uint8_t _SystemInstance,
                              int iDev
                              ) {
  if ( !IsValidDevice(iDev) ) return;

  InitDevices();
  uint8_t DeviceInstance=Devices[iDev].DeviceInformation.GetDeviceInstance();

  if (_DeviceInstanceLower!=0xff ) {
    DeviceInstance=( (DeviceInstance & ~0x07) | (_DeviceInstanceLower & 0x07) );
  }
  if (_DeviceInstanceUpper!=0xff ) {
    DeviceInstance=( (DeviceInstance & ~0xF8) | ((_DeviceInstanceUpper&0x1f)<<3) );
  }

  if ( Devices[iDev].DeviceInformation.GetDeviceInstance()!=DeviceInstance) {
    Devices[iDev].DeviceInformation.SetDeviceInstance(DeviceInstance);
    DeviceInformationChanged=true;
  }

  if (_SystemInstance!=0xff && Devices[iDev].DeviceInformation.GetSystemInstance()!=_SystemInstance) {
    Devices[iDev].DeviceInformation.SetSystemInstance(_SystemInstance);
    DeviceInformationChanged=true;
  }

  if ( IsReadyToSend() ) SendIsoAddressClaim(0xff,iDev);
}

//*****************************************************************************
void tNMEA2000::SetSingleFrameMessages(const unsigned long *_SingleFrameMessages) {
  SingleFrameMessages[0]=_SingleFrameMessages;
}

//*****************************************************************************
void tNMEA2000::SetFastPacketMessages(const unsigned long *_FastPacketMessages) {
  FastPacketMessages[0]=_FastPacketMessages;
}

//*****************************************************************************
void tNMEA2000::ExtendSingleFrameMessages(const unsigned long *_SingleFrameMessages) {
  SingleFrameMessages[1]=_SingleFrameMessages;
}

//*****************************************************************************
void tNMEA2000::ExtendFastPacketMessages(const unsigned long *_FastPacketMessages) {
  FastPacketMessages[1]=_FastPacketMessages;
}

//*****************************************************************************
void tNMEA2000::ExtendTransmitMessages(const unsigned long *_Messages, int iDev) {
  if ( !IsValidDevice(iDev) ) return;
  InitDevices();
  Devices[iDev].TransmitMessages=_Messages;
}

//*****************************************************************************
void tNMEA2000::ExtendReceiveMessages(const unsigned long *_Messages, int iDev) {
  if ( !IsValidDevice(iDev) ) return;
  InitDevices();
  Devices[iDev].ReceiveMessages=_Messages;
}

//*****************************************************************************
void tNMEA2000::SetMode(tN2kMode _N2kMode, unsigned long _N2kSource) {
  InitDevices();
  N2kMode=_N2kMode;
  for (int i=0; i<DeviceCount; i++) {
    Devices[i].N2kSource=_N2kSource+i;
    Devices[i].UpdateAddressClaimEndSource();
  }
  AddressChanged=false;
}

//*****************************************************************************
void tNMEA2000::InitCANFrameBuffers() {
    if ( CANSendFrameBuf==0 && !IsInitialized() ) {
      if ( MaxCANSendFrames>0 ) CANSendFrameBuf = new tCANSendFrame[MaxCANSendFrames];
      N2kDbg("Initialize frame buffer. Size: "); N2kDbg(MaxCANSendFrames); N2kDbg(", address:"); N2kDbgln((uint32_t)CANSendFrameBuf);
      CANSendFrameBufferWrite=0;
      CANSendFrameBufferRead=0;
    }

    // Receive buffer has sense only with interrupt handling. So it must be handled on inherited class.
}

//*****************************************************************************
bool tNMEA2000::Open() {

  if (!DeviceReady) {
    InitCANFrameBuffers();
    InitDevices();

    if ( N2kCANMsgBuf==0 ) {
      if ( MaxN2kCANMsgs==0 ) MaxN2kCANMsgs=5;
      N2kCANMsgBuf = new tN2kCANMsg[MaxN2kCANMsgs];
      for (int i=0; i<MaxN2kCANMsgs; i++) N2kCANMsgBuf[i].FreeMessage();

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
      // On first open try add also default group function handlers
      AddGroupFunctionHandler(new tN2kGroupFunctionHandlerForPGN60928(this)); // NAME handler
      AddGroupFunctionHandler(new tN2kGroupFunctionHandlerForPGN126464(this)); // Rx/Tx list handler
#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
      AddGroupFunctionHandler(new tN2kGroupFunctionHandlerForPGN126993(this)); // Heartbeat handler
#endif      
      AddGroupFunctionHandler(new tN2kGroupFunctionHandlerForPGN126996(this)); // Product information
      AddGroupFunctionHandler(new tN2kGroupFunctionHandlerForPGN126998(this)); // Configuration information handler
      AddGroupFunctionHandler(new tN2kGroupFunctionHandler(this,0)); // Default handler at last
#endif
    }

    DeviceReady = (dbMode!=dm_None) || CANOpen();
    if ( (ForwardStream!=0) && (ForwardType==tNMEA2000::fwdt_Text) ) {
      if ( DeviceReady ) { ForwardStream->println(F("CAN device ready")); } else { ForwardStream->println(F("CAN device failed to open")); }
    }

    delay(200);
    for (int i=0; i<DeviceCount; i++) {
      if ( Devices[i].N2kSource==N2kNullCanBusAddress ) GetNextAddress(i,true); // On restart try address claiming from the beginning
      StartAddressClaim(i);
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
     if ( (PGN & 0xff) != 0 ) return 0;  // for PDU1 format PGN lowest byte has to be 0 for the destination.
     return ( ((unsigned long)(priority & 0x7))<<26 | PGN<<8 | ((unsigned long)Destination)<<8 | (unsigned long)Source);
  } else { // PDU2 format
     return ( ((unsigned long)(priority & 0x7))<<26 | PGN<<8 | (unsigned long)Source);
  }
}

//*****************************************************************************
bool tNMEA2000::SendFrames()
{ uint16_t temp;

  if ( CANSendFrameBuf==0 ) return true; // This can be in case, where inherited class defines own buffering.

  while (CANSendFrameBufferRead!=CANSendFrameBufferWrite) {
    temp = (CANSendFrameBufferRead + 1) % MaxCANSendFrames;
    if ( CANSendFrame(CANSendFrameBuf[temp].id, CANSendFrameBuf[temp].len, CANSendFrameBuf[temp].buf, CANSendFrameBuf[temp].wait_sent) ) {
      CANSendFrameBufferRead=temp;
      N2kFrameOutDbg("Frame unbuffered "); N2kFrameOutDbgln(CANSendFrameBuf[temp].id);
    } else return false;
  }

  return true;
}

//*****************************************************************************
bool tNMEA2000::SendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {

  if ( !SendFrames() || !CANSendFrame(id,len,buf,wait_sent) ) { // If we can not sent frame immediately, add it to buffer
    tCANSendFrame *Frame=GetNextFreeCANSendFrame();
    if ( Frame==0 ) {
      N2kFrameOutDbg("Frame failed "); N2kFrameOutDbgln(id);
      return false;
    }
    len=N2kMax<unsigned char>(len,8);
    Frame->id=id;
    Frame->len=len;
    Frame->wait_sent=wait_sent;
    for (int i=0; i<len; i++) Frame->buf[i]=buf[i];
    N2kFrameOutDbg("Frame buffered "); N2kFrameOutDbgln(id);
  }

  return true;
}

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
//*****************************************************************************
void tNMEA2000::SetHeartbeatInterval(unsigned long interval, bool SetAsDefault, int iDev) {
  if (interval==0xffffffff) return; // Do not change
  InitDevices();
  for (int i=(iDev<0?0:iDev); i<DeviceCount && (iDev<0?true:i<iDev+1); i++) {
    if (interval==0xfffffffe) { // restore default
      Devices[i].HeartbeatInterval=Devices[i].DefaultHeartbeatInterval;
    } else {
      if ( interval>MaxHeartbeatInterval ) interval=MaxHeartbeatInterval;
      if ( interval<1000 ) interval=0;

      Devices[i].HeartbeatInterval=interval;
    }

    if (SetAsDefault) Devices[i].DefaultHeartbeatInterval=Devices[i].HeartbeatInterval;
    Devices[i].NextHeartbeatSentTime=(millis()/Devices[i].HeartbeatInterval+1)*Devices[i].HeartbeatInterval;
  }
}

//*****************************************************************************
void tNMEA2000::SendHeartbeat(int iDev) {
  if ( !IsValidDevice(iDev) ) return;
  tN2kMsg N2kMsg;
  SetHeartbeat(N2kMsg,Devices[iDev].HeartbeatInterval,0);
  SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
void tNMEA2000::SendHeartbeat(bool force) {
  if ( !IsActiveNode() ) return;

  for (int iDev=0; iDev<DeviceCount; iDev++) {
    if ( !IsAddressClaimStarted(iDev) ) {
      if ( force ) Devices[iDev].NextHeartbeatSentTime=0;
      if ( Devices[iDev].HeartbeatInterval>0 && Devices[iDev].NextHeartbeatSentTime<millis() ) {
        tN2kMsg N2kMsg;
        if ( Devices[iDev].NextHeartbeatSentTime==0 ) { Devices[iDev].NextHeartbeatSentTime=millis(); }
        Devices[iDev].NextHeartbeatSentTime+=Devices[iDev].HeartbeatInterval;
        SetHeartbeat(N2kMsg,Devices[iDev].HeartbeatInterval,0);
        SendMsg(N2kMsg,iDev);
      }
    }
  }
}
#endif

//*****************************************************************************
tNMEA2000::tCANSendFrame *tNMEA2000::GetNextFreeCANSendFrame() {
  if (CANSendFrameBuf==0) return 0;

  uint16_t temp = (CANSendFrameBufferWrite + 1) % MaxCANSendFrames;

  if (temp != CANSendFrameBufferRead) {
    CANSendFrameBufferWrite = temp;
    return &(CANSendFrameBuf[CANSendFrameBufferWrite]);
  } else {
    return 0;
  }
}

//*****************************************************************************
void tNMEA2000::SendPendingInformation() {
  for (int i=0; i<DeviceCount; i++ ) {
    if ( Devices[i].QueryPendingProductInformation() ) SendProductInformation(i);
    if ( Devices[i].QueryPendingConfigurationInformation() ) SendConfigurationInformation(i);
  }
}

//*****************************************************************************
// Sends message to N2k bus
//
bool tNMEA2000::SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex) {
  if ( dbMode==dm_None && !Open() ) return false;

  bool result=false;

  if ( DeviceIndex>=DeviceCount) return result;
  N2kMsg.CheckDestination();
  if (DeviceIndex>=0) { N2kMsg.ForceSource(Devices[DeviceIndex].N2kSource); } else { DeviceIndex=0; }

  if ( N2kMsg.Source>N2kMaxCanBusAddress && N2kMsg.PGN!=N2kPGNIsoAddressClaim ) return false; // CAN bus address range is 0-251. Anyway allow ISO address claim mgs.

  unsigned long canId=N2ktoCanID(N2kMsg.Priority,N2kMsg.PGN,N2kMsg.Source, N2kMsg.Destination);
  
  if ( canId==0 ) { // PGN validity - N2ktoCanID returns 0 for invalid PGN
//    if (ForwardStream!=0 && ForwardType==tNMEA2000::fwdt_Text) { ForwardStream->print(F("Invalid PGN ")); ForwardStream->println(N2kMsg.PGN); }
    return false;
  }
  
  if (N2kMode==N2km_ListenOnly) return false; // Do not send anything on listen only mode

  if (N2kMsg.PGN==0) return false;

  switch (dbMode) {
    case dm_None:
      N2kMsgDbg("Send PGN:"); N2kMsgDbgln(N2kMsg.PGN);
      N2kMsgDbg("Can ID:"); N2kMsgDbgln(canId);
      if ( IsAddressClaimStarted(DeviceIndex) && N2kMsg.PGN!=N2kPGNIsoAddressClaim ) return false;

      if (N2kMsg.DataLen<=8 && !IsFastPacket(N2kMsg) ) { // We can send single frame
          DbgPrintBuf(N2kMsg.DataLen, N2kMsg.Data,true);
          result=SendFrame(canId, N2kMsg.DataLen, N2kMsg.Data,false);
          if (!result && ForwardStream!=0 && ForwardType==tNMEA2000::fwdt_Text) { ForwardStream->print(F("PGN ")); ForwardStream->print(N2kMsg.PGN); ForwardStream->println(F(" send failed")); }
          N2kPrintFreeMemory("SendMsg, single frame");
      } else { // Send it as fast packet in multiple frames
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
        if ( N2kMsg.IsTPMessage() ) {
          result=StartSendTPMessage(N2kMsg,DeviceIndex);
        } else
#endif
        {
          unsigned char temp[8]; // {0,0,0,0,0,0,0,0};
          int cur=0;
          int frames=(N2kMsg.DataLen>6 ? (N2kMsg.DataLen-6-1)/7+1+1 : 1 );
          int Order=GetSequenceCounter(N2kMsg.PGN,DeviceIndex)<<5;
          result=true;
          for (int i = 0; i<frames && result; i++) {
              temp[0] = i|Order; //frame counter
              if (i==0) {
                  temp[1] = N2kMsg.DataLen; //total bytes in fast packet
                  //send the first 6 bytes
                  for (int j = 2; j<8; j++) {
                       temp[j]=N2kMsg.Data[cur];
                       cur++;
                   }
                  N2kPrintFreeMemory("SendMsg, fastpacket");
              } else {
                   int j=1;
                   //send the next 7 data bytes
                   for (; j<8 && cur<N2kMsg.DataLen; j++) {
                       temp[j]=N2kMsg.Data[cur];
                       cur++;
                   }
                   for (; j<8; j++) {
                       temp[j]=0xff;
                   }
              }
              // delay(3);
              DbgPrintBuf(8,temp,true);
              result=SendFrame(canId, 8, temp, true);
              if (!result && ForwardStream!=0 && ForwardType==tNMEA2000::fwdt_Text) {
                ForwardStream->print(F("PGN ")); ForwardStream->print(N2kMsg.PGN);
                ForwardStream->print(F(", frame:")); ForwardStream->print(i); ForwardStream->print(F("/")); ForwardStream->print(frames);
                ForwardStream->println(F(" send failed"));
              }
          }
        }
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
bool tNMEA2000::IsFastPacketPGN(unsigned long PGN) {
  if ( IsFastPacketSystemMessage(PGN) || IsMandatoryFastPacketMessage(PGN) || 
       ( FastPacketMessages[0]==0 && IsDefaultFastPacketMessage(PGN) ) ) return true;
  int i;

    for (unsigned char igroup=0; (igroup<N2kMessageGroups); igroup++)  {
      if (FastPacketMessages[igroup]!=0) {
        for (i=0; pgm_read_dword(&FastPacketMessages[igroup][i])!=PGN && pgm_read_dword(&FastPacketMessages[igroup][i])!=0; i++);
        if (pgm_read_dword(&FastPacketMessages[igroup][i])==PGN) {
         return true;
        }
      }
    }

    return false;
}

//*****************************************************************************
bool tNMEA2000::IsFastPacket(const tN2kMsg &N2kMsg) {
  if (N2kMsg.Priority>=0x80) return false; // Special handling for force to send message as single frame.
  
  return IsFastPacketPGN(N2kMsg.PGN);
}

//*****************************************************************************
bool tNMEA2000::CheckKnownMessage(unsigned long PGN, bool &SystemMessage, bool &FastPacket) {
  int i;
//    return true;
    FastPacket=false;
    SystemMessage=false;
    if ( PGN==0 ) { return false; }  // Unknown

    // Check other messages
    if ( SingleFrameMessages[0]==0 && IsDefaultSingleFrameMessage(PGN) ) return true;
    if ( (FastPacket=IsMandatoryFastPacketMessage(PGN)) ) return true;
    if ( FastPacketMessages[0]==0 && (FastPacket=IsDefaultFastPacketMessage(PGN))==true ) return true;
    
    // Check system messages
    if ( IsSingleFrameSystemMessage(PGN) || (FastPacket=IsFastPacketSystemMessage(PGN))==true ) {
      SystemMessage=true;
      return true;
    }
      
    // Check user defined messages
    for (unsigned char igroup=0; (igroup<N2kMessageGroups); igroup++)  {
      if (SingleFrameMessages[igroup]!=0) {
        for (i=0; pgm_read_dword(&SingleFrameMessages[igroup][i])!=PGN && pgm_read_dword(&SingleFrameMessages[igroup][i])!=0; i++);
        if (pgm_read_dword(&SingleFrameMessages[igroup][i])==PGN) return true;
      }

      if (FastPacketMessages[igroup]!=0) {
        for (i=0; pgm_read_dword(&FastPacketMessages[igroup][i])!=PGN && pgm_read_dword(&FastPacketMessages[igroup][i])!=0; i++);
        if (pgm_read_dword(&FastPacketMessages[igroup][i])==PGN) {
         FastPacket=true;
         return true;
        }
      }
    }

    return false;
}

//*****************************************************************************
void CopyBufToCANMsg(tN2kCANMsg &CANMsg, unsigned char start, unsigned char len, unsigned char *buf) {
        for (int j=start; (j<len) & (CANMsg.CopiedLen<CANMsg.N2kMsg.MaxDataLen); j++, CANMsg.CopiedLen++) {
          CANMsg.N2kMsg.Data[CANMsg.CopiedLen]=buf[j];
        }
}

//*****************************************************************************
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
void tNMEA2000::FindFreeCANMsgIndex(unsigned long PGN, unsigned char Source, unsigned char Destination, bool TPMsg, uint8_t &MsgIndex) {
#else
void tNMEA2000::FindFreeCANMsgIndex(unsigned long PGN, unsigned char Source, unsigned char Destination, uint8_t &MsgIndex) {
#endif
  unsigned long OldestMsgTime,CurTime;
  int OldestIndex;

  for (MsgIndex=0, CurTime=OldestMsgTime=millis(), OldestIndex=MaxN2kCANMsgs;
       MsgIndex<MaxN2kCANMsgs &&
       !( N2kCANMsgBuf[MsgIndex].FreeMsg ||
         ( N2kCANMsgBuf[MsgIndex].N2kMsg.PGN==PGN 
           && N2kCANMsgBuf[MsgIndex].N2kMsg.Source==Source
           && N2kCANMsgBuf[MsgIndex].N2kMsg.Destination==Destination
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
           && N2kCANMsgBuf[MsgIndex].N2kMsg.IsTPMessage()==TPMsg
#endif
         )
        );
       MsgIndex++) { // Find free message place
    if (N2kCANMsgBuf[MsgIndex].N2kMsg.MsgTime<OldestMsgTime) {
      OldestIndex=MsgIndex;
      OldestMsgTime=N2kCANMsgBuf[MsgIndex].N2kMsg.MsgTime;
    }
  }
  if ( MsgIndex==MaxN2kCANMsgs && OldestMsgTime+Max_N2kMsgBuf_Time<CurTime) {
    MsgIndex=OldestIndex; // Use the old one, which has timed out
    N2kCANMsgBuf[MsgIndex].FreeMessage();
  }

}

#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)

//*****************************************************************************
bool tNMEA2000::SendTPCM_BAM(int iDev) {
  if ( !IsActiveNode() ) return false;

  tN2kMsg N2kMsg;
  
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=0xff;
  N2kMsg.SetPGN(TP_CM);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(TP_CM_BAM);
  int nBytes=Devices[iDev].PendingTPMsg.DataLen;
  N2kMsg.Add2ByteUInt(nBytes);
  N2kMsg.AddByte(nBytes/7+(nBytes%7!=0?1:0));
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.Add3ByteInt(Devices[iDev].PendingTPMsg.PGN);
  return SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
bool tNMEA2000::SendTPCM_RTS(int iDev) {
  if ( !IsActiveNode() ) return false;

  tN2kMsg N2kMsg;
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=Devices[iDev].PendingTPMsg.Destination;
  N2kMsg.SetPGN(TP_CM);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(TP_CM_RTS);
  int nBytes=Devices[iDev].PendingTPMsg.DataLen;
  N2kMsg.Add2ByteUInt(nBytes);
  N2kMsg.AddByte(nBytes/7+(nBytes%7!=0?1:0));
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.Add3ByteInt(Devices[iDev].PendingTPMsg.PGN);
  return SendMsg(N2kMsg,iDev);
}

unsigned char TPCtsPackets(unsigned char nPackets) { return tNMEA2000::N2kMax<unsigned char>(1,tNMEA2000::N2kMin<unsigned char>(nPackets,TP_MAX_FRAMES)); }

//*****************************************************************************
void tNMEA2000::SendTPCM_CTS(unsigned long PGN, unsigned char Destination, int iDev, unsigned char nPackets, unsigned char NextPacketNumber) {
  tN2kMsg N2kMsg;

  if ( !IsActiveNode() ) return;
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=Destination;
  N2kMsg.SetPGN(TP_CM);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(TP_CM_CTS);
  N2kMsg.AddByte(TPCtsPackets(nPackets));
  N2kMsg.AddByte(NextPacketNumber);
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.Add3ByteInt(PGN);
  SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
void tNMEA2000::SendTPCM_EndAck(unsigned long PGN, unsigned char Destination, int iDev, uint16_t nBytes, unsigned char nPackets) {
  tN2kMsg N2kMsg;

  if ( !IsActiveNode() ) return;
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=Destination;
  N2kMsg.SetPGN(TP_CM);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(TP_CM_ACK);
  N2kMsg.Add2ByteUInt(nBytes);
  N2kMsg.AddByte(nPackets);
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.Add3ByteInt(PGN);
  SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
void tNMEA2000::SendTPCM_Abort(unsigned long PGN, unsigned char Destination, int iDev, unsigned char AbortCode) {
  tN2kMsg N2kMsg;

  if ( !IsActiveNode() ) return;
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=Destination;
  N2kMsg.SetPGN(TP_CM);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(TP_CM_Abort);
  N2kMsg.AddByte(AbortCode);
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.AddByte(0xff); // Reserved;
  N2kMsg.Add3ByteInt(PGN);
  SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
// Caller should take care of not calling this after all has been done. 
// Use HasAllTPDTSent for checking.
bool tNMEA2000::SendTPDT(int iDev) {
  tN2kMsg N2kMsg;
  N2kMsg.Source=Devices[iDev].N2kSource;
  N2kMsg.Destination=Devices[iDev].PendingTPMsg.Destination;
  N2kMsg.SetPGN(TP_DT);
  N2kMsg.Priority=6;
  N2kMsg.AddByte(Devices[iDev].NextDTSequence+1);
  int iByteToSend=Devices[iDev].NextDTSequence*7;
  for ( int i=0; i<7; i++,iByteToSend++ ) {
    if ( iByteToSend<Devices[iDev].PendingTPMsg.DataLen ) {
      N2kMsg.AddByte(Devices[iDev].PendingTPMsg.Data[iByteToSend]);
    } else N2kMsg.AddByte(0xff);
  }
  Devices[iDev].NextDTSequence++;

  return SendMsg(N2kMsg,iDev);
}

//*****************************************************************************
bool tNMEA2000::HasAllTPDTSent(int iDev) {
  return ( Devices[iDev].NextDTSequence*7>=Devices[iDev].PendingTPMsg.DataLen );
}

//*****************************************************************************
bool tNMEA2000::TestHandleTPMessage(unsigned long PGN, unsigned char Source, unsigned char Destination,
                                    unsigned char len, unsigned char *buf,
                                    uint8_t &MsgIndex) {
  MsgIndex=MaxN2kCANMsgs;
  int iDev=FindSourceDeviceIndex(Destination);

  if ( PGN==TP_CM ) {
    unsigned char TP_CM_Control=buf[0];
    int Index=5;
    unsigned long TransportPGN=GetBuf3ByteUInt(Index,buf);

    switch (TP_CM_Control) {
      case TP_CM_BAM:
      case TP_CM_RTS: {
        N2kMsgDbgln("Got TP Command");
        Index=1;
        uint16_t nBytes=GetBuf2ByteUInt(Index,buf);
        uint8_t TPMaxPackets=buf[Index++];
        //Index++; // reserved

        FindFreeCANMsgIndex(TransportPGN,Source,Destination,true,MsgIndex);

        if (MsgIndex==MaxN2kCANMsgs) { // No free msg place
          N2kMsgDbgln("No free msg slot");
          if ( (TP_CM_Control==TP_CM_RTS) && (iDev>=0) ) { // If it was for us and not broadcast, we need to abort transport
            SendTPCM_Abort(TransportPGN,Source,iDev,TP_CM_AbortBusy);
          }
        } else { // Start transport
          N2kMsgDbg("Use msg slot: "); N2kMsgDbgln(MsgIndex);
          bool FastPacket;
          N2kCANMsgBuf[MsgIndex].KnownMessage=CheckKnownMessage(TransportPGN,N2kCANMsgBuf[MsgIndex].SystemMessage,FastPacket);
          if ( nBytes < tN2kMsg::MaxDataLen &&  // Currently we can handle only tN2kMsg::MaxDataLen long messages
               (N2kCANMsgBuf[MsgIndex].KnownMessage || !HandleOnlyKnownMessages()) ) {
            N2kCANMsgBuf[MsgIndex].FreeMsg=false;
            N2kCANMsgBuf[MsgIndex].N2kMsg.Init(7 /* Priority? */,TransportPGN,Source,Destination);
            N2kCANMsgBuf[MsgIndex].CopiedLen=0;
            N2kCANMsgBuf[MsgIndex].LastFrame=0;
            N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen=nBytes;
            N2kCANMsgBuf[MsgIndex].N2kMsg.SetIsTPMessage();
            N2kCANMsgBuf[MsgIndex].TPMaxPackets=TPMaxPackets;
            if ( (TP_CM_Control==TP_CM_RTS) && (iDev>=0) ) { // If it was for us and not broadcast, we need to response
              SendTPCM_CTS(TransportPGN,Source,iDev,N2kCANMsgBuf[MsgIndex].TPMaxPackets,N2kCANMsgBuf[MsgIndex].LastFrame+1);
              N2kCANMsgBuf[MsgIndex].TPRequireCTS=TPCtsPackets(N2kCANMsgBuf[MsgIndex].TPMaxPackets);
            } else {
              N2kCANMsgBuf[MsgIndex].TPMaxPackets=0xff; // TPMaxPackets>0 indicates that it is TP message
            }
          } else { // Too long or unknown
            if ( (TP_CM_Control==TP_CM_RTS) && (iDev>=0) ) { // If it was for us and not broadcast, we need to response
              SendTPCM_Abort(TransportPGN,Source,iDev,TP_CM_AbortBusy);  // Abort
            }
          }
        }
        break;
      }
      case TP_CM_CTS:
        if ( !IsValidDevice(iDev) ) break; // Should never fail
        N2kMsgDbgln("Got TP CTS");
        if ( IsBroadcast(Devices[iDev].PendingTPMsg.Destination) ) break; // We should not get controls for broadcast TP msg
        if ( Devices[iDev].PendingTPMsg.PGN!=TransportPGN ) { // Some failure on communication
          EndSendTPMessage(iDev); // Should we retry from beginning?
          break;
        }
        // Now respond with next data packets
        if ( buf[1]>0 ) { // Note that with 0, receiver wants to have break
          if ( buf[2]-1!=Devices[iDev].NextDTSequence ) { // We got sequence error
            EndSendTPMessage(iDev); // Should we retry from beginning?
            break;
          }
          uint8_t MaxTPSequences=buf[1];
          bool TPDTResult=true;
          for ( uint8_t iSeq=0; TPDTResult &&iSeq<MaxTPSequences && !HasAllTPDTSent(iDev); iSeq++ ) TPDTResult&=SendTPDT(iDev);
          if ( !TPDTResult ) EndSendTPMessage(iDev);
        }
        Devices[iDev].NextDTSendTime=millis()+100; // Set timeout for next response
        break;
      case TP_CM_ACK:
        if ( !IsValidDevice(iDev) ) break; // Should never fail
        N2kMsgDbgln("Got TP ACK");
        if ( IsBroadcast(Devices[iDev].PendingTPMsg.Destination) ) break; // We should not get controls for broadcast TP msg
        EndSendTPMessage(iDev);
        break;
      case TP_CM_Abort:
        if ( !IsValidDevice(iDev) ) break; // Should never fail
        N2kMsgDbgln("Got TP Abort");
        if ( IsBroadcast(Devices[iDev].PendingTPMsg.Destination) ) break; // We should not get controls for broadcast TP msg
        EndSendTPMessage(iDev);
        break;
      default:
        ;
    }
    MsgIndex=MaxN2kCANMsgs;  // After TP_CM, message is newer ready.
    return true;
  } else if ( PGN==TP_DT ) { // Datapacket
    N2kMsgDbgln("Got TP data");
    // So we need to find TP msg which sender and destination matches.
    for (MsgIndex=0;
         MsgIndex<MaxN2kCANMsgs && 
         !( !N2kCANMsgBuf[MsgIndex].FreeMsg 
            && N2kCANMsgBuf[MsgIndex].N2kMsg.IsTPMessage()
            && N2kCANMsgBuf[MsgIndex].N2kMsg.Destination==Destination 
            && N2kCANMsgBuf[MsgIndex].N2kMsg.Source==Source
          );
         MsgIndex++);
    // if (MsgIndex==MaxN2kCANMsgs) N2kMsgDbgln("TP data msg not found");
    // for (int i=1; i<len; i++) N2kMsgDbgln(buf[i]);
    if (MsgIndex<MaxN2kCANMsgs) { // found TP message under reception
      N2kMsgDbg("Use msg slot: "); N2kMsgDbgln(MsgIndex);
      if (N2kCANMsgBuf[MsgIndex].LastFrame+1 == buf[0]) { // Right packet is coming
        // Add packet to the message
        CopyBufToCANMsg(N2kCANMsgBuf[MsgIndex],1,len,buf);
        N2kCANMsgBuf[MsgIndex].LastFrame=buf[0];
        // Transport protocol is slower, so to avoid timeout, we reset message time
        N2kCANMsgBuf[MsgIndex].N2kMsg.MsgTime=millis();
        if ( N2kCANMsgBuf[MsgIndex].CopiedLen>=N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen ) { // all done
          N2kCANMsgBuf[MsgIndex].Ready=true;
          if ( N2kCANMsgBuf[MsgIndex].TPRequireCTS>0 && iDev>=0 ) { // send response
            SendTPCM_EndAck(N2kCANMsgBuf[MsgIndex].N2kMsg.PGN,Source,iDev,N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen,N2kCANMsgBuf[MsgIndex].LastFrame);
          }
        } else {
          if ( N2kCANMsgBuf[MsgIndex].TPRequireCTS>0 && ((N2kCANMsgBuf[MsgIndex].LastFrame)%N2kCANMsgBuf[MsgIndex].TPRequireCTS)==0 ) { // send response
            SendTPCM_CTS(N2kCANMsgBuf[MsgIndex].N2kMsg.PGN,Source,iDev,N2kCANMsgBuf[MsgIndex].TPMaxPackets,N2kCANMsgBuf[MsgIndex].LastFrame+1);
          }
        }
      } else { // Wrong packet - either we lost packet or sender sends wrong, so free this
        N2kMsgDbg("Invalid packet: "); N2kMsgDbgln(buf[0]);
        if ( N2kCANMsgBuf[MsgIndex].TPRequireCTS>0 && iDev>=0 ) { // We need to abort transport
          SendTPCM_Abort(N2kCANMsgBuf[MsgIndex].N2kMsg.PGN,Source,iDev,TP_CM_AbortTimeout);  // Abort transport
        }
        N2kCANMsgBuf[MsgIndex].FreeMessage();

      }
      if ( !N2kCANMsgBuf[MsgIndex].Ready ) MsgIndex=MaxN2kCANMsgs;
    }
    return true; // We handled message
  }

  return false;
}

//*****************************************************************************
bool tNMEA2000::StartSendTPMessage(const tN2kMsg& msg, int iDev) {
  if ( !IsValidDevice(iDev) ) return false;
  
  if ( Devices[iDev].PendingTPMsg.PGN!=0 ) return false; // No room for sending TP message

  int result=false;

  Devices[iDev].PendingTPMsg=msg;
  Devices[iDev].NextDTSequence=0;
  Devices[iDev].NextDTSendTime=millis()+50;
  if ( IsBroadcast(msg.Destination) ) { // Start with BAM
    result=SendTPCM_BAM(iDev);
  } else {
    result=SendTPCM_RTS(iDev);
  }
  
  if ( !result ) EndSendTPMessage(iDev); // Currently no retry
  
  return result;
}

//*****************************************************************************
void tNMEA2000::EndSendTPMessage(int iDev) {
  Devices[iDev].PendingTPMsg.Clear();
  Devices[iDev].NextDTSendTime=0;
}

//*****************************************************************************
void tNMEA2000::SendPendingTPMessages() {
  for (int iDev=0; iDev<DeviceCount; iDev++ ) {
    if ( Devices[iDev].PendingTPMsg.PGN!=0 && Devices[iDev].NextDTSendTime<millis() ) { // Pending message and timed out
      if ( IsBroadcast(Devices[iDev].PendingTPMsg.Destination) ) { // For broadcast we just send next data
        SendTPDT(iDev);
        Devices[iDev].NextDTSendTime=millis()+50;
        if ( HasAllTPDTSent(iDev) ) EndSendTPMessage(iDev); // All done
      } else { // We have not got response from receiver within timeout, so just end. Or should we retry?
        EndSendTPMessage(iDev);
      }
    }
  }
}

#endif

//*****************************************************************************
inline bool IsFastPacketFirstFrame(unsigned char b) { return ((b & 0x1F)==0); }

//*****************************************************************************
// Function handles received CAN frame and adds it to tN2kCANMsg.
// Returns: Index to ready tN2kCANMsg or MaxN2kCANMsgs, if we skipped the frame
//          or message is not ready (fast packet or ISO Multi-Packet)
uint8_t tNMEA2000::SetN2kCANBufMsg(unsigned long canId, unsigned char len, unsigned char *buf) {
  unsigned char Priority;
  unsigned long PGN;
  unsigned char Source;
  unsigned char Destination;
  bool FastPacket;
  bool SystemMessage;
  bool KnownMessage;
  uint8_t MsgIndex=MaxN2kCANMsgs;

    CanIdToN2k(canId,Priority,PGN,Source,Destination);
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    if ( !TestHandleTPMessage(PGN,Source,Destination,len,buf,MsgIndex) )
#endif
    {
      KnownMessage=CheckKnownMessage(PGN,SystemMessage,FastPacket);
      if ( KnownMessage || !HandleOnlyKnownMessages() ) {
        if (FastPacket && !IsFastPacketFirstFrame(buf[0]) ) { // Not first frame
        N2kFrameInDbg("New frame="); N2kFrameInDbg(PGN); N2kFrameInDbg(" frame="); N2kFrameInDbg(buf[0],HEX); N2kFrameInDbgln();
          // Find previous slot for this PGN
          for (MsgIndex=0;
               MsgIndex<MaxN2kCANMsgs &&
               !( N2kCANMsgBuf[MsgIndex].N2kMsg.PGN==PGN 
                  && N2kCANMsgBuf[MsgIndex].N2kMsg.Source==Source
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
                  && !N2kCANMsgBuf[MsgIndex].N2kMsg.IsTPMessage()
#endif            
                );
               MsgIndex++);
          if (MsgIndex<MaxN2kCANMsgs) { // we found start for this message, so add data to it.
            N2kMsgDbg("Use msg slot: "); N2kMsgDbgln(MsgIndex);
            if (N2kCANMsgBuf[MsgIndex].LastFrame+1 == buf[0]) { // Right frame is coming
              N2kCANMsgBuf[MsgIndex].LastFrame=buf[0];
              CopyBufToCANMsg(N2kCANMsgBuf[MsgIndex],1,len,buf);
            } else { // We have lost frame, so free this
              N2kFrameInDbg(millis()); N2kFrameInDbg(", Lost frame ");  N2kFrameInDbg(N2kCANMsgBuf[MsgIndex].LastFrame); N2kFrameInDbg("/");  N2kFrameInDbg(buf[0]); 
              N2kFrameInDbg(", source ");  N2kFrameInDbg(Source); N2kFrameInDbg(" for: "); N2kFrameInDbgln(PGN);
              N2kCANMsgBuf[MsgIndex].FreeMessage();
              MsgIndex=MaxN2kCANMsgs;
            }
          } else {  // Orphan frame
              N2kFrameInDbg(millis()); N2kFrameInDbg(", Orphan frame "); N2kFrameInDbg(buf[0]); N2kFrameInDbg(", source ");  
              N2kFrameInDbg(Source); N2kFrameInDbg(" for: "); N2kFrameInDbgln(PGN);
          }
        } else { // Handle first frame
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
          FindFreeCANMsgIndex(PGN,Source,Destination,false,MsgIndex);
#else          
          FindFreeCANMsgIndex(PGN,Source,Destination,MsgIndex);
#endif        
          if ( MsgIndex<MaxN2kCANMsgs ) { // we found free place, so handle frame
            N2kMsgDbg("Use msg slot: "); N2kMsgDbgln(MsgIndex);
            N2kCANMsgBuf[MsgIndex].FreeMsg=false;
            N2kCANMsgBuf[MsgIndex].KnownMessage=KnownMessage;
            N2kCANMsgBuf[MsgIndex].SystemMessage=SystemMessage;
            N2kCANMsgBuf[MsgIndex].N2kMsg.Init(Priority,PGN,Source,Destination);
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
            N2kCANMsgBuf[MsgIndex].N2kMsg.SetIsTPMessage(false);
#endif            
            N2kCANMsgBuf[MsgIndex].CopiedLen=0;
            if (FastPacket) {
              CopyBufToCANMsg(N2kCANMsgBuf[MsgIndex],2,len,buf);
              N2kFrameInDbg("First frame="); N2kFrameInDbg(PGN);  N2kFrameInDbgln();
              N2kCANMsgBuf[MsgIndex].LastFrame=buf[0];
              N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen=buf[1];
            } else {
              CopyBufToCANMsg(N2kCANMsgBuf[MsgIndex],0,len,buf);
              N2kFrameInDbg("Single frame="); N2kFrameInDbg(PGN); N2kFrameInDbgln();
              N2kCANMsgBuf[MsgIndex].LastFrame=0;
              N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen=len;
            }
          }
        }

        if ( MsgIndex<MaxN2kCANMsgs ) {
          N2kCANMsgBuf[MsgIndex].Ready=(N2kCANMsgBuf[MsgIndex].CopiedLen>=N2kCANMsgBuf[MsgIndex].N2kMsg.DataLen);
          if ( !N2kCANMsgBuf[MsgIndex].Ready ) MsgIndex=MaxN2kCANMsgs; // If packet is not ready, do not return index to it
        }
      }
    }

    return MsgIndex;
}

//*****************************************************************************
 int tNMEA2000::FindSourceDeviceIndex(unsigned char Source) {
   int i=DeviceCount;

     if ( Source<=253 ) {
       for (i=0; i<DeviceCount && Devices[i].N2kSource!=Source; i++);
     }

     return (i<DeviceCount?i:-1);
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
void tNMEA2000::ForwardMessage(const tN2kCANMsg &N2kCanMsg) {
  if ( N2kCanMsg.KnownMessage || !ForwardOnlyKnownMessages() ) ForwardMessage(N2kCanMsg.N2kMsg);
}

//*****************************************************************************
void tNMEA2000::SendIsoAddressClaim(unsigned char Destination, int DeviceIndex) {

  // Some devices (Garmin) request constantly information on network about others
  // 59904 ISO Request:  PGN = 60928
  // So we need to Re-send Address claim, or they will stop detecting us
  if (Destination==0xff && DeviceIndex==-1) DeviceIndex=0;

  if ( DeviceIndex<0 || DeviceIndex>=DeviceCount) return;
  tN2kMsg RespondMsg(Devices[DeviceIndex].N2kSource);

    RespondMsg.Destination=Destination;
    SetN2kISOAddressClaim(RespondMsg,Devices[DeviceIndex].DeviceInformation.GetName());
    SendMsg(RespondMsg,DeviceIndex);
}

template <typename T> void PROGMEM_readAnything (const T * sce, T& dest)
  {
  memcpy_P (&dest, sce, sizeof (T));
  }


//*****************************************************************************
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
void tNMEA2000::SendTxPGNList(unsigned char Destination, int DeviceIndex, bool UseTP) {
#else
void tNMEA2000::SendTxPGNList(unsigned char Destination, int DeviceIndex) {
#endif
  if (Destination==0xff && DeviceIndex==-1) DeviceIndex=0;

  if ( !IsValidDevice(DeviceIndex) ) return;

  tN2kMsg RespondMsg(Devices[DeviceIndex].N2kSource);
  unsigned long PGN;

    RespondMsg.Destination=Destination;
    RespondMsg.SetPGN(126464L);
    RespondMsg.Priority=6;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    RespondMsg.SetIsTPMessage(UseTP);
#endif
    RespondMsg.AddByte(N2kpgnl_transmit);
    // First add default messages
    for (int i=0; (PGN=pgm_read_dword(&DefTransmitMessages[i]))!=0; i++) {
      RespondMsg.Add3ByteInt(PGN);
    }
    if (Devices[DeviceIndex].TransmitMessages!=0) {
      for (int i=0; (PGN=pgm_read_dword(&Devices[DeviceIndex].TransmitMessages[i]))!=0; i++) {
        RespondMsg.Add3ByteInt(PGN);
      }
    }
    SendMsg(RespondMsg,DeviceIndex);
}

//*****************************************************************************
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
void tNMEA2000::SendRxPGNList(unsigned char Destination, int DeviceIndex, bool UseTP) {
#else
void tNMEA2000::SendRxPGNList(unsigned char Destination, int DeviceIndex) {
#endif
  if (Destination==0xff && DeviceIndex==-1) DeviceIndex=0;

  if ( !IsValidDevice(DeviceIndex) ) return;

  tN2kMsg RespondMsg(Devices[DeviceIndex].N2kSource);
  unsigned long PGN;

    RespondMsg.Destination=Destination;
    RespondMsg.SetPGN(126464L);
    RespondMsg.Priority=6;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    RespondMsg.SetIsTPMessage(UseTP);
#endif
    RespondMsg.AddByte(N2kpgnl_receive);
    // First add default messages
    for (int i=0; (PGN=pgm_read_dword(&DefReceiveMessages[i]))!=0; i++) {
      RespondMsg.Add3ByteInt(PGN);
    }
    if (Devices[DeviceIndex].ReceiveMessages!=0) {
      for (int i=0; (PGN=pgm_read_dword(&Devices[DeviceIndex].ReceiveMessages[i]))!=0; i++) {
        RespondMsg.Add3ByteInt(PGN);
      }
    }
    SendMsg(RespondMsg,DeviceIndex);
}

//*****************************************************************************
void SetN2kPGN126996Progmem(tN2kMsg &N2kMsg, const tNMEA2000::tProductInformation *ProductInformation, char *OptionalSerialCode=0) {
  int i;

    N2kMsg.SetPGN(126996L);
    N2kMsg.Priority=6;
    N2kMsg.Add2ByteInt(pgm_read_word(&ProductInformation->N2kVersion));
    N2kMsg.Add2ByteInt(pgm_read_word(&ProductInformation->ProductCode));
    for (i=0; i<Max_N2kModelID_len && pgm_read_byte(&ProductInformation->N2kModelID[i]); i++ ) { N2kMsg.AddByte(pgm_read_byte(&ProductInformation->N2kModelID[i])); }
    for (; i<Max_N2kModelID_len; i++ ) { N2kMsg.AddByte(0); }
    for (i=0; i<Max_N2kSwCode_len && pgm_read_byte(&ProductInformation->N2kSwCode[i]); i++ ) { N2kMsg.AddByte(pgm_read_byte(&ProductInformation->N2kSwCode[i])); }
    for (; i<Max_N2kSwCode_len; i++ ) { N2kMsg.AddByte(0); }
    for (i=0; i<Max_N2kModelVersion_len && pgm_read_byte(&ProductInformation->N2kModelVersion[i]); i++ ) { N2kMsg.AddByte(pgm_read_byte(&ProductInformation->N2kModelVersion[i])); }
    for (; i<Max_N2kModelVersion_len; i++ ) { N2kMsg.AddByte(0); }
    if (OptionalSerialCode) {
      for (i=0; i<Max_N2kModelSerialCode_len && OptionalSerialCode[i]; i++ ) { N2kMsg.AddByte(OptionalSerialCode[i]); }
    } else {
      for (i=0; i<Max_N2kModelSerialCode_len && pgm_read_byte(&ProductInformation->N2kModelSerialCode[i]); i++ ) { N2kMsg.AddByte(pgm_read_byte(&ProductInformation->N2kModelSerialCode[i])); }
    }
    for (; i<Max_N2kModelSerialCode_len; i++ ) { N2kMsg.AddByte(0); }
    N2kMsg.AddByte(pgm_read_byte(&ProductInformation->CertificationLevel));
    N2kMsg.AddByte(pgm_read_byte(&ProductInformation->LoadEquivalency));
}

//*****************************************************************************
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
bool tNMEA2000::SendProductInformation(int iDev) {
  return SendProductInformation(0xff,iDev,false);
}

bool tNMEA2000::SendProductInformation(unsigned char Destination, int iDev, bool UseTP) {
#else
bool tNMEA2000::SendProductInformation(int iDev) {
#endif
  if ( !IsValidDevice(iDev) ) return false;
  tN2kMsg RespondMsg(Devices[iDev].N2kSource);
  int iPIDev=iDev;

    if ( Devices[iPIDev].ProductInformation==0 ) iPIDev=0; // Use first device product information
    if ( Devices[iPIDev].ProductInformation==0 ) return false; // Can not do anything.

    if ( Devices[iPIDev].ProductInformation==Devices[iPIDev].LocalProductInformation ) {
      SetN2kProductInformation(RespondMsg,Devices[iPIDev].ProductInformation->N2kVersion,
                                          Devices[iPIDev].ProductInformation->ProductCode,
                                          Devices[iPIDev].ProductInformation->N2kModelID,
                                          Devices[iPIDev].ProductInformation->N2kSwCode,
                                          Devices[iPIDev].ProductInformation->N2kModelVersion,
                                          Devices[iPIDev].ProductInformation->N2kModelSerialCode,
                                          Devices[iPIDev].ProductInformation->CertificationLevel,
                                          Devices[iPIDev].ProductInformation->LoadEquivalency);
    } else {
      SetN2kPGN126996Progmem(RespondMsg,Devices[iPIDev].ProductInformation);
    }
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    RespondMsg.Destination=Destination;
    RespondMsg.SetIsTPMessage(UseTP);
#endif
    if (SendMsg(RespondMsg,iDev) ) {
      Devices[iDev].ClearPendingProductInformation();
      return true;
    }

    Devices[iDev].SetPendingProductInformation();
    return false;
}

//*****************************************************************************
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
bool tNMEA2000::SendConfigurationInformation(int DeviceIndex) {
  return SendConfigurationInformation(0xff,DeviceIndex,false);
}

bool tNMEA2000::SendConfigurationInformation(unsigned char Destination, int DeviceIndex, bool UseTP) {
#else
bool tNMEA2000::SendConfigurationInformation(int DeviceIndex) {
#endif
  if ( !IsValidDevice(DeviceIndex) ) return false;
  tN2kMsg RespondMsg(Devices[DeviceIndex].N2kSource);

    if ( ConfigurationInformation.ManufacturerInformation!=0 ||
         ConfigurationInformation.InstallationDescription1!=0 ||
         ConfigurationInformation.InstallationDescription2!=0 ) {
        SetN2kConfigurationInformation(RespondMsg,
                                       ConfigurationInformation.ManufacturerInformation,
                                       ConfigurationInformation.InstallationDescription1,
                                       ConfigurationInformation.InstallationDescription2,
                                       LocalConfigurationInformationData==0);
    } else { // No information provided, so respond not available
      SetN2kPGNISOAcknowledgement(RespondMsg,1,0xff,126998L);
    }

#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    RespondMsg.Destination=Destination;
    RespondMsg.SetIsTPMessage(UseTP);
#endif
    if (SendMsg(RespondMsg,DeviceIndex) ) {
      Devices[DeviceIndex].ClearPendingConfigurationInformation();
      return true;
    }

    Devices[DeviceIndex].SetPendingConfigurationInformation();
    return false;
}

//*****************************************************************************
void tNMEA2000::RespondISORequest(const tN2kMsg &N2kMsg, unsigned long RequestedPGN, int iDev) {
    if ( IsAddressClaimStarted(iDev) ) return; // We do not respond any queries during address claiming.
  
    switch (RequestedPGN) {
      case 60928L: /*ISO Address Claim*/  // Someone is asking others to claim their addresses
        SendIsoAddressClaim(0xff,iDev);
        break;
      case 126464L:
        SendTxPGNList(N2kMsg.Source,iDev);
        SendRxPGNList(N2kMsg.Source,iDev);
        break;
      case 126996L: /* Product information */
        SendProductInformation(iDev);
        break;
      case 126998L: /* Configuration information */
        SendConfigurationInformation(iDev);
        break;
      default:
        /* If user has established a handler */
        if (ISORqstHandler!=0) {
          /* and if it handled the request, we are done */
          if (ISORqstHandler(RequestedPGN,N2kMsg.Source,iDev)) {
            return;
          }
        }

        tN2kMsg   N2kMsgR;
        // No user handler, or there was one and it retured FALSE.  Send NAK
        SetN2kPGNISOAcknowledgement(N2kMsgR,1,0xff,RequestedPGN);
        // Direct the response to original requester.
        N2kMsgR.Destination  = N2kMsg.Source;
        SendMsg(N2kMsgR,iDev);
    }
}

//*****************************************************************************
void tNMEA2000::HandleISORequest(const tN2kMsg &N2kMsg) {
  unsigned long RequestedPGN;
  int iDev=FindSourceDeviceIndex(N2kMsg.Destination);

    if ( !tNMEA2000::IsBroadcast(N2kMsg.Destination) && iDev==-1) return; // if destination is not for us, we do nothing

    ParseN2kPGNISORequest(N2kMsg,RequestedPGN);
    N2kMsgDbg("ISO request: "); N2kMsgDbgln(RequestedPGN);
    if (tNMEA2000::IsBroadcast(N2kMsg.Destination)) { // broadcast -> respond from all devices
      for (iDev=0; iDev<DeviceCount; iDev++) RespondISORequest(N2kMsg,RequestedPGN,iDev);
    } else {
      RespondISORequest(N2kMsg,RequestedPGN,iDev);
    }
}

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
//*****************************************************************************
// Document https://www.nmea.org/Assets/20140109%20nmea-2000-corrigendum-tc201401031%20pgn%20126208.pdf
// defines that systems should respond to NMEA Request/Command/Acknowledge group function PGN 126208.
// Here we first call callback and if that will not handle function, we use default handler.
void tNMEA2000::RespondGroupFunction(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev) {
    // Find group function handler for PGN
    tN2kGroupFunctionHandler *pGroupFunctionHandler;
    for ( pGroupFunctionHandler=pGroupFunctionHandlers;
          pGroupFunctionHandler!=0 && pGroupFunctionHandler->PGN!=PGNForGroupFunction && pGroupFunctionHandler->PGN!=0;
          pGroupFunctionHandler=pGroupFunctionHandler->pNext);

    if ( pGroupFunctionHandler==0 || !pGroupFunctionHandler->Handle(N2kMsg,GroupFunctionCode, PGNForGroupFunction,iDev) ) {
      // Default handler is at last on the list, so currently do nothing here
    }
}

//*****************************************************************************
// Document https://www.nmea.org/Assets/20140109%20nmea-2000-corrigendum-tc201401031%20pgn%20126208.pdf
// defines that systems should respond to NMEA Request/Command/Acknowledge group function PGN 126208.
// On the document it is not clear can request be send as broadcast, so we handle it, if we can.
void tNMEA2000::HandleGroupFunction(const tN2kMsg &N2kMsg) {
  tN2kGroupFunctionCode GroupFunctionCode;
  unsigned long PGNForGroupFunction;
  int iDev=FindSourceDeviceIndex(N2kMsg.Destination);

    if ( !tNMEA2000::IsBroadcast(N2kMsg.Destination) && iDev==-1) return; // if destination is not for us, we do nothing

    if (!tN2kGroupFunctionHandler::Parse(N2kMsg,GroupFunctionCode,PGNForGroupFunction)) return;
    N2kMsgDbg("Group function: "); N2kMsgDbgln(PGNForGroupFunction);
    if ( tNMEA2000::IsBroadcast(N2kMsg.Destination) ) { // broadcast -> respond from all devices
      for (iDev=0; iDev<DeviceCount; iDev++) RespondGroupFunction(N2kMsg,GroupFunctionCode,PGNForGroupFunction,iDev);
    } else {
      RespondGroupFunction(N2kMsg,GroupFunctionCode,PGNForGroupFunction,iDev);
    }
}
#endif

//*****************************************************************************
void tNMEA2000::StartAddressClaim(int iDev) {
    if ( IsReadyToSend() ) { // Start address claim automatically
      Devices[iDev].AddressClaimStarted=0;
      if ( (ForwardStream!=0) && ( ForwardType==tNMEA2000::fwdt_Text) ) {
        ForwardStream->print(F("Start address claim for device "));
        ForwardStream->println(iDev);
      }
      SendIsoAddressClaim(0xff,iDev);
      Devices[iDev].AddressClaimStarted=millis();
    }
}

//*****************************************************************************
bool tNMEA2000::IsAddressClaimStarted(int iDev) {
  // Reset address claim after timeout
  if ( Devices[iDev].AddressClaimStarted!=0 ) {
    if ( Devices[iDev].AddressClaimStarted+N2kAddressClaimTimeout<millis()) {
      Devices[iDev].AddressClaimStarted=0;
      // We have claimed our address, so save end source for next possible claim run.
      Devices[iDev].UpdateAddressClaimEndSource();
    }
  }

  return (Devices[iDev].AddressClaimStarted!=0);
}

//*****************************************************************************
void tNMEA2000::HandleISOAddressClaim(const tN2kMsg &N2kMsg) {
  int iDev=FindSourceDeviceIndex(N2kMsg.Source);
  if ( N2kMsg.Source==N2kNullCanBusAddress || iDev==-1 ) return; // if the address is not same as we have, we do nothing

  int Index=0;
  uint64_t CallerName=N2kMsg.GetUInt64(Index);

    if (Devices[iDev].DeviceInformation.GetName()<CallerName) { // We can keep our address, so just reclaim it
      SendIsoAddressClaim(0xff,iDev);
    } else { // we have to try an other address
      if ( Devices[iDev].DeviceInformation.GetName()==CallerName && IsAddressClaimStarted(iDev) ) {
        // If the name is same, then the first instance will get claim and change its address.
        // This should not happen, if user takes care of setting unique ID for device information.
        // If he does not there is no problem with this class, but e.g. Garmin gets crazy.
        // Try to solve situation by changing our device instance.
        Devices[iDev].DeviceInformation.SetDeviceInstance(Devices[iDev].DeviceInformation.GetDeviceInstance()+1);
        DeviceInformationChanged=true;
      } else {
        GetNextAddress(iDev);
        AddressChanged=true;
      }
      StartAddressClaim(iDev);
    }
}

//*****************************************************************************
void tNMEA2000::HandleCommandedAddress(uint64_t CommandedName, unsigned char NewAddress, int iDev) {
  if ( IsBroadcast(NewAddress) ) return;
  if (Devices[iDev].DeviceInformation.GetName() == CommandedName &&
      Devices[iDev].N2kSource!=NewAddress) { // We have been commanded to set our address
    Devices[iDev].N2kSource=NewAddress;
    Devices[iDev].UpdateAddressClaimEndSource();
    StartAddressClaim(iDev);
    AddressChanged=true;
  }
}

//*****************************************************************************
void tNMEA2000::HandleCommandedAddress(const tN2kMsg &N2kMsg) {
  N2kMsgDbg(millis()); N2kMsgDbg(" Commanded address:"); N2kMsgDbgln(N2kMsg.Destination);

  if ( N2kMsg.PGN!=65240L || N2kMsg.DataLen!=9 ) return;

  int iDev=FindSourceDeviceIndex(N2kMsg.Destination);
    if ( !tNMEA2000::IsBroadcast(N2kMsg.Destination) && iDev==-1) return; // if destination is not for us, we do nothing

  int Index=0;
  uint64_t CommandedName=N2kMsg.GetUInt64(Index);
  unsigned char NewAddress=N2kMsg.GetByte(Index);

    if ( iDev==-1 ) {
      for (iDev=0; iDev<DeviceCount; iDev++) HandleCommandedAddress(CommandedName,NewAddress,iDev);
    } else {
      HandleCommandedAddress(CommandedName,NewAddress,iDev);
    }
}

//*****************************************************************************
bool tNMEA2000::ReadResetAddressChanged() {
  bool result=AddressChanged;

  AddressChanged=false;
  return result;
}

//*****************************************************************************
bool tNMEA2000::ReadResetDeviceInformationChanged() {
  bool result=DeviceInformationChanged;

  DeviceInformationChanged=false;
  return result;
}

//*****************************************************************************
void tNMEA2000::GetNextAddress(int DeviceIndex, bool RestartAtAnd) {
  bool FoundSame;
  // Currently simply add address
  // Note that 251 is the last source. We do not send data if address is higher than that.
  
  do {
    if ( Devices[DeviceIndex].N2kSource==N2kNullCanBusAddress ) {
      if ( RestartAtAnd ) {
        // For null address start from beginning.
        Devices[DeviceIndex].N2kSource=14;
        Devices[DeviceIndex].UpdateAddressClaimEndSource();
      } else return;
    } else if (Devices[DeviceIndex].N2kSource!=Devices[DeviceIndex].AddressClaimEndSource) {
      Devices[DeviceIndex].N2kSource++;
      // Roll to start?
      if ( Devices[DeviceIndex].N2kSource>N2kMaxCanBusAddress ) Devices[DeviceIndex].N2kSource=0;
    } else {
      Devices[DeviceIndex].N2kSource=N2kNullCanBusAddress; // Force null address = cannot claim address
      return;
    }
    FoundSame=false;
    // Check that we do not have same on our list
    for (int i=0; i<DeviceCount && !FoundSame; i++) {
      if (i!=DeviceIndex) FoundSame=(Devices[DeviceIndex].N2kSource==Devices[i].N2kSource);
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
          case 65240L: /*Commanded Address*/
            HandleCommandedAddress(N2kCANMsgBuf[MsgIndex].N2kMsg);
            break;
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
          case 126208L: /*NMEA Request/Command/Acknowledge group function*/
            HandleGroupFunction(N2kCANMsgBuf[MsgIndex].N2kMsg);
            break;
#endif
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
    uint8_t MsgIndex;
    static const int MaxReadFramesOnParse=20;
    int FramesRead=0;
//    tN2kMsg N2kMsg;

    if (!Open()) return;  // Can not do much

    if (dbMode != dm_None) return; // No much to do here, when in Debug mode

    SendFrames();
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    SendPendingTPMessages();
#endif
    SendPendingInformation();
#if defined(DEBUG_NMEA2000_ISR)
    TestISR();
#endif

    while (FramesRead<MaxReadFramesOnParse && CANGetFrame(canId,len,buf) ) {           // check if data coming
        FramesRead++;
        N2kMsgDbg("Received frame, can ID:"); N2kMsgDbg(canId); N2kMsgDbg(" len:"); N2kMsgDbg(len); N2kMsgDbg(" data:"); DbgPrintBuf(len,buf,false); N2kMsgDbgln();
        MsgIndex=SetN2kCANBufMsg(canId,len,buf);
        if (MsgIndex<MaxN2kCANMsgs) {
          if ( !HandleReceivedSystemMessage(MsgIndex) ) {
            N2kMsgDbgln(MsgIndex);
            ForwardMessage(N2kCANMsgBuf[MsgIndex]);
          }
//          N2kCANMsgBuf[MsgIndex].N2kMsg.Print(Serial);
          RunMessageHandlers(N2kCANMsgBuf[MsgIndex].N2kMsg);
          N2kCANMsgBuf[MsgIndex].FreeMessage();
          N2kMsgDbg(MsgIndex); N2kMsgDbgln();
        }
    }

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
    SendHeartbeat();
#endif
}

//*****************************************************************************
void tNMEA2000::RunMessageHandlers(const tN2kMsg &N2kMsg) {
  if ( MsgHandler!=0 ) MsgHandler(N2kMsg);
  
  tMsgHandler *MsgHandler=MsgHandlers;
  // Loop through all pgn handlers
  for ( ;MsgHandler!=0 && MsgHandler->GetPGN()==0; MsgHandler=MsgHandler->pNext) MsgHandler->HandleMsg(N2kMsg); 
  // Loop through specific pgn handlers
  for ( ;MsgHandler!=0 && MsgHandler->GetPGN()<=N2kMsg.PGN; MsgHandler=MsgHandler->pNext) {
    if ( MsgHandler->GetPGN()==N2kMsg.PGN ) MsgHandler->HandleMsg(N2kMsg); 
  }
}

//*****************************************************************************
void tNMEA2000::SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg)) {
  MsgHandler=_MsgHandler;
}

//*****************************************************************************
void tNMEA2000::AttachMsgHandler(tMsgHandler *_MsgHandler) {
  if ( _MsgHandler==0 ) return;
  
  if ( _MsgHandler->pNMEA2000==this ) return; // Already attached
  
  DetachMsgHandler(_MsgHandler);
  
  if ( MsgHandlers==0 ) {
    MsgHandlers=_MsgHandler;
  } else {
    tMsgHandler *MsgHandler=MsgHandlers;
    if ( MsgHandler->GetPGN()>_MsgHandler->GetPGN() ) { // Add to first
      _MsgHandler->pNext=MsgHandler;
      MsgHandlers=_MsgHandler;
    } else {
      for ( ; MsgHandler->pNext!=0 && MsgHandler->pNext->GetPGN()<_MsgHandler->GetPGN(); MsgHandler=MsgHandler->pNext );
      _MsgHandler->pNext=MsgHandler->pNext;
      MsgHandler->pNext=_MsgHandler;
    }
  }
  
  _MsgHandler->pNMEA2000=this;
}

//*****************************************************************************
void tNMEA2000::DetachMsgHandler(tMsgHandler *_MsgHandler) {
  if ( _MsgHandler==0 || _MsgHandler->pNMEA2000==0 ) return;
  
  tMsgHandler *MsgHandler=_MsgHandler->pNMEA2000->MsgHandlers;
  
  if ( MsgHandler==_MsgHandler ) { // Is this at first
    _MsgHandler->pNMEA2000->MsgHandlers=MsgHandler->pNext;
  } else {
    for ( ; MsgHandler!=0 && MsgHandler->pNext!=_MsgHandler; MsgHandler=MsgHandler->pNext );
    if ( MsgHandler!=0 ) MsgHandler->pNext=_MsgHandler->pNext;
  }
  _MsgHandler->pNext=0;
  _MsgHandler->pNMEA2000=0;
}

//*****************************************************************************
void tNMEA2000::SetISORqstHandler(bool(*ISORequestHandler)(unsigned long RequestedPGN, unsigned char Requester, int DeviceIndex)) {
  ISORqstHandler=ISORequestHandler;
}

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
//*****************************************************************************
void tNMEA2000::AddGroupFunctionHandler(tN2kGroupFunctionHandler *pGroupFunctionHandler) {
  if (pGroupFunctionHandler==0) return;
  // Add to the end on the list
  if ( pGroupFunctionHandlers==0 ) { // If there is none set, put it to first
    pGroupFunctionHandlers=pGroupFunctionHandler;
  } else {
    tN2kGroupFunctionHandler *pLastGroupFunctionHandler;
    // find last
    for ( pLastGroupFunctionHandler=pGroupFunctionHandlers;
          pLastGroupFunctionHandler->pNext!=0;
          pLastGroupFunctionHandler=pLastGroupFunctionHandler->pNext);
    pLastGroupFunctionHandler->pNext=pGroupFunctionHandler;
  }
}
#endif

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
    N2kMsg.Add3ByteInt(PGN);
}

//*****************************************************************************
// ISO Address Claim
void SetN2kPGN60928(tN2kMsg &N2kMsg, unsigned long UniqueNumber, int ManufacturerCode,
                   unsigned char DeviceFunction, unsigned char DeviceClass,
                   unsigned char DeviceInstance, unsigned char SystemInstance, unsigned char IndustryGroup
                   ) {
    N2kMsg.SetPGN(N2kPGNIsoAddressClaim);
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
    N2kMsg.SetPGN(N2kPGNIsoAddressClaim);
    N2kMsg.Priority=6;

    N2kMsg.AddUInt64(Name);
}

//*****************************************************************************
// Product Information
void SetN2kPGN126996(tN2kMsg &N2kMsg, unsigned int N2kVersion, unsigned int ProductCode,
                     const char *ModelID, const char *SwCode,
                     const char *ModelVersion, const char *ModelSerialCode,
                     unsigned char CertificationLevel, unsigned char LoadEquivalency) {

    N2kMsg.SetPGN(N2kPGNProductInformation);
    N2kMsg.Priority=6;
    N2kMsg.Add2ByteUInt(N2kVersion);
    N2kMsg.Add2ByteUInt(ProductCode);
    N2kMsg.AddStr(ModelID, Max_N2kModelID_len);
    N2kMsg.AddStr(SwCode, Max_N2kSwCode_len);
    N2kMsg.AddStr(ModelVersion, Max_N2kModelVersion_len);
    N2kMsg.AddStr(ModelSerialCode, Max_N2kModelSerialCode_len);
    N2kMsg.AddByte(CertificationLevel);
    N2kMsg.AddByte(LoadEquivalency);
}

bool ParseN2kPGN126996(const tN2kMsg& N2kMsg, unsigned short &N2kVersion, unsigned short &ProductCode,
                     int ModelIDSize, char *ModelID, int SwCodeSize, char *SwCode,
                     int ModelVersionSize, char *ModelVersion, int ModelSerialCodeSize, char *ModelSerialCode,
                     unsigned char &CertificationLevel, unsigned char &LoadEquivalency) {
  if (N2kMsg.PGN!=N2kPGNProductInformation) return false;

  int Index=0;
  N2kVersion=N2kMsg.Get2ByteUInt(Index);
  ProductCode=N2kMsg.Get2ByteUInt(Index);
  N2kMsg.GetStr(ModelIDSize,ModelID,Max_N2kModelID_len,0xff,Index);
  N2kMsg.GetStr(SwCodeSize,SwCode,Max_N2kSwCode_len,0xff,Index);
  N2kMsg.GetStr(ModelVersionSize,ModelVersion,Max_N2kModelVersion_len,0xff,Index);
  N2kMsg.GetStr(ModelSerialCodeSize,ModelSerialCode,Max_N2kModelSerialCode_len,0xff,Index);
  CertificationLevel=N2kMsg.GetByte(Index);
  LoadEquivalency=N2kMsg.GetByte(Index);
  
  return true;
}

//*****************************************************************************
size_t ProgmemStrLen(const char *str) {
  size_t len;
    if (str==0) return 0;
    for (len=0; pgm_read_byte(&(str[len]))!=0; len++ );
    return len;
}

//*****************************************************************************
size_t StrLen(const char *str) {
    if (str==0) return 0;
    return strlen(str);
}

//*****************************************************************************
// Configuration information
void SetN2kPGN126998(tN2kMsg &N2kMsg,
                     const char *ManufacturerInformation,
                     const char *InstallationDescription1,
                     const char *InstallationDescription2,
                     bool UsePgm) {
  size_t TotalLen;
  size_t MaxLen=tN2kMsg::MaxDataLen-6; // Each field has 2 extra bytes
  size_t ManInfoLen;
  size_t InstDesc1Len;
  size_t InstDesc2Len;
  
    if ( UsePgm ) {
      ManInfoLen=ProgmemStrLen(ManufacturerInformation);
      InstDesc1Len=ProgmemStrLen(InstallationDescription1);
      InstDesc2Len=ProgmemStrLen(InstallationDescription2);
    } else {
      ManInfoLen=StrLen(ManufacturerInformation);
      InstDesc1Len=StrLen(InstallationDescription1);
      InstDesc2Len=StrLen(InstallationDescription2);
    }

    if ( ManInfoLen>Max_N2kConfigurationInfoField_len ) ManInfoLen=Max_N2kConfigurationInfoField_len;
    if ( InstDesc1Len>Max_N2kConfigurationInfoField_len ) InstDesc1Len=Max_N2kConfigurationInfoField_len;
    if ( InstDesc2Len>Max_N2kConfigurationInfoField_len ) InstDesc2Len=Max_N2kConfigurationInfoField_len;
  
    TotalLen=0;
    if (TotalLen+ManInfoLen>MaxLen) ManInfoLen=MaxLen-TotalLen;
    TotalLen+=ManInfoLen;
    if (TotalLen+InstDesc1Len>MaxLen) InstDesc1Len=MaxLen-TotalLen;
    TotalLen+=InstDesc1Len;
    if (TotalLen+InstDesc2Len>MaxLen) InstDesc2Len=MaxLen-TotalLen;
    TotalLen+=InstDesc2Len;

    N2kMsg.SetPGN(N2kPGNConfigurationInformation);
    N2kMsg.Priority=6;
    // InstallationDescription1
    N2kMsg.AddByte(InstDesc1Len+2);
    N2kMsg.AddByte(0x01);
    N2kMsg.AddStr(InstallationDescription1,InstDesc1Len,UsePgm);
    
    // InstallationDescription2
    N2kMsg.AddByte(InstDesc2Len+2);
    N2kMsg.AddByte(0x01);
    N2kMsg.AddStr(InstallationDescription2,InstDesc2Len,UsePgm);
    // ManufacturerInformation
    N2kMsg.AddByte(ManInfoLen+2);
    N2kMsg.AddByte(0x01);
    N2kMsg.AddStr(ManufacturerInformation,ManInfoLen,UsePgm);
}

bool ParseN2kPGN126998(const tN2kMsg& N2kMsg,
                       size_t &ManufacturerInformationSize, char *ManufacturerInformation,
                       size_t &InstallationDescription1Size, char *InstallationDescription1,
                       size_t &InstallationDescription2Size, char *InstallationDescription2) {
  if (N2kMsg.PGN!=N2kPGNConfigurationInformation) return false;

  int Index=0;
  return ( N2kMsg.GetVarStr(InstallationDescription1Size,InstallationDescription1,Index) &&
           N2kMsg.GetVarStr(InstallationDescription2Size,InstallationDescription2,Index) &&
           N2kMsg.GetVarStr(ManufacturerInformationSize,ManufacturerInformation,Index) );
}

//*****************************************************************************
// Iso request
void SetN2kPGN59904(tN2kMsg &N2kMsg, uint8_t Destination, unsigned long RequestedPGN) {
    N2kMsg.SetPGN(59904L);
    N2kMsg.Destination=Destination;
    N2kMsg.Priority=6;
    N2kMsg.Add3ByteInt(RequestedPGN);
}

bool ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN) {
  int result=((N2kMsg.DataLen>=3) && (N2kMsg.DataLen<=8));
  RequestedPGN=0;
  if (result) {
    int Index=0;
    RequestedPGN=N2kMsg.Get3ByteUInt(Index);
  }

  return result;
}

//*****************************************************************************
// PGN List (Transmit and Receive)
void SetN2kPGN126464(tN2kMsg &N2kMsg, uint8_t Destination, tN2kPGNList tr, const unsigned long *PGNs) {
  unsigned long PGN;

    N2kMsg.SetPGN(126464L);
    N2kMsg.Destination=Destination;
    N2kMsg.Priority=6;
    N2kMsg.AddByte(tr);

    for (int i=0; (PGN=pgm_read_dword(&PGNs[i]))!=0; i++) {
      N2kMsg.Add3ByteInt(PGN);
    }
}

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
//*****************************************************************************
// Heartbeat
void SetN2kPGN126993(tN2kMsg &N2kMsg, uint32_t timeInterval_ms, uint8_t statusByte) {
	N2kMsg.SetPGN(126993L);
	N2kMsg.Priority=7;
	if ( timeInterval_ms>MaxHeartbeatInterval ) {
	  N2kMsg.Add2ByteUInt(65532);
	} else {
	  N2kMsg.Add2ByteUInt((uint16_t)(timeInterval_ms));
	}
	N2kMsg.AddByte(statusByte);
	N2kMsg.AddByte(0xff); // Reserved
	N2kMsg.Add4ByteUInt(0xffffffff); // Reserved
}
#endif
