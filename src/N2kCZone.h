/*
N2kCZone.h

Updated CZONE helpers for the NMEA2000 library.

CZONE switch banks publish their states via standard NMEA2000 PGNs.
Captures gathered with the Actisense NGT-1 show each Contact 6 Plus bank
broadcasting:
  - PGN 65284 (single-frame) with an 8 byte payload.
  - PGN 65301 (manufacturer fast-packet variant) that repeats the same payload;
    only frames whose lower five bits of byte 3 equal 0x1E contain the switch
    summary.

Both PGNs encode six on/off circuits in data byte 4 where bit0 maps to switch 1,
bit1 to switch 2, …, bit5 to switch 6.
*/

#ifndef N2kCZone_h
#define N2kCZone_h

#include "N2kMsg.h"

#include <cstdint>

// Observed PGNs used by BEP/Mastervolt Contact 6 Plus switch banks
static constexpr unsigned long N2kPGNCZoneSwitchStatusSingleFrame = 65284UL;
static constexpr unsigned long N2kPGNCZoneSwitchStatusFastPacket  = 65301UL;

static constexpr uint8_t N2kCZoneSwitchCount = 6;

struct tN2kCZoneSwitchStatus {
  uint8_t BankInstance = 0;       // Byte 2: matches the device instance reported via PGN 126996
  uint8_t FieldIdentifier = 0;    // Byte 3: 0x1E marks the switch summary record
  uint8_t SwitchBits = 0;         // Byte 4: bit0=SW1 … bit5=SW6, 1 = ON
  bool SwitchBitsValid = false;   // True when FieldIdentifier equals 0x1E
  uint8_t Reserved[3] = {0,0,0};  // Trailing bytes when present (bytes 5-7)
  uint8_t RawLen = 0;             // Actual payload length copied into RawData
  uint8_t RawData[8] = {0};       // Copy of the raw payload for debugging/future work
};

// Parse PGN 65284 single-frame switch status reports.
bool ParseN2kPGN65284CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status);

// Parse PGN 65301 switch status frames. Only frames whose FieldIdentifier
// resolves to 0x1E (lower five bits) are treated as switch summaries.
bool ParseN2kPGN65301CZoneSwitchStatus(const tN2kMsg &N2kMsg,
                                       tN2kCZoneSwitchStatus &Status);

inline bool N2kCZoneIsSwitchOn(const tN2kCZoneSwitchStatus &Status,
                               uint8_t SwitchIndex) {
  if (!Status.SwitchBitsValid) return false;
  if (SwitchIndex == 0 || SwitchIndex > N2kCZoneSwitchCount) return false;
  return (Status.SwitchBits >> (SwitchIndex - 1)) & 0x01u;
}

inline uint8_t N2kCZoneSwitchMask(const tN2kCZoneSwitchStatus &Status) {
  return Status.SwitchBits & ((1u << N2kCZoneSwitchCount) - 1u);
}

#endif // N2kCZone_h
