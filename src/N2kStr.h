/*
 * N2kStr.h
 *
 * Copyright (c) 2025 David Hoy, david@thehoys.com
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
 *  \file   N2kStr.h
 *  \brief  This File contains the class tN2kStr and all necessary functions
 *          to handle a NMEA2000 ASCII/Unicode strings.
 *
 * The class tN2kStr holds all necessary data of an NMEA2000 ASCII/Unicode string.
 */

#ifndef _tN2kStr_H_
#define _tN2kStr_H_

#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>


/**
 * Define n2kwchar_t as a 16-bit unsigned integer for UTF-16 compatibility
 * This is typically used for Unicode strings in NMEA 2000 applications.
 */
typedef uint16_t n2kwchar_t;


/**
 * @class tN2kStr
 * @brief A compact string class supporting both ASCII and Unicode (UTF-16) encodings for NMEA 2000 string fields.
 *
 * This class provides a fixed-size buffer for storing strings in either ASCII or Unicode format,
 * with methods for setting, clearing, and retrieving the string in various encodings.
 * The internal buffer layout is as follows:
 *   - buffer[0]: Length of the buffer (including control byte and data)
 *   - buffer[1]: Encoding control byte (ASCII or Unicode)
 *   - buffer[2...]: String data (ASCII or UTF-16)
 *
 * Features:
 *   - Supports construction from ASCII (char*), Unicode (wchar_t*, std::u16string), and copy construction.
 *   - Provides methods to set and retrieve the string in both ASCII and Unicode formats.
 *   - Ensures buffer size limits and proper null-termination.
 *   - Allows direct access to internal buffer bytes for advanced use cases.
 *
 * Limitations:
 *   - Assumes wchar_t is UTF-16 compatible (true on Windows, may not be on all platforms).
 *   - Maximum data length is 252 bytes (MaxDataLen), with a total buffer size of 254 bytes (MaxBufLen).
 */
class tN2kStr {
public:
    static constexpr uint8_t MaxDataLen = 252;
    static constexpr uint8_t MaxBufLen = MaxDataLen + 2;

    enum Control : uint8_t {
        Unicode = 0,
        ASCII = 1
    };

    /**
     * @brief Default constructor for tN2kStr.
     *
     * Initializes a new instance of the tN2kStr class and clears its contents.
     */
    tN2kStr() {
        clear();
    }

    /**
     * @brief Constructs a tN2kStr object and initializes it with the given C-string.
     * 
     * This constructor calls the set() method to assign the provided string to the internal storage.
     * 
     * @param str Pointer to a null-terminated C-string to initialize the tN2kStr object.
     */
    tN2kStr(const char* str) {
        set(str);
    }

    /**
     * @brief Constructs a tN2kStr object from a UTF-16 string.
     *
     * This constructor initializes the tN2kStr instance by setting its value
     * to the provided UTF-16 encoded string.
     *
     * @param str The UTF-16 encoded string to initialize the tN2kStr object with.
     */
    tN2kStr(const std::u16string& str) {
        set(str);
    }

    /**
     * @brief Copy constructor for tN2kStr.
     *
     * Creates a new tN2kStr object as a copy of the given object.
     *
     * @param other The tN2kStr object to copy from.
     */
    tN2kStr(const tN2kStr& other) {
        std::memcpy(buffer, other.buffer, MaxBufLen);
    }

    /**
     * @brief Constructs a tN2kStr object from a wide-character string.
     * 
     * This constructor initializes the tN2kStr instance by setting its value
     * to the provided wide-character string.
     * 
     * @param str Pointer to a null-terminated wide-character string (const wchar_t*).
     */
    tN2kStr(const wchar_t* str) {
        set(str);
    }   

    /**
     * @brief Copy assignment operator for tN2kStr.
     *
     * Copies the contents of the buffer from another tN2kStr instance into this instance,
     * ensuring that self-assignment is safely handled. The copy is performed using std::memcpy
     * for efficiency, copying exactly MaxBufLen bytes.
     *
     * @param other The tN2kStr instance to copy from.
     * @return Reference to this tN2kStr instance after assignment.
     */
    tN2kStr& operator=(const tN2kStr& other) {
        if (this != &other) {
            std::memcpy(buffer, other.buffer, MaxBufLen);
        }
        return *this;
    }

    /**
     * @brief Clears the buffer and resets it to its initial state.
     *
     * This function sets all bytes in the buffer to zero, then initializes
     * the first two bytes: the first byte is set to 2 (representing the minimum
     * length), and the second byte is set to the ASCII constant.
     */
    void clear() {
        std::memset(buffer, 0, sizeof(buffer));
        buffer[0] = 2; // length byte (min size)
        buffer[1] = ASCII;
    }

    /**
     * @brief Sets the internal string value.
     * 
     * @param str Pointer to a null-terminated C-string to assign.
     *            The function does not take ownership of the pointer.
     */
    void set(const char* str) {
        size_t len = std::strlen(str);
        if (len > MaxDataLen) len = MaxDataLen;
        buffer[0] = static_cast<uint8_t>(len + 2);
        buffer[1] = ASCII;
        std::memcpy(buffer + 2, str, len);
        if (len < MaxDataLen) buffer[2 + len] = 0;
    }

    /**
     * @brief Sets the value using a wide-character string.
     * 
     * @param str Pointer to a null-terminated wide-character string to set.
     */
    void set(const wchar_t* str) {
        size_t len = std::wcslen(str);
        if (len > MaxDataLen) len = MaxDataLen;
        buffer[0] = static_cast<uint8_t>((len * 2) + 2);
        buffer[1] = Unicode;
        // Convert wchar_t to n2kwchar_t and copy
        // Assuming wchar_t is UTF-16 compatible, which is common on Windows
        // Note: This may not work correctly on all platforms if wchar_t is not UTF-16
        if (len > 0) {
            n2kwchar_t *tmp = (n2kwchar_t *)&buffer[2];
            for (size_t i = 0; i < len; ++i) {
                tmp[i] = (n2kwchar_t)str[i];
            }
            tmp[len] = 0; // Null-terminate the string
        }
    }

    /**
     * @brief Sets the value using a UTF-16 string.
     * 
     * @param str The new string value to set, represented as a UTF-16 encoded std::u16string.
     */
    void set(const std::u16string& str) {
        size_t len = str.size();
        if (len > MaxDataLen) len = MaxDataLen & ~1; // keep even for UTF-16
        buffer[0] = static_cast<uint8_t>((len * 2) + 2);
        buffer[1] = Unicode;
        if (len > 0) {
            n2kwchar_t *tmp = (n2kwchar_t *)&buffer[2];
            for (size_t i = 0; i < len; ++i) {
                tmp[i] = (n2kwchar_t)str[i];
            }
            tmp[len] = 0; // Null-terminate the string
        }
    }

    /**
     * @brief Returns the length of the buffer.
     *
     * This function retrieves the length of the buffer by returning the value stored
     * in the first element of the buffer array. The length is represented as an 8-bit unsigned integer.
     *
     * @return The length of the buffer.
     */
    uint8_t length() const {
        return buffer[0];
    }

    /**
     * @brief Retrieves the control byte from the buffer.
     *
     * This method returns the value of the control byte, which is stored at index 1 of the internal buffer.
     *
     * @return The control byte as an unsigned 8-bit integer.
     */
    uint8_t control() const {
        return buffer[1];
    }

    /**
     * @brief Returns a pointer to the data at the specified index in the buffer.
     *
     * @param index The position in the buffer to retrieve the data from.
     * @return A constant pointer to the byte at the given index in the buffer.
     */
    const uint8_t* data(size_t index) const {
        return &buffer[index];
    }

    /**
     * @brief Returns the byte at the specified index in the buffer.
     *
     * This method retrieves the value of the buffer at the given index if it is within bounds.
     * If the index is out of bounds (greater than or equal to MaxBufLen), it returns 0.
     *
     * @param index The position in the buffer to retrieve the byte from.
     * @return The byte value at the specified index, or 0 if the index is out of bounds.
     */
    unsigned char byte(size_t index) const {
        if (index < MaxBufLen) {
            return buffer[index];
        }
        return 0; // out of bounds
    }

    /**
     * @brief Sets the value of a byte at the specified index in the buffer.
     *
     * This method assigns the given value to the buffer at the provided index,
     * only if the index is within the valid range (less than MaxBufLen).
     *
     * @param index The position in the buffer to set the byte.
     * @param value The byte value to assign at the specified index.
     * 
     * @return true if the byte was successfully set, false if the index is out of bounds.
     */
    bool setByte(size_t index, uint8_t value) {
        if (index < MaxBufLen) {
            buffer[index] = value;
            return true;
        }
        return false;
    }

    /**
     * @brief Returns the ASCII representation of the buffer if available.
     *
     * This method checks if the buffer contains ASCII data by verifying the second byte.
     * If the buffer is in ASCII format, it constructs and returns a std::string from the buffer,
     * starting at the third byte and using the length specified by the first byte minus two.
     * If the buffer does not contain ASCII data, an empty string is returned.
     *
     * @return std::string The ASCII string extracted from the buffer, or an empty string if not ASCII.
     */
    std::string asAscii() const {
        if (buffer[1] != ASCII) return {};
        return std::string(reinterpret_cast<const char*>(buffer + 2), buffer[0] - 2);
    }

    /**
     * @brief Returns the buffer contents as an ASCII string if the buffer is in ASCII format.
     *
     * This method checks if the buffer is marked as containing ASCII data by verifying
     * that the second byte of the buffer equals the ASCII identifier. If so, it returns
     * a pointer to the ASCII string starting from the third byte of the buffer.
     * If the buffer is not in ASCII format, it returns nullptr.
     *
     * @return const char* Pointer to the ASCII string if available, or nullptr otherwise.
     */
    const char *asAsciiStr() const {
        if (buffer[1] != ASCII) return nullptr;
        return reinterpret_cast<const char*>(buffer + 2);
    }

    /**
     * @brief Converts the internal buffer to a UTF-16 encoded std::u16string if the buffer contains Unicode data.
     *
     * This method checks if the buffer contains Unicode data by verifying the second byte.
     * If the buffer is not in Unicode format, it returns an empty string.
     * Otherwise, it interprets the buffer (excluding the first two bytes) as UTF-16 encoded characters
     * and constructs a std::u16string of the appropriate length.
     *
     * @return std::u16string The Unicode string representation of the buffer, or an empty string if not Unicode.
     */
    std::u16string asUnicode() const {
        if (buffer[1] != Unicode) return {};
        size_t len = (buffer[0] - 2) / 2;
        const n2kwchar_t* src = reinterpret_cast<const n2kwchar_t*>(buffer + 2);
        std::u16string unicodeStr(src, src + len);
        return unicodeStr;
    }

    /**
     * @brief Returns the internal buffer as a Unicode (wide character) string.
     *
     * This function checks if the buffer contains a Unicode string by verifying the encoding marker.
     * If the buffer is not in Unicode format, it returns nullptr. Otherwise, it returns a pointer
     * to the Unicode string stored in the buffer.
     *
     * @return const wchar_t* Pointer to the Unicode string if the buffer is in Unicode format,
     *                       or nullptr otherwise.
     */
    const wchar_t* asUnicodeStr() const {
        if (buffer[1] != Unicode) return nullptr;
        size_t len = (buffer[0] - 2) / 2;
        const n2kwchar_t* src = reinterpret_cast<const n2kwchar_t*>(buffer + 2);
        // Convert n2kwchar_t (UTF-16) to std::wstring (platform wchar_t)
        static thread_local std::wstring tempWstr;
        tempWstr.assign(len ? len : 0, L'\0');
        for (size_t i = 0; i < len; ++i) {
            tempWstr[i] = static_cast<wchar_t>(src[i]);
        }
        tempWstr.push_back(L'\0');
        return tempWstr.c_str();
    }   

    /**
     * @brief Equality operator for tN2kStr objects.
     *
     * Compares the contents of the internal buffers of two tN2kStr instances.
     * Returns true if the buffers are identical up to MaxBufLen bytes, false otherwise.
     *
     * @param other The tN2kStr instance to compare with.
     * @return true if both buffers are equal, false otherwise.
     */
    bool operator==(const tN2kStr& other) const {
        return std::memcmp(buffer, other.buffer, MaxBufLen) == 0;
    }

    /**
     * @brief Inequality operator for tN2kStr objects.
     *
     * Compares this object with another tN2kStr instance for inequality.
     * Returns true if the two objects are not equal, as determined by the equality operator.
     *
     * @param other The tN2kStr object to compare with.
     * @return true if the objects are not equal, false otherwise.
     */
    bool operator!=(const tN2kStr& other) const {
        return !(*this == other);
    }

private:
    uint8_t buffer[MaxBufLen];
};

#endif // _tN2kStr_H_
