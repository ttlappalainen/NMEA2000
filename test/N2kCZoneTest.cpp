#include "catch.hpp"

#include "../src/N2kCZone.h"

namespace {
void FillMsg(tN2kMsg &msg, unsigned long pgn, std::initializer_list<uint8_t> payload) {
  msg.SetPGN(pgn);
  msg.Priority = 6;
  msg.Source = 0x01;
  msg.Destination = 0xFF;
  msg.DataLen = payload.size();
  size_t i = 0;
  for (uint8_t byte : payload) {
    msg.Data[i++] = byte;
  }
}
}

TEST_CASE("PGN 65284 single-frame switch status parsing") {
  tN2kMsg msg;
  FillMsg(msg, N2kPGNCZoneSwitchStatusSingleFrame,
          {0x27, 0x99, 0x01, 0x1e, 0x07, 0x00, 0x00, 0x00});

  tN2kCZoneSwitchStatus status{};
  REQUIRE(ParseN2kPGN65284CZoneSwitchStatus(msg, status));
  CHECK(status.BankInstance == 0x01);
  CHECK(status.FieldIdentifier == 0x1e);
  CHECK(status.SwitchBitsValid);
  CHECK(status.SwitchBits == 0x07);
  CHECK(status.RawLen == 8);
  CHECK(N2kCZoneIsSwitchOn(status, 1));
  CHECK(N2kCZoneIsSwitchOn(status, 2));
  CHECK(N2kCZoneIsSwitchOn(status, 3));
  CHECK_FALSE(N2kCZoneIsSwitchOn(status, 4));
  CHECK_FALSE(N2kCZoneIsSwitchOn(status, 7));
}

TEST_CASE("PGN 65301 summary frames mirror switch status") {
  tN2kMsg msg;
  FillMsg(msg, N2kPGNCZoneSwitchStatusFastPacket,
          {0x27, 0x99, 0x03, 0x1e, 0x37, 0x00, 0x00, 0x00});

  tN2kCZoneSwitchStatus status{};
  REQUIRE(ParseN2kPGN65301CZoneSwitchStatus(msg, status));
  CHECK(status.BankInstance == 0x03);
  CHECK(status.SwitchBitsValid);
  CHECK(status.SwitchBits == 0x37);
  CHECK(N2kCZoneIsSwitchOn(status, 1));
  CHECK(N2kCZoneIsSwitchOn(status, 2));
  CHECK(N2kCZoneIsSwitchOn(status, 3));
  CHECK_FALSE(N2kCZoneIsSwitchOn(status, 4));
  CHECK(N2kCZoneIsSwitchOn(status, 5));
  CHECK(N2kCZoneIsSwitchOn(status, 6));
}

TEST_CASE("PGN 65301 non-summary frames are ignored") {
  tN2kMsg msg;
  FillMsg(msg, N2kPGNCZoneSwitchStatusFastPacket,
          {0x27, 0x99, 0x03, 0x9e, 0x00, 0x00, 0x00, 0x00});

  tN2kCZoneSwitchStatus status{};
  REQUIRE_FALSE(ParseN2kPGN65301CZoneSwitchStatus(msg, status));
}

TEST_CASE("CZone parsers reject frames with foreign proprietary header") {
  tN2kMsg msg;
  FillMsg(msg, N2kPGNCZoneSwitchStatusSingleFrame,
          {0x27, 0x98, 0x01, 0x1e, 0x07});

  tN2kCZoneSwitchStatus status{};
  REQUIRE_FALSE(ParseN2kPGN65284CZoneSwitchStatus(msg, status));

  FillMsg(msg, N2kPGNCZoneSwitchStatusFastPacket,
          {0x26, 0x99, 0x03, 0x1e, 0x37});
  REQUIRE_FALSE(ParseN2kPGN65301CZoneSwitchStatus(msg, status));
}

TEST_CASE("PGN guard clauses reject unexpected data") {
  tN2kMsg msg;
  FillMsg(msg, 127501UL, {0x00, 0x00, 0x00, 0x00});

  tN2kCZoneSwitchStatus status{};
  REQUIRE_FALSE(ParseN2kPGN65284CZoneSwitchStatus(msg, status));
  REQUIRE_FALSE(ParseN2kPGN65301CZoneSwitchStatus(msg, status));
}
