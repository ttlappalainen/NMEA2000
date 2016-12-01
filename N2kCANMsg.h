/* 
N2kCANMsg.h

Copyright (c) 2015-2016 Timo Lappalainen, Kave Oy, www.kave.fi

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
  : Ready(false),FreeMsg(true),SystemMessage(false) {
	  N2kMsg.Clear();
  }
  tN2kMsg N2kMsg;
  bool Ready;  // Ready for handling
  bool FreeMsg; // Msg is free for fill up
  bool SystemMessage;
  bool KnownMessage;
  unsigned char LastFrame; // Last received frame on fast packets
  unsigned char CopiedLen;
  
public:
  void FreeMessage() {FreeMsg=true; SystemMessage=false; N2kMsg.Clear(); }  
};

#endif
