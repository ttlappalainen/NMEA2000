/*
N2kDeviceList.h

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

#ifndef _N2kDeviceList_H_
#define _N2kDeviceList_H_

#include "NMEA2000.h"

#define N2kMaxBusDevices 254

#define N2kDL_TimeForFirstRequest 1000 // Time in ms for first request after device has been noticed on the bus
#define N2kDL_TimeBetweenPIRequest 1000 // Time in ms between product information requests
#define N2kDL_TimeBetweenCIRequest 1000 // Time in ms between configuration information requests

class tN2kDeviceList : public tNMEA2000::tMsgHandler {
  protected:
    class tInternalDevice : public tNMEA2000::tDevice {
    protected:
      bool ProdILoaded;
      tNMEA2000::tProductInformation ProdI;

      bool ConfILoaded;
      uint16_t ConfISize;
      uint16_t ManISize;
      uint16_t InstDesc1Size;
      uint16_t InstDesc2Size;

      char *ConfI;
      char *ManufacturerInformation;
      char *InstallationDescription1;
      char *InstallationDescription2;

      uint8_t TransmitPGNsSize;
      unsigned long *TransmitPGNs;
      uint8_t ReceivePGNsSize;
      unsigned long *ReceivePGNs;

      public:
        uint8_t nNameRequested; // How many times we have requested.
        unsigned long ProdIRequested; // Time for last request
        uint8_t nProdIRequested; // How many times we have requested.
        unsigned long ConfIRequested; // Time for last request
        uint8_t nConfIRequested; // How many times we have requested.
        unsigned long PGNsRequested; // Time for last request
        uint8_t nPGNsRequested; // How many times we have requested.

        unsigned long LastMessageTime;

      public:
        tInternalDevice(uint64_t _Name, uint8_t _Source=255);
        ~tInternalDevice();

        void SetSource(uint8_t _Source) { Source=_Source; }

        // Device information
        void SetDeviceInformation(uint64_t _Name) { DevI.SetName(_Name); }

        // Product information
        void SetProductInformation(const char *_ModelSerialCode, // Default="". Max 32 chars. Manufacturer's Model serial code
                                   unsigned short _ProductCode=0xffff,  // Default=666. Manufacturer's product code
                                   const char *_ModelID=0, // Default="". Max 33 chars. Manufacturer's  Model ID
                                   const char *_SwCode=0, // Default="". Max 40 chars. Manufacturer's software version code
                                   const char *_ModelVersion=0, // Default="". Max 24 chars. Manufacturer's Model version
                                   unsigned char _LoadEquivalency=0xff,  // Default=1. x * 50 mA
                                   unsigned short _N2kVersion=0xffff, // Default=1300
                                   unsigned char _CertificationLevel=0xff // Default=1
                                  ) {
          ProdI.Set(_ModelSerialCode,_ProductCode,_ModelID,_SwCode,_ModelVersion,_LoadEquivalency,_N2kVersion,_CertificationLevel);
          ProdILoaded=true;
        }
        bool HasProductInformation() const { return ProdILoaded; }
        unsigned short GetN2kVersion() const { return ProdI.N2kVersion; }
        unsigned short GetProductCode() const { return ProdI.ProductCode; }
        const char * GetModelID() const { return ProdI.N2kModelID; }
        const char * GetSwCode() const { return ProdI.N2kSwCode; }
        const char * GetModelVersion() const { return ProdI.N2kModelVersion; }
        const char * GetModelSerialCode() const { return ProdI.N2kModelSerialCode; }
        unsigned short GetCertificationLevel() const { return ProdI.CertificationLevel; }
        unsigned short GetLoadEquivalency() const { return ProdI.LoadEquivalency; }

        // Configuration information
        bool HasConfigurationInformation() const { return ConfILoaded; }
        const char * GetManufacturerInformation() const { return ManufacturerInformation; }
        const char * GetInstallationDescription1() const { return InstallationDescription1; }
        const char * GetInstallationDescription2() const { return InstallationDescription2; }

        char * InitConfigurationInformation(size_t &_ManISize, size_t &_InstDesc1Size, size_t &_InstDesc2Size);
        char * GetManufacturerInformation() { return ManufacturerInformation; }
        char * GetInstallationDescription1() { return InstallationDescription1; }
        char * GetInstallationDescription2() { return InstallationDescription2; }

        const unsigned long * GetTransmitPGNs() const { return TransmitPGNs; }
        const unsigned long * GetReceivePGNs() const { return ReceivePGNs; }
        unsigned long * InitTransmitPGNs(uint8_t count);
        unsigned long * InitReceivePGNs(uint8_t count);

        bool ShouldRequestName() { return GetName()==0 && nNameRequested<20; }
        void SetNameRequested() { nNameRequested++; }

        void ClearProductInformationLoaded() { ProdILoaded=false; ProdIRequested=0; nProdIRequested=0; }
        void ClearProductInformation() { ProdI.Clear(); ClearProductInformationLoaded(); }
        bool ShouldRequestProductInformation() { return ( !ProdILoaded && nProdIRequested<4 ); } // We do not have it and not tried enough
        bool ReadyForRequestProductInformation() { return ( ShouldRequestProductInformation() && millis()-ProdIRequested>N2kDL_TimeBetweenPIRequest && millis()-GetCreateTime()>N2kDL_TimeForFirstRequest ); }
        void SetProductInformationRequested() { ProdIRequested=millis(); nProdIRequested++; }
        bool IsSameProductInformation(tNMEA2000::tProductInformation &Other) {
            if (ProdI.IsSame(Other)) {
              ProdILoaded=true; return true;
            } else {
              return false;
            }
          }
        void ClearConfigurationInformationLoaded() { ConfILoaded=false; ConfIRequested=0; nConfIRequested=0; }
        bool ShouldRequestConfigurationInformation() { return ( !ConfILoaded && nConfIRequested<4 ); } // We do not have it and not tried enough
        bool ReadyForRequestConfigurationInformation() { return ( ShouldRequestConfigurationInformation() && millis()-ConfIRequested>N2kDL_TimeBetweenCIRequest && millis()-GetCreateTime()>N2kDL_TimeForFirstRequest ); }
        void SetConfigurationInformationRequested() { ConfIRequested=millis(); nConfIRequested++; }

        void ClearPGNListLoaded() { PGNsRequested=0; nPGNsRequested=0; }
        bool ShouldRequestPGNList() { return ( ((TransmitPGNs==0) || (ReceivePGNs==0))  && nPGNsRequested<4 ); } // We do not have it and not tried enough
        void SetPGNListRequested() { PGNsRequested=millis(); nPGNsRequested++; }
        bool ReadyForRequestPGNList() { return ( ShouldRequestPGNList() && millis()-PGNsRequested>1000 && millis()-GetCreateTime()>N2kDL_TimeForFirstRequest ); }
    }; // tInternalDevice

  protected:
    tInternalDevice * Sources[N2kMaxBusDevices];  // NMEA 2000 bus can have max 254 nodes.
    uint8_t MaxDevices;
    bool ListUpdated;
    bool HasPendingRequests;

  protected:
    void HandleIsoAddressClaim(const tN2kMsg &N2kMsg); // PGN 60928
    void HandleProductInformation(const tN2kMsg &N2kMsg); // PGN 126996
    void HandleConfigurationInformation(const tN2kMsg &N2kMsg); // PGN 126998
    void HandleSupportedPGNList(const tN2kMsg &N2kMsg); // PGN 126464
    void HandleOther(const tN2kMsg &N2kMsg);
    tN2kDeviceList::tInternalDevice * LocalFindDeviceBySource(uint8_t Source) const;
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByName(uint64_t Name) const;
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByIDs(uint16_t ManufacturerCode, uint32_t UniqueNumber) const;
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByProduct(uint16_t ManufacturerCode, uint16_t ProductCode, uint8_t Source=0xff) const;
    bool RequestProductInformation(uint8_t Source);
    bool RequestConfigurationInformation(uint8_t Source);
    bool RequestSupportedPGNList(uint8_t Source);
    bool RequestIsoAddressClaim(uint8_t Source);
    void AddDevice(uint8_t Source);
    void SaveDevice(tInternalDevice *pDevice, uint8_t Source);

  public:
    tN2kDeviceList(tNMEA2000 *_pNMEA2000);
    void HandleMsg(const tN2kMsg &N2kMsg);

    // Return device by it's bus source address. If there is no device with given
    // source, function returns nul.
    const tNMEA2000::tDevice * FindDeviceBySource(uint8_t Source) const { return LocalFindDeviceBySource(Source); }

    // Return device last message time in millis.
    unsigned long GetDeviceLastMessageTime(uint8_t Source) const {
      tN2kDeviceList::tInternalDevice *dev=LocalFindDeviceBySource(Source);
      return ( dev!=0?dev->LastMessageTime:0 );
    }

    // Return device by it's name. Device name is complete device information data, which is unique
    // for all registered devices and should be unique for own made devices on own bus. Name
    // will be matched according to matching parameter. If there is no device with given
    // name, function returns nul.
    const tNMEA2000::tDevice * FindDeviceByName(uint64_t Name) const { return LocalFindDeviceByName(Name); }

    // Return device by manufacturer identification. Each device should have manufacturer id and unique ID.
    const tNMEA2000::tDevice * FindDeviceByIDs(uint16_t ManufacturerCode, uint32_t UniqueNumber) const { return LocalFindDeviceByIDs(ManufacturerCode, UniqueNumber); }

    // Return device by manufacturer product code. Each device should have product code given by NMEA2000 organization.
    // Search with source = 0xff finds first device. To find all devices with given manufacturer product code,
    // repeat search with found device source until device will not be found.
    const tNMEA2000::tDevice * FindDeviceByProduct(uint16_t ManufacturerCode, uint16_t ProductCode, uint8_t Source=0xff) const { return LocalFindDeviceByProduct(ManufacturerCode, ProductCode, Source); }

    // Check has list updated.
    // Device list will be automatically updated. In stable system list should be ready and stable in few seconds. If you add device on the fly,
    // list will be updated as soon as it start to send data to the bus.
    bool ReadResetIsListUpdated() { if ( ListUpdated ) { ListUpdated=false; return true; } else { return false; } }

    // Return count of found Devices
    uint8_t Count() const;
};

#endif
