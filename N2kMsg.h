/* 
N2kMsg.h

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

Definition for NMEA2000 message class used in my NMEA2000 libraries.

*/

#ifndef _tN2kMsg_H_
#define _tN2kMsg_H_
#include <Arduino.h>

const double   N2kDoubleNA=-1e9;
const uint8_t  N2kUInt8NA=0xff;
const int8_t   N2kInt8NA=0x7f;
const uint16_t N2kUInt16NA=0xffff;
const int16_t  N2kInt16NA=0x7fff;
const uint32_t N2kUInt32NA=0xffffffff;
const int32_t  N2kInt32NA=0x7fffffff;

inline bool N2kIsNA(double v) { return v==N2kDoubleNA; }
inline bool N2kIsNA(uint8_t v) { return v==N2kUInt8NA; }
inline bool N2kIsNA(int8_t v) { return v==N2kInt8NA; }
inline bool N2kIsNA(uint16_t v) { return v==N2kUInt16NA; }
inline bool N2kIsNA(int16_t v) { return v==N2kInt16NA; }
inline bool N2kIsNA(uint32_t v) { return v==N2kUInt32NA; }
inline bool N2kIsNA(int32_t v) { return v==N2kInt32NA; }

void SetBuf8ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf4ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf4ByteUDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf3ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf2ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf2ByteUDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf1ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf1ByteUDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf2ByteInt(int16_t v, int &index, unsigned char *buf);
void SetBuf2ByteUInt(uint16_t v, int &index, unsigned char *buf);
void SetBuf3ByteInt(int32_t v, int &index, unsigned char *buf);
void SetBuf4ByteUInt(uint32_t v, int &index, unsigned char *buf);
void SetBufUInt64(uint64_t v, int &index, unsigned char *buf);
void SetBufStr(const char *str, int len, int &index, unsigned char *buf);

int16_t GetBuf2ByteInt(int &index, const unsigned char *buf);
uint16_t GetBuf2ByteUInt(int &index, const unsigned char *buf);
uint32_t GetBuf4ByteUInt(int &index, const unsigned char *buf);
double GetBuf1ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);
double GetBuf1ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);
double GetBuf2ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);
double GetBuf2ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);
double GetBuf3ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);
double GetBuf4ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);
double GetBuf4ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);
double GetBuf8ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);


class tN2kMsg
{
public:
  static const int MaxDataLen=223; // with fast packet 1. frame can have 6 byte and rest 31 frames 7 bytes
  unsigned char Priority;
  unsigned long PGN;
  mutable unsigned char Source;
  mutable unsigned char Destination;
  int DataLen;
  unsigned char Data[MaxDataLen];
  unsigned long MsgTime;

public:
  tN2kMsg(unsigned char _Source=15);
  void SetPGN(unsigned long _PGN);
  void ForceSource(unsigned char _Source) const { Source=_Source; }
  void CheckDestination() const { if ( (PGN & 0xff)!=0 ) Destination=0xff; } // We can send to specified destination only for PGN:s low byte=0 
  void Init(unsigned char _Priority, unsigned long _PGN, unsigned char _Source, unsigned char _Destination=0xff);
  void Clear();
  bool IsValid() const { return (PGN!=0 && DataLen>0); }
  void Add8ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add4ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add4ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add3ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add2ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add2ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add1ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add1ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);
  void Add2ByteInt(int16_t v);
  void Add2ByteUInt(uint16_t v);
  void Add3ByteInt(int32_t v);
  void Add4ByteUInt(uint32_t v);
  void AddUInt64(uint64_t v);
  void AddByte(unsigned char v);
  void AddStr(const char *str, int len);

  unsigned char GetByte(int &Index) const;
  int16_t Get2ByteInt(int &Index, int16_t def=0x7fff) const;
  uint16_t Get2ByteUInt(int &Index, uint16_t def=0xffff) const;
  uint32_t Get4ByteUInt(int &Index, uint32_t def=0xffffffff) const;
  double Get1ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get1ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get2ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get2ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get3ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get4ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get4ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  double Get8ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;
  bool GetStr(char *StrBuf, int Length, int &Index) const;

  bool Set2ByteUInt(uint16_t v, int &Index);

  void Print(Stream *port, bool NoData=false) const;
  void SendInActisenseFormat(Stream *port) const;
};

void PrintBuf(Stream *port, unsigned char len, const unsigned char *pData);

#endif
