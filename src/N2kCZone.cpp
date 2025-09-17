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
constexpr uint16_t kCZoneProprietaryId = 0x9927u;

bool MatchesCZoneHeader(const tN2kMsg &msg, int &index) {
  return msg.Get2ByteUInt(index) == kCZoneProprietaryId;
}

void CopyRawPayload(const tN2kMsg &msg, tN2kCZoneSwitchStatus &status) {
  const int available = std::min(msg.GetRemainingDataLength(0),
                                 static_cast<int>(sizeof(status.RawData)));
  status.RawLen = static_cast<uint8_t>(available);
  std::fill(std::begin(status.RawData), std::end(status.RawData), 0);

  int rawIndex = 0;
  for (int i = 0; i < available; ++i) {
    status.RawData[i] = msg.GetByte(rawIndex);
  }
}

bool PopulateStatusFields(const tN2kMsg &msg, int &index,
                          tN2kCZoneSwitchStatus &status) {
  if (msg.GetRemainingDataLength(index) < 3) return false;

  status.BankInstance = msg.GetByte(index);
  status.FieldIdentifier = msg.GetByte(index);
  status.SwitchBits = msg.GetByte(index);
  status.SwitchBitsValid = (status.FieldIdentifier == kSwitchSummaryField);

  for (uint8_t &reserved_byte : status.Reserved) {
    reserved_byte = 0;
    if (msg.GetRemainingDataLength(index) > 0) {
      reserved_byte = msg.GetByte(index);
    }
  }

  return true;
}
}  // namespace

bool ParseN2kPGN65284CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status) {
  if (N2kMsg.PGN != N2kPGNCZoneSwitchStatusSingleFrame) return false;
  if (N2kMsg.GetRemainingDataLength(0) < 5) return false;

  int index = 0;
  if (!MatchesCZoneHeader(N2kMsg, index)) return false;

  Status = {};
  CopyRawPayload(N2kMsg, Status);
  if (!PopulateStatusFields(N2kMsg, index, Status)) return false;

  return true;
}

bool ParseN2kPGN65301CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status) {
  if (N2kMsg.PGN != N2kPGNCZoneSwitchStatusFastPacket) return false;
  if (N2kMsg.GetRemainingDataLength(0) < 5) return false;

  int index = 0;
  if (!MatchesCZoneHeader(N2kMsg, index)) return false;

  Status = {};
  CopyRawPayload(N2kMsg, Status);
  if (!PopulateStatusFields(N2kMsg, index, Status)) return false;
  if (!Status.SwitchBitsValid) {
    return false;  // Skip frames that do not carry the switch summary
  }
  return true;
}
