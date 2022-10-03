#include "N2kTimer.h"

uint64_t tN2kSyncScheduler::SyncOffset=0;

// *****************************************************************************
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM)
  // N2kMillis64() and N2kMillis() as inline on header
#elif defined(__linux__) || defined(__linux) || defined(linux)
  #include <time.h>
  uint64_t N2kMillis64() {
    struct timespec ticker;

    clock_gettime(CLOCK_MONOTONIC, &ticker);
    return (((uint64_t)ticker.tv_sec * 1000) + (ticker.tv_nsec / 1000000));
  }
uint32_t N2kMillis() { return N2kMillis64(); }
#else 
  #if defined(ARDUINO)
    // N2kMillis() as inline on header
  #else
    extern "C" {
    // Current uptime in milliseconds. Must be implemented by application.
    extern uint32_t millis();
    }
    uint32_t N2kMillis() { return millis(); }
  #endif

  uint64_t N2kMillis64() {
    static uint32_t RollCount=0;
    static uint32_t LastRead=0;
    uint32_t Now=millis();
    if ( LastRead>Now ) RollCount++;
    LastRead=Now;
    return ((uint64_t)RollCount)<<32 | Now;
  }
#endif

#if !defined(N2kUse64bitSchedulerTime)
// *****************************************************************************
tN2kSchedulerTime tN2kScheduler::Now() {
  return N2kMillis();
}
#endif
