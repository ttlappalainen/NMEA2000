/*
BoardSerialNumber.cpp

Copyright (c) 2015-2018 Timo Lappalainen, Kave Oy, www.kave.fi

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
*/

#include "BoardSerialNumber.h"

#ifndef ARDUINO
#ifdef TARGET_LINUX_ARM
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

uint32_t GetBoardSerialNumber() {
  int fd = open("/dev/vcio", O_RDONLY);

  if (fd == -1) return 0;

  uint32_t property[32] = {
      0x00000000,
      0x00000000,
      0x00010004,
      0x00000010,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000
  };

  property[0] = 10 * sizeof(property[0]);

  if ( ioctl(fd, _IOWR(100, 0, char *), property) == -1 ) return 0;

  close(fd);

  return property[5];
};
#else // not TARGET_LINUX_ARM
uint32_t GetBoardSerialNumber() {
  return 0;
}
#endif

#else // Arduino environment
#include <Arduino.h>
#ifdef __SAM3X8E__  // Arduino DUE
// Code by Mark T from http://forum.arduino.cc/index.php?topic=289190.0

__attribute__ ((section (".ramfunc")))
void _EEFC_ReadUniqueID(uint32_t* pdwUniqueID) {
    unsigned int status ;

    /* Send the Start Read unique Identifier command (STUI) by writing the Flash Command Register with the STUI command.*/
    EFC1->EEFC_FCR = (0x5A << 24) | EFC_FCMD_STUI;
    do
    {
        status = EFC1->EEFC_FSR ;
    } while ( (status & EEFC_FSR_FRDY) == EEFC_FSR_FRDY ) ;

    /* The Unique Identifier is located in the first 128 bits of the Flash memory mapping. So, at the address 0x400000-0x400003. */
    pdwUniqueID[0] = *(uint32_t *)IFLASH1_ADDR;
    pdwUniqueID[1] = *(uint32_t *)(IFLASH1_ADDR + 4);
    pdwUniqueID[2] = *(uint32_t *)(IFLASH1_ADDR + 8);
    pdwUniqueID[3] = *(uint32_t *)(IFLASH1_ADDR + 12);

    /* To stop the Unique Identifier mode, the user needs to send the Stop Read unique Identifier
       command (SPUI) by writing the Flash Command Register with the SPUI command. */
    EFC1->EEFC_FCR = (0x5A << 24) | EFC_FCMD_SPUI ;

    /* When the Stop read Unique Unique Identifier command (SPUI) has been performed, the
       FRDY bit in the Flash Programming Status Register (EEFC_FSR) rises. */
    do
    {
        status = EFC1->EEFC_FSR ;
    } while ( (status & EEFC_FSR_FRDY) != EEFC_FSR_FRDY ) ;
}

uint32_t GetBoardSerialNumber() {
    uint32_t pdwUniqueID[5] ;
    WDT_Disable( WDT ) ;

    _EEFC_ReadUniqueID( pdwUniqueID );

    return pdwUniqueID[0];
}

#elif defined(__MK20DX256__)||defined(__ATMEGA32U4__) || defined(__MK64FX512__) || defined (__MK66FX1M0__) // Teensy 3.2, 3.5, 3.6
uint32_t GetBoardSerialNumber() {
  uint8_t word=0xf;
  uint8_t buf[3];
  uint8_t loc=0;

    // To understand what's going on here, see
    // "Kinetis Peripheral Module Quick Reference" page 85 and
    // "K20 Sub-Family Reference Manual" page 548.

    cli();
    FTFL_FCCOB0 = 0x41;             // Selects the READONCE command
    FTFL_FCCOB1 = word;             // read the given word of read once area
                                    // -- this is one half of the mac addr.
    FTFL_FSTAT = FTFL_FSTAT_CCIF;   // Launch command sequence
    while(!(FTFL_FSTAT & FTFL_FSTAT_CCIF)) {
                                    // Wait for command completion
    }
    *(buf+loc) =   FTFL_FCCOB5;       // collect only the top three bytes,
    *(buf+loc+1) = FTFL_FCCOB6;       // in the right orientation (big endian).
    *(buf+loc+2) = FTFL_FCCOB7;       // Skip FTFL_FCCOB4 as it's always 0.
    //*(buf+loc+3) = FTFL_FCCOB7;
    sei();

  uint32_t SerialNumber=buf[2] | (buf[1] << 8) | (buf[0] << 16);
  return SerialNumber;
}

#elif defined(ARDUINO_ARCH_ESP32)
uint32_t GetBoardSerialNumber() {
    uint8_t chipid[6];
    esp_efuse_mac_get_default(chipid);
    return chipid[0] + (chipid[1]<<8) + (chipid[2]<<16) + (chipid[3]<<24);
}
#else
uint32_t GetBoardSerialNumber() {
  return 0;
}
#endif

#endif
