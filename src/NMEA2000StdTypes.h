/*
NMEA2000StdTypes.h

Copyright (c) 2019-2020 Timo Lappalainen, Kave Oy, www.kave.fi

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

NMEA2000 standard type definitions.

This file contains (and must only contain) type definitions of data types
defined in the NMEA2000 standard.  These all have names of the form "DDnnn".
The language of the definitions should follow exactly that of the standard
document from which the information is drawn.  No abbreviated names. No
mangled capitalisation.

The entries here are mostly enums describing bit field types. Where an enum
member has multiple meanings, each meaning is aliased (see tN2kDD0002 for an
example).

This file is included by N2kTypes.h where the standard code names are aliased
into corresponding long names (once again, exactly as described by NMEA). 
*/

#ifndef _NMEA2000StdTypes_H_
#define _NMEA2000StdTypes_H_

// DD002 - Generic Status Pair
//                
enum tN2kDD002 {
                            N2kDD002_No=0,
                            N2kDD002_Yes=1,
                            N2kDD002_Error=2,
                            N2kDD002_Unavailable=3,
                            N2kDD002_Off=N2kDD002_No,
                            N2kDD002_Disabled=N2kDD002_No,
                            N2kDD002_Reset=N2kDD002_No,
                            N2kDD002_0=N2kDD002_No,
                            N2kDD002_On=N2kDD002_Yes,
                            N2kDD002_Enabled=N2kDD002_Yes,
                            N2kDD002_Set=N2kDD002_Yes,
                            N2kDD002_1=N2kDD002_Yes,
                            N2kDD002_Unknown=N2kDD002_Unavailable
                          };
                          
// DD477 - Windlass Monitoring Events
//
enum tN2kDD477 {
                            N2kDD477_NoErrorsPresent=0,
                            N2kDD477_ControllerUnderVoltageCutout=1,
                            N2kDD477_ControllerOverCurrentCutout=2,
                            N2kDD477_ControllerOverTemperatureCutout=4,
                            N2kDD477_ManufacturerDefined=8
                          };

// DD478 - Windlass Control Events
//
enum tN2kDD478 {
                            N2kDD478_NoErrorsPresent=0,
                            N2kDD478_AnotherDeviceControllingWindlass=1,
                            N2kDD478_Reserved=14
                          };
                          
// DD480 - Windlass Motion States
//                          
enum tN2kDD480 {
                            N2kDD480_WindlassStopped=0,
                            N2kDD480_DeploymentOccurring=1,
                            N2kDD480_RetrievalOccurring=2,
                            N2kDD480_Unavailable=3
                          };

//  DD481 - Rode Type States
//            
enum tN2kDD481 {
                            N2kDD481_ChainPresentlyDetected=0,
                            N2kDD481_RopePresentlyDetected=1,
                            N2kDD481_Error=2,
                            N2kDD481_Unavailable=3
                          };

// DD482 - Anchor Docking States
//
enum tN2kDD482 {
                            N2kDD482_NotDocked=0,
                            N2kDD482_FullyDocked=1,
                            N2kDD482_Error=2,
                            N2kDD482_DataNotAvailable=3
                          };

// DD483 - Windlass Operating Events
//                          
enum tN2kDD483 {
                            N2kDD483_NoErrorsOrEventsPresent=0,
                            N2kDD483_SystemError=1,
                            N2kDD483_SensorError=2,
                            N2kDD483_NoWindlassMotionDetected=4,
                            N2kDD483_RetrievalDockingDistanceReached=8,
                            N2kDD483_EndOfRodeReached=16,
                            N2kDD483_Reserved=32
                          };

// DD484 - Windlass Direction Control
//            
enum tN2kDD484 {
                            N2kDD484_Off=0,             // Status only / cannot command
                            N2kDD484_Down=1,
                            N2kDD484_Up=2,
                            N2kDD484_Reserved=3
                          };

// DD488 - Speed Type
//          
enum tN2kDD488 {
                            N2kDD488_SingleSpeed=0,
                            N2kDD488_DualSpeed=1,
                            N2kDD488_ProportionalSpeed=2,
                            N2kDD488_DataNotAvailable=3
                          };

#endif

