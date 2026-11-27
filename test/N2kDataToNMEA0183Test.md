# NMEA2000 to NMEA0183 example tests

These host tests compile the actual converter, NMEA2000 library and NMEA0183
serializers. They inspect outgoing HDG/HDM/HDT, VHW, VTG and RMC fields. Only the
Arduino clock and output transport are replaced; no CAN hardware is required.

Clone https://github.com/ttlappalainen/NMEA0183 alongside this checkout, then run
from the NMEA2000 root:

```sh
cmake -S . -B build -DNMEA0183_SOURCE_DIR="$(cd ../NMEA0183 && pwd)"
cmake --build build
ctest --test-dir build --output-on-failure
```

With CMake 4, additionally pass `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` for this
project's existing minimum-version declaration. The regression was verified
with NMEA0183 commit `2130abdc08597fbe77996c6b58f9e419a0e44783`.
Without `NMEA0183_SOURCE_DIR`, the existing test targets remain unchanged.

For issue #258 the original converter fails eight cases and passes the zero
variation control. The correction passes all nine cases, including east/west
variation, north wrapping, unavailable angles/references, missing variation,
and alternating true/magnetic heading messages with east/west deviation.
These tests do not establish interoperability with a physical chartplotter.
