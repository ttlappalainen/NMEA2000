# Data display 2
Example reads messages from NMEA 2000 bus and shows some messages decoded showing data in values.
Other messages will be showed in HEX.

If you connect two Arduinos together with NMEA 2000 bus one running this and other running
MessageSender.ino, you can see messages send by MessageSender on serial monitor.

Note! If you use Arduino Mega and you have lot of messages on bus, I prefer to also connect
interrupt some line for MCP2515.