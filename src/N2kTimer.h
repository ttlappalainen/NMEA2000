#ifndef _N2k_TIMER_H_
#define _N2k_TIMER_H_

/*
 * N2kTimer.h
 * 
 * Copyright (c) 2022-2024 Timo Lappalainen, Kave Oy, www.kave.fi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*************************************************************************//**
 * \file  N2kTimer.h
 * \brief The file contains function and classes for best timing performance
 * 
 * Module is designed to hide millis() function from library and provide 
 * best performance way for timings.
 * 
 * N2kMillis64() is 64 bit millisecond timer, which practically never
 * roll over.
 *  
 * tN2kSyncScheduler uses 64 bit timer for message scheduling. It has offset
 * as defined by NMEA2000. It uses global offset for synchronizing messages
 * to open. Use this for scheduling message sending.
 * 
 * For proper synchronization you define tN2kSyncScheduler for each of your 
 * messages as disabled (default constructor) at beginning. On library setup
 * define OnOpen call back with tNMEA2000::SetOnOpen. When OnOpen will be
 * called, it initializes each message scheduler with period and offset. As 
 * default offsets it is good to use different value for each message so 
 * that your system will not try to send them exactly at same time. For
 * single frame message 5-10 ms offset difference is enough.
 * For message after multiframe message offset should be (multiframe message
 * frame count)/2+(5 to 10). NMEA encourages manufacturers to provide the 
 * capability of altering, save and restore period and offset for NMEA
 * certified products on the network. See \ref 
 * tN2kGroupFunctionHandlerForPGN126993::HandleRequest in module 
 * N2kGroupFunctionDefaultHandlers
 * 
 * tN2kScheduler uses fastest method for scheduling next event. 
 * Tested for Teensy and ESP32.
 * 
 * Both schedulers solves roll over problem, which was caused with simple
 * use of millis()
 * 
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

/************************************************************************//**
 * \brief Comparing 2 values even after 32 bit time roll over situation.
 *
 * N2kIsTimeBefore is for comparing T1 and T2 even after 32 bit time roll
 * over situation.
 * 
 * \param T1    Time value 1
 * \param T2    Time value 2
 * \return true    when T1<T2
 * \return false 
 */
inline bool N2kIsTimeBefore(uint32_t T1, uint32_t T2) { return (T2-T1)<INT32_MAX; }

/************************************************************************//**
 * \brief Has time elapsed since start
 * 
 * N2kHasElapsed tests has time elapsed since start. Test works even after 
 * 32 bit time roll over situation.
 * 
 * \param Start   Start time
 * \param Elapsed How much should be elapsed from start
 * \param Now     Compare (current) time, optional, default N2kMillis() 
 * 
 * \return true   when Now>Start+Elapsed
 * \return false 
 */
inline bool N2kHasElapsed(uint32_t Start, uint32_t Elapsed, uint32_t Now=N2kMillis()) { 
  return Now-(Start+Elapsed)<INT32_MAX; 
}

/************************************************************************//**
 * \class   tN2kSyncScheduler
 * \brief   Synchronized Scheduler for timed message sending
 *
 * tN2kSyncScheduler is meant to use for timed message sending. Each messages
 * has defined offset and period, which can be maintained by request group 
 * function. tN2kSyncScheduler uses 64 bit millisecond time to prevent it
 * ever roll over. Offset synchronizes with other schedulers. 
 */
class tN2kSyncScheduler {
protected:
  /** \brief ??? */
  static uint64_t SyncOffset;
  /** \brief Timestamp for next event */
  uint64_t NextTime;
  /** \brief Offset to synchronize with others */
  uint32_t Offset;
  /** \brief Period for scheduler */
  uint32_t Period; 
public:

  /************************************************************************//**
   * \brief Constructor for sync scheduler
   * 
   * This function initializes all parameters.
   *
   * \param Enable      default = false
   * \param _Period     Period [ms] of the scheduler, default = 0
   * \param _Offset     Offset [ms] of the scheduler, default = 0
   */
  tN2kSyncScheduler(bool Enable=false, uint32_t _Period=0, uint32_t _Offset=0) { 
    Offset=_Offset; Period=_Period; 
    if ( Enable ) {
      UpdateNextTime();
    } else {
      Disable();
    }
  }

  /************************************************************************//**
   * \brief Disable Scheduler 
   */
  inline void Disable() { NextTime=N2kScheduler64Disabled; }
  
  /************************************************************************//**
   * \brief Checks if Scheduler is disabled 
   *
   * \return true   -> Scheduler is disabled
   * \return false 
   */
  inline bool IsDisabled() const { return NextTime==N2kScheduler64Disabled; }

  /************************************************************************//**
   * \brief Checks if Scheduler is enabled 
   *
   * \return true   -> Scheduler is enabled
   * \return false 
   */
  inline bool IsEnabled() const { return NextTime!=N2kScheduler64Disabled; }

  /************************************************************************//**
   * \brief Check if ít is time
   *
   * \return true -> when actual timestamp is greater the \ref NextTime
   * \return false 
   */
  inline bool IsTime() { return N2kMillis64()>NextTime; }

  /************************************************************************//**
   * \brief Calculate remaining time 
   *
   * \return uint64_t Remaining time till \ref NextTime
   */
  inline uint64_t Remaining() { return IsTime()?0:NextTime-N2kMillis64(); }

  /************************************************************************//**
   * \brief Set the Period of the Scheduler
   *
   * \param _Period Period in [ms] of the Scheduler
   */
  void SetPeriod(uint32_t _Period) { Period=_Period; if ( Period<1 ) Period=1; UpdateNextTime(); }
  
  /************************************************************************//**
   * \brief Set the Offset of the Scheduler
   *
   * \param _Offset Offset in [ms] of the Scheduler
   */
  void SetOffset(uint32_t _Offset) { Offset=_Offset; UpdateNextTime(); }

  /************************************************************************//**
   * \brief Set the Period And Offset of the Scheduler
   *
   * \param _Period Period in [ms] of the Scheduler
   * \param _Offset Offset in [ms] of the Scheduler
   */
  void SetPeriodAndOffset(uint32_t _Period, uint32_t _Offset) { 
    Period=_Period; 
    Offset=_Offset;
    if ( Period==0 ) Disable(); 
    UpdateNextTime(); 
  }

  /************************************************************************//**
   * \brief Get the Offset of the Scheduler
   *
   * \return uint32_t Offset in [ms] of the scheduler
   */
  uint32_t GetOffset() const { return Offset; }

  /************************************************************************//**
   * \brief Get the Period of the Scheduler
   *
   * \return uint32_t Period in [ms] of the scheduler
   */
  uint32_t GetPeriod() const { return Period; }

  /************************************************************************//**
   * \brief Get the Timestamp of the NextTime 
   *
   * \return uint64_t Timestamp
   */
  uint64_t GetNextTime() const { return NextTime; }

  /************************************************************************//**
   * \brief Update the timestamp for NextTime
   *
   */
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
  /************************************************************************//**
   * \brief Set the SyncOffset of the scheduler
   *
   */
  static void SetSyncOffset() { SyncOffset=N2kMillis64(); }
};

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP_PLATFORM) || defined(__linux__) || defined(__linux) || defined(linux)
  /** \brief Use the 64bit timer for certain platforms */
  #define N2kUse64bitSchedulerTime 1
  /** \brief Defined Value for disabling the scheduler */
  #define N2kSchedulerDisabled 0xffffffffffffffffULL
  using tN2kSchedulerTime=uint64_t;
#else
  #include <limits.h>
  /** \brief Defined Value for disabling the scheduler */
  #define N2kSchedulerDisabled 0xffffffffUL
  using tN2kSchedulerTime=uint32_t;
#endif

//------------------------------------------------------------------------------
// 
/************************************************************************//**
 * \brief Un synchronized Scheduler which can roll over
 * 
 * tN2kScheduler can be used for scheduling, which does not require 
 * synchronization and can roll over. Scheduling works properly even on 
 * 32bit millisecond timer roll over. tN2kScheduler uses fastest method
 * depending of platform.
 * 
 */
class tN2kScheduler {
protected:
  
  /** \brief Timestamp for next event */
  tN2kSchedulerTime NextTime;
  #if !defined(N2kUse64bitSchedulerTime)
  tN2kSchedulerTime Now();
  #endif
public:
  
  /************************************************************************//**
   * \brief Constructor
   *
   * \param _NextTime   Timestamp for next event
   */
  tN2kScheduler(tN2kSchedulerTime _NextTime=N2kSchedulerDisabled) : NextTime(_NextTime) { }

  /************************************************************************//**
   * \brief Disable the Scheduler
   */
  void Disable() { 
    NextTime=N2kSchedulerDisabled; 
  }

  /************************************************************************//**
   * \brief Check if the scheduler is disabled
   *
   * \return true   scheduler is disabled
   * \return false 
   */
  bool IsDisabled() const { return NextTime==N2kSchedulerDisabled; }

  /************************************************************************//**
   * \brief Check if the scheduler is enabled
   *
   * \return true     scheduler is enabled
   * \return false 
   */
  bool IsEnabled() const { return NextTime!=N2kSchedulerDisabled; }

  /************************************************************************//**
   * \brief Is it time for the next event
   *
   * \return true   when now > nextTime
   * \return false 
   */
  bool IsTime() const {
    #if defined(N2kUse64bitSchedulerTime)
    return N2kMillis64()>NextTime;
    #else
    return !IsDisabled() && ( N2kMillis()-NextTime<INT32_MAX );
    #endif
  }

  /************************************************************************//**
   * \brief Set Timestamp for next event relative to now
   *
   * \param _Add Time delay from now for next event
   */
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
