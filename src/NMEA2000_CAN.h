/*
NMEA2000_CAN.h

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

  Just include this <NMEA2000_CAN.h> to your project and it will
  automatically select suitable CAN library according to board
  selected on Arduino development environment. You can still force
  library by adding one of next defines before including NMEA2000_CAN.h:
  #define USE_N2K_CAN 1  // for use with SPI and MCP2515 can bus controller
  #define USE_N2K_CAN 2  // for use with due based CAN
  #define USE_N2K_CAN 3  // for use with Teensy 3.1/3.2/3.5/3.6 boards
  #define USE_N2K_CAN 4  // for use with avr boards
  #define USE_N2K_CAN 5  // for use with socketCAN (linux, etc) systems
  #define USE_N2K_CAN 6  // for use with MBED (ARM) systems
  #define USE_N2K_CAN 7  // for use with ESP32
  #define USE_N2K_CAN 8  // for use with Teensy 3.1/3.2/3.5/3.6/4.0/4.1 boards
  
  Depending of your board you will need to also install "driver" libraries:
  Arduino CAN shield (mcp_can) with MCP2515 chip:
    - https://github.com/ttlappalainen/CAN_BUS_Shield
    - https://github.com/ttlappalainen/NMEA2000_mcp
    
  Arduino DUE internal CAN:
    - https://github.com/ttlappalainen/due_can
    - https://github.com/ttlappalainen/NMEA2000_due
    
  Teensy 3.2-3.6 internal CAN:
    - https://github.com/ttlappalainen/FlexCAN_Library
    - https://github.com/ttlappalainen/NMEA2000_teensy
    
  Teensy 4.X internal CAN:
    - https://github.com/ttlappalainen/NMEA2000_Teensyx
    
  ESP32 internal CAN:
    - https://github.com/ttlappalainen/NMEA2000_esp32
  
  AVR:
    - https://github.com/thomasonw/avr_can
    - https://github.com/thomasonw/NMEA2000_avr
  
  MBED (note that there may be problem with fastpackets on MBED):
    - https://github.com/thomasonw/NMEA2000_mbed
    
  RPi socket CAN:
    -  https://github.com/thomasonw/NMEA2000_socketCAN
  

  There are also library specific defines:
  mcp_can:
    #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
    #define N2k_CAN_INT_PIN 21 // Use interrupt  and it is connected to pin 21
    #define USE_MCP_CAN_CLOCK_SET 8  // possible values 8 for 8Mhz and 16 for 16 Mhz clock
    
  esp32:
    #define ESP32_CAN_TX_PIN GPIO_NUM_16
    #define ESP32_CAN_RX_PIN GPIO_NUM_4
    
  Teensyx:
    #define NMEA2000_TEENSYX_CAN_BUS tNMEA2000_Teensyx::CAN2 // select CAN bus 2
    #define USE_NMEA2000_TEENSYX_FOR_TEENSY_3X // Force NMEA2000_TEENSYX also for Teensy 3.x boards
    
  Arduino DUE:
    #define NMEA2000_ARDUINO_DUE_CAN_BUS tNMEA2000_due::CANDevice1  // Use CAN bus 1 instead of 0 for Arduino DUE
*/


#ifndef _NMEA2000_CAN_H_
#define _NMEA2000_CAN_H_

#include "N2kMsg.h"
#include "NMEA2000.h"

#define USE_N2K_MCP_CAN 1
#define USE_N2K_DUE_CAN 2
#define USE_N2K_TEENSY_CAN 3
#define USE_N2K_AVR_CAN 4
#define USE_N2K_SOCKET_CAN 5
#define USE_N2K_MBED_CAN 6
#define USE_N2K_ESP32_CAN 7
#define USE_N2K_TEENSYX_CAN 8


// Select right CAN according to prosessor
#if !defined(USE_N2K_CAN)
#if  defined(__MBED__)					// Placing mbed 1st in tree, as the following CPUs can also be used in MBED IDE
#define USE_N2K_CAN USE_N2K_MBED_CAN
#elif defined(__SAM3X8E__)
#define USE_N2K_CAN USE_N2K_DUE_CAN
#elif defined(__ATMEGA32U4__)
#define USE_N2K_CAN USE_N2K_TEENSY_CAN
#elif defined(__MK20DX256__) || defined(__MK64FX512__) || defined (__MK66FX1M0__)
#ifndef USE_NMEA2000_TEENSYX_FOR_TEENSY_3X
  #define USE_N2K_CAN USE_N2K_TEENSY_CAN
#else
  #define USE_N2K_CAN USE_N2K_TEENSYX_CAN
#endif
#elif defined(__AVR_AT90CAN32__)||defined(__AVR_AT90CAN64__)||defined(__AVR_AT90CAN128__)|| \
      defined(__AVR_ATmega32C1__)||defined(__AVR_ATmega64C1__)||defined(__AVR_ATmega16M1__)||defined(__AVR_ATmega32M1__)|| defined(__AVR_ATmega64M1__)
#define USE_N2K_CAN USE_N2K_AVR_CAN
#elif defined(__linux__)||defined(__linux)||defined(linux)
#define USE_N2K_CAN USE_N2K_SOCKET_CAN
#elif defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
#define USE_N2K_CAN USE_N2K_ESP32_CAN
#elif defined(__IMXRT1062__)
#define USE_N2K_CAN USE_N2K_TEENSYX_CAN
#else
#define USE_N2K_CAN USE_N2K_MCP_CAN
#endif
#endif

#if USE_N2K_CAN == USE_N2K_DUE_CAN
// Use Arduino Due internal CAN with due_can library
#include <NMEA2000_due.h>
#ifndef NMEA2000_ARDUINO_DUE_CAN_BUS
  #define NMEA2000_ARDUINO_DUE_CAN_BUS
#endif
tNMEA2000 &NMEA2000=*(new tNMEA2000_due(NMEA2000_ARDUINO_DUE_CAN_BUS));

#elif USE_N2K_CAN == USE_N2K_TEENSY_CAN
// Use Teensy 3.1/3.2/3.5/3.6 boards internal CAN FlexCAN library
#include <NMEA2000_teensy.h>    // https://github.com/ttlappalainen/NMEA2000_teensy
#ifndef NMEA2000_TEENSY_VER
#error Update NMEA2000_Teensy for the latest version! 
#endif
tNMEA2000 &NMEA2000=*(new tNMEA2000_teensy());

#elif USE_N2K_CAN == USE_N2K_TEENSYX_CAN
// Use Teensy 3.1/3.2/3.5/3.6/4.0/4.1 boards internal CAN
#include <NMEA2000_Teensyx.h>    // https://github.com/ttlappalainen/NMEA2000_Teensyx
tNMEA2000 &NMEA2000=*(new tNMEA2000_Teensyx(NMEA2000_TEENSYX_CAN_BUS));

#elif USE_N2K_CAN == USE_N2K_AVR_CAN
// Use Atmel AVR internal CAN controller with avr_can library
#include <avr_can.h>            // https://github.com/thomasonw/avr_can
#include <NMEA2000_avr.h>       // https://github.com/thomasonw/NMEA2000_avr
tNMEA2000 &NMEA2000=*(new tNMEA2000_avr());

#elif USE_N2K_CAN == USE_N2K_SOCKET_CAN
// Use socketCAN devices
#ifndef SOCKET_CAN_PORT
  #define SOCKET_CAN_PORT NULL
  #endif
#include <NMEA2000_SocketCAN.h>       // https://github.com/thomasonw/NMEA2000_socketCAN
tNMEA2000 &NMEA2000=*(new tNMEA2000_SocketCAN(SOCKET_CAN_PORT));
tSocketStream serStream;

#elif USE_N2K_CAN == USE_N2K_MBED_CAN
// Use MBED devices
#include <NMEA2000_mbed.h>       // https://github.com/thomasonw/NMEA2000_mbed
tNMEA2000 &NMEA2000=*(new tNMEA2000_mbed());
tmbedStream serStream;

#elif USE_N2K_CAN == USE_N2K_ESP32_CAN
#include <NMEA2000_esp32.h>       // https://github.com/ttlappalainen/NMEA2000_esp32
tNMEA2000 &NMEA2000=*(new tNMEA2000_esp32());

#else  // Use USE_N2K_MCP_CAN
// Use mcp_can library e.g. with Arduino Mega and external MCP2551 CAN bus chip
// CAN_BUS_shield libraries will be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
// There is improved library, which branch can be found on https://github.com/peppeve/CAN_BUS_Shield
// That works also with Maple mini and 8 MHz clock. Hopefully these improvements will be applied to
// original library

#if defined(__STM32F1__) // Maple
#include <MapleIntCompatibility.h>
#endif

#include <SPI.h>
#include <mcp_can.h> // https://github.com/ttlappalainen/CAN_BUS_Shield

#if !defined(N2k_CAN_INT_PIN)
#define N2k_CAN_INT_PIN 0xff   // No interrupt.
#ifndef MCP_CAN_RX_BUFFER_SIZE
#define MCP_CAN_RX_BUFFER_SIZE 1   // Just small buffer to save memory
#endif
#endif

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

tNMEA2000 &NMEA2000=*(new tNMEA2000_mcp(N2k_SPI_CS_PIN,MCP_CAN_CLOCK_SET,N2k_CAN_INT_PIN,MCP_CAN_RX_BUFFER_SIZE));

#endif


#endif

