/*
 * N2kDeviceList.h
 * Copyright (c) 2015-2024 Timo Lappalainen, Kave Oy, www.kave.fi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*************************************************************************//**
 * \file  N2kDeviceList.h
 * \brief This File contains a 
 * 
 * 
 */

#ifndef _N2kDeviceList_H_
#define _N2kDeviceList_H_

#include "NMEA2000.h"

/** \brief  Maximum allowed number of devices on the CAN BUS bus system
 *          is 254
*/
#define N2kMaxBusDevices 254

/** \brief  Time in ms for first request after device has been 
 * noticed on the bus */
#define N2kDL_TimeForFirstRequest 1000 

/** \brief  Time in ms between product information requests */
#define N2kDL_TimeBetweenPIRequest 1000

/** \brief  Time in ms between configuration information requests */
#define N2kDL_TimeBetweenCIRequest 1000 

/************************************************************************//**
 * \class   tN2kDeviceList
 * \brief   Helper class to keep track of all devices on the bus
 * \ingroup group_helperClass
 * 
 * On bus each message contains sender source address. Device source address may be changed
 * due to \ref secRefTermAddressClaiming "address claiming" on power up or when new device 
 * will be added to the bus. For this reason source address can not be used as only filter
 * for similar messages like position data. By using tN2kDeviceList source address related to 
 * \ref secRefTermNAME can be easily found at any time with FindDeviceByName(). 
 * With ReadResetIsListUpdated() can be easily checked
 * is there any changes on device list and then request interesting source address with
 * FindDeviceByName().
 * 
 * Check all search methods:
 * - FindDeviceByName()
 * - FindDeviceByIDs()
 * - FindDeviceByProduct()
 * - FindDeviceBySource()
 * 
 *  This class is derived from \ref tNMEA2000::tMsgHandler.
 */
class tN2kDeviceList : public tNMEA2000::tMsgHandler {
  protected:
    /**********************************************************************//**
     * \class   tInternalDevice
     * \brief   This class represents an internal device
     *
     *  This class is derived from \ref tNMEA2000::tDevice
     */
    class tInternalDevice : public tNMEA2000::tDevice {
    protected:
      /** \brief Product Information has been loaded */
      bool ProdILoaded;
      /** \brief Product Information of this device*/
      tNMEA2000::tProductInformation ProdI;

      /** \brief Product Information has been loaded */
      bool ConfILoaded;
      /** \brief Size of the Config Information (number of bytes)*/
      uint16_t ConfISize;
      /** \brief Size of the Manufacturer Information (number of bytes) */
      uint16_t ManISize;
      /** \brief Size of the Installation Description 1 (number of bytes) */
      uint16_t InstDesc1Size;
      /** \brief Size of the Installation Description 2 (number of bytes) */
      uint16_t InstDesc2Size;

      /** \brief Pointer to the Config Information */
      char *ConfI;
      /** \brief Pointer to the Manufacturer Information */
      char *ManufacturerInformation;
      /** \brief Pointer to the Installation Description 1 */
      char *InstallationDescription1;
      /** \brief Pointer to the Installation Description 2 */
      char *InstallationDescription2;

      /** \brief Size of the transmitted PGN (number of bytes)*/
      uint8_t TransmitPGNsSize;
      /** \brief Transmitted PGNs*/
      unsigned long *TransmitPGNs;
      /** \brief Size of the received PGN (number of bytes)*/
      uint8_t ReceivePGNsSize;
      /** \brief Received PGNs*/
      unsigned long *ReceivePGNs;

      public:
        /** \brief How many times we have requested the name.*/
        uint8_t nNameRequested; 
        /** \brief Time for last request on the Product Information*/
        unsigned long ProdIRequested; 
        /** \brief How many times we have requested the Product
         *         information */
        uint8_t nProdIRequested; 
        /** \brief Time for last request on the Config Information*/
        unsigned long ConfIRequested; 
        /** \brief How many times we have requested the Config
         *          information */
        uint8_t nConfIRequested; 
        /** \brief Time for last request on the PGN */
        unsigned long PGNsRequested; 
        /** \brief How many times we have requested the PGN*/
        uint8_t nPGNsRequested; 
        
        /** \brief Time of the last message*/
        unsigned long LastMessageTime;

      public:
        /******************************************************************//**
         * \brief Construct a new Internal Device object
         * 
         * Init all the variables of the Internal device.
         *
         * \param _Name   Name of the device
         * \param _Source Source address of this device on the bus
         */
        tInternalDevice(uint64_t _Name, uint8_t _Source=255);
        /********************************************** *******************//**
         * \brief Destroy the Internal Device object
         * Clean up all the memory.
         */
        ~tInternalDevice();
        
        /******************************************************************//**
         * \brief Set the Source address of the device
         *
         * \param _Source   Source address of this device
         */
        void SetSource(uint8_t _Source) { Source=_Source; }

        /******************************************************************//**
         * \brief Set the Device Information 
         *
         * \param _Name   Name of the device
         */
        void SetDeviceInformation(uint64_t _Name) { DevI.SetName(_Name); }

        /*******************************************************************//**
         * \brief Set the Product Information of the device
         *
         * \param _ModelSerialCode  Default="". Max 32 chars. Manufacturer's 
         *                          Model serial code
         * \param _ProductCode      Default=666. Manufacturer's product code
         * \param _ModelID          Default="". Max 33 chars. Manufacturer's 
         *                          Model ID
         * \param _SwCode           Default="". Max 40 chars. Manufacturer's 
         *                          software version code
         * \param _ModelVersion     Default="". Max 24 chars. Manufacturer's 
         *                          Model version
         * \param _LoadEquivalency  Default=1. x * 50 mA
         * \param _N2kVersion       Default=1300
         * \param _CertificationLevel Default=1
         */
        void SetProductInformation(const char *_ModelSerialCode, 
                                   unsigned short _ProductCode=0xffff,  
                                   const char *_ModelID=0, 
                                   const char *_SwCode=0, 
                                   const char *_ModelVersion=0, 
                                   unsigned char _LoadEquivalency=0xff,  
                                   unsigned short _N2kVersion=0xffff, 
                                   unsigned char _CertificationLevel=0xff 
                                  ) {
          ProdI.Set(_ModelSerialCode,_ProductCode,_ModelID,_SwCode,_ModelVersion,_LoadEquivalency,_N2kVersion,_CertificationLevel);
          ProdILoaded=true;
        }

        /******************************************************************//**
         * \brief Has the Product Information for the device already been loaded
         * \return true 
         * \return false 
         */
        bool HasProductInformation() const { return ProdILoaded; }
        /******************************************************************//**
         * \brief Get the N2k version of this device
         * \return unsigned short
         */
        unsigned short GetN2kVersion() const { return ProdI.N2kVersion; }
        /******************************************************************//**
         * \brief Get the Product code of this device
         * \return unsigned short
         */
        unsigned short GetProductCode() const { return ProdI.ProductCode; }
        /******************************************************************//**
         * \brief Get the Model ID of this device
         * \return const char *
         */
        const char * GetModelID() const { return ProdI.N2kModelID; }
        /******************************************************************//**
         * \brief Get the Software Code of this device
         * \return const char *
         */
        const char * GetSwCode() const { return ProdI.N2kSwCode; }
        /******************************************************************//**
         * \brief Get the Model Version of this device
         * \return const char *
         */
        const char * GetModelVersion() const { return ProdI.N2kModelVersion; }
        /******************************************************************//**
         * \brief Get the Model Serial Code of this device
         * \return const char *
         */
        const char * GetModelSerialCode() const { return ProdI.N2kModelSerialCode; }
        /******************************************************************//**
         * \brief Get the Certification Level of this device
         * \return unsigned short
         */
        unsigned short GetCertificationLevel() const { return ProdI.CertificationLevel; }
        /******************************************************************//**
         * \brief Get the Load Equivalency  (x * 50 mA) of this device
         * \return unsigned short
         */
        unsigned short GetLoadEquivalency() const { return ProdI.LoadEquivalency; }

        /******************************************************************//**
         * \brief Has the Configuration  Information for the device 
         *        already been loaded
         * \return true 
         * \return false 
         */
        bool HasConfigurationInformation() const { return ConfILoaded; }
        /******************************************************************//**
         * \brief Get the Manufacturer Information of this device
         * \return const char *
         */
        const char * GetManufacturerInformation() const { return ManufacturerInformation; }
        /******************************************************************//**
         * \brief Get the Installation Description 1 of this device
         * \return const char *
         */
        const char * GetInstallationDescription1() const { return InstallationDescription1; }
        /******************************************************************//**
         * \brief Get the Installation Description 2 of this device
         * \return const char *
         */
        const char * GetInstallationDescription2() const { return InstallationDescription2; }

        /******************************************************************//**
         * \brief Initialize the Configuration Information of this device 
         * 
         * This function gives back an empty string for the Configuration 
         * Information. Adequate memory has been allocated and the leading 
         * character ('\0' terminators ) have been added.
         * 
         * \param _ManISize       Size of the Manufacturer Information 
         * \param _InstDesc1Size  Size of the Install Description 1
         * \param _InstDesc2Size  Size of the Install Description 2
         * 
         * \return char* ->  empty Configuration Information
         */
        char * InitConfigurationInformation(size_t &_ManISize, size_t &_InstDesc1Size, size_t &_InstDesc2Size);
        
        /******************************************************************//**
         * \brief Get the Manufacturer Information of this device
         * \return char *
         */
        char * GetManufacturerInformation() { return ManufacturerInformation; }
        /******************************************************************//**
         * \brief Get the Installation Description 1 of this device
         * \return char *
         */
        char * GetInstallationDescription1() { return InstallationDescription1; }
        /******************************************************************//**
         * \brief Get the Installation Description 2 of this device
         * \return char *
         */
        char * GetInstallationDescription2() { return InstallationDescription2; }

        /******************************************************************//**
         * \brief Get the transmitted PGNs of this device
         * \return const unsigned long *
         */
        const unsigned long * GetTransmitPGNs() const { return TransmitPGNs; }
        /******************************************************************//**
         * \brief Get the received PGNs of this device
         * \return const unsigned long *
         */
        const unsigned long * GetReceivePGNs() const { return ReceivePGNs; }

        /******************************************************************//**
         * \brief Initialize an array for transmitted PGNs
         * This functions frees if needed old reservation and reserves new 
         * memory for requested number of PGNs
         * \param count   Number of Transmitted PGNs to be handled
         * \return unsigned long *
         */
        unsigned long * InitTransmitPGNs(uint8_t count);

        /******************************************************************//**
         * \brief Initialize an array for received PGNs
         * This functions frees if needed old reservation and reserves new 
         * memory for requested number of PGNs
         * \param count   Number of Transmitted PGNs to be handled
         * \return unsigned long *
         */
        unsigned long * InitReceivePGNs(uint8_t count);

        /******************************************************************//**
         * \brief Should the Device Name be requested
         * As long as the device name is not set yet and the number of
         * requests is smaller than 20, the name should be requested once more.
         * \return true 
         * \return false 
         */
        bool ShouldRequestName() { return GetName()==0 && nNameRequested<20; }
        /****************************************************************//**
         * \brief Increments the Number of how often the name has already 
         *        been requested */
        void SetNameRequested() { nNameRequested++; }
        /****************************************************************//**
         *  \brief  Resets the Product Information Loaded values of 
         *          the device */        
        void ClearProductInformationLoaded() { ProdILoaded=false; ProdIRequested=0; nProdIRequested=0; }
        /****************************************************************//**
         *  \brief  Clears the Product Information the device */        
        void ClearProductInformation() { ProdI.Clear(); ClearProductInformationLoaded(); }
        /******************************************************************//**
         * \brief Should the Device Product Information be requested
         * As long as the device Product Information is not set yet and the 
         * number of requests is smaller than 4, the name should be 
         * requested once more.
         * \return true 
         * \return false 
         */
        bool ShouldRequestProductInformation() { return ( !ProdILoaded && nProdIRequested<4 ); } 
        /******************************************************************//**
         * \brief Ready for the next request of Device Product Information
         * 
         * There must be a minimum interval between two requests for 
         * product information. see \ref N2kDL_TimeBetweenPIRequest
         * 
         * \return true 
         * \return false 
         */
        bool ReadyForRequestProductInformation() { return ( ShouldRequestProductInformation() && N2kHasElapsed(ProdIRequested,N2kDL_TimeBetweenPIRequest) && N2kHasElapsed(GetCreateTime(),N2kDL_TimeForFirstRequest) ); }
        /****************************************************************//**
         * \brief Increments the Number of how often the Product Information
         *         has already been requested and stores the timestamp*/
        void SetProductInformationRequested() { ProdIRequested=N2kMillis(); nProdIRequested++; }
        /****************************************************************//**
         * \brief Compares two Product Informations
         * \return true
         * \return false
         * */
        bool IsSameProductInformation(tNMEA2000::tProductInformation &Other) {
            if (ProdI.IsSame(Other)) {
              ProdILoaded=true; return true;
            } else {
              return false;
            }
          }
        
        /****************************************************************//**
         *  \brief  Resets the Configuration Information Loaded values of 
         *          the device */        
        void ClearConfigurationInformationLoaded() { ConfILoaded=false; ConfIRequested=0; nConfIRequested=0; }
        /******************************************************************//**
         * \brief Should the Device Configuration Information be requested
         * As long as the device Configuration Information is not set yet 
         * and the number of requests is smaller than 4, the name should be 
         * requested once more.
         * \return true 
         * \return false 
         */
        bool ShouldRequestConfigurationInformation() { return ( !ConfILoaded && nConfIRequested<4 ); } 
        /******************************************************************//**
         * \brief Ready for the next request of Device Configuration
         *        Information
         * 
         * There must be a minimum interval between two requests for 
         * product information. see \ref N2kDL_TimeBetweenCIRequest
         * 
         * \return true 
         * \return false 
         */
        bool ReadyForRequestConfigurationInformation() { return ( ShouldRequestConfigurationInformation() && N2kMillis()-ConfIRequested>N2kDL_TimeBetweenCIRequest && N2kMillis()-GetCreateTime()>N2kDL_TimeForFirstRequest ); }
        /****************************************************************//**
         * \brief Increments the Number of how often the Configuration 
         *        Information has already been requested and stores the 
         *        timestamp*/
        void SetConfigurationInformationRequested() { ConfIRequested=N2kMillis(); nConfIRequested++; }

        /****************************************************************//**
         *  \brief  Resets the PGN List Loaded values of 
         *          the device */        
        void ClearPGNListLoaded() { PGNsRequested=0; nPGNsRequested=0; }
        /******************************************************************//**
         * \brief Should the Device PGN List be requested
         * As long as the device PGN List is not set yet 
         * and the number of requests is smaller than 4, the name should be 
         * requested once more.
         * \return true 
         * \return false 
         */
        bool ShouldRequestPGNList() { return ( ((TransmitPGNs==0) || (ReceivePGNs==0))  && nPGNsRequested<4 ); } // We do not have it and not tried enough
        /****************************************************************//**
         * \brief Increments the Number of how often the PGN List 
         *        has already been requested and stores the 
         *        timestamp*/
        void SetPGNListRequested() { PGNsRequested=N2kMillis(); nPGNsRequested++; }
        /******************************************************************//**
         * \brief Ready for the next request of Device PGN List
         * 
         * There must be a minimum interval between two requests for 
         * product information. see \ref N2kDL_TimeForFirstRequest
         * 
         * \return true 
         * \return false 
         */
        bool ReadyForRequestPGNList() { return ( ShouldRequestPGNList() && N2kHasElapsed(PGNsRequested,1000) && N2kHasElapsed(GetCreateTime(),N2kDL_TimeForFirstRequest) ); }
    }; // tInternalDevice

  protected:
    /********************************************************************//**
     * \brief List of NMEA2000 devices found on the bus
     */
    tInternalDevice * Sources[N2kMaxBusDevices];  
    /** \brief Number of NMEA2000 devices stored in \ref Sources*/
    uint8_t MaxDevices;
    /** \brief The list of devices has been updated*/
    bool ListUpdated;
    /** \brief There are still requests pending*/
    bool HasPendingRequests;

  protected:
    /********************************************************************//**
     * \brief Handle ISO Address Claim Message - PGN 60928
     * 
     * This function handle an ISO address claim message. It checks whether
     * the caller device is already listed at \ref Sources or not.
     * If it is already listed, it checks also if the name at this source 
     * position still matches. When the name doesn't match, it moves the 
     * "old" device to a free spot in \ref Sources.
     * 
     * If the caller is completely unknown, a new device will be placed in 
     * \ref Sources.
     *
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleIsoAddressClaim(const tN2kMsg &N2kMsg);

    /********************************************************************//**
     * \brief Handle a Product Information message - PGN 126996
     * 
     * The message provides product information onto the network that 
     * could be important for determining quality of data coming from 
     * this product. The message is parsed and all data is stored 
     * in \ref tInternalDevice::ProdI
     * 
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleProductInformation(const tN2kMsg &N2kMsg);
    /********************************************************************//**
     * \brief Handle a Configuration Information message - PGN 126998
     * 
     * The message contains free-form alphanumeric fields describing the 
     * installation (e.g., starboard engine room location) of the device 
     * and installation notes (e.g., calibration data).
     * It is parsed and all data is stored in the internal device
     * 
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleConfigurationInformation(const tN2kMsg &N2kMsg);
    /********************************************************************//**
     * \brief Handle a Product Information message - PGN 126464
     *  
     * The PGN 126464 message consists the group function type defined 
     * by the first field. The message will be either a Transmit PGNs or 
     * a Receive PGNs group function that identifies the PGNs transmitted 
     * from or received by a node.
     * This function determines if the PGNs are receive or transmit PGN
     * (see \ref tN2kPGNList) an the stores the PGNs to the corresponding 
     * internal device in \ref Sources.
     * 
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleSupportedPGNList(const tN2kMsg &N2kMsg); 
    /********************************************************************//**
     * \brief Handles all Other messages
     * 
     * If request is pending ( \ref HasPendingRequests == true) it requires
     * a name for every device, then loads product + config informations 
     * and supported PGN lists as needed.
     *
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleOther(const tN2kMsg &N2kMsg);
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the source address
     *
     * \param Source  Source address of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    tN2kDeviceList::tInternalDevice * LocalFindDeviceBySource(uint8_t Source) const;
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the name of the device
     *
     * \param Name  Name of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByName(uint64_t Name) const;
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the manufacturer code and
     *        unique ID
     *
     * \param ManufacturerCode  Manufacturer code of the device to be 
     *                          searched for
     * \param UniqueNumber      Unique ID of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByIDs(uint16_t ManufacturerCode, uint32_t UniqueNumber) const;
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the manufacturer and product
     *        code
     * 
     * \param ManufacturerCode  Manufacturer code of the device to be 
     *                          searched for
     * \param ProductCode       Product code of the device to be 
     *                          searched for
     * \param Source  Source address of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    tN2kDeviceList::tInternalDevice * LocalFindDeviceByProduct(uint16_t ManufacturerCode, uint16_t ProductCode, uint8_t Source=0xff) const;
    /********************************************************************//**
     * \brief Request the product information of a specific device on the bus
     * 
     * This function sends out an ISO Request message in order to obtain
     * more information about a specific device on the bus.
     *
     * \param Source  Destination address of the target device
     * \return true   -> Message was sent successfully
     * \return false 
     */
    bool RequestProductInformation(uint8_t Source);
    /********************************************************************//**
     * \brief Request the configuration information of a specific 
     *        device on the bus
     * 
     * This function sends out an ISO Request message in order to obtain
     * more information about a specific device on the bus.
     *
     * \param Source  Destination address of the target device
     * \return true   -> Message was sent successfully
     * \return false 
     */
    bool RequestConfigurationInformation(uint8_t Source);
    /********************************************************************//**
     * \brief Request the supported PGNs of a specific device on the bus
     * 
     * This function sends out an ISO Request message in order to obtain
     * more information about a specific device on the bus.
     *
     * \param Source  Destination address of the target device
     * \return true   -> Message was sent successfully
     * \return false 
     */
    bool RequestSupportedPGNList(uint8_t Source);
    /********************************************************************//**
     * \brief Request the ISO AddressClaim for a specific device on the bus
     *
     * \param Source  Destination address of the target device
     * \return true   -> Message was sent successfully
     * \return false 
     */
    bool RequestIsoAddressClaim(uint8_t Source);
    /********************************************************************//**
     * \brief Adds a device to \ref Sources
     *
     * \param Source Source address of the device
     */
    void AddDevice(uint8_t Source);
    /********************************************************************//**
     * \brief Saves a device to \ref Sources
     *
     * \param pDevice Pointer to a device
     * \param Source Source address of the device
     */
    void SaveDevice(tInternalDevice *pDevice, uint8_t Source);

  public:
    /********************************************************************//**
     * \brief Constructor for the class
     *
     * Initialize all the attributes of the class
     * 
     * \param _pNMEA2000    Pointer to an \ref NMEA2000 object
     */
    tN2kDeviceList(tNMEA2000 *_pNMEA2000);
    /********************************************************************//**
     * \brief Handle NMEA2000 messages 
     * 
     * Depending on the message PGN the correct handler (see \ref 
     * HandleIsoAddressClaim, \ref HandleProductInformation, 
     * \ref HandleConfigurationInformation, \ref HandleSupportedPGNList, 
     * \ref HandleOther) is chosen. 
     * If there is now device with this source address is listed in \ref
     * Sources, a new device is added ( \ref AddDevice).
     *
     * \param N2kMsg    Reference to a N2kMsg Object, 
     */
    void HandleMsg(const tN2kMsg &N2kMsg);

    // 
    /********************************************************************//**
     * \brief Return device by it's bus source address
     * 
     * Return device by it's bus source address. If there is no device 
     * with given source, function returns null
     * \param Source  Source address of the device to be searched for
     * \return const tNMEA2000::tDevice* 
     */
    const tNMEA2000::tDevice * FindDeviceBySource(uint8_t Source) const { return LocalFindDeviceBySource(Source); }

    // Return device last message time in milliseconds.
    unsigned long GetDeviceLastMessageTime(uint8_t Source) const {
      tN2kDeviceList::tInternalDevice *dev=LocalFindDeviceBySource(Source);
      return ( dev!=0?dev->LastMessageTime:0 );
    }

    // 
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the NAME of the device
     *
     * Return device by it's NAME. Device NAME is complete device 
     * information data, which is unique for all registered devices and 
     * should be unique for own made devices on own bus. NAME will be 
     * matched according to matching parameter. If there is no device with
     * given NAME, function returns null.
     * 
     * \param NAME  NAME of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    const tNMEA2000::tDevice * FindDeviceByName(uint64_t NAME) const { return LocalFindDeviceByName(NAME); }
    
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the manufacturer code and
     *        unique ID
     * 
     * Return device by manufacturer identification. Each device should have 
     * manufacturer id and unique ID.
     * 
     * \param ManufacturerCode  Manufacturer code of the device to be 
     *                          searched for
     * \param UniqueNumber      Unique ID of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    const tNMEA2000::tDevice * FindDeviceByIDs(uint16_t ManufacturerCode, uint32_t UniqueNumber) const { return LocalFindDeviceByIDs(ManufacturerCode, UniqueNumber); }
    
    /********************************************************************//**
     * \brief Find a device in \ref Sources by the manufacturer and product
     *        code
     * 
     * Return device by manufacturer product code. Each device should 
     * have product code given by NMEA2000 organization. Search with 
     * source = 0xff finds first device. To find all devices with given
     * manufacturer product code, repeat search with found 
     * device source until device will not be found.
     * 
     * \param ManufacturerCode  Manufacturer code of the device to be 
     *                          searched for
     * \param ProductCode       Product code of the device to be 
     *                          searched for
     * \param Source  Source address of the device to be searched for
     * \return tN2kDeviceList::tInternalDevice* 
     */
    const tNMEA2000::tDevice * FindDeviceByProduct(uint16_t ManufacturerCode, uint16_t ProductCode, uint8_t Source=0xff) const { return LocalFindDeviceByProduct(ManufacturerCode, ProductCode, Source); }
    
    /************************************************************************//**
     * \brief Check if device list has updated.
     * 
     * Device list will be automatically updated. In stable system list 
     * should be ready and stable in few seconds. If you add device on the
     * fly, list will be updated as soon as it start to send data to the bus.
     * 
     * \return true 
     * \return false 
     */
    bool ReadResetIsListUpdated() { if ( ListUpdated ) { ListUpdated=false; return true; } else { return false; } }

    /************************************************************************//**
     * \brief Return number of known devices in \ref Sources
     *
     * \return Number of devices
     */
    uint8_t Count() const;
};

#endif
