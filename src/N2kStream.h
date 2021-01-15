/*
N2kStream.h

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

I/O stream used in the NMEA2000 libraries.

*/

#ifndef _tN2kStream_H_
#define _tN2kStream_H_

#include "N2kDef.h"
#include <stdint.h>
#include <stddef.h>

#ifdef ARDUINO
// Arduino users get away with using the standard Stream class and its
// subclasses. Forward declare the Stream class here and include Arduino.h in
// the application.
#include <Arduino.h>
typedef Stream N2kStream;
#else
// Non Arduino platforms need to implement this themselves if they want to use
// functions which operate on streams.
class N2kStream {
   public:
   // Returns first byte if incoming data, or -1 on no available data.
   virtual int read() = 0;

   // Write data to stream.
   virtual size_t write(const uint8_t* data, size_t size) = 0;

   // Print string to stream.
   size_t print(const char* str);

#if defined(__AVR__)
   // Flash stored string stream for AVR platforms.
   size_t print(const __FlashStringHelper* str);
   size_t println(const __FlashStringHelper* str);
#endif

   // Print value to stream.
   size_t print(int val, uint8_t radix = 10);

   // Print string and newline to stream.
   size_t println(const char *str);

   // Print value and newline to stream.
   size_t println(int val, uint8_t radix = 10);
};
#endif

#endif
