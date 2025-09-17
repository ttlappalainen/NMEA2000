/*
N2kCZone.cpp

Implementation of the trimmed CZONE helpers that map the observed switch bank
messages to a convenient structure for applications built on top of the
NMEA2000 library.
*/

#include "N2kCZone.h"

#include <algorithm>
#include <iterator>

namespace {
constexpr uint8_t kSwitchSummaryField = 0x1Eu;
constexpr uint16_t kCZoneManufacturerCode = 0x0127u;  // BEP Marine
constexpr uint8_t kCZoneIndustryGroup = 0x03u;         // Marine industry group

bool MatchesCZoneHeader(const tN2kMsg &msg) {
  if (msg.DataLen < 2) return false;
  const uint16_t header = static_cast<uint16_t>(msg.Data[0]) |
                          (static_cast<uint16_t>(msg.Data[1]) << 8);
  const uint16_t manufacturer = header & 0x07ffu;
  const uint8_t industry = static_cast<uint8_t>((header >> 11) & 0x07u);
  return manufacturer == kCZoneManufacturerCode && industry == kCZoneIndustryGroup;
}

void FillStatusFromMsg(const tN2kMsg &msg, tN2kCZoneSwitchStatus &status) {
  status.RawLen = (msg.DataLen > 8) ? 8 : static_cast<uint8_t>(msg.DataLen);
  std::fill(std::begin(status.RawData), std::end(status.RawData), 0);
  for (uint8_t i = 0; i < status.RawLen; ++i) {
    status.RawData[i] = msg.Data[i];
  }

  status.BankInstance = (msg.DataLen > 2) ? msg.Data[2] : 0;
  status.FieldIdentifier = (msg.DataLen > 3) ? msg.Data[3] : 0;
  status.SwitchBits = (msg.DataLen > 4) ? msg.Data[4] : 0;
  status.SwitchBitsValid = (status.FieldIdentifier == kSwitchSummaryField);
  status.Reserved[0] = (msg.DataLen > 5) ? msg.Data[5] : 0;
  status.Reserved[1] = (msg.DataLen > 6) ? msg.Data[6] : 0;
  status.Reserved[2] = (msg.DataLen > 7) ? msg.Data[7] : 0;
}
}  // namespace

bool ParseN2kPGN65284CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status) {
  if (N2kMsg.PGN != N2kPGNCZoneSwitchStatusSingleFrame) return false;
  if (N2kMsg.DataLen < 5) return false;
  if (!MatchesCZoneHeader(N2kMsg)) return false;

  FillStatusFromMsg(N2kMsg, Status);
  return true;
}

bool ParseN2kPGN65301CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status) {
  if (N2kMsg.PGN != N2kPGNCZoneSwitchStatusFastPacket) return false;
  if (N2kMsg.DataLen < 5) return false;
  if (!MatchesCZoneHeader(N2kMsg)) return false;

  FillStatusFromMsg(N2kMsg, Status);
  if (!Status.SwitchBitsValid) {
    return false;  // Skip frames that do not carry the switch summary
  }
  return true;
}
