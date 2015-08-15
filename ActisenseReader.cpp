/* 
ActisenseReader.cpp

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
*/
#include <ActisenseReader.h>

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
   
   byte CheckSum = (byte)((byteSum == 0) ? 0 : (256 - byteSum));
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
   N2kMsg.MsgTime=*((unsigned long *)(&(MsgBuf[8])));
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

  if (ReadStream==0) return false;
  
  while (ReadStream->available() > 0 && !result) {
    int NewByte=ReadStream->read();
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


