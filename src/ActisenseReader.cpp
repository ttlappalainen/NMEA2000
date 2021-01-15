/*
ActisenseReader.cpp

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
*/
#include "ActisenseReader.h"
#include <string.h>

//*****************************************************************************
tActisenseReader::tActisenseReader() {
  DefaultSource=65;
  ReadStream=0;
  ClearBuffer();
}

//*****************************************************************************
void tActisenseReader::ClearBuffer() {
  MsgWritePos=0;
  byteSum=0;
  StartOfTextReceived=false;
  MsgIsComing=false;
  EscapeReceived=false;
}

//*****************************************************************************
bool tActisenseReader::AddByteToBuffer(char NewByte) {
  if (MsgWritePos>=MAX_STREAM_MSG_BUF_LEN) return false;

  MsgBuf[MsgWritePos]=NewByte;
  MsgWritePos++;
  if ( MsgBuf[1]+3!=MsgWritePos ) byteSum+=NewByte; // !Do not add CRC to byteSum
  return true;
}

#define Escape 0x10
#define StartOfText 0x02
#define EndOfText 0x03
#define MsgTypeN2kData 0x93
#define MsgTypeN2kRequest 0x94

//*****************************************************************************
bool tActisenseReader::CheckMessage(tN2kMsg &N2kMsg) {

   N2kMsg.Clear();

   if (MsgWritePos!=MsgBuf[1]+3) {
     return false; // Length does not match. Add type, length and crc
   }

   uint8_t CheckSum = (uint8_t)((byteSum == 0) ? 0 : (256 - byteSum));
   if ( CheckSum!=MsgBuf[MsgWritePos-1] ) {
     return false; // Checksum does not match
   }

   int i=2;
   N2kMsg.Priority=MsgBuf[i++];
   N2kMsg.PGN=GetBuf3ByteUInt(i,MsgBuf);
   N2kMsg.Destination=MsgBuf[i++];
   if ( MsgBuf[0]==MsgTypeN2kData ) {
     N2kMsg.Source=MsgBuf[i++];
     N2kMsg.MsgTime=GetBuf4ByteUInt(i,MsgBuf);
   } else {
     N2kMsg.Source=DefaultSource;
     N2kMsg.MsgTime=millis();
   }
   N2kMsg.DataLen=MsgBuf[i++];

   if ( N2kMsg.DataLen>tN2kMsg::MaxDataLen ) {
     N2kMsg.Clear();
     return false; // Too long data
   }

   for (int j=0; i<MsgWritePos-1; i++, j++) N2kMsg.Data[j]=MsgBuf[i];

   return true;
}

//*****************************************************************************
bool tActisenseReader::IsStart(char ch) {
  return (ch==Escape);
}

//*****************************************************************************
// Read Actisense formatted NMEA2000 message from stream
// Actisense Format:
// <10><02><93><length (1)><priority (1)><PGN (3)><destination (1)><source (1)><time (4)><len (1)><data (len)><CRC (1)><10><03>
// or
// <10><02><94><length (1)><priority (1)><PGN (3)><destination (1)><len (1)><data (len)><CRC (1)><10><03>
bool tActisenseReader::GetMessageFromStream(tN2kMsg &N2kMsg, bool ReadOut) {
  bool result=false;

  if (ReadStream==0) return false;

  int NewByte;
  bool ContinueLoopAvailable=true;

  while ((NewByte = ReadStream->peek()) != -1 && !result && ContinueLoopAvailable) {
//        Serial.println((char)NewByte,HEX);
      if (MsgIsComing) {
        ReadStream->read();
        if (EscapeReceived) {
          switch (NewByte) {
            case Escape: // Escaped Escape
              EscapeReceived=false;
              if (!AddByteToBuffer(NewByte)) ClearBuffer();
              break;
            case EndOfText: // Message ready
              switch (MsgBuf[0]) {
                case MsgTypeN2kData:
                case MsgTypeN2kRequest:
                  result=CheckMessage(N2kMsg);
                  break;
                default:
                  result=false;
              }
              ClearBuffer();
              break;
            case StartOfText: // Start new message
              ClearBuffer();
              StartOfTextReceived=true;
              break;
            default: // Error
              ClearBuffer();
          }
        } else {
          if (NewByte==Escape) {
            EscapeReceived=true;
          } else {
            if (!AddByteToBuffer(NewByte)) ClearBuffer();
          }
        }
      } else {
        switch (NewByte) {
          case StartOfText:
            StartOfTextReceived=false;
            if (EscapeReceived) {
              ReadStream->read(); // Read ch out
              ClearBuffer();
              StartOfTextReceived=true;
            }
            break;
          default:
            EscapeReceived=(NewByte==Escape);
            if (StartOfTextReceived) {
              ReadStream->read(); // Read ch out
              StartOfTextReceived=false;
              MsgIsComing=true;
              AddByteToBuffer(NewByte);
            } else {
              if ( EscapeReceived || ReadOut ) ReadStream->read(); // Read ch out
            }
        }
      }

    ContinueLoopAvailable=ReadOut || Handling();
  }

  return result;
}

//*****************************************************************************
void tActisenseReader::ParseMessages() {
  tN2kMsg N2kMsg;

    while (GetMessageFromStream(N2kMsg)) {
      if (MsgHandler!=0) MsgHandler(N2kMsg);
    }
}


