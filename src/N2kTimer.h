#ifndef _N2k_TIMER_H_
#define _N2k_TIMER_H_

/*
N2kTimer.h

Copyright (c) 2022 Timo Lappalainen, Kave Oy, www.kave.fi

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Module is designed to hide millis() function from library and provide best performace
way for timings.

N2kMillis64() is 64 bit millisecond timer, which practically never roll over.

tN2kSyncScheduler uses 64 bit timer for message scheduling. It has offset as defined by 
NMEA2000. It uses global offset for synchronizing messages to open.
Use this for scheduling message sending.

For proper synchronization you define tN2kSyncScheduler for each of your messages as disabled
(default constructor) at beginning. On library setup define OnOpen call back with tNMEA2000::SetOnOpen.
When OnOpen will be called, initialize each message scheduler with period and offset.
As default offsets it is good to use different value for each message so that your system will not
try to send them exactly at same time. For single frame message 5-10 ms offset difference is enough.
For message after multiframe message offset should be (multiframe message frame count)/2+(5 to 10).
NMEA encourages manufacturers to provide the capability of altering, save and restore
period and offset for NMEA certified products on the network. 
See e.g., tN2kGroupFunctionHandlerForPGN126993::HandleRequest in module N2kGroupFunctionDefaultHandlers

tN2kScheduler uses fastest method for scheduling next event. Tested for Teensy and ESP32.

Both schedulers solves roll over problem, which was caused with simple use of millis()

*/

#include <stdint.h>
#include <limits.h>
#include "N2kDef.h"

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM)
#include <esp_timer.h>
#include <esp_attr.h>
  inline uint64_t N2kMillis64() {
    return esp_timer_get_time()/1000ULL;
  }
  inline uint32_t N2kMillis() { return N2kMillis64(); }
#elif defined(ARDUINO)
  #include <Arduino.h>
  uint64_t N2kMillis64();
  inline uint32_t N2kMillis() { return millis(); }
#else
  uint64_t N2kMillis64();
  uint32_t N2kMillis();
#endif

#define N2kScheduler64Disabled 0xffffffffffffffffULL

// *****************************************************************************
// N2kIsTimeBefore is for comparing T1 and T2 even after 32 bit time roll over situation.
// Input:
//   - T1, T2
// Return true, when T1<T2
inline bool N2kIsTimeBefore(uint32_t T1, uint32_t T2) { return (T2-T1)<LONG_MAX; }

// *****************************************************************************
// N2kHasElapsed tests has time elapsed since start. Test works even after 32 bit time roll over situation.
// Input:
//   - Start    Start time
//   - Elapsed  How much should be elapsed from start
//   - Now      Compare (current) time, optional, default N2kMillis() 
// Return true, when Now>Start+Elapsed
inline bool N2kHasElapsed(uint32_t Start, uint32_t Elapsed, uint32_t Now=N2kMillis()) { 
  return Now-(Start+Elapsed)<LONG_MAX; 
}

//------------------------------------------------------------------------------
// tN2kSyncScheduler is meant to use for timed message sending. Each messages
// has defined offset and period, which can be maintained by request group function.
//
// tN2kSyncScheduler uses 64 bit millisecond time to prevent it ever roll over. Offset syncronizes
// with other schedulers. 
class tN2kSyncScheduler {
protected:
  static uint64_t SyncOffset;
  uint64_t NextTime;
  uint32_t Offset;  // Offset to syncronize with others
  uint32_t Period; 
public:
  tN2kSyncScheduler(bool Enable=false, uint32_t _Period=0, uint32_t _Offset=0) { 
    Offset=_Offset; Period=_Period; 
    if ( Enable ) {
      UpdateNextTime();
    } else {
      Disable();
    }
  }

  inline void Disable() { NextTime=N2kScheduler64Disabled; }
  inline bool IsDisabled() const { return NextTime==N2kScheduler64Disabled; }
  inline bool IsEnabled() const { return NextTime!=N2kScheduler64Disabled; }

  inline bool IsTime() { return N2kMillis64()>NextTime; }
  inline uint64_t Remaining() { return IsTime()?0:NextTime-N2kMillis64(); }

  void SetPeriod(uint32_t _Period) { Period=_Period; if ( Period<1 ) Period=1; UpdateNextTime(); }
  void SetOffset(uint32_t _Offset) { Offset=_Offset; UpdateNextTime(); }
  void SetPeriodAndOffset(uint32_t _Period, uint32_t _Offset) { 
    Period=_Period; 
    Offset=_Offset;
    if ( Period==0 ) Disable(); 
    UpdateNextTime(); 
  }
  uint32_t GetOffset() const { return Offset; }
  uint32_t GetPeriod() const { return Period; }
  uint64_t GetNextTime() const { return NextTime; }

  void UpdateNextTime() {
    if ( Period==0 ) {
      Disable();
      return;
    }
    uint64_t now=N2kMillis64();
    if ( (Offset+SyncOffset)>now ) {
      NextTime=(Offset+SyncOffset);
    } else {
      uint64_t n=(now-(Offset+SyncOffset))/Period;
      NextTime=SyncOffset+Offset+(n+1)*Period;
    }
  }

  static void SetSyncOffset() { SyncOffset=N2kMillis64(); }
};

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM) || defined(__linux__) || defined(__linux) || defined(linux)
  #define N2kUse64bitSchedulerTime 1
  #define N2kSchedulerDisabled 0xffffffffffffffffULL
  using tN2kSchedulerTime=uint64_t;
#else
  #include <limits.h>
  #define N2kSchedulerDisabled 0xffffffffUL
  using tN2kSchedulerTime=uint32_t;
#endif

//------------------------------------------------------------------------------
// tN2kScheduler can be used for scheduling, which does not require synchronization
// and can roll over. Scheduling works properly even on 32 bit millisecond timer roll over. 
// tN2kScheduler uses fastest method depending of platform.
class tN2kScheduler {
protected:
  tN2kSchedulerTime NextTime;
  #if !defined(N2kUse64bitSchedulerTime)
  tN2kSchedulerTime Now();
  #endif
public:
  tN2kScheduler(tN2kSchedulerTime _NextTime=N2kSchedulerDisabled) : NextTime(_NextTime) { }
  void Disable() { 
    NextTime=N2kSchedulerDisabled; 
  }
  bool IsDisabled() const { return NextTime==N2kSchedulerDisabled; }
  bool IsEnabled() const { return NextTime!=N2kSchedulerDisabled; }
  bool IsTime() const {
    #if defined(N2kUse64bitSchedulerTime)
    return N2kMillis64()>NextTime;
    #else
    return !IsDisabled() && ( N2kMillis()-NextTime<LONG_MAX );
    #endif
  }
  void FromNow(uint32_t _Add) {
    #if defined(N2kUse64bitSchedulerTime)
    NextTime=N2kMillis64()+_Add;
    #else
    NextTime=Now()+_Add;
    if ( NextTime==N2kSchedulerDisabled ) NextTime=0; // Roll over
    #endif
  }
};

#endif
