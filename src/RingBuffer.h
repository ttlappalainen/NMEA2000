/*
 * RingBuffer.h
 *
 * Copyright (c) 2020-2022 Timo Lappalainen, Kave Oy, www.kave.fi
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
 * \file  RingBuffer.h
 * \brief Simple tRingBuffer and tPriorityRingBuffer template classes
 * 
 * With tRingBuffer one can save simple data structures to a ring buffer. 
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts for copy time.
 * 
 * \section subSecRBU Example of ring buffer usage
 * 
 * \code
 *  struct tCANData {
 *    uint32_t id;
 *    uint8_t len;
 *    uint8_t data[8];
 *  };
 *
 *  tPriorityRingBuffer<tCANData> CANMessages(100);
 *  ...
 *  void SendMessage() {
 *    tCANData msgOut;
 *    msgOut.id=1;
 *    msgOut.len=1;
 *    msgOut.data[0]=1;
 *    lockData();
 *    CANMessages.add(msgOut);
 *    releaseData();
 *  }
 * \endcode
 * 
 * As an alternative if you want to avoid data copying twice, you can
 * request reference to next data to be saved and copy data directly 
 * to it.
 * 
 * \code
 *  void SendMessage() {
 *    tCANData *msgOut;
 *    lockData();
 *    if ( (msgOut=CANMessages)!=0 ) {
 *      msgOut->id=1;
 *      msgOut->len=1;
 *      msgOut->data[0]=1;
 *    }
 *    releaseData();
 *  }
 * \endcode
 * 
 * tPriorityRingBuffer is similar as tRingBuffer, but it extends 
 * functionality with item priority. When you add items to buffer,
 * you can give them priority and when you read them, highest priority
 * item will be read out first.
 * 
 * ### Debugging
 * 
 * To improve the debugging of the module there is the possibility to
 * enable some print out to the DebugStream
 * 
 * \code
 *  #define RING_BUFFER_ERROR_DEBUG
 *  #define RING_BUFFER_DEBUG
 * \endcode
 * 
 * 
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <cstdint>
/************************************************************************//**
 * \class tRingBuffer
 * \brief Template Class that holds simple data structures in a ring buffer
 * \ingroup group_coreSupplementary
 *
 * With tRingBuffer one can save simple data structures to a ring buffer. 
 * 
 * \sa 
 * \ref subSecRBU
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts for copy time.
 * 
 * \tparam T Template used for the class
 */
template <typename T> class tRingBuffer {

protected:
  /** \brief Pointer to the buffer in memory*/
  T *buffer;
  /** \brief Index of the first data structure in the ring buffer */
  uint16_t head;
  /** \brief Index of the last data structure in the ring buffer */
  uint16_t tail;
  /** \brief Number of data structures that can be stored in the ring buffer*/
  uint16_t size;

public:
  /************************************************************************//**
   * \brief Construct a new Ring Buffer object
   * 
   * A buffer with the corresponding size is allocated inside the memory.
   * 
   * \param _size Size of the ring buffer
   */
  tRingBuffer(uint16_t _size);
  /************************************************************************//**
   * \brief Destroy the Ring Buffer object
   * 
   * All Memory will be freed
   */
  virtual ~tRingBuffer();
  /************************************************************************//**
   * \brief Get the Size of the ring buffer
   * \return uint16_t Size of ring buffer
   */
  uint16_t getSize() const { return size; }
  /************************************************************************//**
   * \brief Checks if the ring buffer is empty
   *
   * \return true 
   * \return false 
   */
  bool isEmpty() const;
  /************************************************************************//**
   * \brief Clears the whole ring buffer */
  void clear();
  /************************************************************************//**
   * \brief Returns the number of Entries in the ring buffer
   * \return uint16_t Number of entries
   */
  uint16_t count();

  /************************************************************************//**
   * \brief Get pointer to next add item if available
   * \return T* 
   */
  T *getAddRef();

  /************************************************************************//**
   * \brief Adds a new value to the ring buffer
   *
   * \param val   Value to be added
   * \return true 
   * \return false -> ring buffer is already full
   */
  bool add(const T &val);

  /************************************************************************//**
   * \brief Get pointer to next read item if available
   * \return const T* 
   */
  const T *getReadRef();

  /************************************************************************//**
   * \brief Reads a value from the ring buffer
   *
   * \param val       Reference to the value that have to be read out
   * \return true     
   * \return false    -> There is no data available inside the buffer
   */
  bool read(T &val);
};

/************************************************************************//**
 * \class tPriorityRingBuffer
 * \brief Template Class that holds simple data structures with given 
 *        priorities in a ring buffer
 * \ingroup group_coreSupplementary
 * 
 * tPriorityRingBuffer is similar as \ref tRingBuffer, but it extends 
 * functionality with item priority. When you add items to buffer,
 * you can give them priority and when you read them, highest priority
 * item will be read out first.
 * \sa 
 * \ref subSecRBU
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts for copy time.
 * 
 * \tparam T Template used for the class
 */
template <typename T> class tPriorityRingBuffer {
protected:
  /** \brief Constant for an invalid index reference for the buffer */
  #define INVALID_RING_REF (uint16_t)(-1)
  /** \brief Constant for an invalid priority reference*/
  #define INVALID_PRIORITY_REF (uint8_t)(-1)

  /************************************************************************//**
   * \struct  tValueSlot
   * \brief   Structure that holds the actual value and meta data
   * 
   * In order to handle all the data entries in the ring buffer with
   * certain priority some additional meta data has to be stored es well.
   * 
   */
  struct tValueSlot {
    /** \brief Value stored in ring buffer*/
    T Value;
    /** \brief Index of the next value in the ring buffer (???)*/
    uint16_t next;
    /** \brief Priority of this value*/
    uint8_t priority;
  };
  /************************************************************************//**
   * \struct  tPriorityRef
   * \brief   Structure handles meta data for each priority
   */
  struct tPriorityRef {
    /** \brief Index of the next value for this priority in the ring buffer */
    uint16_t next;
    /** \brief Index of the last value for this priority in the ring buffer */
    uint16_t last;
    /** \brief Initialize all attributes to default*/
    tPriorityRef() : next(INVALID_RING_REF), last(INVALID_RING_REF) {;}
    /** \brief Initialize all attributes to default*/
    void clear() { next=INVALID_RING_REF; last=INVALID_RING_REF; }
  };
protected:
  /** \brief Index of the first free index in the ring buffer */
  uint16_t head;
  /** \brief Index of the last occupied index in the ring buffer */
  uint16_t tail;
  /** \brief Number of data structures that can be stored in the ring buffer*/
  uint16_t size;
  /** \brief highest priority possible*/
  uint8_t maxPriorities;
  /** \brief Pointer to the ring puffer in memory*/
  tValueSlot *buffer;
  /** \brief */
  tPriorityRef *priorityReferencies;

public:
  /************************************************************************//**
   * \brief Construct a new Priority Ring Buffer object
   *
   * Allocates all the memory needed and initialize attributes and priorities.
   * 
   * \param _size     Size of the ring buffer
   * \param _maxPriorities highest priority possible
   */
  tPriorityRingBuffer(uint16_t _size, uint8_t _maxPriorities=1);
  /************************************************************************//**
   * \brief Destroy the t Priority Ring Buffer object
   * Frees all memory
   */
  virtual ~tPriorityRingBuffer();

  uint16_t getSize() const { return size; }
  /************************************************************************//**
   * \brief Get the Size of the ring buffer
   * \return uint16_t Size of ring buffer
   */
  bool isEmpty(uint8_t _priority=0xff) const;
  /************************************************************************//**
   * \brief Clears the whole ring buffer */
  void clear();
  /************************************************************************//**
   * \brief This function does nothing at the moment!*/
  void clean();
  /************************************************************************//**
   * \brief Returns the number of Entries in the ring buffer
   * \return uint16_t Number of entries
   */
  uint16_t count();
  /************************************************************************//**
   * \brief Add a value to the ring buffer with a given priority
   *
   * \param val         Value to be added
   * \param _priority   Priority for the added value (default = 0)
   * \return true 
   * \return false  -> ring buffer is already full
   */
  bool add(const T &val, uint8_t _priority=0);
  /************************************************************************//**
   * \brief Reads a value from the ring buffer
   *
   * \param val       Reference to the value that have to be read out
   * \return true     
   * \return false    -> There is no data available inside the buffer
   */
  bool read(T &val);

  /************************************************************************//**
   * \brief Get the Reference for the next Add 
   *
   * \param _priority Priority for the value (default = 0)
   * \return T* -> Null, if buffer is full
   */
  T *getAddRef(uint8_t _priority=0);

  /************************************************************************//**
   * \brief Get the Reference for the next Read
   *
   * \param _priority Priority for the value
   * \return const T* 
   */
  const T *getReadRef(uint8_t _priority);
  
  /************************************************************************//**
   * \brief Get reference to highest available.
   *
   * \param *_priority Priority for the value (default = 0)
   * \return const T* 
   */
  const T *getReadRef(uint8_t *_priority=0);
};


#include "RingBuffer.tpp"

#endif
