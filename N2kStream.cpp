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

size_t N2kStream::print(int val) {
   char buf[16];
   return write(itoa(buf, val, 10), strlen(buf));
}

size_t N2kStream::println(const char *str) {
   return print(str) + print("\r\n");
}

#endif
