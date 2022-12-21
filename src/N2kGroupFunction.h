/*
 * N2kGroupFunction.h

 * Copyright (c) 2015-2022 Timo Lappalainen, Kave Oy, www.kave.fi
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

/************************************************************************//**
 * \file    N2kGroupFunction.h
 * \brief   This File contains the definition of the GroupFunctionHandler
 * 
 * Group functions can be used on the bus, to organize the devices on 
 * the network (e.g. setting a device’s PGN instances)
 * 
 * \todo Please double check and give maybe some more context
 * 
 * \sa [NMEA2000 Documentation PGN 126208 ] (https://www.nmea.org/Assets/20140109%20nmea-2000-corrigendum-tc201401031%20pgn%20126208.pdf)
 * ********************************************************************/

#ifndef _N2kGroupFunction_H_
#define _N2kGroupFunction_H_

#ifndef ARDUINO
#include <cstring>
#endif
#include "NMEA2000_CompilerDefns.h"
#include "N2kMsg.h"

#if !defined(N2K_NO_GROUP_FUNCTION_SUPPORT)

#define N2k_KEEP_TRANSMISSION_INTERVAL 0xffffffff
#define N2k_RESTORE_TRANSMISSION_INTERVAL 0xfffffffe // Restore default
#define N2k_MAX_TRANSMISSION_INTERVAL 0xfffffff0
#define N2k_MAX_TRANSMISSION_INTERVAL_OFFSET 0xfffc

/************************************************************************//**
 * \enum  tN2kGroupFunctionCode
 * \brief FunctionCode for the group function
 * 
 * There are seven group functions associated with PGN 126208.  
 * The Write Fields group function may be used to modify a device’s
 * factory default PGN instances to create a unique device with unique 
 * PGN instance numbers.  
 * The Read Fields group function can be similarly used to interrogate 
 * the current PGN instance values of any PGNs not being sent periodically.
 *
 */
enum tN2kGroupFunctionCode {
        /**
         * This message requests the transmission of a specific set of data 
         * in a Parameter Group by setting variable parameters within 
         * theParameter Group specified by the field number. Field number 
         * and parameter value may appear in any order in this message. 
         * When multiple fields and parameters are specified the request is 
         * treated as an "AND" function. This PGN may be used to set the 
         * transmission interval and the delay before the first transmission.
         */
        N2kgfc_Request=0,
        /**
         * The Command Group Function message is directed to a specific 
         * address, the Global Address (255) shall not be used. This command 
         * sets the value of one, some or all parameters in a Parameter Group. 
         * The number of parameters to set is in field 5, then follows the 
         * field number and the new value repeated for each of them. A 
         * Parameter Group may contain one group of parameters out of multiple 
         * instances where the instance number of the group is given in one 
         * field. A command to set any parameter of such a group must contain 
         * the field number and value of the group instance number.
         */
        N2kgfc_Command=1,
        /**
         * The Acknowledgement reply is transmitted in response to a Request, 
         * Command, Read or Write Group Function Message. The Acknowledge Group 
         * Function shall be transmitted in response to every Command Message 
         * received, indicating acknowledgement or containing the appropriate 
         * Error Code. The Acknowledge Group Function is only required in 
         * response to a Request, Read orWrite Group Function that cannot be 
         * satisfied. The Acknowledge Group function shall transmit all fields 
         * applicable to the Group Function being acknowledged, fields where 
         * the error does not exist are set to 0x0 (No Error/Acknowledge).
         */
        N2kgfc_Acknowledge=2,
        /**
         * This Read Fields Group Function provides a means to read specific 
         * fields in a PGN. When Field 2 (PGN number) contains a 
         * non-proprietary PGN number, field 3, field 4 and field 5 are not 
         * included in this message. If the receiver of this message can 
         * comply, the receiver will send to the transmitter a Read Fields 
         * Reply Group Function. If the receiver cannot comply, an Acknowledge 
         * Group Function shall be sent.
         */
        N2kgfc_Read=3,
        /**
         * The Read Fields Reply Group Function is a reply to the Read Fields 
         * Group Function. When the Read Fields Group Function is received, if 
         * the receiver can comply, the Read Fields Reply Group Function will 
         * be transmitted with the resulting read values.
         */
        N2kgfc_ReadReply=4,
        /**
         * This Write Group Function is especially useful when configuring an 
         * instance or reference to be used by a node when transmitting a PGN 
         * such as PGN 127508. Use Fields 7, 9 and 10 to identify the currently 
         * assigned instance value. Use Fields 8, 13 and 14 to identify the new 
         * assigned instance value. When Field 2 (PGN number) contains a 
         * non-proprietary PGN number, field 3, field 4 and field 5 are not 
         * included in this message. If the receiver of this message can 
         * comply, the receiver will send to the transmitter a Write Fields 
         * Reply Group Function. If the receiver cannot comply, an Acknowledge 
         * Group Function shall be sent.
         */
        N2kgfc_Write=5,
        /**
         * The Write Fields Reply Group Function is a reply to the Write Fields 
         * Group Function. When the Write Fields Group Function is received, if 
         * the receiver can comply, the Write Fields Reply Group Function will 
         * be transmitted with the resulting values. If a parameter value is 
         * accepted then the modified value will be returned. If a parameter 
         * value is not accepted, the original value will be returned.
         */
        N2kgfc_WriteReply=6
      };

/************************************************************************//**
 * \enum  tN2kGroupFunctionPGNErrorCode
 * \brief PGN error code used by acknowledge group function
 * 
 * This error code carried inside the acknowledge group function gives 
 * information, if the Request, Command, Read or Write Group Function 
 * Message can be satisfied by the receiving device.
 */
enum tN2kGroupFunctionPGNErrorCode {
                            /** Acknowledge positiv, no error */
                            N2kgfPGNec_Acknowledge=0,
                            /** PGN is not supported */
                            N2kgfPGNec_PGNNotSupported=1,
                            /** PGN is temporarily not available */
                            N2kgfPGNec_PGNTemporarilyNotAvailable=2,
                            /** Access denied */
                            N2kgfPGNec_AccessDenied=3,
                            /** Request or Command is not supported */
                            N2kgfPGNec_RequestOrCommandNotSupported=4,
                            /** Definer Tag is not supported */
                            N2kgfPGNec_DefinerTagNotSupported=5,
                            /** Read or Write is not supported */
                            N2kgfPGNec_ReadOrWriteNotSupported=6
                          };

/************************************************************************//**
 * \enum  tN2kGroupFunctionTransmissionOrPriorityErrorCode
 * \brief Error codes Transmit interval used by acknowledge group function
 * 
 * This error code carried inside the acknowledge group function gives 
 * information, if the Request, Command, Read or Write Group Function 
 * Message can be satisfied by the receiving device.
 */
enum tN2kGroupFunctionTransmissionOrPriorityErrorCode {
                            /** Acknowledge positiv, no error */
                            N2kgfTPec_Acknowledge=0,
                            /** Transmit Interval /Priority not supported */
                            N2kgfTPec_TransmitIntervalOrPriorityNotSupported=1,
                            /** Transmit interval is less than measurement/
                             * calculation interval */
                            N2kgfTPec_TransmitIntervalIsLessThanMeasurementInterval=2,
                            /** Access denied */
                            N2kgfTPec_AccessDenied=3,
                            /** Request is not supported */
                            N2kgfTPec_RequestNotSupported=4
                          };

/************************************************************************//**
 * \enum  tN2kGroupFunctionParameterErrorCode
 * \brief Error Codes Command used by acknowledge group function
 * 
 * This error code carried inside the acknowledge group function gives 
 * information, if the Request, Command, Read or Write Group Function 
 * Message can be satisfied by the receiving device.
 */
enum tN2kGroupFunctionParameterErrorCode {
                            /** Acknowledge positiv, no error */
                            N2kgfpec_Acknowledge=0,
                            /** Invalid request or command parameter field */
                            N2kgfpec_InvalidRequestOrCommandParameterField=1,
                            /** Temporarily unable to comply */
                            N2kgfpec_TemporarilyUnableToComply=2,
                            /** Request or command parameter out-of-range */
                            N2kgfpec_RequestOrCommandParameterOutOfRange=3,
                            /** Access denied */
                            N2kgfpec_AccessDenied=4,
                            /** Request or Command is not supported */
                            N2kgfpec_RequestOrCommandNotSupported=5,
                            /** Read or Write is not supported */
                            N2kgfpec_ReadOrWriteIsNotSupported=6
                          };

class tNMEA2000;
/************************************************************************//**
 * \class   tN2kGroupFunctionHandler
 * \brief   Handler class for Group Functions
 * \ingroup group_coreSupplementary
 * 
 * This class handles all functions which are needed to respond to group 
 * function messages. NMEA 2000 definition requires that devices should 
 * respond group function messages. This class is default handler, which 
 * simply responds “unsupported” for all queries. 
 * 
 * \todo More description is needed, please review
 *
 */
class tN2kGroupFunctionHandler {
  public:
    /**********************************************************************//**
     * \brief 
     * 
     * \todo Add a proper documentation
     * 
     * \tparam T          
     * \param FieldVal 
     * \param MatchVal 
     * \param Mask 
     * \param Match 
     * \param ErrorCode 
     */
    template <typename T> void MatchRequestField(T FieldVal, T MatchVal, T Mask, bool &Match, tN2kGroupFunctionParameterErrorCode &ErrorCode)
    {
      if ( (FieldVal&Mask)!=MatchVal ) {
        ErrorCode=N2kgfpec_RequestOrCommandParameterOutOfRange;
        Match=false;
      } else ErrorCode=N2kgfpec_Acknowledge;
    }

    /*********************************************************************//**
     * \brief 
     *
     * \todo Add a proper documentation
     *
     * \param FieldVal {type} 
     * \param MatchVal {type} 
     * \param Match {type} 
     * \param ErrorCode {type} 
     */
    void MatchRequestField(const char * FieldVal, const char * MatchVal, bool &Match, tN2kGroupFunctionParameterErrorCode &ErrorCode)
    {
      Match&=(strcmp(FieldVal,MatchVal)==0);
      ErrorCode = ( Match ? N2kgfpec_Acknowledge : N2kgfpec_RequestOrCommandParameterOutOfRange );
    }

  private:
    /** \brief Pointer to the Group function handler */
    tN2kGroupFunctionHandler *pNext;
    friend class tNMEA2000;

  protected:
    /** \brief Parameter Group Number (PGN) of this Group Function*/
    unsigned long PGN;
    /** \brief Flag, if the Parameter Group is proprietary   */
    bool Proprietary;
    /** \brief NMEA2000 object in order to send messages to the bus   */
    tNMEA2000 *pNMEA2000;

  protected:
    // deprecated! Use new version with offset test too.
    // deprecated attribute does not work with overrided methods, so easier just force users to update code.
    // virtual tN2kGroupFunctionTransmissionOrPriorityErrorCode GetRequestGroupFunctionTransmissionOrPriorityErrorCode(uint32_t TransmissionInterval) __attribute__ ((deprecated));
  
    /**********************************************************************//**
     * \brief Get the Request Group Function Transmission Or Priority Error 
     *        Code object
     * 
     * This is default handler for Complex Request transmission interval 
     * setting. Overwrite it, if your PGN will support changing interval.  
     * If you support changing interval and offset for your PGN, you can
     * either overwrite function or set UseLimits and provide interval
     * and offset limits.
     * 
     * \note see Code  In NMEA tests "C.3.13.2  Expanded Acknowledgment 
     * Message Timing" tool is Old and does not know interval 
     * 0xFFFFFFFE=Restore Default Interval. So to pass that test, that has
     * to be commented out.
     *  
     * \param TransmissionInterval    Interval for Transmission
     * \param TransmissionIntervalOffset Offset for Transmission Interval
     * \param UseIntervalLimits       Use the Intervall limits  
     * \param IntervalMax             Maximum Interval
     * \param IntervalMin             Minimum Interval
     * \param UseOffsetLimits         Use Offset limits 
     * \param OffsetMax               Maximum offset
     * 
     * \return N2kgfTPec_Acknowledge -> if TransmissionInterval = 0xFFFFFFFF 
     *                                or TransmissionInterval = 0xFFFFFFFE
     *                                (Restore Default Interval)
     * \return N2kgfTPec_TransmitIntervalOrPriorityNotSupported -> all other
     *                                                            intervals
     */
     virtual tN2kGroupFunctionTransmissionOrPriorityErrorCode GetRequestGroupFunctionTransmissionOrPriorityErrorCode(
                              uint32_t TransmissionInterval,
                              uint16_t TransmissionIntervalOffset,
                              bool UseIntervalLimits=false,
                              uint32_t IntervalMax=N2k_MAX_TRANSMISSION_INTERVAL,
                              uint32_t IntervalMin=50,
                              bool UseOffsetLimits=false,
                              uint16_t OffsetMax=N2k_MAX_TRANSMISSION_INTERVAL_OFFSET
                              );
    
    /**********************************************************************//**
     * \brief Default request handler for group function requests for PGN.
     *
     * Default response is "not supported". Certified devices must respond
     * to requests!
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param TransmissionInterval    Transmission interval [ms]
     * \param TransmissionIntervalOffset  Offset to the transmission 
     *                                    interval [10ms]
     * \param NumberOfParameterPairs  Number of parameter pairs contained
     *                                inside the group function message
     * \param iDev        Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true -> always returns true
     */
    virtual bool HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t TransmissionIntervalOffset,
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
                               
    /**********************************************************************//**
     * \brief Handle the response to Group Function "Command"
     *
     * Default response is "not supported". 
     *  
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param PrioritySetting     Priority Setting
     * \param NumberOfParameterPairs  Number of parameter pairs contained
     *                                inside the group function message
     * \param iDev        Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true -> always returns true
     */
    virtual bool HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev);

    /**********************************************************************//**
     * \brief Default handle function for Acknowledge a Group Function
     * 
     * This function handles Acknowledge group function, which is response 
     * for Request, Command, ReadFields or WriteFields group function. 
     * 
     * \note As default, this simply returns true meaning that received
     *       acknowledge has been handled. So you need to override this,
     *        if your device will send one of those commands.
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param PGNErrorCode  PGN Error Code, see 
     *                      \ref tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode   see 
     *                  \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs     Number of parameter pairs contained
     *                                   inside the group function message
     * \param iDev        Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true -> As default, simply return true meaning that received
     *                acknowledge has been handled
     *
     */
    virtual bool HandleAcknowledge(const tN2kMsg &N2kMsg,
                                   tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                   tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                   uint8_t NumberOfParameterPairs,
                                   int iDev);

    /**********************************************************************//**
     * \brief Handle the response to Group Function "Read Fields"
     *
     * 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param ManufacturerCode    Manufacturer Code, This will be set to 
     *                            0xffff for non-propprietary PNGs
     * \param IndustryGroup       Industry Group Code, This will be set to 
     *                            0xff for non-propprietary PNGs
     * \param UniqueID            Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true   ->  always returns true
     * 
     */
    virtual bool HandleReadFields(const tN2kMsg &N2kMsg,
                                  uint16_t ManufacturerCode, 
                                  uint8_t IndustryGroup,
                                  uint8_t UniqueID,
                                  uint8_t NumberOfSelectionPairs,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev);
    
    /**********************************************************************//**
     * \brief Handle the response to Group Function "Read Fields Reply"
     *
     * \warning Under construction! No real code inside, always returns true!
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true -> Always
     */
    virtual bool HandleReadFieldsReply(const tN2kMsg &N2kMsg,int iDev);

    /**********************************************************************//**
     * \brief Handle the response to Group Function "Write Fields"
     *
     * 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param ManufacturerCode    Manufacturer Code, This will be set to 
     *                            0xffff for non-propprietary PNGs
     * \param IndustryGroup       Industry Group Code, This will be set to 
     *                            0xff for non-propprietary PNGs
     * \param UniqueID            Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true   ->  always returns true
     * 
     */
    virtual bool HandleWriteFields(const tN2kMsg &N2kMsg,
                                  uint16_t ManufacturerCode, 
                                  uint8_t IndustryGroup, 
                                  uint8_t UniqueID,
                                  uint8_t NumberOfSelectionPairs,
                                  uint8_t NumberOfParameterPairs,
                                  int iDev);

    /**********************************************************************//**
     * \brief Handle the response to Group Function "Write Fields Reply"
     *
     * \warning Under construction! No real code inside, always returns true!
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true -> Always
     */
    virtual bool HandleWriteFieldsReply(const tN2kMsg &N2kMsg,int iDev);

  public:
    /**********************************************************************//**
     * \brief Construct a new t N2k Group Function Handler object
     *
     * \param _pNMEA2000  Pointer to an NMEA2000 object, see \ref tNMEA2000
     * \param _PGN        Parameter Group Number associated with this 
     *                    Group function
     */
    tN2kGroupFunctionHandler(tNMEA2000 *_pNMEA2000, unsigned long _PGN);

    /**********************************************************************//**
     * \brief Handle for a Group Function
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param GroupFunctionCode     Code for Group Function, see 
     *                              \ref tN2kGroupFunctionCode
     * \param PGNForGroupFunction    PGN for the Group function
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \return true   -> Group Function was handled properly 
     * \return false  -> if (PGN!=PGNForGroupFunction && PGN!=0)
     */
    virtual bool Handle(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev);

    /**********************************************************************//**
     * \brief Get the PGN for the Group Function out of a n2k message
     * 
     * This is a static function for PGN 126208 handling. The 
     * function extracts the PGN (as 3 bytes) from the given 
     * N2K message.
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \return unsigned long -> PGN for Group Function
     */
    static unsigned long GetPGNForGroupFunction(const tN2kMsg &N2kMsg);

    /**********************************************************************//**
     * \brief Parse group function code and PGN from the message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param GroupFunctionCode   Group Function Code, see \ref 
     *                            tN2kGroupFunctionCode
     * \param PGNForGroupFunction PGN for the Group function
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool Parse(const tN2kMsg &N2kMsg,
                            tN2kGroupFunctionCode &GroupFunctionCode,
                            unsigned long &PGNForGroupFunction);

    /**********************************************************************//**
     * \brief Parse parameters from group function Request message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param TransmissionInterval    Transmission interval
     * \param TransmissionIntervalOffset Offset to the transmission interval
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool ParseRequestParams(const tN2kMsg &N2kMsg,
                               uint32_t &TransmissionInterval,
                               uint16_t &TransmissionIntervalOffset,
                               uint8_t  &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start Index of pair parameters on the group function 
     * Request message
     *
     * This is a static function for PGN 126208 handling. 
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param Index       Index where the Request Pair Parameters start
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool StartParseRequestPairParameters(const tN2kMsg &N2kMsg, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from a group function Command message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param PrioritySetting     Priority setting
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool ParseCommandParams(const tN2kMsg &N2kMsg,
                               uint8_t &PrioritySetting,
                               uint8_t &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start Index of pair parameters on the group function 
     * Command message
     *
     * This is a static function for PGN 126208 handling. 
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param Index       Index where the Command Pair Parameters start
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool StartParseCommandPairParameters(const tN2kMsg &N2kMsg, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from group function acknowledge message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param PGNErrorCode  PGN error code, see \ref 
     *                      tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool ParseAcknowledgeParams(const tN2kMsg &N2kMsg,
                               tN2kGroupFunctionPGNErrorCode &PGNErrorCode,
                               tN2kGroupFunctionTransmissionOrPriorityErrorCode &TransmissionOrPriorityErrorCode,
                               uint8_t &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start Index of pair parameters on the group function 
     * ReadOrWrite message
     *
     * This is a static function for PGN 126208 handling. 
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param Proprietary Group function is proprietary
     * \param Index       Index where the Command Pair Parameters start
     * 
     * \return true 
     */
    static bool StartParseReadOrWriteParameters(const tN2kMsg &N2kMsg, bool Proprietary, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from group function ReadOrWrite message
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param ManufacturerCode    Manufacturer Code, This will be set to 
     *                            0xffff for non-propprietary PNGs
     * \param IndustryGroup       Industry Group Code, This will be set to 
     *                            0xff for non-propprietary PNGs
     * \param UniqueID            Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param Proprietary Group function is proprietary
     * 
     * \return true 
     * \return false -> if (N2kMsg.PGN!=126208L)
     */
    static bool ParseReadOrWriteParams(const tN2kMsg &N2kMsg,
                               uint16_t &ManufacturerCode,
                               uint8_t &IndustryGroup,
                               uint8_t &UniqueID,
                               uint8_t &NumberOfSelectionPairs,
                               uint8_t &NumberOfParameterPairs,
                               bool Proprietary=false);
    
    /**********************************************************************//**
     * \brief Setting up the group function message for Read Reply
     * 
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param Destination address of Destination 
     * \param PGN         Parameter group Number
     * \param ManufacturerCode    Manufacturer Code, This will be set to 
     *                            0xffff for non-propprietary PNGs
     * \param IndustryGroup       Industry Group Code, This will be set to 
     *                            0xff for non-propprietary PNGs
     * \param UniqueID            Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param Proprietary Is this a proprietary group function
     */
    static void SetStartReadReply(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                               uint16_t ManufacturerCode,
                               uint8_t IndustryGroup,
                               uint8_t UniqueID,
                               uint8_t NumberOfSelectionPairs,
                               uint8_t NumberOfParameterPairs,
                               bool Proprietary);

    /**********************************************************************//**
     * \brief Setting up the group function message for Write Reply
     * 
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param Destination address of Destination 
     * \param PGN         Parameter group Number
     * \param ManufacturerCode    Manufacturer Code, This will be set to 
     *                            0xffff for non-propprietary PNGs
     * \param IndustryGroup       Industry Group Code, This will be set to 
     *                            0xff for non-propprietary PNGs
     * \param UniqueID            Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param Proprietary Is this a proprietary group function
     */
    static void SetStartWriteReply(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                               uint16_t ManufacturerCode,
                               uint8_t IndustryGroup,
                               uint8_t UniqueID,
                               uint8_t NumberOfSelectionPairs,
                               uint8_t NumberOfParameterPairs,
                               bool Proprietary);

    /**********************************************************************//**
     * \brief Setting up the group function message for Acknowledge
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    Output: NMEA2000 message ready to be send.
     * \param Destination address of Destination 
     * \param PGN         Parameter group Number
     * \param PGNErrorCode  PGN error code, see \ref 
     *                      tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     */
    static void SetStartAcknowledge(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs=0);

    /**********************************************************************//**
     * \brief Change the PGN error code for a group function message
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param PGNErrorCode  PGN error code, see \ref 
     *                      tN2kGroupFunctionPGNErrorCode
     */
    static void ChangePNGErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionPGNErrorCode PGNErrorCode);

    /**********************************************************************//**
     * \brief Change the Transmission or Priority error code code for 
     *        a group function message
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     */
    static void ChangeTransmissionOrPriorityErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode);

    /**********************************************************************//**
     * \brief Add parameter to a Acknowledge group function message
     *
     * \param N2kMsg      Reference to a N2kMsg Object, 
     *                    This message is an Group function message
     * \param ParameterPairIndex Index of the parameter pair
     * \param ErrorCode   Error code to be added to the message
     */
    static void AddAcknowledgeParameter(tN2kMsg &N2kMsg,
                                         uint8_t ParameterPairIndex,
                                         tN2kGroupFunctionParameterErrorCode ErrorCode=N2kgfpec_ReadOrWriteIsNotSupported);

    /**********************************************************************//**
     * \brief Send out an Acknowledge message 
     *
     * \param pNMEA2000     NMEA2000 oject
     * \param Destination   address of Destination 
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * \param PGN           Parameter group Number
     * \param PGNErrorCode  PGN error code, see \ref 
     *                      tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param ParameterErrorCodeForAll Error code for all parameter sets, see 
     *                                 \ref tN2kGroupFunctionParameterErrorCode
     */
    static void SendAcknowledge(tNMEA2000 *pNMEA2000, unsigned char Destination, int iDev, unsigned long PGN,
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs=0,
                                         tN2kGroupFunctionParameterErrorCode ParameterErrorCodeForAll=N2kgfpec_Acknowledge);

};

#endif

#endif
