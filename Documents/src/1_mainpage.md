# NMEA2000 library for C++ {#mainpage}
  
Object oriented NMEA2000 library for Teensy, ESP, Arduino, MBED and Rasberry type Boards.
These board types has been tested, but library can be used also in other systems by writing
compatible CAN driver and wrapper for other hw specific functions.

Library gives you easy way to make different kind of NMEA2000 bus devices like
sensor transducers (battery, temperature, wind, engine, etc.), NMEA2000 information displays,
NMEA2000->PC interface (like Actisense NGT1), NMEA0183->NMEA2000 or NMEA2000->NMEA0183 converter.

Library fulfills NMEA 2000 mandatory functions and behavior. Devices using library can pass NMEA2000
certification tests. Library has been used in several commercial certified products.

If you are familiar with library, here is quick link to \ref changes.

\warning
First of all - as normal - you can use library but there is not any guarantee and you use it with your own risk!
You connect your hardware always with your own risk. I wrote this documentation to help to
connect own hardware to NMEA2000 bus. I do not take any responsible of any errors in document
or any possible damages caused to your devices.

**About library documentation**

This is a new doxygen generated documentation designed by Matthias Werner - great thanks to him for excellent work. Many parts are still based on my original documentation, which I created at beginning of the library. After several years experience I see a need for several updates. So the documentation project will continue with checking and updating. For more details see \ref libDocuGen

## Recommended hardware

See this for \ref secTRecHW and please read document \ref pageHWSetUp before purchasing any hardware.

## Hardware depended libraries

each hardware setup need a specific \ref secHWlib

\section secRef References

- [NMEA2000 Standard](https://www.nmea.org/nmea-2000.html)
- [List of NMEA 2000 registered devices](https://web.nmea.org/products/search)
- [List of NMEA 2000 registrated companies](https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf)
- [Device class and function codes](https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf)
- [ISO address claim](https://web.archive.org/web/20150910070107/http://www.nmea.org/Assets/20140710%20nmea-2000-060928%20iso%20address%20claim%20pgn%20corrigendum.pdf)
- [Group function PGN 126208 handling](https://web.archive.org/web/20170609033039/http://www.nmea.org/Assets/20140109%20nmea-2000-corrigendum-tc201401031%20pgn%20126208.pdf)
- [Heartbeat PGN 126993](https://web.archive.org/web/20170609023206/https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf)

### Thanks

I thank Kees Verruijt for his excellent CanBoat project and Dr Andrew Mason for
OpenSkipper project.  I originally started to learn NMEA 2000 via OpenSkipper
project, which development I continued.  PGN library on OpenSkipper and so on
this library is mostly based on Kees work. Also it appeared that some code
parts are probably originally from CanBoat even I have found them from other
sources.  Actisense message format handling I have learned from OpenSkipper
project.

I also thank for anybody who has extended library with new PGNs or processor
support.

\copyright Copyright (c) 2015-2022 Timo Lappalainen, Kave Oy, www.kave.fi

### MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
