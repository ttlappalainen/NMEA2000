/*
 * ActisenseReader.h
 *
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

/**************************************************************************//**
 * \file  ActisenseReader.h
 * \brief File contains declaration for tActisenseReader class for reading Actisense format messages from stream.
 * 
 * This is class for reading Actisense format messages from given stream and convert
 * it to tN2kMsg. Converted tN2kMsg message can be then sent to the NMEA2000 bus
 * with tNMEA2000::SendMsg();
 * 
 * \note There is an unresolved problem to use programming port with reading 
 * data. Read works fine for a while, but then stops. With e.g. Arduino Due
 * USB port there is no problem.
 * 
*/
#ifndef _ACTISENSE_READER_H_
#define _ACTISENSE_READER_H_

#include "N2kMsg.h"
#include "N2kStream.h"

/************************************************************************//**
 * \class tActisenseReader
 * \brief Class for reading Actisense format messages
 * \ingroup group_helperClass
 * 
 * This is class for reading Actisense format messages from given stream and convert
 * it to tN2kMsg. Converted tN2kMsg message can be then sent to the NMEA2000 bus
 * with tNMEA2000::SendMsg();
 * 
 * \note There is an unresolved problem to use programming port with reading
 * data. Read works fine for a while, but then stops. With e.g. Arduino Due
 * USB port there is no problem.
 * 
 */
class tActisenseReader
{
protected:
    /** \brief Maximum length of the stream message buffer*/
    #define MAX_STREAM_MSG_BUF_LEN 300
    /** \brief Start of text has been received*/
    bool StartOfTextReceived;
    /** \brief A Message is coming*/
    bool MsgIsComing;
    /** \brief Escape character has been received*/
    bool EscapeReceived;
    /** \brief Sum of all bytes is used as kind of check sum*/
    int byteSum;
    /** \brief Buffer for incoming messages from stream*/
    unsigned char MsgBuf[MAX_STREAM_MSG_BUF_LEN];
    /** \brief Current write position inside the buffer */
    int MsgWritePos;
    /** \brief Default source of the N2k message*/
    unsigned char DefaultSource;

protected:
    /** \brief Stream to read from*/
    N2kStream* ReadStream;
    // Handler callback
    void (*MsgHandler)(const tN2kMsg &N2kMsg);

protected:
    /********************************************************************//**
     * \brief Adds a new Byte to the buffer
     *
     * \param NewByte   new Byte to be added
     * \retval true     Success
     * \retval false    Buffer is full
     */
    bool AddByteToBuffer(char NewByte);
    /********************************************************************//**
     * \brief Clears the buffer
     */
    void ClearBuffer();
    
    /********************************************************************//**
     * \brief Checks is Actisense message read from stream valid and builds tN2kMsg message from it.
     *
     * \param N2kMsg    Reference to a destination tN2kMsg Object  
     * \retval true     Message is valid. Message has been copied to N2kMsg
     * \retval false    Length does not match, checksum does not match or data length greater than tN2kMsg::MaxDataLen
     */
    bool CheckMessage(tN2kMsg &N2kMsg);

public:

    /********************************************************************//**
     * \brief Constructor for the class
     * Initialize all class attributes and clear the buffer
     */
    tActisenseReader();

    /********************************************************************//**
     * \brief Set the Read Stream object
     * 
     * Set stream, which would be used for reading messages. You have to
     * open stream first, so e.g. for SerialUSB call begin first.
     * 
     * \param _stream   Stream to read from
     */
     
    void SetReadStream(N2kStream* _stream) { ReadStream=_stream; }

    /********************************************************************//**
     * \brief Set the default source address for the messages
     *
     * If you use an application, which sends data by using Actisense data 
     * request type, the source set by this function will be set as source. 
     * Default=65;
     * 
     * \param source Source address to be used
     */
    void SetDefaultSource(unsigned char source) { DefaultSource=source; }

    /********************************************************************//**
     * \brief Read Actisense formatted NMEA2000 message from stream
     * 
     * GetMessageFromStream is nonblocking function and returns false, if there is
     * no message available. You can either call this or ParseMessages() periodically.
     * 
     * Function reads Actisense formatted NMEA2000 message from stream
     * Actisense Format:
     * <10><02><93><length (1)><priority (1)><PGN (3)><destination (1)><source (1)><time (4)><len (1)><data (len)><CRC (1)><10><03>
     * or
     * <10><02><94><length (1)><priority (1)><PGN (3)><destination (1)><len (1)><data (len)><CRC (1)><10><03>
     * \param N2kMsg    Reference to a N2kMsg Object  
     * \param ReadOut   Parameter is designed to be used if stream has multiprotocol data.
     * 
     * \retval true  for new message received.
     * \retval false if (ReadStream==0)
     */
    bool GetMessageFromStream(tN2kMsg &N2kMsg, bool ReadOut=true);

    /********* **********************************************************//**
     * \brief Checks if character is start of Actisense format
     * 
     * Function is designed to be used if stream has multiprotocol data.
     *
     * \param ch    character
     * \retval true     if (ch==Escape)
     * \retval false 
     */
    bool IsStart(char ch);

    /********************************************************************//**
     * \brief Parse messages from stream
     *
     * Call this periodically on loop as fast as possible. Function is nonblocking
     * and reads data from stream as it is available.
     * 
     * To handle received messages set message handler with SetMsgHandler(). On
     * message handler call GetMessageFromStream() to read arrived message.
     * 
     */
    void ParseMessages();

    /********************************************************************//**
     * \brief Set the Msg Handler object
     *
     * Set message handler to be used as callback within ParseMessages, when new message has
     * been received. On message handler call GetMessageFromStream() to read arrived message.
     * 
     * \param _MsgHandler {type} 
     */
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg)) { MsgHandler=_MsgHandler; }

    /** *****************************************************************//**
     * \brief Indicates if still message handling is needed
     * 
     * Function is designed to be used if stream has multiprotocol data.
     * 
     * \retval true
     * \retval false
     */
    bool Handling() const { return MsgIsComing || EscapeReceived || StartOfTextReceived; }
};

#endif
