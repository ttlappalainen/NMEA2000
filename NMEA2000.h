/*
NMEA2000.h

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

#include "N2kStream.h"
#include "N2kMsg.h"
#include "N2kCANMsg.h"

// Documenta says for leghts 33,40,24,32, but then values
// has not been translated right on devices.
#define Max_N2kModelID_len 32
#define Max_N2kSwCode_len 32
#define Max_N2kModelVersion_len 32
#define Max_N2kModelSerialCode_len 32

//#define Max_N2kSingleFrameMessages_len 50
//#define Max_N2kFastPacketMessages_len 50

#define Max_N2kDevices 1

#define Max_N2kMsgBuf_Time 100
#define N2kMessageGroups 2

class tDeviceInformation {
protected:
  typedef union {
    uint64_t Name;
    struct {
      unsigned long UnicNumberAndManCode; // ManufacturerCode 11 bits , UniqueNumber 21 bits
      unsigned char DeviceInstance;
      unsigned char DeviceFunction;
      unsigned char DeviceClass;
    // I found document: http://www.novatel.com/assets/Documents/Bulletins/apn050.pdf it says about next fields:
    // The System Instance Field can be utilized to facilitate multiple NMEA 2000 networks on these larger marine platforms. NMEA 2000 devices behind a bridge, router, gateway, or as part of some network segment could all indicate this by use and application of the System Instance Field.
    // There should be a way to program these fields by console. Maybe in future.
      unsigned char IndustryGroupAndSystemInstance; // 4 bits each
    };
  } tUnionDeviceInformation;

  tUnionDeviceInformation DeviceInformation;

public:
  uint8_t N2kSource;
  unsigned long PendingProductInformation;
  unsigned long PendingConfigurationInformation;

public:
  tDeviceInformation() { DeviceInformation.Name=0; N2kSource=0; PendingProductInformation=0; PendingConfigurationInformation=0; }
  void SetUniqueNumber(unsigned long _UniqueNumber) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0xffe00000) | (_UniqueNumber&0x1fffff); }
  unsigned long GetUniqueNumber() { return DeviceInformation.UnicNumberAndManCode&0x1fffff; }
  void SetManufacturerCode(uint16_t _ManufacturerCode) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0x1fffff) | (((unsigned long)(_ManufacturerCode&0x7ff))<<21); }
  uint16_t GetManufacturerCode() { return DeviceInformation.UnicNumberAndManCode>>21; }
  void SetDeviceInstance(unsigned char _DeviceInstance) { DeviceInformation.DeviceInstance=_DeviceInstance; }
  unsigned char GetDeviceInstance() { return DeviceInformation.DeviceInstance; }
  void SetDeviceFunction(unsigned char _DeviceFunction) { DeviceInformation.DeviceFunction=_DeviceFunction; }
  unsigned char GetDeviceFunction() { return DeviceInformation.DeviceFunction; }
  void SetDeviceClass(unsigned char _DeviceClass) { DeviceInformation.DeviceClass=((_DeviceClass&0x7f)<<1); }
  unsigned char GetDeviceClass() { return DeviceInformation.DeviceClass>>1; }
  void SetIndustryGroup(unsigned char _IndustryGroup) { DeviceInformation.IndustryGroupAndSystemInstance=(DeviceInformation.IndustryGroupAndSystemInstance&0x0f) | (_IndustryGroup<<4); }
  unsigned char GetIndustryGroup() { return DeviceInformation.IndustryGroupAndSystemInstance>>4; }
  void SetSystemInstance(unsigned char _SystemInstance) { DeviceInformation.IndustryGroupAndSystemInstance=(DeviceInformation.IndustryGroupAndSystemInstance&0xf0) | (_SystemInstance&0x0f); }
  unsigned char GetSystemInstance() { return DeviceInformation.IndustryGroupAndSystemInstance&0x0f; }

  uint64_t GetName()  { return DeviceInformation.Name; }
  void SetPendingProductInformation() { PendingProductInformation=millis()+N2kSource*8; }
  void ClearPendingProductInformation() { PendingProductInformation=0; }
  bool QueryPendingProductInformation() { return (PendingProductInformation?PendingProductInformation<millis():false); }

  void SetPendingPendingConfigurationInformation() { PendingConfigurationInformation=millis()+N2kSource*10; }
  void ClearPendingConfigurationInformation() { PendingConfigurationInformation=0; }
  bool QueryPendingConfigurationInformation() { return (PendingConfigurationInformation?PendingConfigurationInformation<millis():false); }
};

struct tProductInformation {
    unsigned short N2kVersion;
    unsigned short ProductCode;
    // Note that we reserve one extra char for null termination
    char N2kModelID[Max_N2kModelID_len+1];
    char N2kSwCode[Max_N2kSwCode_len+1];
    char N2kModelVersion[Max_N2kModelVersion_len+1];
    char N2kModelSerialCode[Max_N2kModelSerialCode_len+1];
    unsigned char SertificationLevel;
    unsigned char LoadEquivalency;
};

class tNMEA2000
{
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


  struct tProgmemConfigurationInformation {
      const char *ManufacturerInformation;
      const char *InstallationDescription1;
      const char *InstallationDescription2;
  };

  struct tConfigurationInformation {
      char *ManufacturerInformation;
      char *InstallationDescription1;
      char *InstallationDescription2;
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

    bool DeviceReady;
    unsigned long AddressClaimStarted;
    bool AddressChanged;

    // Device information
    tDeviceInformation DeviceInformation[Max_N2kDevices];
    int DeviceCount;
//    unsigned long N2kSource[Max_N2kDevices];

    // Product information
    const tProductInformation *ProductInformation;
    tProductInformation *LocalProductInformation;

    // Configuration information
    const tProgmemConfigurationInformation *ConfigurationInformation;
    tConfigurationInformation *LocalConfigurationInformation;

    const unsigned long *SingleFrameMessages[N2kMessageGroups];
    const unsigned long *FastPacketMessages[N2kMessageGroups];
    
    // Transmit and receive PGNs
    const unsigned long *TransmitMessages;
    const unsigned long *ReceiveMessages;

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
    unsigned char MaxN2kCANMsgs;

    tCANSendFrame *CANSendFrameBuf;
    uint16_t MaxCANSendFrames;
    uint16_t CANSendFrameBufferWrite;
    uint16_t CANSendFrameBufferRead;

    // Handler callbacks
    void (*MsgHandler)(const tN2kMsg &N2kMsg);                  // Normal messages
    bool (*ISORqstHandler)(unsigned long RequestedPGN, unsigned char Requester, int DeviceIndex);                 // 'ISORequest' messages


protected:
    // Virtual functions for different interfaces. Currently there are own classes
    // for Arduino due internal CAN (NMEA2000_due) and external MCP2515 SPI CAN bus controller (NMEA2000_mcp)
    virtual bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true)=0;
    virtual bool CANOpen()=0;
    virtual bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf)=0;

protected:
    bool SendFrames(); // Sends pending frames
    bool SendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true);
    tCANSendFrame *GetNextFreeCANSendFrame();
    // Currently Product Information and Configuration Information will we pended on ISO request.
    // This is because specially for broadcasted response it may take a while, when higher priority
    // devices sends their response.
    void SendPendingInformation();

protected:
    int SetN2kCANBufMsg(unsigned long canId, unsigned char len, unsigned char *buf);
    bool IsFastPacket(unsigned long PGN);
    bool CheckKnownMessage(unsigned long PGN, bool &SystemMessage, bool &FastPacket);
    bool HandleReceivedSystemMessage(int MsgIndex);
    void ForwardMessage(const tN2kMsg &N2kMsg);
    void ForwardMessage(const tN2kCANMsg &N2kCanMsg);
    void HandlePGNListRequest(unsigned char Destination, int DeviceIndex);
    void RespondISORequest(const tN2kMsg &N2kMsg, unsigned long RequestedPGN, int iDev);
    void HandleISORequest(const tN2kMsg &N2kMsg);
    void HandleISOAddressClaim(const tN2kMsg &N2kMsg);
    void HandleCommandedAddress(const tN2kMsg &N2kMsg);
    void GetNextAddress(int DeviceIndex);
    bool IsMySource(unsigned char Source);
    int FindSourceDeviceIndex(unsigned char Source);

    bool ForwardEnabled() const { return ((ForwardMode&FwdModeBit_EnableForward)>0 && (N2kMode!=N2km_SendOnly)); }
    bool ForwardSystemMessages() const { return ((ForwardMode&FwdModeBit_SystemMessages)>0); }
    bool ForwardOnlyKnownMessages() const { return ((ForwardMode&FwdModeBit_OnlyKnownMessages)>0); }
    bool ForwardOwnMessages() const { return ((ForwardMode&FwdModeBit_OwnMessages)>0); }
    bool HandleOnlyKnownMessages() const { return ((ForwardMode&HandleModeBit_OnlyKnownMessages)>0); }
public:
    tNMEA2000();

    // As default there are reservation for 5 messages. If it is not critical to handle all fast packet messages like with N2km_NodeOnly
    // you can set buffer size smaller like 3 or 2 by calling this before open.
    void SetN2kCANMsgBufSize(const unsigned char _MaxN2kCANMsgs) { if (N2kCANMsgBuf==0) { MaxN2kCANMsgs=_MaxN2kCANMsgs; }; }
    // When sending long messages like ProductInformation or GNSS data, there may not be enough buffers for successfully send data
    // This depends of your hw and device source. Device source has effect due to priority of getting sending slot. If your data is
    // critical, use buffer size, which is large enough (default 40 frames).
    // So e.g. Product information takes totally 134 bytes. This needs 20 frames. If you also send GNSS 47 bytes=7 frames.
    // If you want to be sure that both will be sent on any situation, you need at least 27 frame buffer size.
    void SetN2kCANSendFrameBufSize(const uint16_t _MaxCANSendFrames) { if (CANSendFrameBuf==0) { MaxCANSendFrames=_MaxCANSendFrames; }; }

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
                               unsigned short _N2kVersion=0xffff, // Default=1300
                               unsigned char _SertificationLevel=0xff // Default=1
                               );
    // Call this if you want to save RAM and you have defined tProductInformation to PROGMEM as in example BatteryMonitor.ino
    // Note that I have not yet found a way to test is pointer in PROGMEM or not, so this does not work, if you define
    // tProductInformation to RAM.
    void SetProductInformation(const tProductInformation *_ProductInformation);

    // Configuration information is just some extra information about device and manufacturer. Some
    // MFD shows it, some does not. NMEA Reader can show configuration information.
    // You can disable configuration information by calling SetProgmemConfigurationInformation(0);
    void SetConfigurationInformation(const char *ManufacturerInformation,
                                     const char *InstallationDescription1=0,
                                     const char *InstallationDescription2=0);

    // Call this if you want to save RAM and you have defined tConfigurationInformation to PROGMEM as in example BatteryMonitor.ino
    // Note that I have not yet found a way to test is pointer in PROGMEM or not, so this does not work, if you define
    // tProgmemConfigurationInformation to RAM.
    void SetProgmemConfigurationInformation(const tProgmemConfigurationInformation *_ConfigurationInformation);

    // Call these if you wish to override the default message packets supported.  Pointers must be in PROGMEM
    void SetSingleFrameMessages(const unsigned long *_SingleFrameMessages);
    void SetFastPacketMessages (const unsigned long *_FastPacketMessages);
    // Call these if you wish to add own list of supported message packets.  Pointers must be in PROGMEM
    // Note that currently subsequent calls will override previously set list.
    void ExtendSingleFrameMessages(const unsigned long *_SingleFrameMessages);
    void ExtendFastPacketMessages (const unsigned long *_FastPacketMessages);
    // Define information about PGNs, what your system can handle.  Pointers must be in PROGMEM
    // As default for request to PGN list library responds with default messages it handles intenally.
    // With these messages you can extent that list. See example TemperatureMonitor
    void ExtendTransmitMessages(const unsigned long *_SingleFrameMessages);
    void ExtendReceiveMessages (const unsigned long *_FastPacketMessages);

    // Set default device information.
    // For keeping defaults use 0xffff/0xff for int/char values and nul ptr for pointers.
    // Note that ManufacturerCode and UniqueNumber should give unic result on any network.
    // I just decided to use number below for ManufacturerCode as Open Source devices - this is not any number given by NMEA.
    void SetDeviceInformation(unsigned long _UniqueNumber, // Default=1. 21 bit resolution, max 2097151. Each device from same manufacturer should have unique number.
                              unsigned char _DeviceFunction=0xff, // Default=130, PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              unsigned char _DeviceClass=0xff, // Default=25, Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              uint16_t _ManufacturerCode=0xffff,  // Default=2046. Maximum 2046. See the list of codes on http://www.nmea.org/Assets/20140409%20nmea%202000%20registration%20list.pdf
                              unsigned char _IndustryGroup=4  // Default=4, Marine.
                              );

    // ToDo:
    // If your device has several functions, it should have own bus address for each.
    // Note that there is also in class 25 function code 132, which e.g. Alba Combi uses and it sends several different
    // type of data. So I am not sure yet how important it is to use own addresses.
    // Returns Device index to be used on SendMsg. -1, if no more devices can be allocated.
    //int AddDeviceFunction(unsigned char _DeviceFunction,
    //                       unsigned char _DeviceClass,
    //                       unsigned long _N2kSource=0xff
    //                       );

    // Class handles automatically address claiming and tell to the bus about itself.
    void SendIsoAddressClaim(unsigned char Destination=0xff, int DeviceIndex=0);
    bool SendProductInformation(int DeviceIndex=0);
    bool SendConfigurationInformation(int DeviceIndex=0);

    // Set this before open. You can not change mode after Open().
    // Note that other than N2km_ListenOnly modes will automatically start initialization and address claim procedure.
    // You have to call ParseMessages() periodically to handle these procedures.
    // If you know your system, define source something other address you allready have on your bus.
    void SetMode(tN2kMode _N2kMode, unsigned long _N2kSource=15);

    // Set type how messages will be forwarded in listen mode. Defult is fwdt_Actisense
    void SetForwardType(tForwardType fwdType) { ForwardType=fwdType; }

    // Set the stream, where messages will be forwarded in listen mode.
    void SetForwardStream(N2kStream* _stream) { ForwardStream=_stream; }

    // You can call this. It will be called anyway automatically by ParseMessages();
    bool Open();

    // Generate N2k message e.g. by using N2kMessages.h and simply send it to the bus.
    bool SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex=0);

    // Call this periodically to handle N2k messages. Note that even if you only send e.g.
    // temperature to the bus, you should call this so the code will automatically inform
    // abot itselt to others.
    void ParseMessages();

    // Set the message handler for incoming N2kMessages.
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg));             // Normal messages
    void SetISORqstHandler(bool(*ISORequestHandler)(unsigned long RequestedPGN, unsigned char Requester, int DeviceIndex));           // ISORequest messages


    // Read address for current device.
    // Multidevice support is under construction.
    unsigned char GetN2kSource(int DeviceIndex=0) const { if (DeviceIndex>=0 && DeviceIndex<DeviceCount) return DeviceInformation[DeviceIndex].N2kSource; return DeviceInformation[DeviceCount-1].N2kSource; }

    // You can check has this device changed its address. If yes, it is prefeable to
    // save changed address to EEPROM and on next start use that.
    // When you call this, AddressChanged will be reset. Anyway, if system for
    // some reason needs to change its address again, AddressChanged will be set.
    // So you can e.g. in every 10 min check has address changed and if it has, save it.
    bool ReadResetAddressChanged();

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
                     unsigned char SertificationLevel=1, unsigned char LoadEquivalency=1);

inline void SetN2kProductInformation(tN2kMsg &N2kMsg, unsigned int N2kVersion, unsigned int ProductCode,
                     const char *ModelID, const char *SwCode,
                     const char *ModelVersion, const char *ModelSerialCode,
                     unsigned char SertificationLevel=1, unsigned char LoadEquivalency=1) {
  SetN2kPGN126996(N2kMsg,N2kVersion,ProductCode,
                  ModelID,SwCode,ModelVersion,ModelSerialCode,
                  SertificationLevel,LoadEquivalency);
}

//*****************************************************************************
// Configuration information
void SetN2kPGN126998(tN2kMsg &N2kMsg,
                     const char *ManufacturerInformation,
                     const char *InstallationDescription1=0,
                     const char *InstallationDescription2=0);

inline void SetN2kConfigurationInformation(tN2kMsg &N2kMsg,
                     const char *ManufacturerInformation,
                     const char *InstallationDescription1=0,
                     const char *InstallationDescription2=0) {
  SetN2kPGN126998(N2kMsg,
                  ManufacturerInformation,
                  InstallationDescription1,
                  InstallationDescription2);
}


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
  SetN2kPGN126464(N2kMsg,Destination,tN2kPGNList::N2kpgnl_transmit,PGNs);
}

#endif
