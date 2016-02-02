/* 
NMEA2000_CAN.h

2015-2016 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen

  Just include this <NMEA2000_CAN.h> to your project and it will
  automatically select suitable CAN library according to board
  selected on Arduino development environment. You can still force
  library by adding one of next defines before including library:
  #define USE_N2K_MCP_CAN 1  // for use with SPI and MCP2515 can bus controller
  #define USE_N2K_DUE_CAN 2  // for use with due based CAN
  #define USE_N2K_TEENSY_CAN 3 // for use with Teensy 3.1/3.2 boards
  
  There are also library specific defines:
  mcp_can:
    #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
    #define USE_MCP_CAN_CLOCK_SET 8  // possible values 8 for 8Mhz and 16 for 16 Mhz clock
  

  This library is free software; you can redistribute it and/or
  modify it as you like.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/

#ifndef _NMEA2000_CAN_H_
#define _NMEA2000_CAN_H_

#include <N2kMsg.h>
#include <NMEA2000.h>

#define USE_N2K_MCP_CAN 1
#define USE_N2K_DUE_CAN 2
#define USE_N2K_TEENSY_CAN 3

// Select right CAN according to prosessor
#if !defined(USE_N2K_CAN)
#if defined(__SAM3X8E__)
#define USE_N2K_CAN USE_N2K_DUE_CAN
#elif defined(__MK20DX256__)
#define USE_N2K_CAN USE_N2K_TEENSY_CAN
#else
#define USE_N2K_CAN USE_N2K_MCP_CAN
#endif
#endif

#if USE_N2K_CAN == USE_N2K_DUE_CAN
// Use Arduino Due internal can with due_can library
#include <due_can.h>  // https://github.com/collin80/due_can
#include <NMEA2000_due.h>
tNMEA2000_due NMEA2000;

#elif USE_N2K_CAN == USE_N2K_TEENSY_CAN
// Use Teensy 3.1&3.2 board internal can FlexCAN library
#include <FlexCAN.h>
#include <NMEA2000_teensy.h> // https://github.com/sarfata/NMEA2000_teensy
tNMEA2000_teensy NMEA2000;

#else  // Use USE_N2K_MCP_CAN
// Use mcp_can library e.g. with Arduino Mega and external MCP2551 CAN bus chip
// CAN_BUS_shield libraries will be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
// There is improved library, which branch can be found on https://github.com/peppeve/CAN_BUS_Shield
// That works also with Maple mini and 8 MHz clock. Hopefully these improvements will be applied to
// original library

#include <SPI.h>
#include <mcp_can.h> // https://github.com/peppeve/CAN_BUS_Shield
#include <NMEA2000_mcp.h>
#if !defined(N2k_SPI_CS_PIN)
#define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
#endif

#if !defined(USE_MCP_CAN_CLOCK_SET)
#define USE_MCP_CAN_CLOCK_SET 16
#endif

#if USE_MCP_CAN_CLOCK_SET == 8
#define MCP_CAN_CLOCK_SET MCP_8MHz
#else
#define MCP_CAN_CLOCK_SET MCP_16MHz
#endif

#if !defined(N2k_CAN_INT_PIN)
#define N2k_CAN_INT_PIN 0xff   // No interrupt.
#define MCP_CAN_RX_BUFFER_SIZE 1   // Just small buffer to save memory
#endif

tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN,MCP_CAN_CLOCK_SET,N2k_CAN_INT_PIN);

#endif

#endif
