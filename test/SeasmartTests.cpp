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
#include <string>
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
    const char *expectedResult = "$PCDIN,01F119,00000000,0F,2AAF00D1067414FF*59";
    REQUIRE( N2kToSeasmart(msg, 0, buffer, sizeof(buffer)) == strlen(expectedResult) );
    REQUIRE( std::string(buffer) == expectedResult );
  }

  SECTION("export to buffer that is exactly the right size") {
    char buffer[30 + 2*8];
    const char *expectedResult = "$PCDIN,01F119,00000000,0F,2AAF00D1067414FF*59";
    REQUIRE( N2kToSeasmart(msg, 0, buffer, sizeof(buffer)) == strlen(expectedResult) );
    REQUIRE( std::string(buffer) == expectedResult );
  }

  SECTION("export to buffer that is too small") {
    char buffer[10];

    REQUIRE( N2kToSeasmart(msg, 0, buffer, sizeof(buffer)) == 0 );
  }
}

TEST_CASE("SEASMART IMPORT") {
  tN2kMsg msg;
  uint32_t timestamp = 1337;

  SECTION("read valid message") {
    const char *message = "$PCDIN,01F119,00000000,0F,2AAF00D1067414FF*59";

    REQUIRE( SeasmartToN2k(message, timestamp, msg) );
    REQUIRE( msg.PGN == 127257L );
    int index = 0;
    REQUIRE( msg.GetByte(index) == 42 );
    REQUIRE( msg.Get2ByteDouble(0.0001, index) == Approx(DegToRad(1)).margin(0.0001) );
    REQUIRE( msg.Get2ByteDouble(0.0001, index) == Approx(DegToRad(10)).margin(0.0001) );
    REQUIRE( msg.Get2ByteDouble(0.0001, index) == Approx(DegToRad(30)).margin(0.0001) );
    REQUIRE( msg.GetByte(index) == 0xFF );
    REQUIRE( timestamp == 0);

    // Note: Seasmart format does not include priority!
    //REQUIRE( msg.Priority == 2 );
  }

  SECTION("read valid message with lower case hexadecimal") {
    const char *message = "$PCDIN,01f119,00000000,0f,2aaf00d1067414ff*59";

    REQUIRE( SeasmartToN2k(message, timestamp, msg) );
    REQUIRE( msg.PGN == 127257L );
  }

  SECTION("read message with invalid checksum") {
    const char *message = "$PCDIN,01F119,00000000,0F,2AAF00D1067414FF*99";

    REQUIRE( !SeasmartToN2k(message, timestamp, msg) );
  }

  SECTION("read message with empty string") {
    const char *message = "";

    REQUIRE( !SeasmartToN2k(message, timestamp, msg) );
  }
}
