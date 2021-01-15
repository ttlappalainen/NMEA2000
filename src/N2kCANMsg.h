/* 
N2kCANMsg.h

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
*/

#ifndef _tN2kCANMsg_H_
#define _tN2kCANMsg_H_
#include <N2kMsg.h>

class tN2kCANMsg
{
public:
  tN2kCANMsg()
    : Ready(false),FreeMsg(true),SystemMessage(false), KnownMessage(false) 
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
      ,TPRequireCTS(false), TPMaxPackets(0) 
#endif
    {
	  N2kMsg.Clear();
  }
  tN2kMsg N2kMsg;
  bool Ready;  // Ready for handling
  bool FreeMsg; // Msg is free for fill up
  bool SystemMessage;
  bool KnownMessage;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
  unsigned char TPRequireCTS; // =0 no, n=after each n frames
  unsigned char TPMaxPackets; // =0 not TP message. >0 number of packets can be received.
#endif
  unsigned char LastFrame; // Last received frame sequence number on fast packets or multi packet
  unsigned char CopiedLen;
  
public:
  void FreeMessage() { 
    FreeMsg=true; Ready=false; SystemMessage=false; 
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    TPMaxPackets=0; TPRequireCTS=false; 
#endif
    N2kMsg.Clear(); N2kMsg.Source=0; 
  }  
};

#endif
