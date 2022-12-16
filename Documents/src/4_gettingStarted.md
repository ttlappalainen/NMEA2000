# Get your Project Started {#getStarted}

 \tableofcontents
    @brief A short introduction how to get started

  \section secMem Memory requirements

I have tried to measure memory used by library, but it is not so simple, since
there are some automated operations.  With version 11.06.2017 I got results:

- Approximate ROM 26.9 kB
- Approximate RAM  3.4 kB

This is with simple TemperatureMonitor example. This can be squeezed by
setting:

- Add below to setup() before NMEA2000.Open();
....
  NMEA2000.SetN2kCANMsgBufSize(2);
  NMEA2000.SetN2kCANSendFrameBufSize(15);
....

- Defining ProductInformation to PROGMEM as in BatteryMonitor example.
- Disabling all extra features. See NMEA2000_CompilerDefns.h
- Disable interrupt receiving.

With those setting you can go down to appr. 19 kB ROM and 1.9 kB RAM. So for 2
kB devices like Arduino Uno, there is not much for your own code.

\warning By squeezing memory, library can not fulfill certification requirements anymore.

  \section secHWSet Hardware setup

NMEA2000 is inherited from CAN. Many MCUs like Teensy >3.1, ESP32, Arduino Due has already
CAN controller inside. If your MCU does not have CAN controller inside or you need second
external CAN controller, you can use e.g. MCP2515 CAN controller, which is supported by
library (mcp_can).

For final connection to the bus you need CAN bus_transceiver chip. Devices on NMEA2000
bus should be isolated to avoid ground loops. So if you take power from NMEA2000
bus and your device is not connected to ground enywhere else, you can use unisolated tranceiver
like MCP2551, MCP2562 or SN65HVD234.
If you instead feed power to your device directly or e.g. use engine own sensors for measuring,
you have to use isolated tranceivers like ISO1050. Remember also use isolated power supply, if you take power
from bus and have any unisolated connection to anywhere on your whole system.

Easiest for connecting to NMEA2000 bus is to use some ready shield. For more
information on how to wire everything to the bus please see \ref pageHWSetUp

## Breakout Boards

### Teensy 3.2

- <https://www.skpang.co.uk/products/teensy-can-bus-breakout-board-include-teensy-3-2>

### ESP32

- <https://www.tindie.com/products/fusion/can32-an-esp32-dev-board-with-can-bus-v21/>

### Arduino Due

- <http://skpang.co.uk/catalog/dual-can-bus-interface-for-arduino-due-p-1579.html>

### Arduino Mega

- <https://wiki.seeedstudio.com/CAN-BUS_Shield_V1.2/>
  
Note that there are several different shields for CAN bus available and others may use
8 MHz chrystal instead of **default 16 MHz chrystal**. This must be set before including NMEA2000_CAN.h

## Schematics for standalone CAN transceiver

In case you build your tranceiver connection by yourself there are some connection examples
under [documents](https://github.com/ttlappalainen/NMEA2000/tree/master/Documents).

### Teensy 3.2

- [Teensy_Actisense_listener_sender_schematics.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Examples/TeensyActisenseListenerSender/Documents/Teensy_Actisense_listener_sender_schematics.pdf)

### Arduino Due

- [ArduinoDUE_CAN_with_MCP2562.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoDUE_CAN_with_MCP2562.pdf)
- [ArduinoDue_CAN_with_SN65HVD234.jpg](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoDue_CAN_with_SN65HVD234.jpg)

### Arduino Mega

- [ArduinoMega_CAN_with_MCP2515_MCP2551.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoMega_CAN_with_MCP2515_MCP2551.pdf)

### ATmegaxxM1

- [ATmegaxxM1 CAN example.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ATmegaxxM1%20CAN%20example.pdf)

Library has been also used with Maple Mini board.

\section secHWlib Hardware depended libraries

You need at least Arduino Software 1.6.6 for this sample. I'll expect you are
familiar with Arduino and using libraries. When your Arduino environment is
ready.

- Download [NMEA2000](https://github.com/ttlappalainen/NMEA2000) library zip.
- Depending of your board download libraries as zip

@note Take care that you use libraries under my Github, when available! Others may not work right
with NMEA2000!

### Teensy 4.0/4.1 boards with internal CAN

- [NMEA2000_Teensyx](https://github.com/ttlappalainen/NMEA2000_Teensyx)  library.
- Remember also install [Teensyduino](https://www.pjrc.com/teensy/td_download.html) !

CAN library is included to the code so you do not need any extra CAN library with this. NMEA2000_Teensyx
library will replace NMEA2000_teensy library in future. You can already start to use it with
all Teensy boards by forcing it with define (see NMEA2000_CAN.h comment). For critical
projects I prefer to use old library until this has been running under tests for a while.
  
### Teensy 3.1/3.2 or 3.5/3.6 board with internal CAN

- [NMEA2000_teensy](https://github.com/ttlappalainen/NMEA2000_teensy) library.  
- [FlexCAN](https://github.com/ttlappalainen/FlexCAN_Library) library.  
- Remember also install [Teensyduino](https://www.pjrc.com/teensy/td_download.html) !

NMEA2000_teensy has been originally forked from [NMEA2000_teensy](https://github.com/sarfata/NMEA2000_teensy) contributed
by [Thomas Sarlandie](https://twitter.com/sarfata/)

@note FlexCan library is already included in
[Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) extension. In order
to use my fork, you have to remove FlexCAN library from  
`C:\Program Files(x86)\Arduino\hardware\teensy\avr\libraries\FlexCAN`.  
Default FlexCAN library does not work right with NMEA2000.  
  
### ESP32 boards  with internal CAN
  
- [NMEA2000_esp32](https://github.com/ttlappalainen/NMEA2000_esp32) library

ESP32 CAN driver by Thomas Barth has been implemented inside NMEA2000_esp32
so you do not need other drivers for that.

### Arduino Due board with internal CAN

- [NMEA2000_due](https://github.com/ttlappalainen/NMEA2000_due) library
- [due_can](https://github.com/ttlappalainen/due_can) library

### Boards with MCP2515 CAN bus controller (e.g. Arduino Mega, ESP8266)

- [NMEA2000_mcp](https://github.com/ttlappalainen/NMEA2000_mcp) library.
- [mcp_can](https://github.com/ttlappalainen/CAN_BUS_Shield) library.

You can use mcp_can with boards where you connect MCP2515 CAN bus controller with SPI. You can
also use it as secondary CAN for e.g. Teensy 3.2 or ESP32.

mcp_can library is developed version of original mcp_can library.
This allows also to use 8MHz clock and has been tested with
Maple Mini.  mcp_can library can be originally found on
<https://github.com/Seeed-Studio/CAN_BUS_Shield>, but check library status has all improvements
been implemented to Seeed-Studio. At least 18.09.2017 it was up to date.

### AVR processors with internal CAN

- [NMEA2000_avr](https://github.com/thomasonw/NMEA2000_avr) library
- [avr_can](https://github.com/thomasonw/avr_can) library

### MBED boards

- [NMEA2000_mbed](https://github.com/thomasonw/NMEA2000_mbed) library
other related libraries. See origin for MBED port on <https://github.com/thomasonw/NMEA2000_mbed>

### Raspberry Pi

[NMEA2000_socketCAN](https://github.com/ttlappalainen/NMEA2000_socketCAN) library.

NMEA2000_socketCAN has been originally forked from <https://github.com/thomasonw/NMEA2000_socketCAN>

There is a document [Preparing your Raspberry Pi for the NMEA2000 library.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/Preparing%20your%20Raspberry%20Pi%20for%20the%20NMEA2000%20library.pdf) for starting up with RPi.
Hopefully I have time to write more complete document. There is example NMEA2000ToNMEA0183, which
has been tested with RPi 3B.

# Quick Example

Install all libraries to your Arduino IDE (Sketch-Include Library-Add .ZIP library).

- Open `NMEA2000\Examples\TemperatureMonitor`.
- Connect you board to USB and NMEA2000 bus.
- Send sketch to your board.

If you have Multi Function Display (e.g. Garmin GMI-20) on your NMEA2000 bus,
you should see on it's NMEA2000 bus devices new device "Simple temp monitor"
on the list.

Now you are ready to play with your own device. Check also the
`NMEA2000\Examples\ActisenseListener`, which reads all data from NMEA2000 bus and
sends it to PC. `NMEA2000/Examples/ArduinoGateway` allows you to mimic Actisense
NGT-1 and connect e.g. a Raspberry Pi running Signal-K to the NMEA2000 bus with
an Arduino or Teensy.

# Forcing CAN "driver"

was using Arduino Software older than 1.6.6)

In examples there are simple includes:

```cpp
    #include <Arduino.h>
    #include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
```

If that can not be used (like with Arduino IDE older than 1.6.6) or you would like to control naming and used "driver",
you can manually include necessary files. Specially if you want to use secondary CAN bus on your system.

## For use with Teensy 4.x (also with 3.1/3.2/3.5/3.6)

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <NMEA2000_Teensyx.h> // https://github.com/ttlappalainen/NMEA2000_Teensyx
    //
    tNMEA2000_Teensyx NMEA2000;
```

## For use with Teensy 3.1/3.2/3.5/3.6 board and FlexCan

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <FlexCAN.h>
    #include <NMEA2000_teensy.h> // https://github.com/sarfata/NMEA2000_teensy
    //
    tNMEA2000_teensy NMEA2000;
```

## For use with ESP32

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <NMEA2000_esp32.h> // https://github.com/ttlappalainen/NMEA2000_esp32
    //
    tNMEA2000_esp32 NMEA2000;
```

## For use board with MCP2515 SPI can bus tranceiver and mcp_can library

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <SPI.h>
    #include <mcp_can.h> // https://github.com/ttlappalainen/CAN_BUS_Shield
    #include <NMEA2000_mcp.h>
    #define N2k_CAN_INT_PIN 21 // Pin, where interrupt line has been connected
    #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
    //
    tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN,MCP_16MHz,N2k_CAN_INT_PIN);
```

## For use with Arduino due and due_can library

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <due_can.h>  // https://github.com/ttlappalainen/due_can
    #include <NMEA2000_due.h>
    //
    tNMEA2000_due NMEA2000;
```

## For use with Atmel AVR processors internal CAN controller

Your file should start with:

```cpp
    #include <N2kMsg.h>
    #include <NMEA2000.h>
    #include <avr_can.h>            // https://github.com/thomasonw/avr_can
    #include <NMEA2000_avr.h>       // https://github.com/thomasonw/NMEA2000_avr
    //
    tNMEA2000_avr NMEA2000;
```
