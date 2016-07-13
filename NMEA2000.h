/* 
NMEA2000.h

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
#include <N2kMsg.h>
#include <N2kCANMsg.h>

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
  tDeviceInformation() { DeviceInformation.Name=0; }
  void SetUniqueNumber(unsigned long _UniqueNumber) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0xffe00000) | (_UniqueNumber&0x1fffff); }
  unsigned long GetUniqueNumber() { return DeviceInformation.UnicNumberAndManCode&0x1fffff; }
  void SetManufacturerCode(int _ManufacturerCode) { DeviceInformation.UnicNumberAndManCode=(DeviceInformation.UnicNumberAndManCode&0x1fffff) | (((unsigned long)(_ManufacturerCode&0x7ff))<<21); }
  int GetManufacturerCode() { return DeviceInformation.UnicNumberAndManCode>>21; }
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
};

struct tProductInformation {
    unsigned int N2kVersion;
    unsigned int ProductCode;
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


protected:
  // Forward mode bit settings.
  static const int FwdModeBit_EnableForward     = B00000001; // If set, forward is enabled 
  static const int FwdModeBit_SystemMessages    = B00000010; // System messages will be forwarded
  static const int FwdModeBit_OnlyKnownMessages = B00000100; // Only known messages will be forwarded. System messages will be forwarded according its own bit.
  static const int FwdModeBit_OwnMessages       = B00001000; // Forward also all messages, what this device will send
               
protected:
    tDebugMode dbMode; // Default dm_None
    tN2kMode N2kMode; // Default N2km_ListenOnly.
    tForwardType ForwardType; // Default fwdt_Actisense.
    unsigned int ForwardMode; // Default all messages - also system and own.
    Stream *ForwardStream; // Default Serial.
    
    bool DeviceReady;
    unsigned long AddressClaimStarted;
    bool AddressChanged;
    
    // Device information
    tDeviceInformation DeviceInformation[Max_N2kDevices];
    int DeviceCount;
    unsigned long N2kSource[Max_N2kDevices];
    
    // Product information
    const tProductInformation *ProductInformation;
    tProductInformation *LocalProductInformation;
    
    const unsigned long *SingleFrameMessages;
    const unsigned long *FastPacketMessages;
    
protected:
    // Buffer for received messages.
    tN2kCANMsg *N2kCANMsgBuf;
    unsigned char MaxN2kCANMsgs;

    // Handler callback
    void (*MsgHandler)(const tN2kMsg &N2kMsg);
    
protected:
    // Virtual functions for different interfaces. Currently there are own classes
    // for Arduino due internal CAN (NMEA2000_due) and external MCP2515 SPI CAN bus controller (NMEA2000_mcp)
    virtual bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent=true)=0;
    virtual bool CANOpen()=0;
    virtual bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf)=0;

protected:
    int SetN2kCANBufMsg(unsigned long canId, unsigned char len, unsigned char *buf);
    bool CheckKnownMessage(unsigned long PGN, bool &SystemMessage, bool &FastPacket);
    bool HandleReceivedSystemMessage(int MsgIndex);
    void ForwardMessage(const tN2kMsg &N2kMsg);
    void HandleISORequest(const tN2kMsg &N2kMsg);
    void HandleISOAddressClaim(const tN2kMsg &N2kMsg);
    void GetNextAddress(int DeviceIndex);
    bool IsMySource(unsigned char Source);
    int FindSourceDeviceIndex(unsigned char Source);
    
    bool ForwardEnabled() const { return ((ForwardMode&FwdModeBit_EnableForward)>0 && (N2kMode!=N2km_SendOnly)); }
    bool ForwardSystemMessages() const { return ((ForwardMode&FwdModeBit_SystemMessages)>0); }
    bool ForwardOnlyKnownMessages() const { return ((ForwardMode&FwdModeBit_OnlyKnownMessages)>0); }
    bool ForwardOwnMessages() const { return ((ForwardMode&FwdModeBit_OwnMessages)>0); }
public:
    tNMEA2000();
    
    // As default there are reservation for 5 messages. If it is not critical to handle all fast packet messages like with N2km_NodeOnly
    // you can set buffer size smaller like 3 or 2 by calling this before open.    
    void SetN2kCANMsgBufSize(const unsigned char _MaxN2kCANMsgs) { if (N2kCANMsgBuf==0) { MaxN2kCANMsgs=_MaxN2kCANMsgs; }; }
    
    // Define your product information. Defaults will be set on initialization.
    // For keeping defaults use 0xffff/0xff for int/char values and nul ptr for pointers.
    // LoadEquivalency is multiplication of 50 mA, what your device will take power from
    // N2k-bus. E.g. for Arduino only it can be 1 (=50mA). If your device does not take
    // power from bus, set this to 0.
    // Note that serial code has length of 32 so just long enough to carry GUID.
    void SetProductInformation(const char *_ModelSerialCode, // Default="00000001". Max 32 chars. Manufacturer's Model serial code
                               unsigned int _ProductCode=0xffff,  // Default=666. Manufacturer's product code
                               const char *_ModelID=0, // Default="Arduino N2k->PC". Max 33 chars. Manufacturer's  Model ID
                               const char *_SwCode=0, // Default="1.0.0.0". Max 40 chars. Manufacturer's software version code
                               const char *_ModelVersion=0, // Default="1.0.0". Max 24 chars. Manufacturer's Model version
                               unsigned char _LoadEquivalency=0xff,  // Default=1. x * 50 mA
                               unsigned int _N2kVersion=0xffff, // Default=1300
                               unsigned char _SertificationLevel=0xff // Default=1
                               );
    // Call this if you want to save RAM and you have defined tProductInformation to PROGMEM as in example BatteryMonitor.ino
    // Note that I have not yet found a way to test is pointer in PROGMEM or not, so this does not work, if you define
    // tProductInformation to RAM.
    void SetProductInformation(const tProductInformation *_ProductInformation);  
    
    // Call these if you wish to modify the default message packets supported.  Pointers must be in PROGMEM
    void SetSingleFrameMessages(const unsigned long *_SingleFrameMessages);
    void SetFastPacketMessages (const unsigned long *_FastPacketMessages);
 
    // Set default device information.
    // For keeping defaults use 0xffff/0xff for int/char values and nul ptr for pointers.
    // Note that ManufacturerCode and UniqueNumber should give unic result on any network.
    // I just decided to use number below for ManufacturerCode as Open Source devices - this is not any number given by NMEA.
    void SetDeviceInformation(unsigned long _UniqueNumber, // Default=1. 21 bit resolution, max 2097151. Each device from same manufacturer should have unique number.
                              unsigned char _DeviceFunction=0xff, // Default=130, PC Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              unsigned char _DeviceClass=0xff, // Default=25, Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                              int _ManufacturerCode=0xffff,  // Default=2046. Maximum 2046. See the list of codes on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
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
    void SendProductInformation(int DeviceIndex=0);
    void SendConfigurationInformation(int DeviceIndex=0);
    
    // Set this before open. You can not change mode after Open().
    // Note that other than N2km_ListenOnly modes will automatically start initialization and address claim procedure. 
    // You have to call ParseMessages() periodically to handle these procedures.
    // If you know your system, define source something other address you allready have on your bus.
    void SetMode(tN2kMode _N2kMode, unsigned long _N2kSource=15); 
    
    // Set type how messages will be forwarded in listen mode. Defult is fwdt_Actisense
    void SetForwardType(tForwardType fwdType) { ForwardType=fwdType; }

    // Set the stream, where messages will be forwarded in listen mode. Default is &Serial.
    void SetForwardStream(Stream* _stream) { ForwardStream=_stream; }
    
    // You can call this. It will be called anyway automatically by ParseMessages(); 
    bool Open();
    
    // Generate N2k message e.g. by using N2kMessages.h and simply send it to the bus.
    bool SendMsg(const tN2kMsg &N2kMsg, int DeviceIndex=0);

    // Call this periodically to handle N2k messages. Note that even if you only send e.g.
    // temperature to the bus, you should call this so the code will automatically inform
    // abot itselt to others.
    void ParseMessages();

    // Set the message handler for incoming N2kMessages.
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg));
    
    // Read address for current device.
    // Multidevice support is under construction.
    unsigned char GetN2kSource(int DeviceIndex=0) const { if (DeviceIndex>=0 && DeviceIndex<DeviceCount) return N2kSource[DeviceIndex]; return N2kSource[DeviceCount-1]; }

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

    void SetDebugMode(tDebugMode _dbMode);
};

// ISO Acknowledgement
void SetN2kPGN59392(tN2kMsg &N2kMsg, unsigned char Control, unsigned char GroupFunction, unsigned long PGN);
inline void SetN2kPGNISOAcknowledgement(tN2kMsg &N2kMsg, unsigned char Control, unsigned char GroupFunction, unsigned long PGN) {
  SetN2kPGN59392(N2kMsg,Control,GroupFunction,PGN);
}

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

bool ParseN2kPGN59904(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN);

inline bool ParseN2kPGNISORequest(const tN2kMsg &N2kMsg, unsigned long &RequestedPGN) {
  return ParseN2kPGN59904(N2kMsg, RequestedPGN);
}

#endif
