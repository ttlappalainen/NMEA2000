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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "Seasmart.h"

/* Some private helper functions to generate hex-serialized NMEA messages */
static const char *hex = "0123456789ABCDEF";

static int appendByte(char *s, uint8_t byte) {
  s[0] = hex[byte >> 4];
  s[1] = hex[byte & 0xf];
  return 2;
}

static int append2Bytes(char *s, uint16_t i) {
  appendByte(s, i >> 8);
  appendByte(s + 2, i & 0xff);
  return 4;
}

static int appendWord(char *s, uint32_t i) {
  append2Bytes(s, i >> 16);
  append2Bytes(s + 4, i & 0xffff);
  return 8;
}

static uint8_t nmea_compute_checksum(const char *sentence) {
  if (sentence == 0) {
    return 0;
  }

  // Skip the $ at the beginning
  int i = 1;

  int checksum = 0;
  while (sentence[i] != '*') {
    checksum ^= sentence[i];
    i++;
  }
  return checksum;
}

size_t N2kToSeasmart(const tN2kMsg &msg, uint32_t timestamp, char *buffer, size_t size) {
  size_t pcdin_sentence_length = 6+1+6+1+8+1+2+1+msg.DataLen*2+1+2 + 1;

  if (size < pcdin_sentence_length) {
    return 0;
  }

  char *s = buffer;

  strcpy(s, "$PCDIN,");
  s += 7;
  s += appendByte(s, msg.PGN >> 16);
  s += append2Bytes(s, msg.PGN & 0xffff);
  *s++ = ',';
  s += appendWord(s, timestamp);
  *s++ = ',';
  s += appendByte(s, msg.Source);
  *s++ = ',';

  for (int i = 0; i < msg.DataLen; i++) {
    s += appendByte(s, msg.Data[i]);
  }

  *s++ = '*';
  s += appendByte(s, nmea_compute_checksum(buffer));
  *s = 0;

  return (size_t)(s - buffer);
}

/*
 * Attemts to read n bytes in hexadecimal from input string to value.
 *
 * Returns true if successful, false otherwise.
 */
static bool readNHexByte(const char *s, unsigned int n, uint32_t &value) {
  if (strlen(s) < 2*n) {
    return -1;
  }
  for (unsigned int i = 0; i < 2*n; i++) {
    if (!isxdigit(s[i])) {
      return -1;
    }
  }

  char sNumber[2*n + 1];
  strncpy(sNumber, s, sizeof(sNumber));
  sNumber[sizeof(sNumber) - 1] = 0;

  value = strtol(sNumber, 0, 16);
  return true;
}

bool SeasmartToN2k(const char *buffer, uint32_t &timestamp, tN2kMsg &msg) {
  msg.Clear();

  const char *s = buffer;
  if (strncmp("$PCDIN,", s, 6) != 0) {
    return false;
  }
  s += 7;

  uint32_t pgnHigh;
  uint32_t pgnLow;
  if (!readNHexByte(s, 1, pgnHigh)) {
    return false;
  }
  s += 2;
  if (!readNHexByte(s, 2, pgnLow)) {
    return false;
  }
  s += 5;
  msg.PGN = (pgnHigh << 16) + pgnLow;

  if (!readNHexByte(s, 4, timestamp)) {
    return false;
  }
  s += 9;

  uint32_t source;
  if (!readNHexByte(s, 1, source)) {
    return false;
  }
  msg.Source = source;
  s += 3;

  int dataLen = 0;
  while (s[dataLen] != 0 && s[dataLen] != '*') {
    dataLen++;
  }
  if (dataLen % 2 != 0) {
    return false;
  }
  dataLen /= 2;

  msg.DataLen = dataLen;
  if (msg.DataLen > msg.MaxDataLen) {
    return false;
  }
  for (int i = 0; i < dataLen; i++) {
    uint32_t byte;
    if (!readNHexByte(s, 1, byte)) {
      return false;
    }
    msg.Data[i] = byte;
    s += 2;
  }

  // Skip the terminating '*' which marks beginning of checksum
  s += 1;
  uint32_t checksum;
  if (!readNHexByte(s, 1, checksum)) {
    return false;
  }

  if (checksum != nmea_compute_checksum(buffer)) {
    return false;
  }

  return true;
}
