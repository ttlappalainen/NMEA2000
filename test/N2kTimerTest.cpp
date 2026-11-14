#include <catch.hpp>
#include <N2kTimer.h>
#include <vector>
#include <algorithm>

namespace {
std::vector<uint64_t> clockTicks;
size_t clockIndex;
void SetClock(std::initializer_list<uint64_t> ticks) {
  clockTicks=ticks;
  clockIndex=0;
}
class TestScheduler : public tN2kSyncScheduler {
public:
  void SetNext(uint64_t next) { NextTime=next; }
};
}

// Link the real header's scheduler against a deterministic clock instead of
// N2kTimer.cpp, without changing a device's clock or sleeping in the tests.
uint64_t tN2kSyncScheduler::SyncOffset=0;
uint64_t N2kMillis64() {
  return clockTicks[std::min(clockIndex++,clockTicks.size()-1)];
}
uint32_t N2kMillis() { return static_cast<uint32_t>(N2kMillis64()); }

TEST_CASE("Scheduler remaining time before the deadline") {
  TestScheduler scheduler;
  scheduler.SetNext(1000);
  SetClock({900});
  REQUIRE(scheduler.Remaining()==100);
}

TEST_CASE("Scheduler remaining time at the deadline") {
  TestScheduler scheduler;
  scheduler.SetNext(1000);
  SetClock({1000});
  REQUIRE(scheduler.Remaining()==0);
}

TEST_CASE("Scheduler remaining time after the deadline") {
  TestScheduler scheduler;
  scheduler.SetNext(1000);
  SetClock({1001});
  REQUIRE(scheduler.Remaining()==0);
}

TEST_CASE("Scheduler remaining time cannot underflow across clock reads") {
  TestScheduler scheduler;
  scheduler.SetNext(1000);
  SetClock({999,1001});
  REQUIRE(scheduler.Remaining()<=1);
  SetClock({1002});
  REQUIRE(scheduler.Remaining()==0);
}
