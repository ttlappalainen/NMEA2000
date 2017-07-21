/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include <catch.hpp>
#include <N2kMsg.h>
#include <N2kMessages.h>
#include <Seasmart.h>
#include <string.h>

TEST_CASE("SEASMART EXPORT", "[seasmart]") {
  tN2kMsg msg;

  // PGN127257 - "Attitude"
  // SID: 42 - YAW: 1 degrees - Pitch: 10 degrees - Roll: 30 degrees
  msg.SetPGN(127257L);
  msg.Priority=2;
  msg.AddByte(42);
  msg.Add2ByteDouble(DegToRad(1),0.0001);
  msg.Add2ByteDouble(DegToRad(10),0.0001);
  msg.Add2ByteDouble(DegToRad(30),0.0001);
  msg.AddByte(0xff); // Reserved


  SECTION("export to large buffer") {
    char buffer[512];
    const char *expectedResult = "$PCDIN,01F119,00000000,0F,2AAF00D1067414FF*59\r\n";
    REQUIRE( N2kToSeasmart(msg, 0, buffer, sizeof(buffer)) == strlen(expectedResult) );
    REQUIRE( buffer == expectedResult );
  }

  SECTION("export to too small buffer") {
    char buffer[10];

    REQUIRE( N2kToSeasmart(msg, 0, buffer, sizeof(buffer)) == sizeof(buffer) );
    REQUIRE( buffer[sizeof(buffer) - 1] == '\0' );
  }
}

TEST_CASE("SEASMART IMPORT") {
  REQUIRE(false);
}
