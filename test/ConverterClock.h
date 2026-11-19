// The Arduino example expects millis() from Arduino.h. Supply only its
// declaration for host builds; test/millis.cpp provides the fixed test clock.
#include <stdint.h>
extern "C" uint32_t millis();
