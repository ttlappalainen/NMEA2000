/*
 * N2kMsg.h
 * 
 * Copyright (c) 2015-2024 Timo Lappalainen, Kave Oy, www.kave.fi
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

/**************************************************************************//**
 *  \file   N2kMsg.h
 *  \brief  This File contains the class tN2kMsg and all necessary functions 
            to handle a NMEA2000 Message
 * 
 * The class tN2kMsg holds all necessary data of an NMEA2000 message. An object 
 * of this class will be passed to the SetN2kPGNxxxx function and there it is 
 * "filled" with the corresponding data. To extract data out of a NMEA2000 
 * message, the ParseN2kPGNxxxx functions can be used.  
 * As all the data on the NMEA2000 messages is stored in a byte array, there
 * are plenty of helper functions to inject or extract certain datatypes 
 * in and out of this byte array.
 * 
 * \sa Parse and Set Function in \ref N2kMessages.h
 * 
 */

#ifndef _tN2kMsg_H_
#define _tN2kMsg_H_

#include "N2kStream.h"
#include "N2kDef.h"
#include <stdint.h>

/** \brief  Constant "Not Available" for a double value*/
const double   N2kDoubleNA=-1e9;
/** \brief  Constant "Not Available" for a float value*/
const float    N2kFloatNA=-1e9;
/** \brief  Constant "Not Available" for a unsigned 8bit int value*/
const uint8_t  N2kUInt8NA=0xff;
/** \brief  Constant "Not Available" for a signed 8bit value*/
const int8_t   N2kInt8NA=0x7f;
/** \brief  Constant "Not Available" for a unsigned 16bit int value*/
const uint16_t N2kUInt16NA=0xffff;
/** \brief  Constant "Not Available" for a signed 16bit int value*/
const int16_t  N2kInt16NA=0x7fff;
/** \brief  Constant "Not Available" for a unsigned 32bit int value*/
const uint32_t N2kUInt32NA=0xffffffff;
/** \brief  Constant "Not Available" for a signed 32bit int value*/
const int32_t  N2kInt32NA=0x7fffffff;
/** \brief  Constant "Not Available" for a unsigned 64bit int value*/
const uint64_t N2kUInt64NA=0xffffffffffffffffLL;
/** \brief  Constant "Not Available" for a signed 64bit int  value*/
const int64_t  N2kInt64NA=0x7fffffffffffffffLL;

#ifndef BIT
#define BIT(n) (1 << n)
#endif

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {double} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(double v) { return v==N2kDoubleNA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {float} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(float v) { return v==N2kFloatNA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {uint8_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(uint8_t v) { return v==N2kUInt8NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {int8_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(int8_t v) { return v==N2kInt8NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {uint16_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(uint16_t v) { return v==N2kUInt16NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {int16_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(int16_t v) { return v==N2kInt16NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {uint32_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(uint32_t v) { return v==N2kUInt32NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {int32_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(int32_t v) { return v==N2kInt32NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {uint64_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(uint64_t v) { return v==N2kUInt64NA; }

/**************************************************************************//**
 * \brief Verify that the specified value is equal to "Not available".
 * 
 * \param v  {int64_t} Value to be verified
 * \return true:      Value equal to "Not available"
 * \return false:     Value not equal to "Not available"
 */
inline bool N2kIsNA(int64_t v) { return v==N2kInt64NA; }

/**************************************************************************//**
 * \brief Writes a float value into a byte array buffer
 * 
 * To write a float value into a certain position of an byte array buffer
 * the function memcpy is used. If the value represents "not available", the
 * corresponding constant is copied into the buffer.
 * 
 * \param v {float} Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBufFloat(float v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double value into a byte array buffer  
 *
 * \param v {double} Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 * 
 */
void SetBufDouble(double v, int &index, unsigned char *buf);


/**************************************************************************//**
 * \brief Writes a double signed value into a byte array buffer using 8 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf8ByteDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double signed value into a byte array buffer using 4 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf4ByteDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double unsigned value into a byte array buffer using 4 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf4ByteUDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double signed value into a byte array buffer using 3 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf3ByteDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double signed value into a byte array buffer using 2 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf2ByteDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double unsigned value into a byte array buffer using 2 bytes
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf2ByteUDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double signed value into a byte array buffer using 1 byte
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf1ByteDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a double unsigned value into a byte array buffer using 1 byte
 *
 * To write a double value into a certain position of an byte array buffer
 * the function memcpy is used. Out of range will be handled. So if given
 * value can not be converted to fixed integer, "out of range" constant
 * will be set. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer. The fixed point integer mechanism is used.
 * 
 * \param v {double} Value to be stored in the Buffer
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf1ByteUDouble(double v, double precision, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes an integer value into a byte array buffer using 2 bytes
 * To write a integer value into a certain position of an byte array buffer
 * the function memcpy is used.
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer
 * 
 * \param v         Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf2ByteInt(int16_t v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes an unsigned integer value into a byte array buffer using 
 * 2 bytes
 * 
 * To write a integer value into a certain position of an byte array buffer
 * the function memcpy is used. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer
 * 
 * \param v         Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf2ByteUInt(uint16_t v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes an integer value into a byte array buffer using 3 bytes
 * To write a integer value into a certain position of an byte array buffer
 * the function memcpy is used. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer
 * 
 * \param v         Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf3ByteInt(int32_t v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes an unsigned integer value into a byte array buffer u
 * ing 4 bytes
 * To write a integer value into a certain position of an byte array buffer
 * the function memcpy is used. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer
 * 
 * \param v         Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBuf4ByteUInt(uint32_t v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes an unsigned integer 64bit value into a byte array buffer
 * using 8 bytes
 * To write a integer value into a certain position of an byte array buffer
 * the function memcpy is used. 
 * 
 * There are different functions depending on how many byte should be used
 * in the buffer
 * 
 * \param v         Value to be stored in the Buffer
 * \param index     Position where the value should be placed inside the buffer
 * \param buf       Pointer to the byte array buffer 
 */
void SetBufUInt64(uint64_t v, int &index, unsigned char *buf);

/**************************************************************************//**
 * \brief Writes a string into a byte array buffer
 * 
 * \param str   String as pointer to a char array
 * \param len   Length of the string
 * \param index Position where the value should be placed inside the buffer
 * \param buf   Pointer to the byte array buffer 
 * \param UsePgm {bool} use the pgm_read_byte function
 * \param fillChar fill the rest of the buffer with this character
 */
void SetBufStr(const char *str, int len, int &index, unsigned char *buf, bool UsePgm=false, unsigned char fillChar=0x0);

/**************************************************************************//**
 * \brief Extracts 2 bytes out of the given buffer
 * and converts it to an integer value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \return      integer value
 */
int16_t GetBuf2ByteInt(int &index, const unsigned char *buf);

/**************************************************************************//**
 * \brief Extracts 2 bytes out of the given buffer
 * and converts it to an integer value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \return      integer value
 */
uint16_t GetBuf2ByteUInt(int &index, const unsigned char *buf);

/**************************************************************************//**
 * \brief Extracts 3 bytes out of the given buffer
 * and converts it to an integer value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \return      integer value
 */
uint32_t GetBuf3ByteUInt(int &index, const unsigned char *buf);

/**************************************************************************//**
 * \brief Extracts 4 bytes out of the given buffer
 * and converts it to an integer value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \return      integer value
 */
uint32_t GetBuf4ByteUInt(int &index, const unsigned char *buf);

/**************************************************************************//**
 * \brief Extracts 8 bytes out of the given buffer
 * and converts it to an integer value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \return      integer value
 */
uint64_t GetBuf8ByteUInt(int &index, const unsigned char *buf);

/**************************************************************************//**
 * \brief Extracts the specified numbers of bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf1ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts 1 byte out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf1ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);

/**************************************************************************//**
 * \brief Extracts 2 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf2ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts 2 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf2ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);

/**************************************************************************//**
 * \brief Extracts 3 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf3ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts 4 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf4ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts 4 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf4ByteUDouble(double precision, int &index, const unsigned char *buf, double def=-1);

/**************************************************************************//**
 * \brief Extracts 8 bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param precision the value is rounded to the given precision (e.g. 0.01)
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBuf8ByteDouble(double precision, int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts bytes out of the given buffer
 * and converts it to a double value.
 * 
 * The fixed point integer mechanism is used.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      double value
 */
double GetBufDouble(int &index, const unsigned char *buf, double def=0);

/**************************************************************************//**
 * \brief Extracts the specified numbers of bytes out of the given buffer
 * and converts it to an float value.
 * 
 * \param index       position inside the byte array \ref tN2kMsg::Data, getting
 *                    incremented according to the number of bytes
 *                    extracted
 * \param buf   Pointer to the byte array buffer 
 * \param def   default value that will be returned if the byte array
 *              equal to "not available"
 * \return      float value
 */
float GetBufFloat(int &index, const unsigned char *buf, float def=0);

/**************************************************************************//**
 * \class tN2kMsg
 * \brief This class contains all the data of an NMEA2000 message
 * \ingroup group_core
 * 
 * The class tN2kMsg holds all necessary data of an NMEA2000 message. An object 
 * of this class will be passed to the SetN2kPGNxxxx function and there it is 
 * "filled" with the corresponding data. To extract data out of a NMEA2000 
 * message, the ParseN2kPGNxxxx functions can be used. 
 * 
 * \sa N2kMessages.h
 */
class tN2kMsg
{
public:
  /************************************************************************//**
   * \brief Maximum number of bytes that can be stored in the data buffer 
   * With fast packet the first frame can have 6 byte and rest 31 frames
   * 7 bytes.
   */
  static const int MaxDataLen=223;
  /** \brief Priority of the NMEA2000 message*/
  unsigned char Priority;
  /** \brief Parameter Group Number (PGN) of the NMEA2000 message*/
  unsigned long PGN;
  /** \brief Source of the NMEA2000 message*/
  mutable unsigned char Source;
  /** \brief Destination of the NMEA2000 message*/
  mutable unsigned char Destination;
  /** \brief Number of bytes already stored in \ref tN2kMsg::Data of this message*/
  int DataLen;
  /** \brief Byte array which carries all the data of the NMEA2000 message*/
  unsigned char Data[MaxDataLen];
  /** \brief timestamp (ms since start [max 49days]) of the NMEA2000 message*/
  unsigned long MsgTime;
protected:
  /** \brief Fills the whole data buffer with 0xff*/
  void ResetData();

#if !defined(N2K_NO_ISO_MULTI_PACKET_SUPPORT)
protected:
  /** \brief Message is a MultiPacket Message*/
  bool TPMessage;
public:

  /************************************************************************//**
   * \brief Set the MultiPacket Message status
   * \param tp {bool} true if message is an MultiPacket Message
   */
  void SetIsTPMessage(bool tp=true) { TPMessage=tp; }

  /************************************************************************//**
   * \brief Determine if the message is flagged as MultiPacket Message
   * \return true   Message is a MultiPacket Message
   * \return false  Message is not a MultiPacket Message
   */
  bool IsTPMessage() const { return TPMessage; }

#endif
public:
  /************************************************************************//**
   * \brief Construct a new t N2k Msg object
   * 
   * \param _Source   Source address of the NMEA2000 device sending this message
   *                  [range 0 .. 251] default = 15
   * \param _Priority Priority of the Message [0 .. 7] default = 6
   * \param _PGN      Parameter Group Number [decimal]
   * \param _DataLen  Length off the data field
   */
  tN2kMsg(unsigned char _Source=15, unsigned char _Priority=6, unsigned long _PGN=0, int _DataLen=0);

  /************************************************************************//**
   * \brief Set the Parameter Group Number of the message   * 
   * \param _PGN Parameter Group Number [decimal]
   */
  void SetPGN(unsigned long _PGN);

  /************************************************************************//**
   * \brief Set the Source of the message 
   * \param _Source   Source address of the NMEA2000 device sending this message
   *                  [range 0 .. 251] default = 15
   */
  void ForceSource(unsigned char _Source) const { Source=_Source; }

  /************************************************************************//**
   * \brief Validation check for the message destination
   * We can send to specified destination only for PGN:s low byte=0
   * - PGN 060928 (0xEE00 ) ISO Address Claim
   * - PGN 059392 (0xE800) - ISO Acknowledgment
   * - ...
   */
  void CheckDestination() const { if ( (PGN & 0xff)!=0 ) Destination=0xff; }

  /************************************************************************//**
   * \brief Initialisation of the N2kMsg object
   * 
   * \param _Priority Priority of the Message [0 .. 7]
   * \param _PGN      Parameter Group Number [decimal]
   * \param _Source   Source address of the NMEA2000 device sending this message
   *                  [range 0 .. 251]
   * \param _Destination Destination address of the NMEA2000 device sending 
   *                     this message [range 0 .. 251]
   */
  void Init(unsigned char _Priority, unsigned long _PGN, unsigned char _Source, unsigned char _Destination=0xff);

  /************************************************************************//**
   * \brief Clears the content of the N2kMsg object
   * The method sets the \ref PGN, \ref DataLen and \ref MsgTime to zero.
   */
  virtual void Clear();

  /************************************************************************//**
   * \brief Checks if the Message Meta Content is valid
   * Basic check if the message object is valid, mean datalen >0 and PGN != 0.
   * \return true   message is valid
   * \return false  message is not valid
   */
  bool IsValid() const { return (PGN!=0 && DataLen>0); }

  /************************************************************************//**
   * \brief Get the Remaining Data Length 
   * 
   * \param Index Position inside the buffer
   * 
   * \return int - Remaining Data length
   */
  int GetRemainingDataLength(int Index) const { return DataLen>Index?DataLen-Index:0; }

  /************************************************************************//**
   * \brief Get the Available Data Length 
   * The method calculates the available data length with 
   * \ref MaxDataLen - \ref DataLen
   * 
   * \return int Available Data Length 
   */
  int GetAvailableDataLength() const { return MaxDataLen-DataLen; }

  /************************************************************************//**
   * \brief Add float value to the buffer
   * This method adds a float value to the message buffer using \ref SetBufFloat
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void AddFloat(float v, float UndefVal=N2kFloatNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 8 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add8ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 4 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add4ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 4 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add4ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 3 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add3ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 2 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add2ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);

  /************************************************************************//**
   * \brief Add double value to the buffer using 2 bytes
   * 
   * The fixed point integer mechanism is used.
   * The value will be added to the end (indicated by \ref DataLen) of
   * the byte array \ref Data.
   * 
   * \param precision the value is rounded to the given precision (e.g. 0.01)
   * \param v         value to add
   * \param UndefVal  "not available" value
   */
  void Add2ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);

 /************************************************************************//**
  * \brief Add double value to the buffer using 1 byte
  * 
  * The fixed point integer mechanism is used.
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param v         value to add
  * \param UndefVal  "not available" value
  */
  void Add1ByteDouble(double v, double precision, double UndefVal=N2kDoubleNA);

 /************************************************************************//**
  * \brief Add double value to the buffer using 1 byte
  * 
  * The fixed point integer mechanism is used.
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param v         value to add
  * \param UndefVal  "not available" value
  */
  void Add1ByteUDouble(double v, double precision, double UndefVal=N2kDoubleNA);

 /************************************************************************//**
  * \brief Add integer value to the buffer using 2 bytes
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void Add2ByteInt(int16_t v);

 /************************************************************************//**
  * \brief Add unsigned integer value to the buffer using 2 bytes
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void Add2ByteUInt(uint16_t v);

 /************************************************************************//**
  * \brief Add integer value to the buffer using 3 bytes
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void Add3ByteInt(int32_t v);

 /************************************************************************//**
  * \brief Add unsigned integer value to the buffer using 4 bytes
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void Add4ByteUInt(uint32_t v);

 /************************************************************************//**
  * \brief Add unsigned integer value to the buffer using 8 bytes
  * The value will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void AddUInt64(uint64_t v);

 /************************************************************************//**
  * \brief Add byte value to the buffer
  * The byte will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param v         value to add
  */
  void AddByte(unsigned char v);

 /************************************************************************//**
  * \brief Add string value to the buffer
  * The string will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * \param str   String as pointer to a char array
  * \param len   Length of the string
  * \param UsePgm {bool} use the pgm_read_byte function
  */
  void AddStr(const char *str, int len, bool UsePgm=false);

 /************************************************************************//**
  * \brief Add string value to the buffer
  * This method determines the length of the string by it self using strlen().
  * The string will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * 
  * \param str   String as pointer to a char array
  * \param UsePgm {bool} use the pgm_read_byte function
  */
  void AddVarStr(const char *str, bool UsePgm=false);

 /************************************************************************//**
  * \brief Add byte array to the buffer
  * 
  * The buffer will be added to the end (indicated by \ref DataLen) of
  * the byte array \ref Data.
  * 
  * \param buf     Pointer to the byte array 
  * \param bufLen  Number of bytes to add
  */
  void AddBuf(const void *buf, size_t bufLen);

 /************************************************************************//**
  * \brief Get the value from a byte out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \return  value represented in the byte array
  */
  unsigned char GetByte(int &Index) const;
  
 /************************************************************************//**
  * \brief Get an integer from 2 bytes out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  int16_t Get2ByteInt(int &Index, int16_t def=0x7fff) const;

 /************************************************************************//**
  * \brief Get an unsigned integer from 2 bytes out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  uint16_t Get2ByteUInt(int &Index, uint16_t def=0xffff) const;

 /************************************************************************//**
  * \brief Get an unsigned integer from 3 bytes out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  uint32_t Get3ByteUInt(int &Index, uint32_t def=0xffffffff) const;

 /************************************************************************//**
  * \brief Get an unsigned integer from 4 bytes out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  uint32_t Get4ByteUInt(int &Index, uint32_t def=0xffffffff) const;

 /************************************************************************//**
  * \brief Get an unsigned integer from 8 bytes out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  uint64_t GetUInt64(int &Index, uint64_t def=0xffffffffffffffffULL) const;

 /************************************************************************//**
  * \brief Get a double from 1 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get1ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;

  /************************************************************************//**
  * \brief Get a double from 1 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get1ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;

  /************************************************************************//**
  * \brief Get a double from 2 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get2ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;

  /************************************************************************//**
  * \brief Get a double from 2 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get2ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;

  /************************************************************************//**
  * \brief Get a double from 3 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get3ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;

  /************************************************************************//**
  * \brief Get a double from 4 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get4ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;

 /************************************************************************//**
  * \brief Get a double from 4 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get4ByteUDouble(double precision, int &Index, double def=N2kDoubleNA) const;

 /************************************************************************//**
  * \brief Get a double from 8 bytes out of \ref Data
  * The fixed point integer mechanism is used.
  * 
  * \param precision the value is rounded to the given precision (e.g. 0.01)
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        double value
  */
  double Get8ByteDouble(double precision, int &Index, double def=N2kDoubleNA) const;

 /************************************************************************//**
  * \brief Get a float out of \ref Data
  * 
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * \param def     default value when data is unavailable
  * \return        integer value
  */
  float  GetFloat(int &Index, float def=N2kFloatNA) const;
  
 /************************************************************************//**
  * \brief Get a string out of \ref Data
  * 
  * \param StrBuf  Pointer to a char array as string
  * \param Length  Length of the string
  * \param Index       position inside the byte array \ref Data, getting
  *                    incremented according to the number of bytes
  *                    extracted
  * 
  * \return true   String data has been extracted
  * \return false  not successful, no string data available
  */
  bool GetStr(char *StrBuf, size_t Length, int &Index) const;

  /************************************************************************//**
   * \brief Get a string out of \ref Data
   * 
   * \param StrBufSize  Size of String Buffer
   * \param StrBuf      Pointer to a char array as string
   * \param Length      Length of the string
   * \param nulChar     used "null Char" in the message (e.g. '@' in AIS)
   * \param Index       position inside the byte array \ref Data, getting
   *                    incremented according to the number of bytes
   *                    extracted
   * 
   * \return true   String data has been extracted
   * \return false  not successful, no string data available
   */
  bool GetStr(size_t StrBufSize, char *StrBuf, size_t Length, unsigned char nulChar, int &Index) const;

  /************************************************************************//**
   * \brief Get a string out of \ref Data
   * This method determines the length of the string by it self, using 
   * the byte (index -2) inside \ref Data
   *
   * \param StrBufSize  Size of String Buffer
   * \param StrBuf      Pointer to a char array as string
   * \param Index       position inside the byte array \ref Data, getting
   *                    incremented according to the number of bytes
   *                    extracted
   * 
   * \return true   String data has been extracted
   * \return false  not successful, no string data available
   */
  bool GetVarStr(size_t &StrBufSize, char *StrBuf, int &Index) const;

  /************************************************************************//**
   * \brief Get a byte array out of \ref Data
   * 
   * \param buf         Pointer to byte array
   * \param Length      Length of byte array to be extracted
   * \param Index       position inside the byte array \ref Data, getting
   *                    incremented according to the number of bytes
   *                    extracted
   * \return true   data has been extracted
   * \return false  not successful, no data available
   */
  bool GetBuf(void *buf, size_t Length, int &Index) const;

  /************************************************************************//**
   * \brief Set a byte in \ref Data
   * 
   * \param v           Value to add
   * \param Index       position inside the byte array \ref Data, getting
   *                    incremented according to the number of bytes
   *                    extracted
   * \return true       success
   * \return false      Index is grater then DataLen
   */
  bool SetByte(uint8_t v, int &Index);

  /************************************************************************//**
   * \brief Set a 2byte unsigned integer in \ref Data
   * 
   * \param v           Value to add
   * \param Index       position inside the byte array \ref Data, getting
   *                    incremented according to the number of bytes
   *                    extracted
   * \return true       success
   * \return false      Index is grater then DataLen
   */
  bool Set2ByteUInt(uint16_t v, int &Index);

  /************************************************************************//**
   * \brief Print out the whole content of the N2kMsg Object
   * 
   * \param port      port where to stream, see \ref N2kStream
   * \param NoData    if true the data buffer will not be printed
   */
  void Print(N2kStream *port, bool NoData=false) const;

  /************************************************************************//**
   * \brief Print out the whole content of the N2kMsg Object
   * using the Actisense Format
   * 
   * \param port      port where to stream, see \ref N2kStream
   */
  void SendInActisenseFormat(N2kStream *port) const;
};

/************************************************************************//**
 * \brief Print out a buffer (byte array)
 * 
 * \param port    port where to stream, see \ref N2kStream
 * \param len     Number of bytes to be printed 
 * \param pData   Pointer to the buffer
 * \param AddLF   true will add a LineFeed at the end
 */
void PrintBuf(N2kStream *port, unsigned char len, const unsigned char *pData, bool AddLF=false);

#endif
