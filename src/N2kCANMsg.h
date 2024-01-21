/* 
 * N2kCANMsg.h
 * Copyright (c) 2015-2024 Timo Lappalainen, Kave Oy, www.kave.fi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*************************************************************************//**
 * \file  N2kCANMsg.h
 * \brief File declares tN2kCANMsg class used internally on tNMEA2000.
 * 
 * Class is used by the tNMEA2000 class to handle incoming NMEA2000 messages.
 */

#ifndef _tN2kCANMsg_H_
#define _tN2kCANMsg_H_
#include "N2kMsg.h"

/************************************************************************//**
 * \class tN2kCANMsg
 * 
 * \brief Class used internally on tNMEA2000 to handle incoming NMEA2000 messages.
 * \ingroup group_core
 * 
 */
class tN2kCANMsg
{
public:
  /************************************************************************//**
   * \brief Constructor of class \ref tN2kCANMsg
   *
   */
  tN2kCANMsg()
    : Ready(false),FreeMsg(true),SystemMessage(false), KnownMessage(false) 
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
      ,TPRequireCTS(false), TPMaxPackets(0) 
#endif
    {
	  N2kMsg.Clear();
  }
  /** \brief Reference to a N2kMsg Object, Output: NMEA2000 message ready 
   * to be send.*/
  tN2kMsg N2kMsg;
  /** \brief Message ready for handling?   */
  bool Ready;
  /** \brief Message is free for fill up   */
  bool FreeMsg; 
  /** \brief Message is a system message*/
  bool SystemMessage;
  /** \brief Message is already known   */
  bool KnownMessage;
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
  /** \brief =0 no, n=after each n frames   */
  unsigned char TPRequireCTS; 
  /** \brief =0 not TP message. >0 number of packets can be received  */
  unsigned char TPMaxPackets; 
#endif
  /** \brief  Last received frame sequence number on fast 
   *          packets or multi packet  */
  unsigned char LastFrame; // 
  /** \brief  Length of copied bytes */
  unsigned char CopiedLen;
  
public:
  /************************************************************************//**
   * \brief Free the message
   * 
   * This resets the whole message and clears all the content
   *
   */
  void FreeMessage() { 
    FreeMsg=true; Ready=false; SystemMessage=false; 
#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
    TPMaxPackets=0; TPRequireCTS=false; 
#endif
    N2kMsg.Clear(); N2kMsg.Source=0; 
  }  
};

#endif
