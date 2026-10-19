#include "catch.hpp"
#include "N2kMsg.h"
#include "Seasmart.h"
#include <cstring>
#include <string>

namespace {
// Copy into an exactly-sized heap buffer so an over-read is not hidden by slack.
bool parseExact(const std::string &sentence, tN2kMsg &msg, uint32_t &ts) {
  char *buf = new char[sentence.size() + 1];
  std::memcpy(buf, sentence.data(), sentence.size());
  buf[sentence.size()] = 0;
  bool ok = SeasmartToN2k(buf, ts, msg);
  delete[] buf;
  return ok;
}
}

TEST_CASE("Seasmart parser rejects truncated $PCDIN sentences without reading past them") {
  tN2kMsg msg;
  uint32_t ts;
  const char *truncated[] = {
      "$PCDIN",
      "$PCDIN,",
      "$PCDIN,01F010",
      "$PCDIN,01F010,",
      "$PCDIN,01F010,0000AC05",
      "$PCDIN,01F010,0000AC05,",
      "$PCDIN,01F010,0000AC05,23",
      "$PCDIN,01F010,0000AC05,23,",
      "$PCDIN,01F010,0000AC05,23,0110277FFF7FFFFD",      // no '*'
      "$PCDIN,01F010,0000AC05,23,0110277FFF7FFFFD*",     // no checksum digits
      "$PCDIN,01F010,0000AC05,23,0110277FFF7FFFFD*5",
  };
  for (const char *t : truncated) {
    INFO(t);
    REQUIRE_FALSE(parseExact(t, msg, ts));
  }
}

TEST_CASE("Seasmart parser still accepts a well-formed sentence") {
  tN2kMsg msg;
  uint32_t ts = 0;
  REQUIRE(parseExact("$PCDIN,01F112,0000AC05,23,0110277FFF7FFFFD*24", msg, ts));
  REQUIRE(msg.PGN == 127250);
  REQUIRE(msg.Source == 0x23);
  REQUIRE(msg.DataLen == 8);
  REQUIRE(ts == 0x0000AC05);
}
