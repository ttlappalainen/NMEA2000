/* 
ActisenseReader.h

2015 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

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

  
This is class for reading Actisense format messages from given stream.

There is unresolved problem to use programming port with reading data.
Read works fine for a while, but then stops. With e.g. Arduino Due
USB port there is no problem.
*/
#ifndef _ACTISENSE_READER_H_
#define _ACTISENSE_READER_H_
#include <N2kMsg.h>

class tActisenseReader
{
protected:
    #define MAX_STREAM_MSG_BUF_LEN 300
    bool StartOfTextReceived;
    bool MsgIsComing;
    bool EscapeReceived;
    int byteSum;
    // Buffer for incoming messages from stream
    char MsgBuf[MAX_STREAM_MSG_BUF_LEN];
    int MsgWritePos;
  
protected:
    Stream* ReadStream;
    // Handler callback
    void (*MsgHandler)(const tN2kMsg &N2kMsg);
  
protected:
    bool AddByteToBuffer(char NewByte);
    void ClearBuffer();
    bool CheckMessage(tN2kMsg &N2kMsg);
    
public:
    tActisenseReader();
    // Set stream, which would be used for reading messages. You have to
    // open stream first, so e.g. for SerialUSB call begin first.
    void SetReadStream(Stream* _stream) { ReadStream=_stream; }
    
    // You can either call this or ParseMessages periodically.
    bool GetMessageFromStream(tN2kMsg &N2kMsg);
    
    // Set message handler with SetMsgHandler and then call this periodically or use GetMessageFromStream
    void ParseMessages();
    // Set message handler to be used in ParseMessages, when message has been received.
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg)) { MsgHandler=_MsgHandler; }
};

#endif
