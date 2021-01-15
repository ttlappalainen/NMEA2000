/*
  RingBuffer.h

  Copyright (c) 2020-2021 Timo Lappalainen

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

  Simple tRingBuffer and tPriorityRingBuffer template classes.

  With tRingBuffer one can save simple data structures to ring buffer. 
  Note that you have to take care of data locking from other threads or interrupts for copy time.
  E.g.

  struct tCANData {
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
  };

  tPriorityRingBuffer<tCANData> CANMessages(100);
  ...
  void SendMessage() {
    tCANData msgOut;
    msgOut.id=1;
    msgOut.len=1;
    msgOut.data[0]=1;
    lockData();
    CANMessages.add(msgOut);
    releaseData();
  }

  As an alternative if you want to avoid data copying twice, you can request reference to next
  data to be saved and copy data directly to it.

  void SendMessage() {
    tCANData *msgOut;
    lockData();
    if ( (msgOut=CANMessages)!=0 ) {
      msgOut->id=1;
      msgOut->len=1;
      msgOut->data[0]=1;
    }
    releaseData();
  }

  tPriorityRingBuffer is similar as tRingBuffer, but it extends functionality with item priority. When you add
  items to buffer, you can give them priority and when you read them, highest priority item will be read out
  first.
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <cstdint>

template <typename T> class tRingBuffer {

protected:
  T *buffer;
  uint16_t head;
  uint16_t tail;
  uint16_t size;

public:
  tRingBuffer(uint16_t _size);
  virtual ~tRingBuffer();
  uint16_t getSize() const { return size; }
  bool isEmpty() const;
  void clear();
  uint16_t count();
  // Get pointer to next add item if available.
  T *getAddRef();
  bool add(const T &val);
  // Get pointer to next read item if available.
  const T *getReadRef();
  bool read(T &val);
};

template <typename T> class tPriorityRingBuffer {
protected:
  #define INVALID_RING_REF (uint16_t)(-1)
  #define INVALID_PRIORITY_REF (uint8_t)(-1)

  struct tValueSlot {
    T Value;
    uint16_t next;
    uint8_t priority;
  };

  struct tPriorityRef {
    uint16_t next;
    uint16_t last;
    tPriorityRef() : next(INVALID_RING_REF), last(INVALID_RING_REF) {;}
    void clear() { next=INVALID_RING_REF; last=INVALID_RING_REF; }
  };
protected:
  uint16_t head;
  uint16_t tail;
  uint16_t size;
  uint8_t maxPriorities;
  tValueSlot *buffer;
  tPriorityRef *priorityReferencies;

public:
  tPriorityRingBuffer(uint16_t _size, uint8_t _maxPriorities=1);
  virtual ~tPriorityRingBuffer();
  uint16_t getSize() const { return size; }
  bool isEmpty(uint8_t _priority=0xff) const;
  void clear();
  void clean();
  uint16_t count();

  bool add(const T &val, uint8_t _priority=0);
  bool read(T &val);

  T *getAddRef(uint8_t _priority=0);
  const T *getReadRef(uint8_t _priority);
  // Get reference to highest available.
  const T *getReadRef(uint8_t *_priority=0);
};


#include "RingBuffer.tpp"

#endif
