/*
NMEA2000.h

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


NMEA2000 device class definition.

With NMEA2000 device class you can send, read and forward messages to NMEA2000 bus.
As default library creates system, which acts like Actisense NGT NMEA2000->PC interface
forwarding all messages from bus to PC. By changing mode to N2km_NodeOnly, one can make
e.g. temperature source device to NMEA2000 bus.

!Note. Each device on NMEA2000 bus should have own address on range 0-253. This class
uses J1939 automatic address claiming (or dynamic addressing). So that you can start
your device with some address set by method SetMode(...). It is also important to set
your device "name" with method SetDeviceInformation(...) so that it would be unique.
If you do not set "name" to unique, you devices changes address on start randomly.
In principle they should still work fine.

It is also good idea to save device address to the EEPROM. In this way if you connect
two of your devices to the bus, they will do the automatic address claiming. If later
save address to the EEPROM and use that on next start, they does not need to change
address anymore. See also method ReadResetAddressChanged().
*/

#ifndef _NMEA2000_H_
#define _NMEA2000_H_

#include "NMEA2000_CompilerDefns.h"
#include "N2kStream.h"
#include "N2kMsg.h"
#include "N2kCANMsg.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
#include "N2kGroupFunction.h"
#endif

#define N2kPGNIsoAddressClaim 60928L
#define N2kPGNProductInformation 126996L
#define N2kPGNConfigurationInformation 126998L

// Document says for leghts 33,40,24,32, but then values
// has not been translated right on devices.
#define Max_N2kModelID_len 32
#define Max_N2kSwCode_len 32
#define Max_N2kModelVersion_len 32
#define Max_N2kModelSerialCode_len 32
// Define length of longest info string above + 1 termination char
#define Max_N2kProductInfoStrLen 33

// I do not know what standard says about max field length, but according to tests NMEAReader crashed with
// lenght >=90. Some device was reported not to work string length over 70.
#define Max_N2kConfigurationInfoField_len 71  // 70 + '/0'


#define Max_N2kMsgBuf_Time 100
#define N2kMessageGroups 2
#define N2kMaxCanBusAddress 251
#define N2kNullCanBusAddress 254

class tNMEA2000
{
public:
  static bool IsProprietaryMessage(unsigned long PGN);
  static void ClearCharBuf(size_t MaxLen, char *buf);
  static void SetCharBuf(const char *str, size_t MaxLen, char *buf);
  static void ClearSetCharBuf(const char *str, size_t MaxLen, char *buf);
  // max and min are not available on all systems, so use own definition.
  template <typename T> static T N2kMax(T a, T b) { return (a>b?a:b); }
  template <typename T> static T N2kMin(T a, T b) { return (a<b?a:b); }

  struct tProductInformation {
      unsigned short N2kVersion;
      unsigned short ProductCode;
      // Note that we reserve one extra char for null termination
      char N2kModelID[Max_N2kModelID_len+1];
      char N2kSwCode[Max_N2kSwCode_len+1];
      char N2kModelVersion[Max_N2kModelVersion_len+1];
      char N2kModelSerialCode[Max_N2kModelSerialCode_len+1];
      unsigned char CertificationLevel;
      unsigned char LoadEquivalency;

      void Set(const char *_ModelSerialCode, // Default="". Max 32 chars. Manufacturer's Model serial code
               unsigned short _ProductCode=0xffff,  // Default=666. Manufacturer's product code
               const char *_ModelID=0, // Default="". Max 33 chars. Manufacturer's  Model ID
               const char *_SwCode=0, // Default="". Max 40 chars. Manufacturer's software version code
               const char *_ModelVersion=0, // Default="". Max 24 chars. Manufacturer's Model version
               unsigned char _LoadEquivalency=0xff,  // Default=1. x * 50 mA
               unsigned short _N2kVersion=0xffff, // Default=2101
               unsigned char _CertificationLevel=0xff // Default=0
              ) {
        N2kVersion=(_N2kVersion!=0xffff?_N2kVersion:2101);
        ProductCode=_ProductCode;
        ClearSetCharBuf(_ModelID,sizeof(N2kModelID),N2kModelID);
        ClearSetCharBuf(_SwCode,sizeof(N2kSwCode),N2kSwCode);
        ClearSetCharBuf(_ModelVersion,sizeof(N2kModelVersion),N2kModelVersion);
        ClearSetCharBuf(_ModelSerialCode,sizeof(N2kModelSerialCode),N2kModelSerialCode);
        CertificationLevel=(_CertificationLevel!=0xff?_CertificationLevel:0);
        LoadEquivalency=(_LoadEquivalency!=0xff?_LoadEquivalency:1);
      }

      void Clear();
      bool IsSame(const tProductInformation &Other);
  };

  class tDeviceInformation {
  protected:
    typedef union {
      uint64_t Name;
      struct {
        uint32_t UnicNumberAndManCode; // ManufacturerCode 11 bits , UniqueNumber 21 bits
        unsigned char DeviceInstance;
        unsigned char DeviceFunction;
        unsigned char DeviceClass;
      // I found document: http://www.novatel.com/assets/Documents/Bulletins/apn050.pdf it says about next fields:
      // The System Instance Field can be utilized to facilitate multiple NMEA 2000 networks on these larger marine platforms.
      // NMEA 2000 devices behind a bridge, router, gateway, or as part of some network segment could all indicate this by use
      // and application of the System Instance Field.
      // DeviceInstance and SystemInstance fields can be now changed by function SetDeviceInformationInstances or
      // by NMEA 2000 group function. Group function handling is build in the library.
        unsigned char IndustryGroupAndSystemInstance; // 4 bits each
      };
    } tUnionDeviceInformation;

    tUnionDeviceInformation DeviceInformation;

  public:
    tDeviceInformation() { DeviceInformation.Name=0; }
    void SetUniqueNumber(uint32_t _UniqueNumber) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0xffe00000) | (_UniqueNumber&0x1fffff); }
    uint32_t GetUniqueNumber() const { return DeviceInformation.UnicNumberAndManCode&0x1fffff; }
    void SetManufacturerCode(uint16_t _ManufacturerCode) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0x1fffff) | (((unsigned long)(_ManufacturerCode&0x7ff))<<21); }
    uint16_t GetManufacturerCode() const { return DeviceInformation.UnicNumberAndManCode>>21; }
    void SetDeviceInstance(unsigned char _DeviceInstance) { DeviceInformation.DeviceInstance=_DeviceInstance; }
    unsigned char GetDeviceInstance() const { return DeviceInformation.DeviceInstance; }
    unsigned char GetDeviceInstanceLower() const { return DeviceInformation.DeviceInstance & 0x07; }
    unsigned char GetDeviceInstanceUpper() const { return (DeviceInformation.DeviceInstance>>3) & 0x1f; }
    void SetDeviceFunction(unsigned char _DeviceFunction) { DeviceInformation.DeviceFunction=_DeviceFunction; }
    unsigned char GetDeviceFunction() const { return DeviceInformation.DeviceFunction; }
    void SetDeviceClass(unsigned char _DeviceClass) { DeviceInformation.DeviceClass=((_DeviceClass&0x7f)<<1); }
    unsigned char GetDeviceClass() const { return DeviceInformation.DeviceClass>>1; }
    void SetIndustryGroup(unsigned char _IndustryGroup) { DeviceInformation.IndustryGroupAndSystemInstance=(DeviceInformation.IndustryGroupAndSystemInstance&0x0f) | (_IndustryGroup<<4) | 0x80; }
    unsigned char GetIndustryGroup() const { return (DeviceInformation.IndustryGroupAndSystemInstance>>4) & 0x07; }
    void SetSystemInstance(unsigned char _SystemInstance) { DeviceInformation.IndustryGroupAndSystemInstance=(DeviceInformation.IndustryGroupAndSystemInstance&0xf0) | (_SystemInstance&0x0f); }
    unsigned char GetSystemInstance() const { return DeviceInformation.IndustryGroupAndSystemInstance&0x0f; }

    uint64_t GetName() const { return DeviceInformation.Name; }
    void SetName(uint64_t _Name) { DeviceInformation.Name=_Name; }
    inline bool IsSame(uint64_t Other) { return GetName()==Other; }
  };

  class tDevice {
    protected:
      uint8_t Source;
      unsigned long CreateTime;
      tDeviceInformation DevI;

    public:
      tDevice(uint64_t _Name, uint8_t _Source=255) { Source=_Source; DevI.SetName(_Name); CreateTime=millis(); }
      virtual ~tDevice() {;}
      uint8_t GetSource() const { return Source; }

      unsigned long GetCreateTime() const { return CreateTime; }

      // Device information
      inline uint64_t GetName() const { return DevI.GetName(); }
      inline bool IsSame(uint64_t Other) { return DevI.IsSame(Other); }
      inline uint32_t GetUniqueNumber() const { return DevI.GetUniqueNumber(); }
      inline uint16_t GetManufacturerCode() const { return DevI.GetManufacturerCode(); }
      inline unsigned char GetDeviceInstance() const { return DevI.GetDeviceInstance(); }
      inline unsigned char GetDeviceInstanceLower() const { return DevI.GetDeviceInstanceLower(); }
      inline unsigned char GetDeviceInstanceUpper() const { return DevI.GetDeviceInstanceUpper(); }
      inline unsigned char GetDeviceFunction() const { return DevI.GetDeviceFunction(); }
      inline unsigned char GetDeviceClass() const { return DevI.GetDeviceClass(); }
      inline unsigned char GetIndustryGroup() const { return DevI.GetIndustryGroup(); }
      inline unsigned char GetSystemInstance() const { return DevI.GetSystemInstance(); }

      // Product information
      virtual unsigned short GetN2kVersion() const=0;
      virtual unsigned short GetProductCode() const=0;
      virtual const char * GetModelID() const=0;
      virtual const char * GetSwCode() const=0;
      virtual const char * GetModelVersion() const=0;
      virtual const char * GetModelSerialCode() const=0;
      virtual unsigned short GetCertificationLevel() const=0;
      virtual unsigned short GetLoadEquivalency() const=0;

      // Configuration information
      virtual const char * GetManufacturerInformation() const { return 0; }
      virtual const char * GetInstallationDescription1() const { return 0; }
      virtual const char * GetInstallationDescription2() const { return 0; }

      virtual const unsigned long * GetTransmitPGNs() const { return 0; }
      virtual const unsigned long * GetReceivePGNs() const { return 0; }
  };

  class tMsgHandler {
    private:
      friend class tNMEA2000;
      tMsgHandler *pNext;
      tNMEA2000 *pNMEA2000;
    protected:
      unsigned long PGN;
      virtual void HandleMsg(const tN2kMsg &N2kMsg)=0;
      tNMEA2000 *GetNMEA2000() { return pNMEA2000; }
    public:
      tMsgHandler(unsigned long _PGN=0, tNMEA2000 *_pNMEA2000=0) {
        PGN=_PGN; pNext=0; pNMEA2000=0;
        if ( _pNMEA2000!=0 ) _pNMEA2000->AttachMsgHandler(this);
      }
      virtual ~tMsgHandler() { if ( pNMEA2000!=0 ) pNMEA2000->DetachMsgHandler(this); }
      inline unsigned long GetPGN() const { return PGN; }
  };

public:
  // Type how to forward messages in listen mode
  typedef enum { fwdt_Actisense, // Forwards messages to output port in Actisense format. Note that some Navigation sw uses this.
                 fwdt_Text // Forwards messages to output port in clear text. This is e.g. for debuging.
                } tForwardType;

  // System mode. Meaning how it acts in NMEA2000 bus.
  typedef enum { N2km_ListenOnly, // Default mode. Listen bus and forwards messages to default port in Actisense format. You can not send any data to the bus.
                 N2km_NodeOnly, // This is for devices, which only sends data to the bus e.g. RPM or temperature monitor. Remember to set right device information first.
                 N2km_ListenAndNode, // In this mode, device can be e.g. temperature monitor and as N2km_ListenOnly.
                 N2km_SendOnly, // Only for message sending. Device will not inform itself to the bus. Messages will not be forwarded to the stream.
                                // By setting message handler, you can still read messages and handle them by yourself.
                 N2km_ListenAndSend // Listen bus and forwards messages to default port in Actisense format. Messages can be send. Device will not inform itself to the bus.
               } tN2kMode;

  // For debugging we have some cases for SendMsg
  typedef enum {dm_None, // Directs data to CAN bus
                dm_ClearText, // Directs sended data to serial as clear text
                dm_Actisense, //  Directs sended data to serial as Actisense format.
                } tDebugMode;

  struct tConfigurationInformation {
      const char *ManufacturerInformation;
      const char *InstallationDescription1;
      const char *InstallationDescription2;
  };

protected:
  class tInternalDevice {
  public:
    uint8_t N2kSource;
    tDeviceInformation DeviceInformation;
    // Product information
    const tProductInformation *ProductInformation;
    tProductInformation *LocalProductInformation;
    char *ManufacturerSerialCode;
    unsigned long PendingIsoAddressClaim;
    unsigned long PendingProductInformation;
    unsigned long PendingConfigurationInformation;
    unsigned long AddressClaimStarted;
    uint8_t AddressClaimEndSource;
    // Transmit and receive PGNs
    const unsigned long *TransmitMessages;
    const unsigned long *ReceiveMessages;
    // Fast packet PGNs sequence counters
    size_t MaxPGNSequenceCounters;
    unsigned long *PGNSequenceCounters;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
      tN2kMsg PendingTPMsg;
      unsigned long NextDTSendTime; // Time, when next data packet can be send on TP broadcast
      uint8_t NextDTSequence;
#endif
#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
    unsigned long HeartbeatInterval;
    unsigned long DefaultHeartbeatInterval;
    unsigned long NextHeartbeatSentTime;
#endif


  public:
    tInternalDevice() {
      N2kSource=0;
      ProductInformation=0; LocalProductInformation=0; ManufacturerSerialCode=0;
      PendingIsoAddressClaim=0; PendingProductInformation=0; PendingConfigurationInformation=0;
      AddressClaimStarted=0; AddressClaimEndSource=N2kMaxCanBusAddress; //GetNextAddressFromBeginning=true;
      TransmitMessages=0; ReceiveMessages=0;
      MaxPGNSequenceCounters=0; PGNSequenceCounters=0;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
      NextDTSendTime=0;
      NextDTSequence=0;
#endif

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
      HeartbeatInterval=60000;
      DefaultHeartbeatInterval=60000;
      NextHeartbeatSentTime=0;
#endif
    }
    void SetPendingIsoAddressClaim(unsigned long FromNow=2) { PendingIsoAddressClaim=millis()+FromNow; }
    bool QueryPendingIsoAddressClaim() { return (PendingIsoAddressClaim?PendingIsoAddressClaim<millis():false); }
    void ClearPendingIsoAddressClaim() { PendingIsoAddressClaim=0; }

    void SetPendingProductInformation() { PendingProductInformation=millis()+187+N2kSource*8; } // Use strange increment to avoid synchronize
    void ClearPendingProductInformation() { PendingProductInformation=0; }
    bool QueryPendingProductInformation() { return (PendingProductInformation?PendingProductInformation<millis():false); }

    void SetPendingConfigurationInformation() { PendingConfigurationInformation=millis()+187+N2kSource*10; } // Use strange increment to avoid synchronize
    void ClearPendingConfigurationInformation() { PendingConfigurationInformation=0; }
    bool QueryPendingConfigurationInformation() { return (PendingConfigurationInformation?PendingConfigurationInformation<millis():false); }
    void UpdateAddressClaimEndSource() {
      AddressClaimEndSource=N2kSource;
      if ( AddressClaimEndSource>0 ) { AddressClaimEndSource--; } else { AddressClaimEndSource=N2kMaxCanBusAddress; }
    }
  };

protected:
  // Forward mode bit settings.
  static const int FwdModeBit_EnableForward        = BIT(0); // If set, forward is enabled
  static const int FwdModeBit_SystemMessages       = BIT(1); // System messages will be forwarded
  static const int FwdModeBit_OnlyKnownMessages    = BIT(2); // Only known messages will be forwarded. System messages will be forwarded according its own bit.
  static const int FwdModeBit_OwnMessages          = BIT(3); // Forward also all messages, what this device will send
  static const int HandleModeBit_OnlyKnownMessages = BIT(4); // Only known messages will be handled.

protected:
    tDebugMode dbMode; // Default dm_None
    tN2kMode N2kMode; // Default N2km_ListenOnly.
    tForwardType ForwardType; // Default fwdt_Actisense.
    unsigned int ForwardMode; // Default all messages - also system and own.
    N2kStream *ForwardStream;
    tMsgHandler *MsgHandlers;

    bool DeviceReady;
    bool AddressChanged;
    bool DeviceInformationChanged;

    // Device information
    tInternalDevice *Devices;
    int DeviceCount;
//    unsigned long N2kSource[Max_N2kDevices];

    // Configuration information
    char *LocalConfigurationInformationData;
    tConfigurationInformation ConfigurationInformation;

    const unsigned long *SingleFrameMessages[N2kMessageGroups];
    const unsigned long *FastPacketMessages[N2kMessageGroups];

    class tCANSendFrame
    {
    public:
      unsigned long id;
      unsigned char len;
      unsigned char buf[8];
      bool wait_sent;

    public:
      void Clear() {id=0; len=0; for (int i=0; i<8; i++) { buf[i]=0; } }
    };

protected:
    // Buffer for received messages.
    tN2kCANMsg *N2kCANMsgBuf;
    uint8_t MaxN2kCANMsgs;

    tCANSendFrame *CANSendFrameBuf;
    uint16_t MaxCANSendFrames;
    uint16_t CANSendFrameBufferWrite;
    uint16_t CANSendFrameBufferRead;
    uint16_t MaxCANReceiveFrames;

    // Handler callbacks
    void (*MsgHandler)(const tN2kMsg &N2kMsg);                  // Normal messages
    bool (*ISORqstHandler)(unsigned long RequestedPGN, unsigned char Requester, int DeviceIndex);                 // 'ISORequest' messages
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
    tN2kGroupFunctionHandler *pGroupFunctionHandlers;
#endif

protected:
    // Virtual functions for different interfaces. Currently there are own classes
    // for Arduino due internal CAN (NMEA2000_due), external MCP2515 SPI CAN bus controller (NMEA2000_mcp),
    // Teensy FlexCAN (NMEA2000_Teensy), NMEA2000_avr for AVR, NMEA2000_mbed for MBED and NMEA2000_socketCAN for e.g. RPi.
    virtual bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true)=0;
    virtual bool CANOpen()=0;
    virtual bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf)=0;
    // This will be called on Open() before any other initialization. Inherit this, if buffers can be set for the driver
    // and you want to change size of library send frame buffer size. See e.g. NMEA2000_teensy.cpp.
    virtual void InitCANFrameBuffers();
#if defined(DEBUG_NMEA2000_ISR)
    virtual void TestISR() {;}
#endif

protected:
    bool SendFrames(); // Sends pending frames
    bool SendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true);
    tCANSendFrame *GetNextFreeCANSendFrame();
    // Currently Product Information and Configuration Information will we pended on ISO request.
    // This is because specially for broadcasted response it may take a while, when higher priority
    // devices sends their response.
    void SendPendingInformation();

protected:
    void InitDevices();
    bool IsInitialized() { return (N2kCANMsgBuf!=0); }
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    void FindFreeCANMsgIndex(unsigned long PGN, unsigned char Source, unsigned char Destination, bool TPMsg, uint8_t &MsgIndex);
#else
    void FindFreeCANMsgIndex(unsigned long PGN, unsigned char Source, unsigned char Destination, uint8_t &MsgIndex);
#endif
    uint8_t SetN2kCANBufMsg(unsigned long canId, unsigned char len, unsigned char *buf);
    bool IsFastPacketPGN(unsigned long PGN);
    bool IsFastPacket(const tN2kMsg &N2kMsg);
    bool CheckKnownMessage(unsigned long PGN, bool &SystemMessage, bool &FastPacket);
    bool HandleReceivedSystemMessage(int MsgIndex);
    void ForwardMessage(const tN2kMsg &N2kMsg);
    void ForwardMessage(const tN2kCANMsg &N2kCanMsg);
    void RespondISORequest(const tN2kMsg &N2kMsg, unsigned long RequestedPGN, int iDev);
    void HandleISORequest(const tN2kMsg &N2kMsg);
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
    void RespondGroupFunction(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev);
    void HandleGroupFunction(const tN2kMsg &N2kMsg);
#endif
    void StartAddressClaim(int iDev);
    void StartAddressClaim();
    bool IsAddressClaimStarted(int iDev);
    void HandleISOAddressClaim(const tN2kMsg &N2kMsg);
    void HandleCommandedAddress(uint64_t CommandedName, unsigned char NewAddress, int iDev);
    void HandleCommandedAddress(const tN2kMsg &N2kMsg);
    void GetNextAddress(int DeviceIndex, bool RestartAtEnd=false);
    bool IsMySource(unsigned char Source);
    int FindSourceDeviceIndex(unsigned char Source);
    int GetSequenceCounter(unsigned long PGN, int iDev);
    size_t GetFastPacketTxPGNCount(int iDev);

    bool ForwardEnabled() const { return ((ForwardMode&FwdModeBit_EnableForward)>0 && (N2kMode!=N2km_SendOnly)); }
    bool ForwardSystemMessages() const { return ((ForwardMode&FwdModeBit_SystemMessages)>0); }
    bool ForwardOnlyKnownMessages() const { return ((ForwardMode&FwdModeBit_OnlyKnownMessages)>0); }
    bool ForwardOwnMessages() const { return ((ForwardMode&FwdModeBit_OwnMessages)>0); }
    bool HandleOnlyKnownMessages() const { return ((ForwardMode&HandleModeBit_OnlyKnownMessages)>0); }

    void RunMessageHandlers(const tN2kMsg &N2kMsg);

    bool HandleReceivedMessage(unsigned char Destination) {
      return (/* HandleMessagesToAnyDestination() */ true ||
              tNMEA2000::IsBroadcast(Destination) ||
              FindSourceDeviceIndex(Destination)>=0);
    }
    bool IsActiveNode() { return (N2kMode==N2km_NodeOnly || N2kMode==N2km_ListenAndNode); }
    bool IsValidDevice(int iDev) const { return (iDev>=0 && iDev<DeviceCount ); }
    bool IsReadyToSend() const {
      return ( (DeviceReady || dbMode!=dm_None) &&
               (N2kMode!=N2km_ListenOnly) &&
               (N2kMode!=N2km_SendOnly) &&
               (N2kMode!=N2km_ListenAndSend)
             );
    }


#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    // Transport protocol handlers
    bool TestHandleTPMessage(unsigned long PGN, unsigned char Source, unsigned char Destination,
                             unsigned char len, unsigned char *buf,
                             uint8_t &MsgIndex);
    bool SendTPCM_BAM(int iDev);
    bool SendTPCM_RTS(int iDev);
    void SendTPCM_CTS(unsigned long PGN, unsigned char Destination, int iDev, unsigned char nPackets, unsigned char NextPacketNumber);
    void SendTPCM_EndAck(unsigned long PGN, unsigned char Destination, int iDev, uint16_t nBytes, unsigned char nPackets);
    void SendTPCM_Abort(unsigned long PGN, unsigned char Destination, int iDev, unsigned char AbortCode);
    bool SendTPDT(int iDev);
    bool HasAllTPDTSent(int iDev);
    bool StartSendTPMessage(const tN2kMsg& msg, int iDev);
    void EndSendTPMessage(int iDev);
    void SendPendingTPMessages();
#endif
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
    void CopyProgmemConfigurationInformationToLocal();
    bool InstallationDescriptionChanged;
#endif
public:
    tNMEA2000();

    // Your device can show multiple devices on the bus. If you define more than on device, call this before any other setting.
    void SetDeviceCount(const uint8_t _DeviceCount);

    // As default there are reservation for 5 messages. If it is not critical to handle all fast packet messages like with N2km_NodeOnly
    // you can set buffer size smaller like 3 or 2 by calling this before Open().
    void SetN2kCANMsgBufSize(const uint8_t _MaxN2kCANMsgs) { if (N2kCANMsgBuf==0) { MaxN2kCANMsgs=_MaxN2kCANMsgs; }; }

    // When sending long messages like ProductInformation or GNSS data, there may not be enough buffers for successfully send data
    // This depends of your hw and device source. Device source has effect due to priority of getting sending slot. If your data is
    // critical, use buffer size, which is large enough (default 40 frames).
    // So e.g. Product information takes totally 134 bytes. This needs 20 frames. If you also send GNSS 47 bytes=7 frames.
    // If you want to be sure that both will be sent on any situation, you need at least 27 frame buffer size.
    // If you use this function, call it once before Open() and before any device related function like SetProductInformation.
    virtual void SetN2kCANSendFrameBufSize(const uint16_t _MaxCANSendFrames) { if ( !IsInitialized() ) { MaxCANSendFrames=_MaxCANSendFrames; }; }

    // Some CAN drivers allows interrupted receive frame buffering. You can set receive buffer size with this function.
    // If you use this function, call it once before Open();
    virtual void SetN2kCANReceiveFrameBufSize(const uint16_t _MaxCANReceiveFrames) { if ( !IsInitialized() ) MaxCANReceiveFrames=_MaxCANReceiveFrames; }

    // Define your product information. Defaults will be set on initialization.
    // For keeping defaults use 0xffff/0xff for int/char values and nul ptr for pointers.
    // LoadEquivalency is multiplication of 50 mA, what your device will take power from
    // N2k-bus. E.g. for Arduino only it can be 1 (=50mA). If your device does not take
    // power from bus, set this to 0.
    // Note that serial code has length of 32 so just long enough to carry GUID.
    void SetProductInformation(const char *_ModelSerialCode, // Default="00000001". Max 32 chars. Manufacturer's Model serial code
                               unsigned short _ProductCode=0xffff,  // Default=666. Manufacturer's product code
                               const char *_ModelID=0, // Default="Arduino N2k->PC". Max 33 chars. Manufacturer's  Model ID
                               const char *_SwCode=0, // Default="1.0.0.0". Max 40 chars. Manufacturer's software version code
                               const char *_ModelVersion=0, // Default="1.0.0". Max 24 chars. Manufacturer's Model version
                               unsigned char _LoadEquivalency=0xff,  // Default=1. x * 50 mA
                               unsigned short _N2kVersion=0xffff, // Default=2101
                               unsigned char _CertificationLevel=0xff, // Default=1
                               int iDev=0
                               );
    // Call this if you want to save RAM and you have defined tProductInformation to PROGMEM as in example BatteryMonitor.ino
    // Note that I have not yet found a way to test is pointer in PROGMEM or not, so this does not work, if you define
    // tProductInformation to RAM.
    void SetProductInformation(const tProductInformation *_ProductInformation, int iDev=0);

    // Configuration information is just some extra information about device installation and manufacturer. Some
    // MFD shows it, some does not. NMEA Reader can show configuration information.
    // InstallationDescription1 and InstallationDescription2 can be changed as default during runtime
    // by NMEA 2000 group function commands. That can be done e.g. with NMEA Reader.    // You can disable configuration information by calling SetProgmemConfigurationInformation(0);
    void SetConfigurationInformation(const char *ManufacturerInformation,
                                     const char *InstallationDescription1=0,
                                     const char *InstallationDescription2=0);

    // Call this if you want to save RAM and you have defined configuration information strings to PROGMEM as in example BatteryMonitor.ino
    void SetProgmemConfigurationInformation(const char *ManufacturerInformation,
                                     const char *InstallationDescription1=0,
                                     const char *InstallationDescription2=0);

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
    bool IsTxPGN(unsigned long PGN, int iDev=0);
    const tNMEA2000::tProductInformation * GetProductInformation(int iDev, bool &IsProgMem) const;
    unsigned short GetN2kVersion(int iDev=0) const;
    unsigned short GetProductCode(int iDev=0) const;
    void GetModelID(char *buf, size_t max_len, int iDev=0) const;
    void GetSwCode(char *buf, size_t max_len, int iDev=0) const;
    void GetModelVersion(char *buf, size_t max_len, int iDev=0) const;
    void GetModelSerialCode(char *buf, size_t max_len, int iDev=0) const;
    unsigned char GetCertificationLevel(int iDev=0) const;
    unsigned char GetLoadEquivalency(int iDev=0) const;
    void SetInstallationDescription1(const char *InstallationDescription1);
    void SetInstallationDescription2(const char *InstallationDescription2);
    void GetInstallationDescription1(char *buf, size_t max_len);
    void GetInstallationDescription2(char *buf, size_t max_len);
    void GetManufacturerInformation(char *buf, size_t max_len);
    bool ReadResetInstallationDescriptionChanged();
#endif

    // Call these if you wish to override the default message packets supported.  Pointers must be in PROGMEM
    void SetSingleFrameMessages(const unsigned long *_SingleFrameMessages);
    void SetFastPacketMessages (const unsigned long *_FastPacketMessages);
    // Call these if you wish to add own list of supported message packets.  Pointers must be in PROGMEM
    // Note that currently subsequent calls will override previously set list.
    void ExtendSingleFrameMessages(const unsigned long *_SingleFrameMessages);
    void ExtendFastPacketMessages (const unsigned long *_FastPacketMessages);
    // Define information about PGNs, what your system can handle.  Pointers must be in PROGMEM
    // As default for request to PGN list library responds with default messages it handles internally.
    // With these messages you can extent that list. See example TemperatureMonitor
    void ExtendTransmitMessages(const unsigned long *_SingleFrameMessages, int iDev=0);
    void ExtendReceiveMessages(const unsigned long *_FastPacketMessages, int iDev=0);

    // Set default device information.
    // For keeping defaults use 0xffff/0xff for int/char values and nul ptr for pointers.
    // Note that ManufacturerCode and UniqueNumber should give unic result on any network.
    // I just decided to use number below for ManufacturerCode as Open Source devices - this is not any number given by NMEA.
    void SetDeviceInformation(unsigned long _UniqueNumber, // Default=1. 21 bit resolution, max 2097151. Each device from same manufacturer should have unique number.
                              unsigned char _DeviceFunction=0xff, // Default=130, PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              unsigned char _DeviceClass=0xff, // Default=25, Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              uint16_t _ManufacturerCode=0xffff,  // Default=2046. Maximum 2046. See the list of codes on http://www.nmea.org/Assets/20140409%20nmea%202000%20registration%20list.pdf
                              unsigned char _IndustryGroup=4,  // Default=4, Marine.
                              int iDev=0
                              );
    void SetDeviceInformationInstances(
                              uint8_t _DeviceInstanceLower=0xff, // 0xff means no change
                              uint8_t _DeviceInstanceUpper=0xff,
                              uint8_t _SystemInstance=0xff,
                              int iDev=0
                              );
    const tDeviceInformation GetDeviceInformation(int iDev=0) { if (iDev<0 || iDev>=DeviceCount) return tDeviceInformation(); return Devices[iDev].DeviceInformation; }

    // Class handles automatically address claiming and tell to the bus about itself.
    void SendIsoAddressClaim(unsigned char Destination=0xff, int DeviceIndex=0, unsigned long delay=0);
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    bool SendProductInformation(unsigned char Destination, int DeviceIndex, bool UseTP);
    bool SendConfigurationInformation(unsigned char Destination, int DeviceIndex, bool UseTP);
    void SendTxPGNList(unsigned char Destination, int DeviceIndex, bool UseTP=false);
    void SendRxPGNList(unsigned char Destination, int DeviceIndex, bool UseTP=false);
#else
    void SendTxPGNList(unsigned char Destination, int DeviceIndex);
    void SendRxPGNList(unsigned char Destination, int DeviceIndex);
#endif
    bool SendProductInformation(int DeviceIndex=0);
    bool SendConfigurationInformation(int DeviceIndex=0);

#if !defined(N2K_NO_HEARTBEAT_SUPPORT)
    // According to document https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf
    // all NMEA devices shall transmit heartbeat PGN 126993.
    // With this function you can set transmission interval in ms (range 1000-655320 ms, default 60000). Set <1000 to disable it.
    // You can temporaly change interval by setting SetAsDefault parameter to false. Then you can restore default interval
    // with interval parameter value 0xfffffffe
    void SetHeartbeatInterval(unsigned long interval, bool SetAsDefault=true, int iDev=-1);
    // Heartbeat interval may be changed by e.g. MFD by group function. I have not yet found should changed value be saved
    // for next startup or not.
    unsigned long GetHeartbeatInterval(int iDev=0) { if (iDev<0 || iDev>=DeviceCount) return 60000; return Devices[iDev].HeartbeatInterval; }
    // Send heartbeat for specific device.
    void SendHeartbeat(int iDev);
    // Library will automatically send heartbeat, if interval is >0. You can also manually send it any time or force sent, if interval=0;
    void SendHeartbeat(bool force=false);
#endif

    // Set this before open. You can not change mode after Open().
    // Note that other than N2km_ListenOnly modes will automatically start initialization and address claim procedure.
    // You have to call ParseMessages() periodically to handle these procedures.
    // If you know your system, define source something other address you allready have on your bus.
    void SetMode(tN2kMode _N2kMode, uint8_t _N2kSource=15);

    // Set type how messages will be forwarded in listen mode. Defult is fwdt_Actisense
    void SetForwardType(tForwardType fwdType) { ForwardType=fwdType; }

    // Set the stream, where messages will be forwarded in listen mode.
    void SetForwardStream(N2kStream* _stream) { ForwardStream=_stream; }

    // You can call this. It will be called anyway automatically by ParseMessages();
    // Note that after Open() you should start loop ParseMessages() without delays.
    bool Open();

    // This is preliminary function for e.g. battery powered or devices, which may
    // go to sleep or of the bus in any way. Function is under development.
    void Restart();

    // Generate N2k message e.g. by using N2kMessages.h and simply send it to the bus.
    bool SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex=0);

    // Call this periodically to handle N2k messages. Note that even if you only send e.g.
    // temperature to the bus, you should call this so the code will automatically inform
    // about itselt to others. Take care that your loop to call ParseMessages() does not have
    // long delays. 
    void ParseMessages();

    // Set the message handler for incoming N2kMessages.
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg));             // Old style - callback function pointer
    void AttachMsgHandler(tMsgHandler *_MsgHandler);
    void DetachMsgHandler(tMsgHandler *_MsgHandler);
    void SetISORqstHandler(bool(*ISORequestHandler)(unsigned long RequestedPGN, unsigned char Requester, int DeviceIndex));           // ISORequest messages
#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)
    void RemoveGroupFunctionHandler(tN2kGroupFunctionHandler *pGroupFunctionHandler);
    void AddGroupFunctionHandler(tN2kGroupFunctionHandler *pGroupFunctionHandler);
#endif
    // Read address for current device.
    // Multidevice support is under construction.
    unsigned char GetN2kSource(int DeviceIndex=0) const { if (DeviceIndex>=0 && DeviceIndex<DeviceCount) return Devices[DeviceIndex].N2kSource; return Devices[0].N2kSource; }
    // Set source for the given device. Has to be called:
    // - after SetMode
    // - before Open
    void SetN2kSource(unsigned char _iAddr, int _iDev=0);

    // You can check has this device changed its address. If yes, it is mandatory to
    // save changed address to e.g. EEPROM and use that on next start.
    // When you call this, AddressChanged will be reset. Anyway, if system for
    // some reason needs to change its address again, AddressChanged will be set.
    // So you can e.g. in every 10 min check has address changed and if it has, save it.
    bool ReadResetAddressChanged();
    // This is like ReadResetAddressChanged, but informs has DeviceInstances or SystemInstance changed.
    // It is also mandatory to save changed info to the e.g. EEPROM ReadResetAddressChanged and use that on next start.
    bool ReadResetDeviceInformationChanged();

    // Control how messages will be forwarded to stream in listen mode
    void EnableForward(bool v=true) {
        if (v) { ForwardMode |= FwdModeBit_EnableForward;  } else { ForwardMode &= ~FwdModeBit_EnableForward; }
    }
    void SetForwardSystemMessages(bool v=true) {
        if (v) { ForwardMode |= FwdModeBit_SystemMessages;  } else { ForwardMode &= ~FwdModeBit_SystemMessages; }
      }
    void SetForwardOnlyKnownMessages(bool v=true) {
        if (v) { ForwardMode |= FwdModeBit_OnlyKnownMessages;  } else { ForwardMode &= ~FwdModeBit_OnlyKnownMessages; }
      }
    void SetForwardOwnMessages(bool v=true) {
        if (v) { ForwardMode |= FwdModeBit_OwnMessages;  } else { ForwardMode &= ~FwdModeBit_OwnMessages; }
      }
    void SetHandleOnlyKnownMessages(bool v=true) {
        if (v) { ForwardMode |= HandleModeBit_OnlyKnownMessages;  } else { ForwardMode &= ~HandleModeBit_OnlyKnownMessages; }
      }

    void SetDebugMode(tDebugMode _dbMode);

    static bool IsBroadcast(unsigned char Source) { return Source==0xff; }

};

//*****************************************************************************
// ISO Acknowledgement
void SetN2kPGN59392(tN2kMsg &N2kMsg, unsigned char Control, unsigned char GroupFunction, unsigned long PGN);
inline void SetN2kPGNISOAcknowledgement(tN2kMsg &N2kMsg, unsigned char Control, unsigned char GroupFunction, unsigned long PGN) {
  SetN2kPGN59392(N2kMsg,Control,GroupFunction,PGN);
}

//*****************************************************************************
// ISO Address Claim
void SetN2kPGN60928(tN2kMsg &N2kMsg, unsigned long UniqueNumber, int ManufacturerCode,
                   unsigned char DeviceFunction, unsigned char DeviceClass,
                   unsigned char DeviceInstance=0, unsigned char SystemInstance=0, unsigned char IndustryGroup=4
                   );

void SetN2kPGN60928(tN2kMsg &N2kMsg, uint64_t Name);

inline void SetN2kISOAddressClaim(tN2kMsg &N2kMsg, unsigned long UniqueNumber, int ManufacturerCode,
                   unsigned char DeviceFunction, unsigned char DeviceClass,
                   unsigned char DeviceInstance=0, unsigned char SystemInstance=0, unsigned char IndustryGroup=4
                   ) {
  SetN2kPGN60928(N2kMsg, UniqueNumber, ManufacturerCode, DeviceFunction, DeviceClass,
                 DeviceInstance, SystemInstance, IndustryGroup);
}

inline void SetN2kISOAddressClaim(tN2kMsg &N2kMsg, uint64_t Name) {
  SetN2kPGN60928(N2kMsg, Name);
}

//*****************************************************************************
// Product information
void SetN2kPGN126996(tN2kMsg &N2kMsg, unsigned int N2kVersion, unsigned int ProductCode,
                     const char *ModelID, const char *SwCode,
                     const char *ModelVersion, const char *ModelSerialCode,
                     unsigned char CertificationLevel=1, unsigned char LoadEquivalency=1);

inline void SetN2kProductInformation(tN2kMsg &N2kMsg, unsigned int N2kVersion, unsigned int ProductCode,
                     const char *ModelID, const char *SwCode,
                     const char *ModelVersion, const char *ModelSerialCode,
                     unsigned char CertificationLevel=1, unsigned char LoadEquivalency=1) {
  SetN2kPGN126996(N2kMsg,N2kVersion,ProductCode,
                  ModelID,SwCode,ModelVersion,ModelSerialCode,
                  CertificationLevel,LoadEquivalency);
}

bool ParseN2kPGN126996(const tN2kMsg& N2kMsg, unsigned short &N2kVersion, unsigned short &ProductCode,
                     int ModelIDSize, char *ModelID, int SwCodeSize, char *SwCode,
                     int ModelVersionSize, char *, int ModelSerialCodeSize, char *ModelSerialCode,
                     unsigned char &CertificationLevel, unsigned char &LoadEquivalency);

//*****************************************************************************
// Configuration information
void SetN2kPGN126998(tN2kMsg &N2kMsg,
                     const char *ManufacturerInformation,
                     const char *InstallationDescription1=0,
                     const char *InstallationDescription2=0,
                     bool UsePgm=false);

inline void SetN2kConfigurationInformation(tN2kMsg &N2kMsg,
                     const char *ManufacturerInformation,
                     const char *InstallationDescription1=0,
                     const char *InstallationDescription2=0,
                     bool UsePgm=false) {
  SetN2kPGN126998(N2kMsg,
                  ManufacturerInformation,
                  InstallationDescription1,
                  InstallationDescription2,
                  UsePgm);
}

bool ParseN2kPGN126998(const tN2kMsg& N2kMsg,
                       size_t &ManufacturerInformationSize, char *ManufacturerInformation,
                       size_t &InstallationDescription1Size, char *InstallationDescription1,
                       size_t &InstallationDescription2Size, char *InstallationDescription2);

//*****************************************************************************
// ISO request
void SetN2kPGN59904(tN2kMsg &N2kMsg, uint8_t Destination, unsigned long RequestedPGN);

inline void SetN2kPGNISORequest(tN2kMsg &N2kMsg, uint8_t Destination, unsigned long RequestedPGN) {
  SetN2kPGN59904(N2kMsg,Destination,RequestedPGN);
}

bool ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN);

inline bool ParseN2kPGNISORequest(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN) {
  return ParseN2kPGN59904(N2kMsg, RequestedPGN);
}

enum tN2kPGNList {N2kpgnl_transmit=0, N2kpgnl_receive=1 };

//*****************************************************************************
// PGN List (Transmit and Receive)
// List of PGNs must be null terminated and
// defined as PROGMEM e.g. const unsigned long TransmitMessages[] PROGMEM={130310L,0};
void SetN2kPGN126464(tN2kMsg &N2kMsg, uint8_t Destination, tN2kPGNList tr, const unsigned long *PGNs);

inline void SetN2kPGNTransmitList(tN2kMsg &N2kMsg, uint8_t Destination, const unsigned long *PGNs) {
  SetN2kPGN126464(N2kMsg,Destination,N2kpgnl_transmit,PGNs);
}

//*****************************************************************************
// Heartbeat
// Input:
//  - time interval in msec (0.01 - 655.32s )
//  - status byte - set to 0x00
//			/ Output:
//  - N2kMsg NMEA2000 message ready to be send.
void SetN2kPGN126993(tN2kMsg &N2kMsg, uint32_t timeInterval_ms, uint8_t statusByte);

inline void SetHeartbeat(tN2kMsg &N2kMsg, uint32_t timeInterval_ms, uint8_t statusByte) {
	SetN2kPGN126993(N2kMsg, timeInterval_ms, statusByte);
}

#endif
