/*
N2kDef.h

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

Type definitions and utility macros used in the NMEA2000 libraries.

*/

#ifndef _tN2kDef_H_

#include <stdint.h>

// Application execution delay. Must be implemented by application.
extern void delay(unsigned long ms);

// Current uptime in milliseconds. Must be implemented by application.
extern uint32_t millis();

// Macro sets for handling constant data. Some platforms are able to store
// constant strings and data on flash in order to reduce RAM consumption.
//
// - CSTR, A constant string.
// - CDATA, Constant data.
// - GET_CINT8, Get a constant 8bit integer.
// - GET_CINT16, Get a constant 16bit integer.
// - GET_CINT32, Get a constant 32bit integer.

#ifdef __AVR__
// AVR8 uses progmem.
#include <avr/pgmspace.h>
#define CSTR(str)       str PROGMEM
#define CDATA(data)     data PROGMEM
#define GET_CINT8(var)  pgm_read_byte(&var)
#define GET_CINT16(var) pgm_read_word(&var)
#define GET_CINT32(var) pgm_read_dword(&var)
#else
// No specific const storage support (default).
#define CSTR(str)       str
#define CDATA(data)     data
#define GET_CINT8(var)  var
#define GET_CINT16(var) var
#define GET_CINT32(var) var
#endif

#endif
