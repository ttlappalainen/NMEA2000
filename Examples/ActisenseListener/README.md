# Message sender
Example reads messages from NMEA 2000 bus and forwards them to serial in Actisense format.

Connect Arduino with this code to NMEA 2000 bus and listen port with Actisense NMEA Reader
so you will see all messages translated.

Note! If you use Arduino Mega and you have lot of messages on bus, I prefer to also connect
interrupt some line for MCP2515.
