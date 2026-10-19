#include "catch.hpp"
#include "ActisenseReader.h"
#include "N2kMsg.h"
#include "N2kStream.h"
#include <vector>
#include <cstdint>

namespace {
struct ByteStream : public N2kStream {
  std::vector<uint8_t> bytes;
  size_t pos = 0;
  int read() override { return pos < bytes.size() ? bytes[pos++] : -1; }
  int peek() override { return pos < bytes.size() ? bytes[pos] : -1; }
  size_t write(const uint8_t *, size_t size) override { return size; }
};

// Build a BST frame: DLE STX <type> <len> <body> <checksum> DLE ETX, escaping 0x10.
std::vector<uint8_t> frame(uint8_t type, const std::vector<uint8_t> &body) {
  std::vector<uint8_t> payload{type, (uint8_t)body.size()};
  payload.insert(payload.end(), body.begin(), body.end());
  unsigned sum = 0;
  for (uint8_t b : payload) sum += b;
  payload.push_back((uint8_t)((256 - (sum % 256)) % 256));
  std::vector<uint8_t> out{0x10, 0x02};
  for (uint8_t b : payload) {
    out.push_back(b);
    if (b == 0x10) out.push_back(0x10);
  }
  out.push_back(0x10);
  out.push_back(0x03);
  return out;
}

// N2K data (0x93) body: prio, PGN(3), dst, src, time(4), dataLen, then `trailing` data bytes.
std::vector<uint8_t> n2kDataBody(unsigned long pgn, uint8_t dataLen, size_t trailing) {
  std::vector<uint8_t> body{6, (uint8_t)pgn, (uint8_t)(pgn >> 8), (uint8_t)(pgn >> 16), 255, 65, 0xD2, 0x04, 0, 0, dataLen};
  body.insert(body.end(), trailing, 0x41);
  return body;
}

bool decode(const std::vector<uint8_t> &bytes, tN2kMsg &msg) {
  ByteStream s;
  s.bytes = bytes;
  tActisenseReader r;
  r.SetReadStream(&s);
  bool got = false;
  for (int i = 0; i < 64 && s.pos < s.bytes.size(); i++) got |= r.GetMessageFromStream(msg, true);
  return got;
}
}

TEST_CASE("Actisense reader decodes a well-formed N2K data frame") {
  tN2kMsg msg;
  REQUIRE(decode(frame(0x93, n2kDataBody(127250, 8, 8)), msg));
  REQUIRE(msg.PGN == 127250);
  REQUIRE(msg.DataLen == 8);
  REQUIRE(msg.Source == 65);
}

TEST_CASE("Actisense reader rejects a frame whose length byte exceeds the declared data length") {
  // DataLen 200 passes the MaxDataLen check, but the frame carries 244 data bytes:
  // the copy used to run to the frame length and write past tN2kMsg::Data[223].
  tN2kMsg msg;
  REQUIRE_FALSE(decode(frame(0x93, n2kDataBody(129794, 200, 244)), msg));
}

TEST_CASE("Actisense reader rejects a frame whose data is shorter than declared") {
  tN2kMsg msg;
  REQUIRE_FALSE(decode(frame(0x93, n2kDataBody(127250, 8, 4)), msg));
}
