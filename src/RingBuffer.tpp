/*
  RingBuffer.tpp  (RingBuffer template code)

  Copyright (c) 2020-2024 Timo Lappalainen

  The MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/

#include "RingBuffer.h"

//#define RING_BUFFER_ERROR_DEBUG
//#define RING_BUFFER_DEBUG
//#define RING_BUFFER_INIT_DEBUG

#if defined(RING_BUFFER_DEBUG) || defined(RING_BUFFER_ERROR_DEBUG) || defined(RING_BUFFER_INIT_DEBUG)
  #include <Arduino.h>
  #ifndef DebugStream
  #define DebugStream Serial
  #endif
#endif

#if defined(RING_BUFFER_ERROR_DEBUG)
  # define RingBufferErrDbgf(fmt, args...)   DebugStream.printf (fmt , ## args)
#else
  # define RingBufferErrDbgf(fmt, args...)
#endif

#if defined(RING_BUFFER_DEBUG)
  # define RingBufferDbgf(fmt, args...)   DebugStream.printf (fmt , ## args)
#else
  # define RingBufferDbgf(fmt, args...)
#endif

#if defined(RING_BUFFER_INIT_DEBUG)
  # define RingBufferInitDbgf(fmt, args...)   DebugStream.printf (fmt , ## args)
#else
  # define RingBufferInitDbgf(fmt, args...)
#endif


// *****************************************************************************
template<typename T>
tRingBuffer<T>::tRingBuffer(uint16_t _size) : head(0), tail(0), size(_size) {
  if ( size<3 ) size=3;
  buffer=new T[size];
}

// *****************************************************************************
template<typename T>
tRingBuffer<T>::~tRingBuffer() {
  delete[] buffer;
}

// *****************************************************************************
template<typename T>
bool tRingBuffer<T>::isEmpty() const {
  return head==tail;
}

// *****************************************************************************
template<typename T>
void tRingBuffer<T>::clear() {
  head=tail=0;
}

// *****************************************************************************
template<typename T>
uint16_t tRingBuffer<T>::count() {
    int32_t entries = head - tail;

    if ( entries < 0 ) entries += size;

    return (uint16_t)entries;
}

// *****************************************************************************
template<typename T>
bool tRingBuffer<T>::add(const T &val) {
  uint16_t nextEntry = (head + 1) % size;

  // Check if the ring buffer is full
  if ( nextEntry == tail ) return false;

  // Add the element to the ring
  memcpy(&(buffer[head]), &val, sizeof(T));

  // Bump the head to point to the next free entry
  head = nextEntry;

  return true;
}

// *****************************************************************************
template<typename T>
bool tRingBuffer<T>::read(T &val) {
  // Check if the ring buffer has data available
  if ( isEmpty() ) return false;

  // copy the message
  memcpy(&val, &buffer[tail], sizeof(T));

  // Bump the tail pointer
  tail = (tail + 1) % size;

  return (true);
}

// *****************************************************************************
template<typename T>
T *tRingBuffer<T>::getAddRef() {
  T *ret=0;

  uint16_t nextEntry = (head + 1) % size;

  // Check if the ring buffer is full
  if ( nextEntry == tail ) return ret;

  ret=&(buffer[head]);

  // Bump the head to point to the next free entry
  head = nextEntry;

  return ret;
}

// *****************************************************************************
template<typename T>
const T *tRingBuffer<T>::getReadRef() {
  // Check if the ring buffer has data available
  if ( isEmpty() ) return 0;

  T *ret=&(buffer[tail]);

  // Bump the tail pointer
  tail = (tail + 1) % size;

  return ret;
}

// *****************************************************************************
template<typename T>
T *tRingBuffer<T>::peek() {
  // Check if the ring buffer has data available
  if ( isEmpty() ) return 0;

  T *ret=&(buffer[tail]);

  return ret;
}

#define INVALID_PRIORITY 0xff

//==============================================================================
// class tPriorityRingBuffer

// *****************************************************************************
template<typename T>
tPriorityRingBuffer<T>::tPriorityRingBuffer(uint16_t _size, uint8_t _maxPriorities) : 
    head(0), tail(0), size(_size), maxPriorities(_maxPriorities) {
  if ( size<3 ) size=3;
  if ( maxPriorities<1 ) maxPriorities=1;
  if ( maxPriorities==255 ) maxPriorities=254;
  RingBufferInitDbgf("tPriorityRingBuffer<T>::tPriorityRingBuffer, size:%u, priorities:%u\n",size,maxPriorities);
  RingBufferInitDbgf(" - size:%u, priorities:%u, memory required: %u\n",size,maxPriorities,size*sizeof(tValueSlot)+maxPriorities*sizeof(tPriorityRef));
  buffer=new tValueSlot[size];
  RingBufferInitDbgf(" - ring buffer allocated to %x\n",(uint32_t)buffer);
  priorityReferencies=new tPriorityRef[maxPriorities];
  RingBufferInitDbgf(" - priorityReferencies allocated to %x\n",(uint32_t)priorityReferencies);
}

// *****************************************************************************
template<typename T>
tPriorityRingBuffer<T>::~tPriorityRingBuffer() {
  delete[] buffer;
  delete[] priorityReferencies;
}

// *****************************************************************************
template<typename T>
uint32_t tPriorityRingBuffer<T>::getMemSize(uint16_t _size, uint8_t _maxPriorities) {
  if ( _size<3 ) _size=3;
  if ( _maxPriorities<1 ) _maxPriorities=1;
  if ( _maxPriorities==255 ) _maxPriorities=254;

  return sizeof(::tPriorityRingBuffer<T>)+_size*sizeof(tValueSlot)+_maxPriorities*sizeof(tPriorityRef);
}

// *****************************************************************************
template<typename T>
bool tPriorityRingBuffer<T>::isEmpty(uint8_t _priority) const {
  if ( _priority>=maxPriorities) {
    return head==tail;
  } else {
    return priorityReferencies[_priority].next==INVALID_RING_REF;
  }
}

// *****************************************************************************
template<typename T>
void tPriorityRingBuffer<T>::clear() {
  head=tail=0;
  for ( uint16_t i=0; i<maxPriorities; i++ ) priorityReferencies[i].clear();
}

// *****************************************************************************
template<typename T>
void tPriorityRingBuffer<T>::clean() {
}

// *****************************************************************************
template<typename T>
uint16_t tPriorityRingBuffer<T>::count() {
    int32_t entries = head - tail;

    if ( entries < 0 ) entries += size;

    return (uint16_t)entries;
}

// *****************************************************************************
template<typename T>
bool tPriorityRingBuffer<T>::add(const T &val, uint8_t _priority) {
  T *newRef;
  if ( (newRef=getAddRef(_priority))!=0 ) {
    memcpy(newRef, &val, sizeof(T));
    return true;
  }

  return false;
}

// *****************************************************************************
template<typename T>
bool tPriorityRingBuffer<T>::read(T &val) {
  const T *newRef;
  if ( (newRef=getReadRef())!=0 ) {
    memcpy(&val, newRef, sizeof(T));
    return true;
  }

  return false;
}

// *****************************************************************************
template<typename T>
T *tPriorityRingBuffer<T>::getAddRef(uint8_t _priority) {
  T *ret=0;

  if ( _priority>=maxPriorities ) _priority=maxPriorities-1;

  uint16_t nextEntry = (head + 1) % size;

  // Check if the ring buffer is full
  if ( nextEntry == tail ) {
    clean();
    // check again
    if ( nextEntry == tail ) {
      RingBufferErrDbgf("tPriorityRingBuffer<T>::getAddRef, ring buffer full\n");
      return ret;
    }
  }

  ret=&(buffer[head].Value);
  buffer[head].priority=_priority;
  buffer[head].next=INVALID_RING_REF;
  if ( priorityReferencies[_priority].next==INVALID_RING_REF ) {
    priorityReferencies[_priority].next=head;
  } else {
    buffer[priorityReferencies[_priority].last].next=head;
  }
  priorityReferencies[_priority].last=head;

  RingBufferDbgf("tPriorityRingBuffer<T>::getAddRef, added new item head:%u, priority:%u\n",head,_priority);

  // Bump the head to point to the next free entry
  head = nextEntry;

  return ret;
}

// *****************************************************************************
template<typename T>
const T *tPriorityRingBuffer<T>::getReadRef(uint8_t _priority) {
  const T *ret=0;

  if ( _priority>=maxPriorities ) _priority=maxPriorities-1;

  uint16_t ref=priorityReferencies[_priority].next;
  if ( ref==INVALID_RING_REF ) {
    RingBufferDbgf("tPriorityRingBuffer<T>::getReadRef, no items of priority:%u\n",_priority);
    return ret;
  }

  ret=&(buffer[ref].Value);
  priorityReferencies[_priority].next=buffer[ref].next;
  if ( priorityReferencies[_priority].next==INVALID_RING_REF ) {
    priorityReferencies[_priority].last=INVALID_RING_REF;
  }
  // Update tail, if we are removing first
  if ( ref==tail ) {
    for ( tail = (tail + 1) % size; 
          tail!=head && buffer[tail].priority==INVALID_PRIORITY;
          tail = (tail + 1) % size );
  }
  buffer[ref].next=INVALID_RING_REF; // Release slot
  buffer[ref].priority=INVALID_PRIORITY;

  RingBufferDbgf("tPriorityRingBuffer<T>::getReadRef, read item ref:%u, priority:%u, tail:%u\n",ref,_priority,tail);

  return ret;
}

// *****************************************************************************
template<typename T>
const T *tPriorityRingBuffer<T>::getReadRef(uint8_t *_priority) {
  for ( uint8_t _pri=0; _pri<maxPriorities; _pri++ ) {
    if ( priorityReferencies[_pri].next!=INVALID_RING_REF ) {
      if ( _priority!=0 ) *_priority=_pri;
      return getReadRef(_pri);
    }
  }

  return 0;
}
