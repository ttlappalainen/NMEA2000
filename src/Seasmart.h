/*
  The MIT License

  Copyright (c) 2017-2021 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef _Seasmart_h_
#define _Seasmart_h_

#include <N2kMsg.h>

/**
 * Converts a tN2kMsg into a $PCDIN NMEA sentence, following the Seasmart
 * specification.  The buffer must be at least (30 + 2*msg.DataLen) bytes long.
 *
 * Reference: http://www.seasmart.net/pdf/SeaSmart_HTTP_Protocol_RevG_043012.pdf
 *
 * If the buffer is not long enough, this function returns 0 and does not do
 * anything.
 *
 * If the buffer is long enough, this function returns the number of bytes
 * written including the terminating \0 (but this function does not add the
 * NMEA separator \r\n).
 */
size_t N2kToSeasmart(const tN2kMsg &msg, uint32_t timestamp, char *buffer, size_t size);

/**
 * Converts a null terminated $PCDIN NMEA sentence into a tN2kMsg and updates
 * timestamp with the timestamp of the sentence.
 *
 * Reference: http://www.seasmart.net/pdf/SeaSmart_HTTP_Protocol_RevG_043012.pdf
 *
 * The NMEA \r\n terminator is not required.
 *
 * Returns true if it succeeded.
 * Returns false if an error occured.
 */
bool SeasmartToN2k(const char *buffer, uint32_t &timestamp, tN2kMsg &msg);

#endif
