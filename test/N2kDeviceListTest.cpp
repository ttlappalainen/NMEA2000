#include "catch.hpp"
#include "N2kDeviceList.h"
#include "N2kMessages.h"
#include "NMEA2000.h"

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define N2K_TEST_ASAN 1
#endif
#endif
#if defined(__SANITIZE_ADDRESS__)
#define N2K_TEST_ASAN 1
#endif
#if defined(N2K_TEST_ASAN)
// tNMEA2000 and the device list allocate for the lifetime of the device and never
// free; under a sanitized build that is reported as a leak at exit, which is not
// what this test is about.
extern "C" const char *__asan_default_options() { return "detect_leaks=0"; }
#endif

namespace {
// A bus-less tNMEA2000 so the device list has something to attach to.
struct NullBus : public tNMEA2000 {
  bool CANOpen() override { return true; }
  bool CANSendFrame(unsigned long, unsigned char, const unsigned char *, bool) override { return true; }
  bool CANGetFrame(unsigned long &, unsigned char &, unsigned char *) override { return false; }
};

tN2kMsg heading(uint8_t source) {
  tN2kMsg msg;
  SetN2kPGN127250(msg, 1, 1.0, 0.0, 0.0, N2khr_true);
  msg.Source = source;
  return msg;
}

tN2kMsg addressClaim(uint8_t source, uint64_t name) {
  tN2kMsg msg;
  msg.SetPGN(60928L);
  msg.Priority = 6;
  msg.Source = source;
  msg.Destination = 255;
  msg.AddUInt64(name);
  return msg;
}
}

TEST_CASE("device list survives repeated ISO address claims with NAME 0 from one source") {
  NullBus bus;
  tN2kDeviceList list(&bus);

  // On a live bus any message reserves an unnamed device for its source. Without
  // a bus the same unnamed entry is created by a first claim carrying NAME 0.
  list.HandleMsg(addressClaim(0x23, 0));
  REQUIRE(list.FindDeviceBySource(0x23) != 0);

  // A second claim with NAME 0 used to match that very entry by name, delete it
  // and keep using the freed object (heap-use-after-free under ASan).
  list.HandleMsg(addressClaim(0x23, 0));
  REQUIRE(list.FindDeviceBySource(0x23) != 0);

  // Still usable afterwards.
  list.HandleMsg(heading(0x23));
  REQUIRE(list.FindDeviceBySource(0x23) != 0);
}

TEST_CASE("device list moves a named device that re-claims from another address") {
  NullBus bus;
  tN2kDeviceList list(&bus);
  const uint64_t name = 0x1122334455667788ULL;

  list.HandleMsg(addressClaim(0x23, name));
  REQUIRE(list.FindDeviceBySource(0x23) != 0);
  REQUIRE(list.FindDeviceByName(name) != 0);

  list.HandleMsg(addressClaim(0x42, name)); // same NAME now claims 0x42
  REQUIRE(list.FindDeviceBySource(0x42) != 0);
  REQUIRE(list.FindDeviceByName(name) == list.FindDeviceBySource(0x42));
}
