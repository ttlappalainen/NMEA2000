/*
 * N2kGroupFunction.h

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

/************************************************************************//**
 * \file    N2kGroupFunction.h
 * \brief   Base and default group function PGN 126208 message handler class.
 * 
 * Group functions can be used for requesting, commanding and configuring other
 * bus devices. Example setting depth sounder offset can be done by command group
 * function. This is standard way and other devices should support it. Other common
 * examples are changing device or system information, setting installation description,
 * changing message period.
 * 
 * There are 7 different group functions defined by first field of PGN 126208
 * - Request group funtion is used to request specific transmit PGN from device.
 * - Command group function can be used to command device to change fields.
 * - Acknowledge group function is respond for for other devices command group function or
 *  request, read or write fields group function in case of error.
 * - Read fields group function is to request specific field from specific PGN from device.
 * - Read fields reply group function is for responding read fields group function request.
 * - Write fields group function is to write specific field on specific PGN on device.
 * - Write fields reply group function is for responding write fields group function.
 * 
 * Request group function is mandatory for all certified devices. The default handler can 
 * only respond "unsupported" to requests other than system PGNs. To fulfill certification
 * requirement developer should inherit base class, write supported functions and register
 * handler class to library with tNMEA2000::AddGroupFunctionHandler(). One can find examples for
 * implementation from N2kGroupFunctionDefaultHandlers.h. See e.g., tN2kGroupFunctionHandlerForPGN126998.
 * 
 * Library has inherited default handlers for required system PGNs declared in
 * N2kGroupFunctionDefaultHandlers.h . Those can be used as sample for other
 * PGN group function handlers.
 * 
 * \sa [NMEA2000 Documentation PGN 126208 ] (https://web.archive.org/web/20170609033039/http://www.nmea.org/Assets/20140109%20nmea-2000-corrigendum-tc201401031%20pgn%20126208.pdf)
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
         * response to a Request, Read fields or Write fields Group Function that cannot be 
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
                            /** Acknowledge positive, no error */
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
                            /** Acknowledge positive, no error */
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
                            /** Acknowledge positive, no error */
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
 * \brief   Base handler class for Group Functions
 * \ingroup group_coreSupplementary
 * 
 * This class handles all functions which are needed to respond to group 
 * function messages. NMEA 2000 definition requires that devices should 
 * respond group function messages. This class is base class and works also as default handler, which 
 * simply responds “unsupported” for all group functions. To fulfill certification
 * requirement developer should inherit this base class, write supported functions and register
 * handler class to library with tNMEA2000::AddGroupFunctionHandler(). One can find examples for
 * implementation from N2kGroupFunctionDefaultHandlers.h. See e.g., tN2kGroupFunctionHandlerForPGN126998.
 *
 * Library has inherited default handlers for required system PGNs declared in
 * N2kGroupFunctionDefaultHandlers.h . Those can be used as sample for other
 * PGN group function handlers.
 * 
 * NMEA 2000 library calls each registered handler class until some responses to \ref Handle
 * call true, meaning that it handled request. After get true return library stops requesting
 * handling for rest registered handler classes.
 * 
 * \note If your device transmits several e.g., 130316, you must handle them with single inherited
 * handler. This is because e.g., if some device request 130316 from your device without parameters,
 * you can not return true, since then library does not handle others and you can not return false,
 * since then at end library responses with default acknowledge group function with "unsupported".
 */
class tN2kGroupFunctionHandler {
  public:
    /**********************************************************************//**
     * \brief Template class to test does field on group function match to PGN field value
     * 
     * Some group functions contains filter fields which should match to requested/commanded 
     * PGN field. E.g., you may receive request group function globally addressed (255)
     * with some group of field filters. Before you respond request all fields must
     * match to your PGN. See example tN2kGroupFunctionHandlerForPGN126996::HandleRequest().
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
     * \brief Class to test does string field on group function match to PGN field value
     *
     * Some group functions contains filter fields which should match to requested/commanded 
     * PGN field. E.g., you may receive request group function globally addressed (255)
     * with some group of field filters. Before you respond request all fields must
     * match to your PGN. See example tN2kGroupFunctionHandlerForPGN126996::HandleRequest().
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
     * \brief Get request group function transmission or priority error code
     * 
     * Function for resolving transmission or priority error code in tN2kGroupFunctionHandler::HandleRequest
     * function according requested input and interval/offset limits requested PGN accepts.
     * Function resolves only error code. If result is N2kgfTPec_Acknowledge you have to
     * handle also possible change for interval/offset elsewhere on your inherited HandleRequest.
     * 
     * If you support changing for interval and/or offset for your PGN simply use function
     * as is. If you need to set offset limits set UseIntervalLimits and/or UseOffsetLimits
     * and provide limits. For special handling you can override function.
     * 
     * \note On NMEA certification tests "C.3.13.2  Expanded Acknowledgment 
     * Message Timing" old tool does not know interval 0xFFFFFFFE=Restore Default Interval
     * and so fails test. Use new test tool sw version or comment that out.
     * 
     * **See example of using function on**
     *  - \ref tN2kGroupFunctionHandlerForPGN126993::HandleRequest
     *  - \ref tN2kGroupFunctionHandlerForPGN126996::HandleRequest
     * 
     *  
     * \param TransmissionInterval    Interval for Transmission
     * \param TransmissionIntervalOffset Offset for Transmission Interval
     * \param UseIntervalLimits       Use the Interval limits
     * \param IntervalMax             Maximum Interval
     * \param IntervalMin             Minimum Interval
     * \param UseOffsetLimits         Use Offset limits 
     * \param OffsetMax               Maximum offset
     * 
     * \retval N2kgfTPec_Acknowledge  Both interval or offset are within limits or requested
     *                                to keep current or restore default.
     * \retval N2kgfTPec_TransmitIntervalOrPriorityNotSupported For other cases. HandleRequest
     *                                should not change requested PGN interval or offset and should
     *                                return acknowledge group function with error code.
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
     * \brief Handle response for request group function.
     *
     * Default response is "not supported". Certified devices must have inherited
     * handler and respond for requests for all device transmit PGNs.
     * 
     * Use \ref GetRequestGroupFunctionTransmissionOrPriorityErrorCode to get error code
     * for transmission interval/offset request. If that returns N2kgfTPec_Acknowledge, modify
     * your PGN transmission interval/offset according to request.
     * 
     * Handler must returns true, if it handled request, false otherwise.
     * 
     * **See example of overrided handler on**
     *  - \ref tN2kGroupFunctionHandlerForPGN60928::HandleRequest
     * 
     * \param N2kMsg                      Reference to request group function message (PGN 126208)
     * \param TransmissionInterval        Transmission interval [ms]
     * \param TransmissionIntervalOffset  Offset to the transmission 
     *                                    interval [10ms]
     * \param NumberOfParameterPairs      Number of parameter pairs contained
     *                                    inside the group function message
     * \param iDev        Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true  If request was handled.
     * \retval false If request was not handled.
     */
    virtual bool HandleRequest(const tN2kMsg &N2kMsg,
                               uint32_t TransmissionInterval,
                               uint16_t TransmissionIntervalOffset,
                               uint8_t  NumberOfParameterPairs,
                               int iDev);
                               
    /**********************************************************************//**
     * \brief Handle response for command group function
     *
     * Default response is "not supported". If you support command group function
     * for your transmit PGN, inherit default handler and override HandleCommand for it.
     *  
     * Handler must returns true, if it handled command, false otherwise.
     *  
     *  **See example of overrided handler on**
     *  - \ref tN2kGroupFunctionHandlerForPGN60928::HandleCommand
     * 
     * \param N2kMsg                  Reference to command group function message (PGN 126208)
     * \param PrioritySetting         Priority Setting
     * \param NumberOfParameterPairs  Number of parameter pairs contained
     *                                inside the group function message
     * \param iDev        Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true  If command was handled.
     * \retval false If command was not handled.
     */
    virtual bool HandleCommand(const tN2kMsg &N2kMsg, uint8_t PrioritySetting, uint8_t NumberOfParameterPairs, int iDev);

    /**********************************************************************//**
     * \brief Default handle function for acknowledge group function
     * 
     * This function handles Acknowledge group function, which is response 
     * for Request, Command, ReadFields or WriteFields group function. 
     * 
     * \note As default, this simply returns true meaning that received
     *       acknowledge has been handled. So you need to override this,
     *       if your device will send one of group functions to other devices.
     * 
     * \param N2kMsg                     Reference to acknowledge group function message (PGN 126208)
     * \param PGNErrorCode               PGN Error Code, see \ref tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode  see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs     Number of parameter pairs contained
     *                                   inside the group function message
     * \param iDev                       Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true  If acknowledge was handled.
     * \retval false If acknowledge was not handled.
     *
     */
    virtual bool HandleAcknowledge(const tN2kMsg &N2kMsg,
                                   tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                   tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                   uint8_t NumberOfParameterPairs,
                                   int iDev);

    /**********************************************************************//**
     * \brief Handle response for read fields group function
     *
     * Default response is "not supported". If you support read fields group function
     * for your transmit PGN, inherit default handler and override HandleReadFields for it.
     *  
     * Handler must returns true, if it handled read fields, false otherwise.
     * 
     * \param N2kMsg                   Reference to read fields group function message (PGN 126208)
     * \param ManufacturerCode         Manufacturer Code, This will be set to 
     *                                 0xffff for non-propprietary PNGs
     * \param IndustryGroup            Industry Group Code, This will be set to 
     *                                 0xff for non-propprietary PNGs
     * \param UniqueID                 Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param iDev                     Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true  If read fields was handled.
     * \retval false If read fields was not handled.
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
     * \brief Handle read fields reply group function
     *
     * \note As default, this simply returns true meaning that received
     *       read fields reply has been handled. So you need to override this,
     *       if your device will send read fields group functions to other devices.
     * 
     * \param N2kMsg        Reference to read fields reply group function message (PGN 126208)
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true  If read fields reply was handled.
     * \retval false If read fields reply was not handled.
     */
    virtual bool HandleReadFieldsReply(const tN2kMsg &N2kMsg,int iDev);

    /**********************************************************************//**
     * \brief Handle response for write fields group function
     *
     * Default response is "not supported". If you support write fields group function
     * for your transmit PGN, inherit default handler and override HandleWriteFields for it.
     *  
     * Handler must returns true, if it handled write fields, false otherwise.
     * 
     * \param N2kMsg              Reference to write fields group function message (PGN 126208)
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
     * \retval true  If write fields was handled.
     * \retval false If write fields was not handled.
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
     * \brief Handle write fields reply group function
     *
     * \note As default, this simply returns true meaning that received
     *       write fields reply has been handled. So you need to override this,
     *       if your device will send write fields group functions to other devices.
     * 
     * \param N2kMsg        Reference to write fields reply group function message (PGN 126208)
     * \param iDev          Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true    Always
     */
    virtual bool HandleWriteFieldsReply(const tN2kMsg &N2kMsg,int iDev);

  public:
    /**********************************************************************//**
     * \brief Construct a new tN2kGroupFunctionHandler object
     *
     * \param _pNMEA2000  Pointer to an NMEA2000 object, see \ref tNMEA2000
     * \param _PGN        Parameter Group Number associated with this 
     *                    Group function
     */
    tN2kGroupFunctionHandler(tNMEA2000 *_pNMEA2000, unsigned long _PGN);

    /**********************************************************************//**
     * \brief Handle group function message
     * 
     * Function parses group function PGN 126208 and forwards handling to specific
     * handler like \ref HandleRequest. This simplifies code for user inherited
     * tN2kGroupFunctionHandler class, since often it is enough just override HandleRequest
     * and let other handlers use default response.
     * 
     * \sa
     *  - \ref HandleRequest
     *  - \ref HandleCommand
     *  - \ref HandleAcknowledge
     *  - \ref HandleReadFields
     *  - \ref HandleReadFieldsReply
     *  - \ref HandleWriteFields
     *  - \ref HandleWriteFieldsReply
     *
     * \param N2kMsg                Reference to a N2kMsg Object, 
     *                              This message is an Group function message
     * \param GroupFunctionCode     Code for Group Function, see 
     *                              \ref tN2kGroupFunctionCode
     * \param PGNForGroupFunction    PGN for the Group function
     * \param iDev                  Index off the device in \ref tNMEA2000::Devices
     * 
     * \retval true    Group Function was handled properly 
     * \retval false   if (PGN!=PGNForGroupFunction && PGN!=0) or group function was not handled.
     */
    virtual bool Handle(const tN2kMsg &N2kMsg, tN2kGroupFunctionCode GroupFunctionCode, unsigned long PGNForGroupFunction, int iDev);

    /**********************************************************************//**
     * \brief Get requested/commanded etc. PGN from group function message
     * 
     * This is a static function for PGN 126208 handling. The 
     * function reads PGN (as 3 bytes) from the group 
     * function message.
     *
     * \param N2kMsg            Reference to a N2kMsg Object.
     *                          This message must be PGN 126208 group function message
     * \return unsigned long -> Requested/commanded etc. PGN for Group Function.
     *                          If N2kMsg is not PGN 126208, result can be anything.
     */
    static unsigned long GetPGNForGroupFunction(const tN2kMsg &N2kMsg);

    /**********************************************************************//**
     * \brief Parse group function code and requested/commanded etc. PGN from group function message
     *
     * This is a static function for PGN 126208 handling.
     * 
     * \param N2kMsg              Reference to a N2kMsg Object.
     *                            This message should be PGN 126208 group function message
     * \param GroupFunctionCode   Group Function Code, see \ref 
     *                            tN2kGroupFunctionCode
     * \param PGNForGroupFunction PGN for the Group function
     * 
     * \retval true 
     * \retval false   if (N2kMsg.PGN!=126208L)
     */
    static bool Parse(const tN2kMsg &N2kMsg,
                            tN2kGroupFunctionCode &GroupFunctionCode,
                            unsigned long &PGNForGroupFunction);

    /**********************************************************************//**
     * \brief Parse parameters from request group function message
     *
     * This is a static function for PGN 126208 handling.
     * 
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param TransmissionInterval     Transmission interval
     * \param TransmissionIntervalOffset Offset to the transmission interval
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \retval true 
     * \retval false   if (N2kMsg.PGN!=126208L)
     */
    static bool ParseRequestParams(const tN2kMsg &N2kMsg,
                               uint32_t &TransmissionInterval,
                               uint16_t &TransmissionIntervalOffset,
                               uint8_t  &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start Index of pair parameters from request group function message
     *
     * This is a static function for PGN 126208 handling. 
     *
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param Index                    Index where the Request Pair Parameters start
     * 
     * \retval true 
     * \retval false  if (N2kMsg.PGN!=126208L)
     */
    static bool StartParseRequestPairParameters(const tN2kMsg &N2kMsg, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from command group function message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param PrioritySetting          Priority setting
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \retval true 
     * \retval false   if (N2kMsg.PGN!=126208L)
     */
    static bool ParseCommandParams(const tN2kMsg &N2kMsg,
                               uint8_t &PrioritySetting,
                               uint8_t &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start Index of pair parameters from command group function message
     *
     * This is a static function for PGN 126208 handling. 
     *
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param Index                    Index where the Command Pair Parameters start
     * 
     * \retval true 
     * \retval false    if (N2kMsg.PGN!=126208L)
     */
    static bool StartParseCommandPairParameters(const tN2kMsg &N2kMsg, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from acknowledge group function message
     *
     * This is a static function for PGN 126208 handling. 
     * 
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param PGNErrorCode             PGN error code, see \ref tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * 
     * \retval true 
     * \retval false    if (N2kMsg.PGN!=126208L)
     */
    static bool ParseAcknowledgeParams(const tN2kMsg &N2kMsg,
                               tN2kGroupFunctionPGNErrorCode &PGNErrorCode,
                               tN2kGroupFunctionTransmissionOrPriorityErrorCode &TransmissionOrPriorityErrorCode,
                               uint8_t &NumberOfParameterPairs);

    /**********************************************************************//**
     * \brief Get start index of pair parameters on the read or write group function message
     *
     * This is a static function for PGN 126208 read or write fields group function handling. 
     *
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param Proprietary              Group function is proprietary
     * \param Index                    Index where the Command Pair Parameters start
     * 
     * \retval true 
     */
    static bool StartParseReadOrWriteParameters(const tN2kMsg &N2kMsg, bool Proprietary, int &Index);

    /**********************************************************************//**
     * \brief Parse parameters from read or write group function message
     *
     * \param N2kMsg                   Reference to a N2kMsg Object.
     *                                 This message should be PGN 126208 group function message
     * \param ManufacturerCode         Manufacturer Code, This will be set to 
     *                                 0xffff for non-propprietary PNGs
     * \param IndustryGroup            Industry Group Code, This will be set to 
     *                                 0xff for non-propprietary PNGs
     * \param UniqueID                 Unique ID for the device
     * \param NumberOfSelectionPairs   Number of Selection pairs contained
     *                                 inside the group function message
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     * \param Proprietary Group function is proprietary
     * 
     * \retval true 
     * \retval false   if (N2kMsg.PGN!=126208L)
     */
    static bool ParseReadOrWriteParams(const tN2kMsg &N2kMsg,
                               uint16_t &ManufacturerCode,
                               uint8_t &IndustryGroup,
                               uint8_t &UniqueID,
                               uint8_t &NumberOfSelectionPairs,
                               uint8_t &NumberOfParameterPairs,
                               bool Proprietary=false);
    
    /**********************************************************************//**
     * \brief Setup start parameters for read reply group function message
     * 
     * This is a static function for PGN 126208 read fields group function handling. 
     *
     * \param N2kMsg                   Input: Reference to a N2kMsg Object \n
     *                                 Output: Read reply group function message
     *                                 prepared with start parameters.
     * \param Destination              Destination source address 
     * \param PGN                      PGN handled
     * \param ManufacturerCode         Manufacturer Code, This will be set to 
     *                                 0xffff for non-propprietary PNGs
     * \param IndustryGroup            Industry Group Code, This will be set to 
     *                                 0xff for non-propprietary PNGs
     * \param UniqueID                 Unique ID for the device
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
     * \brief Setup start parameters for write reply group function message
     * 
     * This is a static function for PGN 126208 write fields group function handling. 
     *
     * \param N2kMsg                   Input: Reference to a N2kMsg Object \n
     *                                 Output: Write reply group function message
     *                                 prepared with start parameters.
     * \param Destination              Destination source address
     * \param PGN                      PGN handled
     * \param ManufacturerCode         Manufacturer Code, This will be set to 
     *                                 0xffff for non-propprietary PNGs
     * \param IndustryGroup            Industry Group Code, This will be set to 
     *                                 0xff for non-propprietary PNGs
     * \param UniqueID                 Unique ID for the device
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
     * This is a static function for setting up start of PGN 126208 acknowledge group function. 
     * 
     * \param N2kMsg                   Input: Reference to a N2kMsg Object \n
     *                                 Output: Acknowledge group function message
     *                                 prepared with start parameters.
     * \param Destination              Destination source address
     * \param PGN                      PGN handled
     * \param PGNErrorCode             PGN error code, see \ref tN2kGroupFunctionPGNErrorCode
     * \param TransmissionOrPriorityErrorCode Transmission or Priority error
     *                                 code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     * \param NumberOfParameterPairs   Number of parameter pairs contained
     *                                 inside the group function message
     */
    static void SetStartAcknowledge(tN2kMsg &N2kMsg, unsigned char Destination, unsigned long PGN,
                                         tN2kGroupFunctionPGNErrorCode PGNErrorCode,
                                         tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode,
                                         uint8_t NumberOfParameterPairs=0);

    /**********************************************************************//**
     * \brief Change the PGN error code for a acknowledge group function message
     *
     * This is a static function for changing PGN error code of prepared
     * PGN 126208 acknowledge group function.
     * 
     * \param N2kMsg                   Input: Prepared acknowledge group function message \n
     *                                 Output: Acknowledge group function message
     *                                 with updated PGN error code.
     * \param PGNErrorCode  PGN error code, see \ref tN2kGroupFunctionPGNErrorCode
     */
    static void ChangePNGErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionPGNErrorCode PGNErrorCode);

    /**********************************************************************//**
     * \brief Change transmission interval or priority error code for 
     *        acknowledge group function message
     *
     * This is a static function for changing transmission interval or priority error code of prepared
     * PGN 126208 acknowledge group function.
     * 
     * \param N2kMsg                   Input: Prepared acknowledge group function message \n
     *                                 Output: Acknowledge group function message
     *                                 with updated transmission interval or priority error code.
     * \param TransmissionOrPriorityErrorCode Transmission interval or priority error code
     *     code, see \ref tN2kGroupFunctionTransmissionOrPriorityErrorCode
     */
    static void ChangeTransmissionOrPriorityErrorCode(tN2kMsg &N2kMsg, tN2kGroupFunctionTransmissionOrPriorityErrorCode TransmissionOrPriorityErrorCode);

    /**********************************************************************//**
     * \brief Add parameter error code to acknowledge group function message
     *
     * This is a static function for adding parameter error code to prepared
     * PGN 126208 acknowledge group function.
     * 
     * \param N2kMsg                   Input: Prepared acknowledge group function message \n
     *                                 Output: Acknowledge group function message
     *                                 with added parameter error code.
     * \param ParameterPairIndex Index of the parameter pair
     * \param ErrorCode   Error code to be added to the message
     */
    static void AddAcknowledgeParameter(tN2kMsg &N2kMsg,
                                         uint8_t ParameterPairIndex,
                                         tN2kGroupFunctionParameterErrorCode ErrorCode=N2kgfpec_ReadOrWriteIsNotSupported);

    /**********************************************************************//**
     * \brief Send out an acknowledge group function message 
     * 
     * Function builds simple acknowledge group function message with given parameters.
     * This is useful for sending response for no parameters or parameters with same
     * error code.
     *
     * \param pNMEA2000                NMEA2000 object
     * \param Destination              Destination source address
     * \param iDev                     Index off the device in \ref tNMEA2000::Devices
     * \param PGN                      PGN handled
     * \param PGNErrorCode             PGN error code, see \ref tN2kGroupFunctionPGNErrorCode
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
