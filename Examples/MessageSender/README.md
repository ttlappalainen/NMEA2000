# Message sender
NMEA 2000 message sender example. It sends several different messages to the
NMEA 2000 bus. If you connect two Arduinos together with NMEA 2000 bus one running this and other running
DataDisplay2.ino, you can see messages send from this on serial monitor on that port, which is connected
to Arduino running DataDisplay2.ino. If you connect this to real NMEA 2000 bus, you can see messages 
also on your Multi Function Device.

MessageSender has simple command line interface. With simple commands you e.g. disable all messages and
then enable them one by one. To get available commands type ? and line feed to console to get help.
