/* 
N2kCANMsg.h

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

#ifndef _tN2kCANMsg_H_
#define _tN2kCANMsg_H_
#include <N2kMsg.h>

class tN2kCANMsg
{
public:
  tN2kMsg N2kMsg;
  bool Ready;  // Ready for handling
  bool FreeMsg; // Msg is free for fill up
  bool SystemMessage;
  unsigned char LastFrame; // Last received frame on fast packets
  unsigned char CopiedLen;
  
public:
  void FreeMessage() {FreeMsg=true; SystemMessage=false; N2kMsg.Clear(); }  
};

#endif
