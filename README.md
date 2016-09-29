# NMEA2000
NMEA2000 library for Arduino

Object oriented NMEA2000 library for Arduino Boards

Library gives you a easy way to make NMEA2000->PC interface like Actisense NGT1
or e.g. temperature sensor node to NMEA2000 bus.

To use Arduino Due board internal CAN you will also need NMEA2000_due library and
due_can library, which you can download from  https://github.com/collin80/due_can

To use Arduino boards with MCP2515 CAN bus controller, you will also need NMEA2000_mcp library
and mcp_can library. mcp_can library can be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
Currently NMEA2000_mcp library uses developed version, which allows also to use 8MHz clock and
has been tested with Maple Mini. So until these improvements has not been implemented to Seeed-Studio
library, use library found on https://github.com/peppeve/CAN_BUS_Shield

To use a Teensy 3.1/3.2 board internal CAN with a MCP2551 CAN transceiver (or similar, see FlexCAN documentation), 
you will need the [NMEA2000_teensy](https://github.com/sarfata/NMEA2000_teensy) contributed by 
[Thomas Sarlandie](https://twitter.com/sarfata/) and the [FlexCAN library](https://github.com/teachop/FlexCAN_Library).
