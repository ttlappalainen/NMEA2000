This is a very barebones working example of a barbeones ESP32 project in PlatformIO.

It has 2 "senders" that are just placeholders, you can insert your own code to actually read 
whatever sensors that you are using.  This will send the information to 2 different PGNs, 
as well as print any incoming messages to the Serial port.

src/main.cpp contains all the required code, and the platformio.ini file contains the required dependencies.
main.cpp is pretty well commented up to illustrate what the intent of the code is.