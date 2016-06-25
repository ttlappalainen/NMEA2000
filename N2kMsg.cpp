/* 
N2kMsg.cpp

2015-2016 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

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
*/

#include <N2kMsg.h>
//#include <MemoryFree.h>  // For testing used memory

#define Escape 0x10
#define StartOfText 0x02
#define EndOfText 0x03
#define MsgTypeN2k 0x93

#define MaxActisenseMsgBuf 400

//*****************************************************************************
tN2kMsg::tN2kMsg(unsigned char _Source) {
  Init(6,0,_Source,255);
}

//*****************************************************************************
void tN2kMsg::SetPGN(unsigned long _PGN) {
  Clear();
  PGN=_PGN;
  MsgTime=millis();
}

//*****************************************************************************
void tN2kMsg::Init(unsigned char _Priority, unsigned long _PGN, unsigned char _Source, unsigned char _Destination) {
  DataLen=0;
  Priority=_Priority & 0x7;
  SetPGN(_PGN);
  Source=_Source;
  Destination=_Destination;
}

//*****************************************************************************
void tN2kMsg::Clear() {
  PGN=0;
  DataLen=0;
  MsgTime=0;
}

//*****************************************************************************
void tN2kMsg::Add8ByteDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf8ByteDouble(v,precision,DataLen,Data);
  } else {
    SetBuf4ByteUInt(N2kUInt32NA,DataLen,Data);
    SetBuf4ByteUInt(N2kInt32NA,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add4ByteDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf4ByteDouble(v,precision,DataLen,Data);
  } else {
    SetBuf4ByteUInt(N2kInt32NA,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add4ByteUDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf4ByteUDouble(v,precision,DataLen,Data);
  } else {
    SetBuf4ByteUInt(N2kUInt32NA,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add3ByteDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf3ByteDouble(v,precision,DataLen,Data);
  } else {
    SetBuf3ByteInt(0x7fffff,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add2ByteDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf2ByteDouble(v,precision,DataLen,Data);
  } else {
    SetBuf2ByteUInt(N2kInt16NA,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add2ByteUDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf2ByteUDouble(v,precision,DataLen,Data);
  } else {
    SetBuf2ByteUInt(N2kUInt16NA,DataLen,Data);
  }
}

//*****************************************************************************
void tN2kMsg::Add1ByteDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf1ByteDouble(v,precision,DataLen,Data);
  } else {
    AddByte(N2kInt8NA);
  }
}

//*****************************************************************************
void tN2kMsg::Add1ByteUDouble(double v, double precision, double UndefVal) {
  if (v!=UndefVal) {
    SetBuf1ByteUDouble(v,precision,DataLen,Data);
  } else {
    AddByte(N2kUInt8NA);
  }
}

//*****************************************************************************
void tN2kMsg::Add2ByteInt(int16_t v) {
  SetBuf2ByteInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::Add2ByteUInt(uint16_t v) {
  SetBuf2ByteUInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::Add3ByteInt(int32_t v) {
  SetBuf3ByteInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::Add4ByteUInt(uint32_t v) {
  SetBuf4ByteUInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::AddUInt64(uint64_t v) {
  SetBufUInt64(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::AddByte(unsigned char v) {
  Data[DataLen]=v; DataLen++;
}

//*****************************************************************************
void tN2kMsg::AddStr(const char *str, int len) {
  SetBufStr(str,len,DataLen,Data);
}

//*****************************************************************************
unsigned char tN2kMsg::GetByte(int &Index) const {
  if (Index<DataLen) {
    return Data[Index++];
  } else return 0xff;
}  
  
//*****************************************************************************
int16_t tN2kMsg::Get2ByteInt(int &Index, int16_t def) const {
  if (Index+2<=DataLen) {
    return GetBuf2ByteInt(Index,Data);
  } else return def;
}  
  
//*****************************************************************************
uint16_t tN2kMsg::Get2ByteUInt(int &Index, uint16_t def) const {
  if (Index+2<=DataLen) {
    return GetBuf2ByteUInt(Index,Data);
  } else return def;
}  
  
//*****************************************************************************
uint32_t tN2kMsg::Get4ByteUInt(int &Index, uint32_t def) const {
  if (Index+4<=DataLen) {
    return GetBuf4ByteUInt(Index,Data);
  } else return def;
}  

//*****************************************************************************
double tN2kMsg::Get1ByteDouble(double precision, int &Index, double def) const {
  if (Index<DataLen) {
    return GetBuf1ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get1ByteUDouble(double precision, int &Index, double def) const {
  if (Index<DataLen) {
    return GetBuf1ByteUDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get2ByteDouble(double precision, int &Index, double def) const {
  if (Index+2<=DataLen) {
    return GetBuf2ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get2ByteUDouble(double precision, int &Index, double def) const {
  if (Index+2<=DataLen) {
    return GetBuf2ByteUDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get3ByteDouble(double precision, int &Index, double def) const {
  if (Index+3<=DataLen) {
    return GetBuf3ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get4ByteDouble(double precision, int &Index, double def) const {
  if (Index+4<=DataLen) {
    return GetBuf4ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get4ByteUDouble(double precision, int &Index, double def) const {
  if (Index+4<=DataLen) {
    return GetBuf4ByteUDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get8ByteDouble(double precision, int &Index, double def) const {
  if (Index+8<=DataLen) {
    return GetBuf8ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
bool tN2kMsg::GetStr(char *StrBuf, int Length, int &Index) const {
  unsigned char vb;
  bool nullReached = false;
  StrBuf[0] = '\0';
  if (Index+Length<=DataLen) {
    for (int i=0; i<Length; i++) {
      vb = GetByte(Index);
      if (! nullReached) {
        if (vb == 0x00 || vb == '@') {
          nullReached = true; // either null or '@' (AIS null character)
          StrBuf[i] = '\0';
          StrBuf[i+1] = '\0';
        } else {
          StrBuf[i] = vb;
          StrBuf[i+1] = '\0';
        }
      } else {
        StrBuf[i] = '\0';
        StrBuf[i+1] = '\0';
      }
    }
    return true;
  } else return false;
}

//*****************************************************************************
bool tN2kMsg::Set2ByteUInt(uint16_t v, int &Index) {
  if (Index+2<=DataLen) {
    SetBuf2ByteUInt(v,Index,Data);
    return true;
  } else
    return false;
}

//*****************************************************************************
void SetBuf8ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  double fp=precision*1e6;
  long long fpll=1/fp;
  long long vll=v*1e6;
    vll*=fpll;

/* Does not work. Why?
  long long *vll=(long long *)(&buf[index]);
    (*vll)=v*1e6;
    (*vll)*=fpll;
    
    index+=8;
*/    

    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;

}

//*****************************************************************************
void SetBuf4ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  int32_t *vi=(int32_t *)(&buf[index]);
  index+=4;
  
  (*vi)=(int32_t)round(v/precision);
}

//*****************************************************************************
void SetBuf4ByteUDouble(double v, double precision, int &index, unsigned char *buf) {
  uint32_t *vi=(uint32_t *)(&buf[index]);
  index+=4;
  
  (*vi)=(uint32_t)round(v/precision);
}

//*****************************************************************************
void SetBuf3ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  long vl;
  vl=(long)round(v/precision);
    SetBuf3ByteInt(vl,index,buf);
}

//*****************************************************************************
int16_t GetBuf2ByteInt(int &index, const unsigned char *buf) {
  int16_t *vi=(int16_t *)(&buf[index]);
  index+=2;
  
  return *vi;
}

//*****************************************************************************
uint16_t GetBuf2ByteUInt(int &index, const unsigned char *buf) {
  uint16_t *vi=(uint16_t *)(&buf[index]);
  index+=2;
  
  return *vi;
}

//*****************************************************************************
uint32_t GetBuf4ByteUInt(int &index, const unsigned char *buf) {
  uint32_t *vi=(uint32_t *)(&buf[index]);
  index+=4;
  
  return *vi;
}

//*****************************************************************************
double GetBuf1ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  int8_t *vi=(int8_t *)(&buf[index]);
  index+=1;
  
  if (*vi==0x7f) return def;
  
  return ((double)(*vi))*precision;
}

//*****************************************************************************
double GetBuf1ByteUDouble(double precision, int &index, const unsigned char *buf, double def) {
  uint8_t *vi=(uint8_t *)(&buf[index]);
  index+=1;
  
  if (*vi==0xff) return def;
  
  return ((double)(*vi))*precision;
}

//*****************************************************************************
double GetBuf2ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  int16_t *vi=(int16_t *)(&buf[index]);
  index+=2;
  
  if (*vi==0x7fff) return def;
  
  return ((double)(*vi))*precision;
}

//*****************************************************************************
double GetBuf2ByteUDouble(double precision, int &index, const unsigned char *buf, double def) {
  uint16_t *vi=(uint16_t *)(&buf[index]);
  index+=2;
  
  if (*vi==0xffff) return def;
  
  return ((double)(*vi))*precision;
}

//*****************************************************************************
double GetBuf8ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  // Next does not work on any board I tested
//  long long *vll=(long long *)(&buf[index]);
//  index+=8;
  unsigned long *vllo=(unsigned long *)(&buf[index]);
  index+=4;
  long *vlhi=(long *)(&buf[index]);
  index+=4;
  
  if ( (*vlhi==0x7fff) && (*vllo==0xffff) ) return def;
  double v=*vlhi * 4294967296.0;
  
  if (v>=0) { v += *vllo; } else { v -= *vllo; }
  
  // Below did not work even with Due
  //long long vll=*vlhi;
  //vll<<=32;
  //vll|=*vllo;
 
  
  //Serial.print(*vlhi,HEX); Serial.print(","); Serial.println(*vllo,HEX); 


//  if (((unsigned long long)(vll))==0xffffffffffffffff) return def;
  
  return v*precision;
}

//*****************************************************************************
double GetBuf3ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  long *vl=(long *)(&buf[index]);
  long vll=*vl;
  index+=3;

  // We use only 3 bytes, so set highest byte to 0
  vll&=0x00ffffff;
  if (vll==0x007fffff) return def;
  
  return ((double)(vll))*precision;
}

//*****************************************************************************
double GetBuf4ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  long *vl=(long *)(&buf[index]);
  index+=4;

  if (*vl==0x7fffffff) return def;
  
  return ((double)(*vl))*precision;
}

//*****************************************************************************
double GetBuf4ByteUDouble(double precision, int &index, const unsigned char *buf, double def) {
  unsigned long *vl=(unsigned long *)(&buf[index]);
  index+=4;

  if (*vl==0xffffffff) return def;
  
  return ((double)(*vl))*precision;
}

//*****************************************************************************
void SetBuf2ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  int16_t *vi=(int16_t *)(&buf[index]);
  index+=2;
  
  (*vi)=(int16_t)round(v/precision);
}

//*****************************************************************************
void SetBuf2ByteUDouble(double v, double precision, int &index, unsigned char *buf) {
  uint16_t *vi=(uint16_t *)(&buf[index]);
  index+=2;
  
  (*vi)=(uint16_t)round(v/precision);
}

//*****************************************************************************
void SetBuf1ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  int8_t *vi=(int8_t *)(&buf[index]);
  index+=1;
  
  (*vi)=(int8_t)round(v/precision);
}

//*****************************************************************************
void SetBuf1ByteUDouble(double v, double precision, int &index, unsigned char *buf) {
  uint8_t *vi=(uint8_t *)(&buf[index]);
  index+=1;
  
  (*vi)=(uint8_t)round(v/precision);
}

//*****************************************************************************
void SetBuf2ByteInt(int16_t v, int &index, unsigned char *buf) {
    buf[index]=lowByte(v); index++;
    buf[index]=highByte(v); index++;
}

//*****************************************************************************
void SetBuf2ByteUInt(uint16_t v, int &index, unsigned char *buf) {
    buf[index]=lowByte(v); index++;
    buf[index]=highByte(v); index++;
}

//*****************************************************************************
void SetBuf3ByteInt(int32_t v, int &index, unsigned char *buf) {
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; index++;
}

//*****************************************************************************
void SetBuf4ByteUInt(uint32_t v, int &index, unsigned char *buf) {
  uint32_t *vl=(uint32_t *)(&buf[index]);
  index+=4;
  
  (*vl)=v;
}

//*****************************************************************************
void SetBufUInt64(uint64_t v, int &index, unsigned char *buf) {
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; index++;
}

//*****************************************************************************
void SetBufStr(const char *str, int len, int &index, unsigned char *buf) {
  int i=0;
  for (; i<len && str[i]!=0; i++, index++) {
    buf[index]=str[i];
  }
  for (; i<len; i++, index++) {
    buf[index]=0;
  }
}

//*****************************************************************************
void PrintBuf(Stream *port, unsigned char len, const unsigned char *pData) {
    for(int i = 0; i<len; i++) {
      if (i>0) { port->print(","); };
      port->print(pData[i],HEX);
    }
}

//*****************************************************************************
void tN2kMsg::Print(Stream *port, bool NoData) const {
  if (!IsValid()) return;
  port->print(F("Pri:")); port->print(Priority);
  port->print(F(" PGN:")); port->print(PGN);
  port->print(F(" Source:")); port->print(Source);
  port->print(F(" Dest:")); port->print(Destination);
  port->print(F(" Len:")); port->print(DataLen);
  if (!NoData) {
    port->print(F(" Data:"));
    PrintBuf(port,DataLen,Data);
  }
  port->print(F("\r\n"));
}

//*****************************************************************************
void AddByteEscapedToBuf(unsigned char byteToAdd, byte &idx, unsigned char *buf, int &byteSum)
{
  buf[idx++]=byteToAdd;
  byteSum+=byteToAdd;

  if (byteToAdd == Escape) {
    buf[idx++]=Escape;
  }
}

//*****************************************************************************
// Actisense Format:
// <10><02><93><length (1)><priority (1)><PGN (3)><destination (1)><source (1)><time (4)><len (1)><data (len)><CRC (1)><10><03>
void tN2kMsg::SendInActisenseFormat(Stream *port) const {
  unsigned long _PGN=PGN;
  unsigned long _MsgTime=MsgTime;
  byte msgIdx=0;
  int byteSum = 0;
  byte CheckSum;
  unsigned char ActisenseMsgBuf[MaxActisenseMsgBuf];
  
  if (!IsValid()) return;
  // Serial.print("freeMemory()="); Serial.println(freeMemory());
  
  ActisenseMsgBuf[msgIdx++]=Escape;
  ActisenseMsgBuf[msgIdx++]=StartOfText;
  AddByteEscapedToBuf(MsgTypeN2k,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(DataLen+11,msgIdx,ActisenseMsgBuf,byteSum); //length does not include escaped chars
  AddByteEscapedToBuf(Priority,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _PGN>>=8;
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _PGN>>=8;
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(Destination,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(Source,msgIdx,ActisenseMsgBuf,byteSum);
  // Time?
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(DataLen,msgIdx,ActisenseMsgBuf,byteSum);
  

  for (int i = 0; i < DataLen; i++) AddByteEscapedToBuf(Data[i],msgIdx,ActisenseMsgBuf,byteSum);
  byteSum %= 256;

  CheckSum = (byte)((byteSum == 0) ? 0 : (256 - byteSum));
  ActisenseMsgBuf[msgIdx++]=CheckSum;
  if (CheckSum==Escape) ActisenseMsgBuf[msgIdx++]=CheckSum;

  ActisenseMsgBuf[msgIdx++] = Escape;
  ActisenseMsgBuf[msgIdx++] = EndOfText;
  
  port->write(ActisenseMsgBuf,msgIdx);
//  Serial.print("Actisense data:");
//  PrintBuf(msgIdx,ActisenseMsgBuf);
//  Serial.print("\r\n");
}
