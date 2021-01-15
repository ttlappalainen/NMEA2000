/*
N2kDeviceList.cpp

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

#include <stdlib.h>
#include "N2kDeviceList.h"

//#define N2kDeviceList_HANDLE_IN_DEBUG

#if defined(N2kDeviceList_HANDLE_IN_DEBUG)
#define DebugStream Serial
# define N2kHandleInDbg(fmt, args...)     DebugStream.print (fmt , ## args)
# define N2kHandleInDbgln(fmt, args...)   DebugStream.println (fmt , ## args)
#else
# define N2kHandleInDbg(fmt, args...)
# define N2kHandleInDbgln(fmt, args...)
#endif

//*****************************************************************************
tN2kDeviceList::tN2kDeviceList(tNMEA2000 *_pNMEA2000) : tNMEA2000::tMsgHandler(0,_pNMEA2000) {
  for (uint8_t i=0; i<N2kMaxBusDevices; i++) Sources[i]=0;
  MaxDevices=0;
  ListUpdated=false;
  HasPendingRequests=true;
}

//*****************************************************************************
tN2kDeviceList::tInternalDevice * tN2kDeviceList::LocalFindDeviceBySource(uint8_t Source) const {
  if ( Source>=N2kMaxBusDevices ) return 0;

  return Sources[Source];
}

//*****************************************************************************
tN2kDeviceList::tInternalDevice * tN2kDeviceList::LocalFindDeviceByName(uint64_t Name) const {
  tInternalDevice *result=0;

    for (uint8_t i=0; i<MaxDevices && result==0; i++) {
      if ( Sources[i]!=0 && Sources[i]->IsSame(Name) ) result=Sources[i];
    }

    return result;
}

//*****************************************************************************
tN2kDeviceList::tInternalDevice * tN2kDeviceList::LocalFindDeviceByIDs(uint16_t ManufacturerCode, uint32_t UniqueNumber) const {
  tInternalDevice *result=0;

    if ( ManufacturerCode==N2kUInt16NA && UniqueNumber==N2kUInt32NA ) return result;

    for (uint8_t i=0; i<MaxDevices && result==0; i++) {
      if ( Sources[i]!=0 &&
           (ManufacturerCode==N2kUInt16NA || Sources[i]->GetManufacturerCode()==ManufacturerCode) &&
           (UniqueNumber==N2kUInt32NA || Sources[i]->GetUniqueNumber()==UniqueNumber) ) result=Sources[i];
    }

    return result;
}

//*****************************************************************************
tN2kDeviceList::tInternalDevice * tN2kDeviceList::LocalFindDeviceByProduct(uint16_t ManufacturerCode, uint16_t ProductCode, uint8_t Source) const {
  tInternalDevice *result=0;

    if ( Source<MaxDevices ) { Source++; } else { Source=0; }

    if ( ManufacturerCode==N2kUInt16NA || ProductCode==N2kUInt16NA ) return result;

    for (uint8_t i=Source; i<MaxDevices && result==0; i++) {
      if ( Sources[i]!=0 &&
           Sources[i]->GetManufacturerCode()==ManufacturerCode &&
           Sources[i]->GetProductCode()==ProductCode ) result=Sources[i];
    }

    return result;
}

//*****************************************************************************
bool tN2kDeviceList::RequestProductInformation(uint8_t Source) {
  tN2kMsg N2kMsg;

    SetN2kPGNISORequest(N2kMsg,Source,N2kPGNProductInformation);
    return GetNMEA2000()->SendMsg(N2kMsg);
}

//*****************************************************************************
bool tN2kDeviceList::RequestConfigurationInformation(uint8_t Source) {
  tN2kMsg N2kMsg;

    SetN2kPGNISORequest(N2kMsg,Source,N2kPGNConfigurationInformation);
    return GetNMEA2000()->SendMsg(N2kMsg);
}

//*****************************************************************************
bool tN2kDeviceList::RequestSupportedPGNList(uint8_t Source) {
  tN2kMsg N2kMsg;

    SetN2kPGNISORequest(N2kMsg,Source,126464L);
    return GetNMEA2000()->SendMsg(N2kMsg);
}

//*****************************************************************************
bool tN2kDeviceList::RequestIsoAddressClaim(uint8_t Source) {
  tN2kMsg N2kMsg;

    SetN2kPGNISORequest(N2kMsg,Source,N2kPGNIsoAddressClaim);
    return GetNMEA2000()->SendMsg(N2kMsg);
}

//*****************************************************************************
void tN2kDeviceList::HandleMsg(const tN2kMsg &N2kMsg) {
  if ( N2kMsg.Source>=N2kMaxBusDevices ) return;

  if ( Sources[N2kMsg.Source]==0 ) {
    switch ( N2kMsg.PGN ) {
      case N2kPGNIsoAddressClaim:break; // fall to default handler
      case N2kPGNProductInformation:
      case N2kPGNConfigurationInformation:
      case 126464L: AddDevice(N2kMsg.Source); break; // Create device and fall to default handler
      default: AddDevice(N2kMsg.Source); return;
    }
  }

  switch ( N2kMsg.PGN ) {
    case N2kPGNIsoAddressClaim: HandleIsoAddressClaim(N2kMsg); break;
    case N2kPGNProductInformation: HandleProductInformation(N2kMsg); break;
    case N2kPGNConfigurationInformation: HandleConfigurationInformation(N2kMsg); break;
    case 126464L: HandleSupportedPGNList(N2kMsg); break;
    default: HandleOther(N2kMsg);
  }

  if ( Sources[N2kMsg.Source]!=0 ) {
    // If device has been off and appears again and we still do not have name,
    // start request sequence again
    if ( Sources[N2kMsg.Source]->GetName()==0 &&
         Sources[N2kMsg.Source]->nNameRequested>0 && 
         Sources[N2kMsg.Source]->LastMessageTime+60000 < millis() ) {
      Sources[N2kMsg.Source]->nNameRequested=0;
      HasPendingRequests=true;
    }
    Sources[N2kMsg.Source]->LastMessageTime=millis();
  }
}

//*****************************************************************************
void tN2kDeviceList::HandleOther(const tN2kMsg &N2kMsg) {
  if ( N2kMsg.Source>=N2kMaxBusDevices ) return;

//  N2kHandleInDbg(millis()); N2kHandleInDbg(" PGN: "); N2kHandleInDbgln(N2kMsg.PGN);

  if ( !HasPendingRequests ) return;

  HasPendingRequests=false;

  // Require name for every device.
  if ( Sources[N2kMsg.Source]->ShouldRequestName() && RequestIsoAddressClaim(N2kMsg.Source) ) {
    Sources[N2kMsg.Source]->SetNameRequested();
    HasPendingRequests=true;
  }

  // First we try to request product information for all devices
  for ( int i=0; i<MaxDevices; i++) {
    if ( Sources[i]!=0 ) {
      // Test do we need product information for this device
      if ( Sources[i]->ReadyForRequestProductInformation() ) {
        if ( RequestProductInformation(Sources[i]->GetSource()) ) {
          N2kHandleInDbg(millis()); N2kHandleInDbg(" Request product information for source: "); N2kHandleInDbgln(Sources[i]->GetSource());
          Sources[i]->SetProductInformationRequested();
          HasPendingRequests=true;
          return;
        }
      } else {
        HasPendingRequests|=Sources[i]->ShouldRequestProductInformation();
      }
    }
  }
  if ( HasPendingRequests ) return;
  // We come up to here, if have requested all product infromation
  // Start to request configuration information for devices.
  for ( int i=0; i<MaxDevices; i++) {
    if ( Sources[i]!=0 ) {
      // Test do we need product information for this device
      if ( Sources[i]->ReadyForRequestConfigurationInformation() ) {
        if ( RequestConfigurationInformation(Sources[i]->GetSource()) ) {
          N2kHandleInDbg(millis()); N2kHandleInDbg(" Request configuration information for source: "); N2kHandleInDbgln(Sources[i]->GetSource());
          Sources[i]->SetConfigurationInformationRequested();
          HasPendingRequests=true;
          return;
        }
      } else {
        HasPendingRequests|=Sources[i]->ShouldRequestConfigurationInformation();
      }
    }
  }
  if ( HasPendingRequests ) return;

  // Finally query supported PGN lists
  for ( int i=0; i<MaxDevices; i++) {
    if ( Sources[i]!=0 ) {
      // Test do we need product information for this device
      if ( Sources[i]->ReadyForRequestPGNList() ) {
        if ( RequestSupportedPGNList(Sources[i]->GetSource()) ) {
          N2kHandleInDbg(millis()); N2kHandleInDbg(" Request supported PGN lists for source: "); N2kHandleInDbgln(Sources[i]->GetSource());
          Sources[i]->SetPGNListRequested();
          HasPendingRequests=true;
          return;
        }
      } else {
        HasPendingRequests|=Sources[i]->ShouldRequestPGNList();
      }
    }
  }
}

//*****************************************************************************
void tN2kDeviceList::AddDevice(uint8_t Source){
  if ( RequestIsoAddressClaim(Source) ) {  // Request device information
    SaveDevice(new tInternalDevice(0),Source); // We have now device on this source, so we will not do continuos query.
    HasPendingRequests=true;
  }
}

//*****************************************************************************
void tN2kDeviceList::SaveDevice(tInternalDevice *pDevice, uint8_t Source) {
  if ( Source>=N2kMaxBusDevices ) return;

  pDevice->SetSource(Source);
  Sources[Source]=pDevice;
  if ( Source>=MaxDevices ) MaxDevices=Source+1;
}

//*****************************************************************************
void tN2kDeviceList::HandleIsoAddressClaim(const tN2kMsg &N2kMsg) {
  if ( N2kMsg.PGN!=N2kPGNIsoAddressClaim ) return;

  int Index=0;
  uint64_t CallerName=N2kMsg.GetUInt64(Index);
  tInternalDevice *pDevice=0;

  // First check do we already have recorded caller
  if ( N2kMsg.Source<N2kMaxBusDevices && Sources[N2kMsg.Source]!=0 ) {
    pDevice=Sources[N2kMsg.Source];
    N2kHandleInDbg("ISO address claim. Caller:"); N2kHandleInDbg((uint32_t)CallerName); N2kHandleInDbg(", uniq:" ); N2kHandleInDbgln(pDevice->GetUniqueNumber());
    if ( pDevice->GetName()==0 ) {  // Device reservation made by HandleMsg, Name has not set yet
      tInternalDevice *pDevice2=LocalFindDeviceByName(CallerName); // Find does this actually exist with other source
      if ( pDevice2!=0 ) { // We have already seen that message on other address, so move it here
        delete pDevice;
        Sources[pDevice2->GetSource()]=0;
        SaveDevice(pDevice2,N2kMsg.Source);
        pDevice=pDevice2;
      } else {
        pDevice->SetDeviceInformation(CallerName);
        ListUpdated=true;
        N2kHandleInDbg("Saving name for source:"); N2kHandleInDbgln(N2kMsg.Source);
      }
    } else if (!pDevice->IsSame(CallerName) ) { // exists, but name does not match. So device on this source position has claimed address and this has taken its place
      // Just move old device to some empty place
      uint8_t i;
      for (i=0; i<N2kMaxBusDevices && Sources[i]!=0; i++);
      // If we found empty place, move it there.
      if ( i<N2kMaxBusDevices ) {
        SaveDevice(pDevice,i);
        RequestIsoAddressClaim(0xff);  // Request addresses for all nodes.
      } else { // If not, we just delete device, since we can not do much with it. This would be extremely unexpected.
        delete pDevice;
      }
      Sources[N2kMsg.Source]=0;
      pDevice=0;
    } else { // Name is caller -> we have device on list on its place.
      return;
    }
  }

  if ( pDevice==0 ) {
    // New or changed source
    pDevice=LocalFindDeviceByName(CallerName);
    if ( pDevice!=0 ) { // Address changed, simply move device to new place.
      Sources[pDevice->GetSource()]=0;
      SaveDevice(pDevice,N2kMsg.Source);
      N2kHandleInDbg("Source updated: "); N2kHandleInDbgln(pDevice->GetSource());
    } else { // New device
      pDevice=new tInternalDevice(CallerName);
      SaveDevice(pDevice,N2kMsg.Source);
    }
  }

  // In any address change, we request information again.
  pDevice->ClearProductInformationLoaded();
  HasPendingRequests=true;

  ListUpdated=true;
}

//*****************************************************************************
void tN2kDeviceList::HandleProductInformation(const tN2kMsg &N2kMsg) {
  tNMEA2000::tProductInformation ProdI;
//  unsigned long t1=micros();

  if ( N2kMsg.Source>=N2kMaxBusDevices || Sources[N2kMsg.Source]==0 ) return;

  tInternalDevice *pDevice=Sources[N2kMsg.Source];

  N2kHandleInDbg(" Handle product information for source: "); N2kHandleInDbgln(N2kMsg.Source);

  if ( !pDevice->HasProductInformation() &&
       ParseN2kPGN126996(N2kMsg,ProdI.N2kVersion,ProdI.ProductCode,
                         sizeof(ProdI.N2kModelID),ProdI.N2kModelID,sizeof(ProdI.N2kSwCode),ProdI.N2kSwCode,
                         sizeof(ProdI.N2kModelVersion),ProdI.N2kModelVersion,sizeof(ProdI.N2kModelSerialCode),ProdI.N2kModelSerialCode,
                         ProdI.CertificationLevel,ProdI.LoadEquivalency) ) {
    if ( !pDevice->IsSameProductInformation(ProdI) ) {
      pDevice->SetProductInformation(ProdI.N2kModelSerialCode,ProdI.ProductCode,ProdI.N2kModelID,ProdI.N2kSwCode,ProdI.N2kModelVersion,
                                     ProdI.LoadEquivalency,ProdI.N2kVersion,ProdI.CertificationLevel);
      ListUpdated=true;
    }
  }

//  unsigned long t2=micros();
//  if ( ListUpdated ) { Serial.print("  Updated source: "); Serial.println(N2kMsg.Source); }
//  Serial.print(" - 126996 elapsed: "); Serial.println(t2-t1);
}

//*****************************************************************************
void tN2kDeviceList::HandleConfigurationInformation(const tN2kMsg &N2kMsg) {

  if ( N2kMsg.Source>=N2kMaxBusDevices || Sources[N2kMsg.Source]==0 ) return;

//  unsigned long t1=micros();
  size_t ManISize;
  size_t InstDesc1Size;
  size_t InstDesc2Size;

  tInternalDevice *pDevice=Sources[N2kMsg.Source];

  N2kHandleInDbg(" Handle configuration information for source: "); N2kHandleInDbgln(N2kMsg.Source);

  if ( ParseN2kPGN126998(N2kMsg,ManISize,0,InstDesc1Size,0,InstDesc2Size,0) ) { // First query required size
    pDevice->InitConfigurationInformation(ManISize,InstDesc1Size,InstDesc2Size);
    int TotalSize=ManISize+InstDesc1Size+InstDesc2Size;
    if ( TotalSize>0 ) {
      ParseN2kPGN126998(N2kMsg,
                        ManISize,pDevice->GetManufacturerInformation(),
                        InstDesc1Size,pDevice->GetInstallationDescription1(),
                        InstDesc2Size,pDevice->GetInstallationDescription2());
    }
    ListUpdated=true;
  }

//  unsigned long t2=micros();
//  if ( ListUpdated ) { Serial.print("  Updated source: "); Serial.println(N2kMsg.Source); }
//  Serial.print(" - 126996 elapsed: "); Serial.println(t2-t1);
}

//*****************************************************************************
void tN2kDeviceList::HandleSupportedPGNList(const tN2kMsg &N2kMsg) {

  if ( N2kMsg.Source>=N2kMaxBusDevices || Sources[N2kMsg.Source]==0 ) return;

  int Index=0;
  tN2kPGNList N2kPGNList=(tN2kPGNList)N2kMsg.GetByte(Index);
  uint8_t PGNCount=(N2kMsg.DataLen-Index)/3;
  tInternalDevice *pDevice=Sources[N2kMsg.Source];
  unsigned long * PGNList=0;
  uint8_t iPGN;

  switch (N2kPGNList) {
    case N2kpgnl_transmit:
      PGNList=pDevice->InitTransmitPGNs(PGNCount);
      break;
    case N2kpgnl_receive:
      PGNList=pDevice->InitReceivePGNs(PGNCount);
      break;
  }
  if ( PGNList!=0 ) {
    for (iPGN=0; iPGN<PGNCount; iPGN++) { PGNList[iPGN]=N2kMsg.Get3ByteUInt(Index); }
    PGNList[iPGN]=0;
  }

  ListUpdated=true;
}

//*****************************************************************************
uint8_t tN2kDeviceList::Count() const {
  uint8_t ret=0;

  for ( size_t i=0; i<MaxDevices; i++ ) if ( Sources[i]!=0 ) ret++;

  return ret;
}

// tN2kDeviceList::tInternalDevice

//*****************************************************************************
tN2kDeviceList::tInternalDevice::tInternalDevice(uint64_t _Name, uint8_t _Source) : tNMEA2000::tDevice(_Name,_Source) {
  ProdI.Clear(); ProdILoaded=false; ConfILoaded=false;
  ConfI=0; ConfISize=0; ManufacturerInformation=0; InstallationDescription1=0; InstallationDescription2=0;
  TransmitPGNsSize=0; TransmitPGNs=0; ReceivePGNsSize=0; ReceivePGNs=0;
  nNameRequested=0;
  ClearProductInformationLoaded();
  ClearConfigurationInformationLoaded();
  ClearPGNListLoaded();
}

//*****************************************************************************
tN2kDeviceList::tInternalDevice::~tInternalDevice() {
  if ( ConfI!=0 ) free(ConfI);
  if ( TransmitPGNs!=0 ) free(TransmitPGNs);
  if ( ReceivePGNs!=0 ) free(ReceivePGNs);
}

//*****************************************************************************
char * tN2kDeviceList::tInternalDevice::InitConfigurationInformation(size_t &_ManISize, size_t &_InstDesc1Size, size_t &_InstDesc2Size) {
  if ( _ManISize>0 ) _ManISize++; // Reserve '/0' terminator
  if ( _InstDesc1Size>0 ) _InstDesc1Size++; // Reserve '/0' terminator
  if ( _InstDesc2Size>0 ) _InstDesc2Size++; // Reserve '/0' terminator
  uint16_t _ConfISize=_ManISize+_InstDesc1Size+_InstDesc2Size;
  if ( ConfI!=0 && ConfISize<_ConfISize ) { // We can not fit new data, so release mem.
    free(ConfI); ConfI=0; ConfISize=0;
  }
  if ( ConfI==0 ) {
    ConfISize=_ConfISize;
    ConfI=(char*)(ConfISize>0?malloc(ConfISize):0);
    if ( _ManISize>0 ) {
      ManufacturerInformation=ConfI;
      ManufacturerInformation[0]='\0';
    } else ManufacturerInformation=0;
    if ( _InstDesc1Size>0 ) {
      InstallationDescription1=ConfI+_ManISize;
      InstallationDescription1[0]='\0';
    } else InstallationDescription1=0;
    if ( _InstDesc2Size>0 ) {
      InstallationDescription2=ConfI+_ManISize+_InstDesc1Size;
      InstallationDescription2[0]='\0';
    } else InstallationDescription2=0;
  }
  ConfILoaded=true;
  return ConfI;
}

//*****************************************************************************
unsigned long * tN2kDeviceList::tInternalDevice::InitTransmitPGNs(uint8_t count) {
  if (TransmitPGNs!=0 && TransmitPGNsSize<count ) { free(TransmitPGNs); TransmitPGNs=0; TransmitPGNsSize=0; } // Free old reservation
  if (TransmitPGNs==0) { TransmitPGNs=(unsigned long *)malloc((count+1)*sizeof(unsigned long)); TransmitPGNsSize=count; }
  if (TransmitPGNs!=0) TransmitPGNs[0]=0;
  return TransmitPGNs;
}

//*****************************************************************************
unsigned long * tN2kDeviceList::tInternalDevice::InitReceivePGNs(uint8_t count) {
  if (ReceivePGNs!=0 && ReceivePGNsSize<count ) { free(ReceivePGNs); ReceivePGNs=0; ReceivePGNsSize=0; } // Free old reservation
  if (ReceivePGNs==0) { ReceivePGNs=(unsigned long *)malloc((count+1)*sizeof(unsigned long)); ReceivePGNsSize=count; }
  if (ReceivePGNs!=0) ReceivePGNs[0]=0;
  return ReceivePGNs;
}
