# Setting Up Hardware Environment{#pageHWSetUp}

  @brief Setting up a basic Hardware environment

\tableofcontents

This manual is meant for beginners who starts to build their own devices. There is general
information, links and hints about building NMEA2000 network on \ref secNMEA2000bus.

Since I published NMEA2000 library I had several times guided beginners with hardware
problems. In this manual I try to give collected information for recommended and required
hardware for making NMEA2000 devices and how to connect them to NMEA2000 bus.

\warning You connect your hardware always with your own risk. I wrote this document in order
to help you connecting your own hardware to the NMEA2000 bus. I do not take any responsibility
of any errors in the document or any possible damages caused to your devices.

## Some terminology

- Board. Development board like Teensy 3.2, ESP32, Raspberry Pi, Arduino Due etc.
- CAN controller. Is a chip or internal unit of processor, which takes care of CAN
  (or NMEA2000) bus communication.
- CAN transceiver (or just transceiver). “Level shifter” chip between CAN controller
  Tx/Rx and physical CAN bus CAN-H/CAN-L.
- Termination resistor. 120 ohm resistor on both end of main CAN bus. **Do not forget these!**

## Recommended hardware {#secTRecHW}

I personally prefer to use Teensy 4.0, 4.1 (or 3.2, 3.5, 3.6 which are at end of life) or ESP32 board. Those boards have internal CAN controller and require only either isolated ISO1050 or unisolated MCP2562 transceiver for NMEA2000 connection.
With default settings library requires roughly **about 30 kB rom and 4 kB RAM in normal operation**.
So you should have **at least 8 kB RAM in your processor**. If you have Arduino Mega board,
it is OK for testing and for small projects, but I do not prefer to buy one for new project.
Arduino Due is better, but it is physically bigger and eats more power than Teensies or ESP32.
If you have Arduino Uno or any board with less than 8 kB RAM, please read all library issues
and Memory requirements on library ReadMe and then forget it. If you anyway want to use it
and you have trouble to get it working, please do not set any issues.

### CAN controller {#subCANcontr}

Some boards (or processors) have internal CAN controller. In board there will be simply
CAN Tx/CAN Rx pins, which often works with 3.3 V levels. Good examples of this kind of boards
are recommended Teensy 4.0, 4.1 (or 3.2, 3.5, 3.6 which are at end of life) or ESP32 boards.
Arduino DUE has also internal CAN controller. Never connect CAN Tx/CAN Rx pins directly to
NMEA2000 bus. If you use board without internal CAN controller or you want to have second CAN
controller e.g., for Teensy 3.2 or ESP32, you can use external one. Currently only supported is
MCP2515. When you search “CAN bus shield” to buy, you have to take care of:

- Library supports only MCP2515 with either 16 MHz or 8 MHz clock. **16 MHz is preferred**.
- Check SPI levels on your board. Most shields operate only with 5 V. If your board I/O is
  only 3.3 V tolerant, you should find shield with 3.3 V transceiver. MCP2551 or TJA1050 will not work.

It is relatively easy to solder “CAN bus shield” by self. There is schematics in document
ArduinoMega CAN with MCP2515 MCP2551.pdf. If you need to use that with 3.3 V processor, you
can replace MCP2551 to MCP2562.

### CAN Transceiver {#subCANtrans}

CAN controller takes care of CAN (or NMEA2000) bus communication. To connect it to the bus,
you need some CAN transceiver. The transceiver has two tasks. Receiving: it converts the data
stream from CAN bus levels to levels that the CAN controller uses. It usually has protective
circuitry to protect the CAN controller. Transmitting: it converts the data stream from the CAN
controller to CAN bus levels.

#### Recommended transceiver {#subsubRecTra}

I recommend either isolated ISO1050  or unisolated MCP2562 transceiver. ISO1050 has own power pin for both sides so it also works with any logic levels. MCP2562 operates with 5 V and it has own pin to define Tx/Rx logic levels for 3.3 V devices. There is lot of issues with SN65HVD230 transceiver. If you use that, please do not open issue – it is not library problem.

I do not know why there has been so much problems with SN65HVD230 - according datasheet it
should be fine. I have had problems with MCP2562 only twice. Once I mixed Vdd and Vcc - worked
fine after connected them right. Another time I connected STBY pin to Vdd, which puts chip to stand by.

\note
**This is very important.**  
If you use unisolated tranceiver like MCP2562, you may cause ground loop in your boat. Ground loops may generate other weird problems. You can read more about it e.g., from Ground loop in Wikipedia <https://en.wikipedia.org/wiki/Ground_loop_(electricity)>.  
If you e.g.,feed NMEA2000 bus on center of the boat and you have unisolated device, which
also has ground by sensor on front of the boat, you have created ground loop. It may work
without problems or may not. You may have communication errors etc.

#### Unisolated transceiver {#subsubUnIso}

Unisolated tranceiver (like MCP2562) means that it will share the ground with NMEA2000 bus
and with your board. You can use unisolated transceiver, if you power your device from NMEA2000
bus and it does not connect to ground from another path like through sensor.

- Devices with onboard temperature, humidity or atmospheric pressure sensors. E.g., device
  with 1-wire temperature sensor DS18S20 or DHT11 humidity sensor can be made with
  unisolated tranceiver.
- Devices with other sensors like pressure using BMP280 via I<sup>2</sup>C, ultrasonic tank measurement, 
  hall effect current, etc. particular care must be made so that the sensor does not connect to ground in
  any other than getting power from your board. **Even their bodies must be strictly isolate so that they cannot create a separate path to
  ground.**
- Devices reading bus and converting data to WiFi.
- Devices with digital input or output as far you use isolation on I/O side. E.g.,
  optoisolators for input and relays for output.

In any case devices with unisolated tranceiver must be powered from NMEA2000 bus and they
**can not be connected to unisolated sensors.**
If you program unisolated device through USB connector with laptop, you are in safe when
laptop is powered only by batteries. USB will connect your laptop ground to NMEA2000 bus
ground, but since laptop is only battery powered, it is “floating” system. If you connect
charger to your laptop same time, then it depends of charger do you have ground loop or not.
Since this is just temporary connection, it should be OK.

##### Microchip MCP2562

The MCP2561/2 is a high-speed CAN, fault-tolerant device that serves as the interface between a CAN
protocol controller and the physical bus. The MCP2561/2 device provides differential transmit and
receive capability for the CAN protocol controller, and is fully compatible with the ISO-11898-2 and
ISO-11898-5 standards. It will operate at speeds of up to 1 Mb/s.

Typically, each node in a CAN system must have a device to convert the digital
signals generated by a CAN controller to signals suitable for transmission over
the bus cabling (differential output). It also provides a buffer between the CAN
controller and the high-voltage spikes that can be generated on the CAN bus by
outside sources (EMI, ESD, electrical transients, etc.).

\image html MCP2562.png

[MCHP 2562 Datasheet](http://ww1.microchip.com/downloads/en/devicedoc/20005167c.pdf)

#### Isolated transceiver {#subsubIso}

Isolated tranceiver has either optical or galvanic (ISO1050) isolation. Those has to be
powered from both sides with isolated power, so you need also at least two power supplies.
You must take care that both sides will also have always separate grounds and they do not
touch in any way. Isolated device is safest solution in all cases.

##### TI ISO1050 Isolated CAN Transceiver

The ISO1050 is a galvanically isolated CAN transceiver that meets the specifications of the
ISO11898-2 standard. The device has the logic input and output buffers separated by
a silicon oxide (SiO2) insulation barrier that provides galvanic isolation of up
to 5000 VRMS for ISO1050DW and 2500 VRMS for ISO1050DUB. Used in conjunction with isolated
power supplies, the device prevents noise currents on a data bus or other circuits
from entering the local ground and interfering with or damaging sensitive circuitry.
As a CAN transceiver, the device provides differential transmit capability to the bus
and differential receive capability to a CAN controller at signaling rates up to
1 megabit per second (Mbps). The device is designed for operation in especially harsh
environments, and it features cross-wire, overvoltage and loss of ground protection
from –27 V to 40 V and overtemperature shutdown, as well as –12-V to 12-V common-mode
range.
The ISO1050 is characterized for operation over the ambient temperature range of –55°C to 105°C.

\image html iso1050.png

[ISO1050 Datasheet](https://www.ti.com/lit/ds/symlink/iso1050.pdf)

\section secSchem Schematics sample

### Feeding power from NMEA2000 {#subFeedPwr}

Note that development boards have been designed to be powered from USB. If you power
your board from NMEA2000, you need to check how you can isolate USB power. All Teensy
boards has “5V pad”, which you can cut. E.g., Teensy 3.2

\image html teensy32_pinout.png

### Teensy 3.2, simple unisolated {#subTEunIso}

\image html teensy32_uniso.png
I used this simple schematic at the beginning with 1-wire or other onboard or isolated sensors.

### Teensy 3.2, simple isolated {#subTEiso}

\image html teensy32_iso.png
With isolated tranceiver you are safe to connect different sensors. You can e.g.,
measure engine temperature from engine by using resistor divider.

### More protection for tranceiver {#subTEperf}

Simple samples above did not have any protection to the bus. Unfortunately, power
supplies etc. may cause your device to emit noise to the bus – or opposite. To
have better protection, you can use something like this.
\image html iso_trans.png

### Feed isolated power from NMEA2000 {#subFeedPwe}

If your device does not require much power and you want to feed it totally from
NMEA2000 bus, you can use something like this.
\image html feed_pwr.png
Here Net-S is 12 V and GND-ISO is Gnd from NMEA2000 bus. You then feed your
device from +5V/GND. Note that with this connection you do not have isolated
12 V for sensors. If you need that you have to use build your device with
separated power supply as in chapter Teensy 3.2, simple isolated.

## Testing devices {#secTesting}

When your device hardware is ready and you have some other device to test it with,
you can make simple test bench at home. Simply connect your device NMEA2000 CAN-H/CAN-L
with another device. Note that if you use e.g., MFD as other device, that **needs to be
powered on NMEA2000 connector** and also with external connector, if it has one. This is
because MFD NMEA2000 tranceiver is isolated and powered from NMEA2000 bus.
If you do not have MFD, but have several own devices use example DataDisplay2 on
one device to listen bus.
Add 120 ohm termination resistor to both end of your test bus. **The most common problem
has been missing or wrong termination resistors!**

\image html can_wiring.png

Upload e.g., example TemperatureMonitor to your device and run it. It has been tested
with Teensy, ESP32, Arduino DUE, Arduino Mega etc. When you run it, you should see it
on your MFD device list or whatever your other device is.
If you do not, check at least 10 times your connections, termination resistors,
powering. Power off your system and measure that there is 60 ohm resistance between
CAN-H and CAN-L. If everything is fine and you use some “CAN bus shield” with MCP2515,
check the configuration on software for it is right. Library defaults 16 MHz clock
and SPI CS pin 53. In 100% of cases problem has been in hardware or software configuration.
When you have got it working, you can start to develop your own software.

## Most common hardware errors {#secCommErr}

If you test your device with e.g., example TemperatureMonitor, it should work. If it
start to print “Sent failed” after a while, you will most probably have some hardware
failure. “Sent failed” means that your device CAN controller can not send data to the
bus. “Sent failed” message will come with delay, when send buffer has been filled up
so it depends of your buffer size how long it will take.

- False or unconnected termination resistors. Measure that unpowered NMEA2000 bus has
  60 ohm resistance between CAN-h and CAN-L.
- CAN-L/CAN-H mixed
- CAN Rx/Tx mixed.
- Tranceiver broken
- CAN controller broken
- Invalid configuration settings for external CAN controller (MCP2515/mcp_can)
- Invalid CAN Tx/Rx pins connected. (Teensy 3.5, 3.6, 4.0, 4.1)
- Invalid CAN Tx/Rx pin definition e.g., with ESP32. Check that you used driver default pins,
  have proper definitions prior including driver or define pins on driver constructor.
- Wrong logic levels on tranceiver or external CAN controller (CAN bus shield).
- Isolated receiver like MFD NMEA2000 not powered.
- Tranceiver set to disabled state.

## NMEA2000 bus {#secNMEA2000bus}

NMEA_2000 bus is electrically CAN bus. NMEA2000 protocol is based on SAE_J1939,
with specific messages for the marine environment. The one important thing differing
from CAN is that bus speed is fixed to 250kb. On CAN bus it can be up to 1 Mb or
even variable. If you are interested, dig more information from internet.
NMEA2000 bus is like ethernet so that you can have multiple devices on
same bus, they all can send information to the bus and there is no bus
master device. The biggest difference to traditional NMEA0183 connection
is that on NMEA0183 only one device can send data.

- [NMEA_2000](https://en.wikipedia.org/wiki/NMEA_2000)
- [SAE_J1939](https://en.wikipedia.org/wiki/SAE_J1939)

### Bus topology {#subTopology}

In NMEA2000 bus there is one main bus called backbone or trunk. Devices to
bus will be connected with drop cables. There are lot of good documents available.
Look e.g. Proper NMEA 2000 Installation by NMEA organization, Complete guide to
building an NMEA2000 network by Actisense, Network Installation guide by Maretron.

- [2012 ibex full  nmea installation.pdf>](https://web.archive.org/web/20190602112104/https://www.nmea.org/Assets/2012%20ibex%20full%20%20nmea%20installation.pdf)
- [complete-guide-to-building-an-nmea-2000-network](https://actisense.com/complete-guide-to-building-an-nmea-2000-network/)
- [Network Installation Guide.pdf](https://www.maretron.com/products/pdf/Network%20Installation%20Guide.pdf)

Important things are:

- You do not exceed maximum backbone length (100 m for light cable).
- You have one 120 ohm termination resistor on each end of backbone. If you
   measure unpowered bus with ohm meter, you should measure 60 ohm resistance
   between CAN-H and CAN-L.
- You do not exceed maximum drop cable length, which is 6 m. E.g. in sailing
   boats, if your bus will go to top of the mast, you have to have that as
   backbone and so have termination resistor at top of the mast.
- You power bus from exactly one point. This also means that your bus ground
   wire must be connected ground only on that point to avoid ground loops.

### Bus connectors {#subConnetor}

If you need just one or two cable, it may be easiest to buy it from marine
dealer, but if you start to build whole NMEA2000 network, you may save a
lot by using industry standard parts.
NMEA2000 bus uses same cabling as DeviceNet, which is used in industry.
So you can build your NMEA2000 network with DeviceNet cables and connectors.

- [DeviceNet](https://en.wikipedia.org/wiki/DeviceNet)

The most common connector used in NMEA2000 is M12 5-pin Micro-C connector.

<table style="border: none"><tr>
<td>\image html microC_male.png</td>
<td>\image html microC_female.png</td>
</tr><tr>
<td>\image html conn1.png</td>
<td>\image html conn2.png</td>
</tr></table >

By searching from internet with ”M12 5-pin Micro-C connector” you will find
lot of alternatives cheaper that from local marine dealer. In pictures above
are connectors with screw terminals and easy to install. To get smaller sizes
you can use connectors with solder terminals.

\note If you search connector with fixed cable, take care that you select it
      with DeviceNet colouring.

You can search also “DeviceNet connector” and you will find commonly used connector:
\image html conn3.png
With above model it is easy to make e.g., hub-box instead of using expensive Micro-C hubs.

### Bus cables {#subCable}

In fixed cabling I have used DeviceNet PUR L45467-F16-W8 (1 x 2 x 0,67
mm2 + 1 x 2 x 0,38 mm2). I have used this because it happens to be my cable
dealer type. Just google “DeviceNet cable” to find alternatives.
You can use other types of cables, but to make life easier using NMEA2000 or
DeviceNet cable you have right pairing, proper shielding and
colours: + = red, - = Gnd, CAN-H = white, CAN-L = blue, and shield.

<table style="border: none"><tr>
<td>\image html cable.png</td>
<td>\image html cross_cable.png</td>
</tr></table >

### Prepare for failures {#secFailure}

If you had problem on old NMEA0183 connection, only one device went off. With
NMEA2000 there is risk that one failing device will lock the whole bus and all
your devices will be then off. Simple preparing may safe you from full disaster.

- Make drawing of your NMEA2000 bus connections.
- Make layout drawing of your boat about all NMEA2000 bus connection points
   so that you can easily locate them. Mark also power feed place.
- Make simple wire with bus connector, which you can use to check bus
   voltage with volt meter at any connection point.
- Prepare with 1-2 extra termination resistor pieces.
- Make plan, which order you unplug devices from bus in case of failure.

In case you have problems on the bus and it is not clear, simply take drawings and:

- Unplug device from some connection point.
- If it does not help, unplug rest of backbone at that point and
  replace with termination resistor.
- If it does not help, measure voltage on that point.
- If voltage is ok, leave this unconnected and try other connection point.
