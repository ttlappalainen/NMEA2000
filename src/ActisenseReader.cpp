/*
ActisenseReader.cpp

Copyright (c) 2015-2017 Timo Lappalainen, Kave Oy, www.kave.fi

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
#define MsgTypeN2k 0x93

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
   if (MsgBuf[12]>tN2kMsg::MaxDataLen) {
     return false; // Too long data
   }

   N2kMsg.Priority=MsgBuf[2];
   N2kMsg.PGN=(unsigned long)(MsgBuf[3]) | (unsigned long)(MsgBuf[4])<<8  | (unsigned long)(MsgBuf[5])<<16;
   N2kMsg.Destination=MsgBuf[6];
   N2kMsg.Source=MsgBuf[7];
   // N2kMsg.MsgTime=*((unsigned long *)(&(MsgBuf[8])));  // this causes warning: dereferencing type-punned pointer will break strict-aliasing rules
   memcpy(&(N2kMsg.MsgTime), &(MsgBuf[8]), 4);
   N2kMsg.DataLen=MsgBuf[12];
   for (int i=13, j=0; i<MsgWritePos-1; i++, j++) N2kMsg.Data[j]=MsgBuf[i];

   return true;
}

//*****************************************************************************
// Read Actisense formatted NMEA2000 message from stream
// Actisense Format:
// <10><02><93><length (1)><priority (1)><PGN (3)><destination (1)><source (1)><time (4)><len (1)><data (len)><CRC (1)><10><03>
bool tActisenseReader::GetMessageFromStream(tN2kMsg &N2kMsg) {
  bool result=false;

  if (ReadStream==0)
    return false;

  int NewByte;
  while ((NewByte = ReadStream->read()) != -1 && !result) {
//        Serial.println((char)NewByte,HEX);
      if (MsgIsComing) {
        if (EscapeReceived) {
          switch (NewByte) {
            case Escape: // Escaped Escape
              EscapeReceived=false;
              if (!AddByteToBuffer(NewByte)) ClearBuffer();
              break;
            case EndOfText: // Message ready
              switch (MsgBuf[0]) {
                case MsgTypeN2k:
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
              ClearBuffer();
              StartOfTextReceived=true;
            }
            break;
          default:
            if (StartOfTextReceived) {
              StartOfTextReceived=false;
              MsgIsComing=true;
              AddByteToBuffer(NewByte);
            }
        }
        EscapeReceived=(NewByte==Escape);
      }
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


