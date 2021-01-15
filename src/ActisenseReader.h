/*
ActisenseReader.h

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


This is class for reading Actisense format messages from given stream.

There is unresolved problem to use programming port with reading data.
Read works fine for a while, but then stops. With e.g. Arduino Due
USB port there is no problem.
*/
#ifndef _ACTISENSE_READER_H_
#define _ACTISENSE_READER_H_

#include "N2kMsg.h"
#include "N2kStream.h"

class tActisenseReader
{
protected:
    #define MAX_STREAM_MSG_BUF_LEN 300
    bool StartOfTextReceived;
    bool MsgIsComing;
    bool EscapeReceived;
    int byteSum;
    // Buffer for incoming messages from stream
    unsigned char MsgBuf[MAX_STREAM_MSG_BUF_LEN];
    int MsgWritePos;
    unsigned char DefaultSource;

protected:
    N2kStream* ReadStream;
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
    void SetReadStream(N2kStream* _stream) { ReadStream=_stream; }

    // If you use application, which sends data by using Actisense data request type, source
    // set by this function will be set as source. Default=65;
    void SetDefaultSource(unsigned char source) { DefaultSource=source; }
    
    // You can either call this or ParseMessages periodically.
    bool GetMessageFromStream(tN2kMsg &N2kMsg, bool ReadOut=true);

    bool IsStart(char ch);
    // Set message handler with SetMsgHandler and then call this periodically or use GetMessageFromStream
    void ParseMessages();
    // Set message handler to be used in ParseMessages, when message has been received.
    void SetMsgHandler(void (*_MsgHandler)(const tN2kMsg &N2kMsg)) { MsgHandler=_MsgHandler; }

    bool Handling() const { return MsgIsComing || EscapeReceived || StartOfTextReceived; }
};

#endif
