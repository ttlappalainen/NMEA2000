/* 
N2kMsg.h

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

Definition for NMEA2000 message class used in my NMEA2000 libraries.

*/

#ifndef _tN2kMsg_H_
#define _tN2kMsg_H_
#include <Arduino.h>

void SetBuf8ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf4ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf2ByteDouble(double v, double precision, int &index, unsigned char *buf);
void SetBuf2ByteInt(int v, int &index, unsigned char *buf);
void SetBuf4ByteUInt(unsigned long v, int &index, unsigned char *buf);
void SetBufUInt64(uint64_t v, int &index, unsigned char *buf);
void SetBufStr(const char *str, int len, int &index, unsigned char *buf);

double GetBuf2ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);
double GetBuf4ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);


class tN2kMsg
{
public:
  unsigned char Priority;
  unsigned long PGN;
  mutable unsigned char Source;
  mutable unsigned char Destination;
  int DataLen;
  unsigned char Data[255];
  unsigned long MsgTime;

public:
  tN2kMsg(unsigned char _Source=15);
  void SetPGN(unsigned long _PGN);
  void ForceSource(unsigned char _Source) const { Source=_Source; }
  void CheckDestination() const { if ( (PGN & 0xff)!=0 ) Destination=0xff; } // We can send to specified destination only for PGN:s low byte=0 
  void Init(unsigned char _Priority, unsigned long _PGN, unsigned char _Source, unsigned char _Destination=0xff);
  void Clear();
  bool IsValid() const { return (PGN!=0 && DataLen>0); }
  void Add8ByteDouble(double v, double precision);
  void Add4ByteDouble(double v, double precision);
  void Add2ByteDouble(double v, double precision);
  void Add2ByteInt(int v);
  void Add4ByteUInt(unsigned long v);
  void AddUInt64(uint64_t v);
  void AddByte(unsigned char v);
  void AddStr(const char *str, int len);

  unsigned char GetByte(int &Index) const;
  double Get2ByteDouble(double precision, int &Index, double def=0) const;
  double Get4ByteDouble(double precision, int &Index, double def=0) const;

  void Print(Stream *port, bool NoData=false) const;
  void SendInActisenseFormat(Stream *port) const;
};

void PrintBuf(Stream *port, unsigned char len, const unsigned char *pData);

#endif
