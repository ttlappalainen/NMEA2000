#include "N2kStream.h"
#include <string.h>
#include <stdlib.h>

#ifdef ARDUINO
// Arduino uses its own implementation.
#else
size_t N2kStream::print(const char *str) {
   if(str == 0)
      return 0;

   return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

#if defined(__AVR__)
size_t N2kStream::print(const __FlashStringHelper* str) {
   size_t bytes_written = 0;
   PGM_P src = (PGM_P) str;
   for(uint8_t c = pgm_read_byte(src); c != 0; ++src, ++bytes_written)
      write(&c, 1);

   return bytes_written;
}
#endif


size_t N2kStream::print(int val) {
   char buf[16];
   return write((const uint8_t*) itoa(val, buf, 10), strlen(buf));
}

size_t N2kStream::println(const char *str) {
   return print(str) + print("\r\n");
}

#endif
