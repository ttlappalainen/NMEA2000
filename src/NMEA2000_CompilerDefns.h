/*
NMEA2000_CompilerDefns.h

Copyright (c) 2015-2021 Timo Lappalainen, Kave Oy, www.kave.fi

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

/*
As default NMEA 2000 library will have all NMEA 2000 mandatory features automatically enabled.
Some rarely used or new requirements takes so much memory, that you may want to leave
them away. Arduino IDE does not have possibility to define compiler definitions on
project, so here is sample file, where you can disable some features and save memory.
Note that YOU SHOULD NEWER UPDATE this file to the library with definitions enabled! If
you want to change any definitions inside, copy this file under you libraries to e.g.
path NMEA2000_my and remove it from downloaded NMEA2000 library path.


*/

#ifndef _NMEA2000_COMPILER_DEFNS_H_
#define _NMEA2000_COMPILER_DEFNS_H_

// ISO Multipacket is a other way to send long messages instead of NMEA 2000 fastpacket.
// Normally NMEA 2000 devices communicates by using fastpacket, but this is mandatory
// for certified NMEA 2000 devices.
// This uses appr. 2 kB of rom. No measured ram effect.
// #define N2K_NO_ISO_MULTI_PACKET_SUPPORT 1  // Uncomment this, if you do not need ISO Multi-packet support

// Group functions has been used to control different settings. Mandatory default handlers sets heartbeat interval
// and node "name" device instance lower, upper and system instance values.
// This uses appr. 4 kB of rom and 100 B ram
// #define N2K_NO_GROUP_FUNCTION_SUPPORT 1 // Uncomment this, if you do not need Group Function support

// Heartbeat is new way to other devices see that node is alive. Support for this has been slowly
// implemented to new firmwares. Since this is new feature I'll expect it takes time before
// e.g. MFD:s can rely this. 
// This uses appr. 0.5 kB of rom. No measured ram effect.
// #define N2K_NO_HEARTBEAT_SUPPORT 1 // Uncomment this, if you do not want device to send heartbeat 

#endif
