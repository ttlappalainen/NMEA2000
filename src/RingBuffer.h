/*
 * RingBuffer.h
 *
 * Copyright (c) 2020-2024 Timo Lappalainen, Kave Oy, www.kave.fi
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
 * With tRingBuffer one can save values to a ring buffer. Value can be
 * simple value or data structure.
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts around calls using buffer handling routines.
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
 * request pointer to next value to be saved and copy data directly 
 * to it.
 * 
 * \code
 *  void SendMessage() {
 *    tCANData *msgOut;
 *    lockData();
 *    if ( (msgOut=CANMessages.getAddRef())!=0 ) {
 *      msgOut->id=1;
 *      msgOut->len=1;
 *      msgOut->data[0]=1;
 *    }
 *    releaseData();
 *  }
 * \endcode
 * 
 * tPriorityRingBuffer is similar as tRingBuffer, but it extends 
 * functionality with value priority. When you add values to buffer,
 * you can give them priority and when during read you can define
 * which priority value should be read out or read highest priority
 * value.
 * 
 * ### Debugging
 * 
 * To improve the debugging of the module there is the possibility to
 * enable some print out to the DebugStream
 * 
 * \code
 *  #define RING_BUFFER_ERROR_DEBUG
 *  #define RING_BUFFER_DEBUG
 *  #define RING_BUFFER_INIT_DEBUG
 * \endcode
 * 
 * 
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <cstdint>
/************************************************************************//**
 * \class tRingBuffer
 * \brief Template Class that holds values in a ring buffer
 * \ingroup group_coreSupplementary
 *
 * With tRingBuffer one can save values to a ring buffer. Value can be
 * simple value or data structure.
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts around calls using buffer handling routines.
 * 
 * \sa 
 * - \ref subSecRBU
 * 
 * \tparam T Template used for the class
 */
template <typename T> class tRingBuffer {

protected:
  /** \brief Pointer to the ring buffer of values in memory*/
  T *buffer;
  /** \brief Index of the first value in the ring buffer */
  uint16_t head;
  /** \brief Index of the last value in the ring buffer */
  uint16_t tail;
  /** \brief Number of values that can be stored in the ring buffer*/
  uint16_t size;

public:
  /************************************************************************//**
   * \brief Construct a new ring buffer object
   * 
   * A buffer with the corresponding size of values will be allocated to the memory.
   * 
   * \param _size Size of the ring buffer
   */
  tRingBuffer(uint16_t _size);
  /************************************************************************//**
   * \brief Destroy the ring buffer object
   * 
   * All memory will be freed
   */
  virtual ~tRingBuffer();
  /************************************************************************//**
   * \brief Get the size of the ring buffer
   * \retval uint16_t size of ring buffer
   */
  uint16_t getSize() const { return size; }
  /************************************************************************//**
   * \brief Checks if the ring buffer is empty
   *
   * \retval true  Buffer is empty
   * \retval false Buffer has data
   */
  bool isEmpty() const;
  /************************************************************************//**
   * \brief Clears the whole ring buffer */
  void clear();
  /************************************************************************//**
   * \brief Returns the number of values in the ring buffer
   * \retval uint16_t Number of values in the ring buffer
   */
  uint16_t count();

  /************************************************************************//**
   * \brief Get the pointer to new value added to the ring buffer
   *
   * Using getAddRef instead of add saves data copying between values. By using
   * add, one must first setup value, which will be then copied to buffer
   * within add. By using getAddRef one can request pointer to next buffer value and
   * copy data directly to it.
   * 
   * \note In multitasking or with interrupts one must take care that buffer
   * will no be used before data handling has been finished. This 
   * differs from add, where it is enough to use locking only around calling add.
   * 
   * \retval "T *"      Pointer to added new value
   * \retval 0          Buffer is full
   */
  T *getAddRef();

  /************************************************************************//**
   * \brief Adds a new value to the ring buffer
   *
   * \sa 
   *  - \ref getAddRef
   * 
   * \param val         Value to be added
   * \retval true       Add succeeded
   * \retval false      Buffer is full
   */
  bool add(const T &val);

  /************************************************************************//**
   * \brief Get the pointer to value read out from the ring buffer
   *
   * Using getReadRef instead of read saves data copying between values. By using
   * read data will be copied to value, which one then may forward
   * elsewhere. By using getReadRef one can request pointer to value and
   * forward data directly from it.

   * \note In multitasking or with interrupts one must take care that buffer
   * will no be used before data handling has been finished. This 
   * differs from read, where it is enough to use locking only around calling read.
   * 
   * \retval "const T*" Pointer to value read out from the ring buffer
   * \retval 0          No values available
   */
  const T *getReadRef();

  /************************************************************************//**
   * \brief Get pointer to next value to be read out from the ring buffer.
   *        Function does not read value out from the ring buffer.
   * 
   * \retval "const T*" Pointer to value, which would be next read out 
   *                    from the ring buffer
   * \retval 0          No values available, buffer is empty.
   */
  T *peek();
  
  /************************************************************************//**
   * \brief Reads a value out from the ring buffer
   *
   * \sa 
   *  - \ref getReadRef
   * 
   * \param val       Reference to the value read out from the ring buffer
   * \retval true     Value read out from buffer.
   *                  Value has been copied to parameter val
   * \retval false    There is no data available inside the buffer.
   *                  Parameter val will be unchanged.
   */
  bool read(T &val);
};

/************************************************************************//**
 * \class tPriorityRingBuffer
 * \brief Template Class that holds values with given 
 *        priorities in a ring buffer
 * \ingroup group_coreSupplementary
 * 
 * tPriorityRingBuffer is similar as \ref tRingBuffer, but it extends 
 * functionality with value priority. When you add values to the buffer,
 * you can give them priority. When you read them out you can get highest
 * priority or request specific priority value.
 * 
 * The advantage of tPriorityRingBuffer is that it will reserve one
 * continuous block for all priorities instead of reserve one buffer for each
 * priority.
 * 
 * \note You have to take care of data locking from other threads 
 * or interrupts around calls using buffer handling routines.
 * 
 * \sa 
 * \ref subSecRBU
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
   * certain priority some additional meta data has to be stored as well.
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
  /** \brief Pointer to the ring puffer in memory */
  tValueSlot *buffer;
  /** \brief Pointer to priority references in buffer */
  tPriorityRef *priorityReferencies;
  /** \brief Index of the first free index in the ring buffer */
  uint16_t head;
  /** \brief Index of the last occupied index in the ring buffer */
  uint16_t tail;
  /** \brief Number of values that can be stored in the ring buffer*/
  uint16_t size;
  /** \brief highest priority possible*/
  uint8_t maxPriorities;

public:
  /************************************************************************//**
   * \brief Construct a new Priority ring buffer object
   *
   * Allocates all the memory needed, initialize attributes and priorities.
   * 
   * \param _size     Size of the ring buffer
   * \param _maxPriorities highest priority possible
   */
  tPriorityRingBuffer(uint16_t _size, uint8_t _maxPriorities=1);
  /************************************************************************//**
   * \brief Destroy the priority ring buffer object
   * Frees all memory
   */
  virtual ~tPriorityRingBuffer();

  /************************************************************************//**
   * \brief Get the size of the ring buffer
   * \retval uint16_t Size of ring buffer
   */
  uint16_t getSize() const { return size; }
  /************************************************************************//**
   * \brief Get the size of memory required by ring buffer
   * 
   * This is static function and can be used to test how much ring buffer
   * would require memory with given size and count of priorities.
   * Function can be used e.g., for checking available memory or possible
   * memory "bank" handling.
   * 
   * \return uint32_t Memory required by ring buffer
   */
  static uint32_t getMemSize(uint16_t _size, uint8_t _maxPriorities=1);
  /************************************************************************//**
   * \brief Check is ring buffer empty
   * 
   * Result depends of _priority parameter. If valid priority within
   * initial _maxPriorities is provided, function returns empty state of
   * given priority. Otherwise it returns empty state of whole buffer.
   * 
   * \param  _priority  Which buffer
   * \retval bool Buffer empty state
   */
  bool isEmpty(uint8_t _priority=0xff) const;
  /************************************************************************//**
   * \brief Clears the whole ring buffer */
  void clear();
  /************************************************************************//**
   * \brief This function does nothing at the moment!*/
  void clean();
  /************************************************************************//**
   * \brief Returns the number of values in the ring buffer
   * 
   * Currently function returns simple buffer count instead of true value
   * count. Priority ring buffer may have values released between other values.
   * 
   * \retval uint16_t Number of values in buffer
   */
  uint16_t count();
  /************************************************************************//**
   * \brief Add a value to the ring buffer with a given priority
   *
   * \sa 
   *  - \ref getAddRef
   * 
   * \param val         Value to be added
   * \param _priority   Priority for the added value (default = 0)
   * \retval true       Add succeeded
   * \retval false      Buffer is full
   */
  bool add(const T &val, uint8_t _priority=0);
  /************************************************************************//**
   * \brief Reads highest priority value out from the ring buffer
   *
   * \sa 
   *  - \ref getReadRef
   * 
   * \param val       Reference to the value read out from buffer
   * \retval true     Value read out from buffer.
   *                  Value has been copied to parameter val
   * \retval false    There is no data available inside the buffer.
   *                  Parameter val will be unchanged.
   */
  bool read(T &val);

  /************************************************************************//**
   * \brief Get the pointer to new value added to the ring buffer
   *
   * Using getAddRef instead of add saves data copying between values. By using
   * add, one must first setup value, which will be then copied to buffer
   * within add. By using getAddRef one can request pointer to next buffer value and
   * copy data directly to it.
   * 
   * \note In multitasking or with interrupts one must take care that buffer
   * will no be used before data handling has been finished. This 
   * differs from add, where it is enough to use locking only around calling add.
   * 
   * \param _priority Priority for the value (default = 0)
   * \retval "T *"      Pointer to added new value
   * \retval 0          Buffer is full
   */
  T *getAddRef(uint8_t _priority=0);

  /************************************************************************//**
   * \brief Get the pointer to value with given priority read out from the ring buffer
   *
   * Using getReadRef instead of read saves data copying between values. By using
   * read data will be copied to value, which one then may forward
   * elsewhere. By using getReadRef one can request pointer to value and
   * forward data directly from it.

   * \note In multitasking or with interrupts one must take care that buffer
   * will no be used before data handling has been finished. This 
   * differs from read, where it is enough to use locking only around calling read.
   * 
   * \param _priority   Priority for the value read out
   * \retval "const T*" Pointer to value read out from the ring buffer
   * \retval 0          No values available with given priority
   */
  const T *getReadRef(uint8_t _priority);
  
  /************************************************************************//**
   * \brief Get pointer to highest priority value read out from the the ring buffer.
   *
   * Using getReadRef instead of read saves data copying between values. By using
   * read data will be copied to value, which one then may forward
   * elsewhere. By using getReadRef one can request pointer to value and
   * forward data directly from it.

   * \note In multitasking or with interrupts one must take care that buffer
   * will no be used before data handling has been finished. This 
   * differs from read, where it is enough to use locking only around calling read.
   * 
   * \param *_priority   Pointer to priority, which will be set to priority
   *                     of the value read out.
   * \retval "const T*"  Pointer to value read out from the ring buffer
   * \retval 0           No values available.
   */
  const T *getReadRef(uint8_t *_priority=0);
};


#include "RingBuffer.tpp"

#endif
