# Reference for C++ library {#pgLibRef}

\tableofcontents

## Introduction

NMEA2000 is object oriented C++ library, which should make it simple to develop own NMEA2000
based devices. I created it because I wanted to get rid of limitations of expensive devices
on market. With my first own device on my yacht, I could replace three devices – NMEA0183
combiner, NMEA0183->NMEA2000 Converter, NMEA2000->PC converter - with single Arduino Due
board and few extra chips.
To use NMEA2000 library you need basic skills for programming. I also try to write simple
instructions to start from scratch. Even I have been doing sw and hw development for years,
it took some time to dig information from internet to get started with Arduino and do
simple tasks like installing library.
For using NMEA2000 library I prefer Teensy 3.2, 3.5, 4.0, 4.1 or ESP32 with MCP2562 chip.
For simple things Arduino Mega with CANBUS shield or schematics found
under [documents](https://github.com/ttlappalainen/NMEA2000/tree/master/Documents) is
ok. I have not tested smaller boards and I prefer to forget any board less than 8 kB RAM.

## Start from scratch and make your first NMEA2000 device

Lets think you have temperature sensor and you like to have it visible on NMEA2000 bus.
Here I expect that:

- you already have Arduino Programming IDE installed. If not, do first “5.1 Getting started
  with Arduino Programming IDE”.
- you know how to import .zip libraries to Arduino. If not, read help on “5.2.1 Importing library from zip”.
- you have your Arduino board connected to USB on PC.
- NOTE! Arduino IDE serial monitor must be closed!
- You have right board selected from IDE Tools-Board” and right COM port selected from IDE Tools-Port:
  
## Preparing to use NMEA2000 library

At first we have to download and install necessary libraries for Arduino and other useful tools.  
Download and install Arduino libraries according to your hardware see \ref secHWlib.

If you want to read or decode NMEA2000 messages, you can download and install
“NMEA Reader”-application from <http://www.actisense.com/media/?product=nmea-reader-and-ebl-reader>

## Try NMEA2000 library with TemperatureMonitor example

### Try on PC only

Now we are ready to try sample without N2k bus connection. So you need only Arduino board and
PC. No CANBUS shield or extra chips yet!

- Open Example TemperatureMonitor sketch, which came with NMEA2000 library. Select from
  IDE “Open…”-libraries/NMEA2000-master/Example/TemperatureMonitor/TemperatureMonitor.ino
- uncomment lines
  
```cpp
    // Serial.begin(115200);
    // NMEA2000.SetForwardStream(&Serial);
```

and comment line

```cpp
    NMEA2000.EnableForward(false);
```

within setup() function.

- If you are using Arduino Mega, uncomment line
  
```cpp
    // NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); 
```

within setup() function.

We need to do this, since Mega board does not have build in CAN bus controller, so the
library would fail, when it tries to open CAN on non debug mode.

- Upload sketch to board.
- Start “NMEA Reader” and from toolbar dropdown select your Arduino COM port.
  
Now you should see:  

```shell
    "ISO address claim" (PGN 60928)  
    "Environmental parameters" (PGN 130310) 
    "Environmental parameters" (PGN 130311)  
    "Temperature" (PGN 130312)  
```

messages on “NMEA Reader”.

\note If you want to upload sketch again to the board, you have to release Arduino
programming COM port by selecting port “- NOT SELECTED –“ on “NMEA Reader”.

### A bit deeper look in to the TemperatureMonitor example

First note that if you want to modify and save this sample, you have to save it on other location.

#### Change to use read real data

If you are happy with sending only cabin and water temperatures to the N2k bus,
you simply change functions double ReadCabinTemp() and double ReadWaterTemp().
So e.g. if you have sensor providing 10 mV/ °C, you can change the function:

```cpp
    double ReadCabinTemp() {
    return (5.0 * analogRead(A0)*100/1024);
    }
```

Do the same for ReadWaterTemp() (naturally you have to use other analog input e.g.
A1) and you have your own ready NMEA2000 temperature monitor.

#### Remove unnecessary messages

With the library N2k messages will be sent by call NMEA2000.SendMsg(N2kMsg). Before
sending you have to setup the variable N2kMsg. In that example it has been done for
Temperature message with call:

```cpp
    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
```

So if you do not need cabin temperature message (PGN 130312), simply delete that
call SetN2kTemperature(…) and next call NMEA2000.SendMsg(N2kMsg); Similarly if you
do not provide water temperature, delete calls SetN2kOutsideEnvironmentalParameters(…)
and  call NMEA2000.SendMsg(N2kMsg); after that.  
Now just upload sketch and open right port to “NMEA Reader” and you should see
only "ISO address claim" (PGN 60928) and "Environmental parameters"
(PGN 130311) on “NMEA Reader”.

#### Change the device information

On NMEA2000 bus each device will tell to the bus what it is. This can be found
on sample under setup() function. Call SetDeviceInformation is important for right
functionality on the bus. If you have only this own device on the bus, you can leave
it like it is. In other case take deeper look in to the explanation for the function.
Note that if you add several own devices to the bus, you have to configure parameters
for this function so that each device will get unic “name” as a combination of
these parameters.  
Call SetProductInformation is not so important, but defines nice to know information.
If you have some “Multi Function Display” (MFD) on the bus, it will show on the
device list information set by this function. So you can e.g. set the “Manufacturer's
Model ID” to “Jack’s temperature monitor” and version code and model version as you
like, so on your MFD you can see that and choose right device.

### Connecting your sample device to the NMEA2000 bus

So now we have sketch, which works on the PC. If your sketch runs and shows
information in “NMEA Reader”, it can be easily connected to the N2k bus. For
electrical connection you need either CANBUS shield for Arduino Mega or you
can build your own shield. You can find more information at \ref pageHWSetUp.

There is also drawing for Teensy on [Teensy_Actisense_listener_sender_schematics.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Examples/TeensyActisenseListenerSender/Documents/Teensy_Actisense_listener_sender_schematics.pdf),
which has additional USB with Pololu chip, which is only required for two USB solutions. So
Prepare physical connection to the NMEA2000 bus and comment line:

```cpp
    NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense);
```

within setup() function, if you have previously uncommented it. Upload the sketch
and check the result.  
On your NMEA2000 “Multi Function Display” (MFD, like Garmin GMI20) add temperature
reading visible. For that I can not give more help, since each MFD has different
user interface. Now you should see the temperature reading on your MFD.

If you are not interested in N2k bus messages and you want to add your own debug
code, you have to disable message forwarding by uncommenting line:

```cpp
    NMEA2000.EnableForward(false);
```

### Try NMEA2000 library with WindMonitor example

This is exactly same as TemperatureMonitor, but sends wind data to the bus. Only necessary
function names has been changed. So follow the “ Try NMEA2000 library with
TemperatureMonitor example”.

## Arduino Programming IDE

### Getting started with Arduino Programming IDE

- Download and install [Arduino software](https://www.arduino.cc/en/Main/Software) for suitable OS
- If you are using Teensy based boards, download and install also [Teensyduino](https://www.pjrc.com/teensy/td_download.html)

\note With Teensyduino you have to check which version of Arduino IDE it supports!
Check situation of Teensy FlexCAN library problem under <https://github.com/ttlappalainen/NMEA2000/issues/50>

- Start newly installed Arduino development IDE from “Arduino”-icon.
- Connect your board to USB
- Select right board for IDE from Tools-Board
  
 \note If you have “Arduino Due” do not mix it to “Arduino Duemilanove”. Select “Arduino
 Due (programming port)”. If you do not have that in selection list add support for due
 board selecting Tools-Board:-“Boards Manager…”. Then type “due” to filter box and click
 found “Arduino SAM Boards…”. Then on left click “Install”

- Select right COM port for IDE from Tools-Port: E.g. for Arduino Mega you should see on
  the list port: COMx (Arduino/Genuino Mega or Mega 2560)
- Now open your first sketch to IDE. Open File-Examples-01.Basics-AnalogReadSerial
- To see example output, open serial monitor: Tools-“Serial monitor”
- Upload sketch by IDE toolbar button or from menu Sketch-Upload.
- Now you should see board analog pin0 value running on serial monitor.

Now you are ready for your own developing.

### Arduino Libraries

Arduino has both system wide and user libraries.

Arduino has both system wide and user libraries.

- System wide libraries are visible for all users and they are located as default
  e.g. on 64 bit Windows OS under "Program Files (x86)>Arduino>libraries"
- User libraries are visible only for current user and they will be located as
  default  e.g. on Windows OS under "Documents>Arduino>libraries"

Arduino IDE installation automatically installs system wide libraries. When you add
library with Arduino IDE, it will be added as default can make some library visible
for all users by moving installed library from your personal libraries to system wide libraries.
### Importing library from zip

If someone points you direct link as .zip file to Arduino library, you need only:

- Download library to your PC
- On Arduino IDE select Sketch-“Include Library”-“Add .ZIP Library…”
- Navigate to you download path and select the .zip file you just downloaded and press Open.

So now you should have that library on your personal libraries.

### Importing libraries from GitHub

Importing library from GitHub is also very simple. So you have heard or found good link to someone GitHub page.

- Open browser to the link. E.g. <https://github.com/ttlappalainen>
- Select repository, which should contain the Arduino library you are looking for.
  E.g. NMEA2000 on link above.
- Somewhere on page – currently on right – there is button “Clone or download”.
  Click that and then on popup button “Download ZIP” and choose “Save to disk” and press “OK”
- Now just follow "Importing library from zip".

## NMEA2000 library reference

NMEA2000 is closed standard. You can buy documents for it for high price. As far as I have understood,
then you also accept that you are not allowed to tell any third party about document contents. So all
the information and experience I have has been collected from free documents and mostly from OpenSkipper
project, which development I have been continued. So there may be errors on library. Anyway have tried
to make it simple to use, but still giving “NMEA2000 compatibility”.

### Relevant Classes

#### Abstract base class tNMEA2000

The tNMEA2000 class contains functions for NMEA2000 handling like e.g. sending and reading messages.
This is purely an abstract class and to use a real device you must use an appropriate inherited class.
Normally you use functions within this class.

#### Abstract base class tNMEA2000::tMsgHandler

Inherit this class for your own message handler. When new message arrives, all attached handlers,
which matches message PGN, will be called by ParseMessages. You can define this handler for single
PGN or for all by setting PGN for base class to 0. You can also attach and detach class handler from
tNMEA2000 object.

#### Inherited classes according to your Board

There are own inherited class for each CAN interface type. As described below e.g. for Arduino Due you
use due_can and NMEA2000_due libraries. Since Arduino IDE 1.6.6 it has been possible to include libraries
within other headers, so to use any of currently tested board you can simply add includes

```cpp
  #include <Arduino.h>
  #include "NMEA2000_CAN.h"
```

So the "#include <NMEA2000_CAN.h>" will then automatically select right library according to the board
you have selected on IDE (see \ref USE_N2K_CAN). If you want strictly control includes or the board will
not be right selected by NMEA2000_CAN.h, use includes described on each inherited class or force
selection with define before include. E.g.

```cpp
  #include <Arduino.h>
  #define USE_N2K_CAN 1  // Force mcp_can
  #include "NMEA2000_CAN.h"
```

\note Alle the inherit classes come along with the library according to your hardware setup. see \ref secHWlib

##### Inherited class tNMEA2000_mcp

The tNMEA2000_mcp is class for using library with CANBUS shield or according to schematic
[ArduinoMega_CAN_with_MCP2515_MCP2551.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoMega_CAN_with_MCP2515_MCP2551.pdf), which I have made for sample.  
To use this class, you need to include  in your project beginning:

```cpp
  #include <SPI.h>
  #include <mcp_can.h> 
  #include "NMEA2000_mcp.h"
  #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
  #define N2k_CAN_INT_PIN 21 // Use interrupt  and it is connected to pin 21
  tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN);
```

You can find the mcp_can library from <https://github.com/ttlappalainen/CAN_BUS_Shield/archive/master.zip.>
Originally it was developed by SeedStudio <https://github.com/Seeed-Studio/CAN_BUS_Shield>. That library was
last merged 18.09.2017. I have not yet tested changes after that.

##### Inherited class tNMEA2000_due

The tNMEA2000_due class is for using library with Arduino Due, which has internal can bus controller. I
personally prefer this board, since it is more powerful. It also has separate USB port. Also the physical
interface to the bus is simpler. You can find sample schematic on
[ArduinoDUE_CAN_with_MCP2562.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoDUE_CAN_with_MCP2562.pdf).  
To use this class, you need to include  in your project beginning:

```cpp
  #include <due_can.h>
  #include "NMEA2000_due.h"
  tNMEA2000_due NMEA2000;
```

You can find the due_can library from [due_can](https://github.com/ttlappalainen/due_can).
That had been originally forked from <https://github.com/collin80/due_can>, but Collin library has
some changes I have not tested.

##### Inherited class tNMEA2000_teensy

The tNMEA2000_teensy class is for using library with Teensy 3.2, 3.5, 3.6 boards, which has internal
can bus controller. Physical interface is similar as with Arduino Due.  
To use this class, you need to include  in your project beginning:

```cpp
  #include "NMEA2000_teensy.h"
  tNMEA2000_teensy NMEA2000;
```

##### Inherited class tNMEA2000_esp32

The tNMEA2000_esp32 class is for using library with ESP32 based boards, which has internal can bus
controller. Physical interface is similar as with Arduino Due or Teensy.  
To use this class, you need to include  in your project beginning:

```cpp
  #include "NMEA2000_esp32.h"
  tNMEA2000_esp32 NMEA2000;
```

##### Inherited class tNMEA2000_avr

The tNMEA2000_teensy class is for using library avr based boards. I have not yet experience
of them, but thomasonw uses them.
To use this class, you need to include  in your project beginning:

```cpp
  #include "NMEA2000_avr.h"
  tNMEA2000_avr NMEA2000;
```

#### Message container class tN2kMsg

This is the class for containing N2k (PGN) message. It contains pure data and functions to
handle it. At least for now there is no automated system, which could you easily fill or
read data for specific PGN. You have to know the PGN information fields like for NMEA0183
data fields. I and other developers have written functions, which has been defined on
N2kMessages.h, which helps you the fill or read data on tN2kMsg object.  

As in example TemperatureMonitor you send PGN 130312 (Temperature) message with simple code:

```cpp
  tN2kMsg N2kMsg;

  SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
  NMEA2000.SendMsg(N2kMsg);
```

So as you see, you do not need to know much about tN2kMsg to use it. It is simple container.

\note There are functions SetN2k130312 and inline function SetN2kTemperature, which
simply calls SetN2kPGN130312. This makes code easier to read (I hope), since you do not
need to see PGN numbers there. But it’s your choice, which function you will use.  

\note If you need something, which is not already on N2kMessages.h, I’ll add it
there or if you are familiar with NMEA 2000 message structures and Github, you can fork
library and add it yourself. So then it is available for anybody.

#### Group function (PGN 126208) handler class tN2kGroupFunctionHandler

NMEA 2000 definition requires than devices should respond group function messages. This
class is default handler, which simply responds “unsupported” for all queries.  
see \ref tN2kGroupFunctionHandler

\todo More Details in documentation needed here ....

#### Inherited group function handler tN2kGroupFunctionHandlerForPGN60928

This class handles mandatory group function requests and commands for PGN 60928.  
see \ref tN2kGroupFunctionHandlerForPGN60928

#### Device list collector class tN2kDeviceList

This class can be used to read devices on the connected NMEA 2000 bus. When class has been
attached to tNMEA2000 object, it will automatically collect and update information of devices on the bus.
On the NMEA 2000 bus all devices (also called nodes) has own source address. This source can
change e.g. when new devices will be added. In principle on steady system also source will be
same. The safer way is to use device “Name” which should be unique. So if you e.g. have two
speed logs and you want to specify the one you want to use, you can use tN2kDeviceList class
to search source address for specified name and then read log messages from that source.

### Using tNMEA2000

There are several examples for using library with different functions like for only listen bus
data (ActisenseListener.ino), sending temperature (TemperatureMonitor.ino) or wind data
(WindMonitor.ino) from your own sources or displaying bus data somehow (DisplayData.ino
and DisplayData2.ino). With combination of those or adding NMEA0183 library functions, you
can have full control for your N2k bus information.
I have already own device, which reads data from old NMEA0183 devices and forwards it to the
N2k bus, but same device also forwards data from bus to the PC. It also listens on input
pin – MOB and when that is activated, it will start to send route information back to
the activation position. I have also temperature and fuel consumption monitors to the N2k bus.  
So below is short description of member functions, which hopefully gives you better knowledge
why something has been used on samples.

#### Device modes

In principle all devices should act as node on N2k bus. But if you are only reading messages
on bus, why to tell anybody. So I have defined different modes how tNMEA2000 class behaves.


##### tNMEA2000::N2km_ListenOnly

This is default mode. The device simply listens data from bus and forwards it to the forward
stream. Look example ActisenseListener, you need only 20 line for making device to read data
on N2k bus.  
Also if you like to make a device, which displays some data on bus on e.g. TFT display, you can
use this mode. There is simple example DataDisplay for that.

##### tNMEA2000::N2km_NodeOnly

In this mode device will only send data to the N2k bus. I also automatically informs itself
to other devices on the bus and does required address claiming automatically. The device does not
send as default anything to any forward stream.  
Use this mode for device, which simply e.g. reads data from analog or digital input or
NMEA0183 bus and sends it to the N2k bus. Look example TemperatureMonitor.

##### tNMEA2000::N2km_ListenAndNode

In this mode device works fully to both directions. It also forwards all data from bus to
the forward stream, which you can define with function \ref tNMEA2000::SetForwardStream.

##### tNMEA2000::N2km_SendOnly

In this mode it is like tNMEA2000::N2km_NodeOnly, but it does not do automatic address
claiming and does not forward any messages from N2k bus to stream. So this is useful,
if you e.g. have some control pair like autopilot keypad/”control unit” and you want
to fool that keypad sends something to the “control unit”. Then you also need to know
also source addresses of keypad and “control unit”.  
I have used this mode e.g. with example ActisenseSender and my NMEA Simulator found on
<http://www.kave.fi/Apps/> .

##### tNMEA2000::N2km_ListenAndSend

This is like the tNMEA2000::N2km_SendOnly mode, but it also forwards messages from N2k
bus to the stream. In this mode one can have invisible gateway device between computer
and N2k bus. This mode can be used e.g. if one has a PC application, which is capable to
read and send messages in Actisense format to serial port.
I have used this mode with example TeensyActisenseListenerSender and Actisense NMEA Reader
and NMEA Simulator.

#### Message forwarding

Normally on N2k bus a device either shows data from bus (MFD devices) or sends data
to the bus (wind, GPS, temperature etc.). With this library you can also get messages
forwarded to the stream. In listen mode, the device will read all messages from N2k bus
and forwards all messages to the ForwardStream. For forwarding you have to define a
forward stream with function \ref tNMEA2000::SetForwardStream. Of course you also need to
open a stream first e.g. with Serial.begin(115200);  
Messages will be forwarded as default in Actisense format. This is supported by at least
some PC chart plotter applications. With default format Actisence “NMEA Reader”, which
we used on sample, you can show message data. A better visualizer is OpenSkipper, on which
you can tailor your own displays.  

\note Default own messages send with tNMEA2000.SendMsg will be forwarded even when your
device has been set to node only mode. This may disturb your developing, if
you e.g. want to write own clear text messages within your code. You can disable that by either:

- tNMEA2000::EnableForward(false) to disable forwarding totally
- tNMEA2000::SetForwardOwnMessages(false) to disable own messages. But then, if your device
  is in listen mode, it will still forward messages from bus.
- tNMEA2000::SetForwardOnlyKnownMessages(true) to define that only known messages will be
  forwarded. The known messages are system messages and listed single frame or fast packet
  messages. See also \ref tNMEA2000::SetSingleFrameMessages, \ref tNMEA2000::SetFastPacketMessages,
  \ref tNMEA2000::ExtendSingleFrameMessages and \ref tNMEA2000::ExtendFastPacketMessages.
  
#### Debug mode

Debug mode is as default set to tNMEA2000::dm_None. In other debug modes device will not
send anything to the N2k bus.
Debug mode tNMEA2000::dm_Actisense is usefull, if you do not have board with internal CAN
bus controller, because in that mode it does not try to open CAN bus during sending message.
Instead it simply writes message to the forward stream, which you can read e.g.
with “NMEA Reader” and see that your device is sending message right.  

Debug mode tNMEA2000::dm_ClearText defines that messages will be send to forward stream
as clear text. Unfortunately on clear text there are only source, destination, priority
data length and PGN shown as clear text. Data itself will be shown as hex.  

#### Multi device support {#descMultiDeviceSupport}

In NMEA 2000 bus you have devices or also called nodes. Normally each physical device is one
device on the bus. I have not seen any requirement that a device could not serve as engine
information (device class 50 – propulsion) or sensor information (device class 75 - Sensor
Communication Interface) interface at same time and just use device class 25 (Inter/Intranetwork
Device) and function 132 (Analog to NMEA 2000 Gateway). Anyway I just noticed that e.g.
B&G Vulcan 7 chartplotter acts as multi device. I do not know are these devices different
chips inside Vulcan or are they within same code as my multi device.  
If you like show different functions as own device on the bus, you can do that with multi
device support. All you need to do is to first set device count (see SetDeviceCount), set
device and product information for each device and on sending messages use right device
index. See example MultiDevice.

#### Member functions

##### SetDeviceCount

With this function you can enable multi device support. As default there is only one device.

\note To enable multi device support, you need to call this before any other tNMEA2000 class function.

##### SetN2kCANMsgBufSize

With this function you can set size of buffer, where system stores incoming messages. The default
size is 5 messages.  
Some messages are just single frame messages and they will be read in and handled immediately on
call to ParseMessages. For multi framed fast packet messages there is no guarantee that all frames
will arrive in order. So these messages will be buffered and saved until all frames has been received.
If it is not critical to handle all fast packet messages like with N2km_NodeOnly, you can set buffer
size smaller like 3 or 2 by calling this before open.

##### SetN2kCANSendFrameBufSize

With this function you can set size of buffer, where system saves frames of messages to be sent.
The default size is 40 frames so totally 320 bytes. Call this before any device related function
like SetProductInformation.  
When sending long messages like ProductInformation or GNSS data, there may not be enough low level
buffers for sending data successfully. This depends of your hw and device source. Device source
has an effect due to priority of getting sending slot on low level device. If your data is critical,
use a buffer size, which is large enough.  
E.g. Product information takes totally 134 bytes. This needs 20 frames. GNSS contains 47 bytes,
which needs 7 frames. If you want to be sure that both will be sent on any situation, you need
at least 27 frame buffer size.

##### SetProductInformation

If you are using device modes tNMEA2000::N2km_NodeOnly or tNMEA2000::N2km_ListenAndNode, it would
be good that you set this information. With this you can set how e.g. Multi Function Displays
(MFD) will show your device information. This is not critical, but nice to have it right.
See example TemperatureMonitor.ino.

##### SetDeviceInformation

If you are using device modes tNMEA2000::N2km_NodeOnly or tNMEA2000::N2km_ListenAndNode, it is
critical that you set this information.

\note You should set information so that it is unique over the world! Well if
you are making device only for your own yacht N2k bus, it is enough to be
unique there. So e.g. if you have two temperature monitors made by this library, you have to set at
least first parameter UniqueNumber different for both of them.  

Device information will be used to choose right address for your device (also called node) on
the bus. Each device must have an own address. Library will do this automatically, so it is enough
that you call this function on setup to define your device.

##### SetDeviceInformationInstances

With this function you can set device instance lower, device instance upper and system instance values.

##### GetDeviceInformation

With this function you can read current device information. Normally device information contains
what you have set during initializing with SetDeviceInformation and SetDeviceInformationInstances
functions.

\note device information instances can be changed by the NMEA 2000 group function by
e.g. using system configuration device. So you should time to time check if they have changed and
save changed data to e.g. EEPROM for use on startup.  

See \ref tNMEA2000::ReadResetDeviceInformationChanged

##### SetConfigurationInformation

With this function you can set configuration information, which will be saved on device RAM.
As an alternative you can set configuration information saved on progmem with
\ref tNMEA2000::SetProgmemConfigurationInformation  
Configuration information is just some extra information about device and manufacturer. Some
MFDs shows it, some does not. E.g. NMEA Reader can show configuration information.

##### SetProgmemConfigurationInformation

With this function you can set configuration information, which will be saved on device program memory.
See example BatteryMonitor.ino.  
As default system has build in configuration information on progmem. If you do not want to have
configuration information at all, you can disable it by calling SetConfigurationInformation(0);

##### SetSingleFrameMessages

As a default library has a list of known messages. With this function user can override default
list of single frame messages. See also \ref tNMEA2000::ExtendSingleFrameMessages.

##### SetFastPacketMessages

As a default library has a list of known messages. With this function user can override default
list of fast packet messages. See also \ref tNMEA2000::ExtendFastPacketMessages.

\note If an incoming fast packet message is not known, it will be treated as single frame
message. So if you want to handle unknown fast packet message, you need to duplicate frame
collection logic from library to your code. So it is easier to have fast packet messages listed
on library, if you want to handle them.

##### ExtendSingleFrameMessages

As a default library has a list of known messages. With this function user can add own list of
known single frame messages.

\note Subsequent calls will overwrite the previously set list

##### ExtendFastPacketMessages

As a default library has a list of known messages. With this function user can add own list of
known fast packet messages.

\note Currently subsequent calls will override previously set list.  

\note If an incoming fast packet message is not known, it will be treated as single frame
message. So if you want to handle unknown fast packet message, you need to duplicate frame collection
logic from library to your code. So it is easier to have fast packet messages listed on library,
if you want to handle them.

##### ExtendTransmitMessages

System should respond to PGN 126464 request with messages the system transmits and receives. The
library will automatically respond with system the messages it uses. With this method you can
add messages, which your own code sends

\note that this is valid only for device modes \ref tNMEA2000::N2km_NodeOnly and \ref tNMEA2000::N2km_ListenAndNode.

##### ExtendReceiveMessages

Method is like \ref tNMEA2000::ExtendTransmitMessages, but extends messages you code handles.

##### SendIsoAddressClaim

This is automatically used by class. You only need to use this, if you want to write your own
behavior for address claiming.

##### SendProductInformation

This is automatically used by class. You only need to use this, if you want to write your own
behavior for providing product information.

##### SendConfigurationInformation

This is automatically used by class. You only need to use this, if you want to write your own
behavior for providing configuration information.

##### SetHeartbeatInterval

According to document [20140102 nmea-2000-126993 heartbeat pgn corrigendum.pdf](https://web.archive.org/web/20170609023206/https://www.nmea.org/Assets/20140102%20nmea-2000-126993%20heartbeat%20pgn%20corrigendum.pdf) all NMEA devices shall
transmit heartbeat PGN 126993. With this function you can set transmission interval in ms
(range 1000-655320 ms, default 60000). Set <1000 to disable it. You can temporary change
interval by setting SetAsDefault parameter to false. Then you can restore default interval
with interval parameter value 0xfffffffe

##### GetHeartbeatInterval

Heartbeat interval may be changed by e.g. MFD by group function. I have not yet found if a
changed value should be saved for next startup or not like address.

##### SendHeartbeat

Library will automatically send heartbeat, if interval is >0. You can also manually send it
any time or force sent, if interval=0;

##### SetMode

With SetMode you can define how your node acts on N2k bus. See \ref tNMEA2000::Devices modes.
With this function you can also set default address for your device. It is mandatory
that once your device has been connected to the bus, it tries always use last used address.
Due to address claiming, your device may change its address, when you add new devices to the bus.
So you should save last used address to the e.g. EEPROM and on startup read it there and use
it as parameter for SetMode. You can check if your address you set originally by SetMode
has changed by using function \ref tNMEA2000::SetN2kSource and you can read current address by
function \ref tNMEA2000::GetN2kSource.  

So you could do next:

```cpp
  void setup() {
  ...
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,GetLastSavedN2kAddressFromEEPROM());
  ...
  }

  ...
  void loop() {
    SendN2kTemperature();
    NMEA2000.ParseMessages();
    if (NMEA2000.ReadResetAddressChanged() ) {
      SaveN2kAddressToEEPROM(GetN2kSource());
    }
  }
```

See example TemperatureMonitor.ino.

##### SetForwardType

With this function user can set how messages will be forwarded to the stream. Possible values are:

- tNMEA2000::fwdt_Actisense (default) forwards messages is Actisense format. Some navigation softwares
  can read this format.
- tNMEA2000::fwdt_Text  forwards messages to output port in clear text. I see this useful only for testing
  with normal serial monitors.

##### SetForwardStream

As default, forward stream has been set to null. For e.g. Arduino Due you can set it to SerialUSB,
so you can use Serial for other things. You can of coarse use any stream available on your device.  
See example ActisenseListenerSender.ino.

##### Open

You can call this on Setup(). It will be called anyway automatically by first call of ParseMessages().

##### SendMsg

When you want to send some message to the N2k bus, you call this. Before calling you have to prepare
tN2kMsg type of message e.g. by using some function in N2kMessages.  

\note As default tNMEA2000 object is as default in tNMEA2000::N2km_ListenOnly mode. So if
you want to send messages, you have to set right mode in Setup().

The function returns true, if the message was sent successfully, otherwise it return false. SendMsg may fail,
if there is not room for message frames on sending buffer or device is not open.  
SendMsg does not always send message immediately. If lower level sending function fails, SendMsg
will buffer message frames and try to send them on next call to SendMsg or ParseMessages. So
to have reliable sending, you need a sending buffer, which is large enough.  
See example TemperatureMonitor.ino.

##### ParseMessages

You have to call this periodically on loop(), otherwise tNMEA2000 object will not work at all.

\note It is not good practice to have any delay() on your loop(), since then also handling of
this will be delayed.

See example TemperatureMonitor.ino.

##### SetMsgHandler

If you want to do something with messages read from N2k bus, easiest is to set message handler,
which will be then called by ParseMessages, if there are new messages. This is the case e.g. if
you have LCD display on your Arduino and you want to show some fluid level on it.  
See example DataDisplay.ino or DataDisplay2.ino

##### AttachMsgHandler

SetMsgHandler allows you to define only one handler to your system. If you like to do it by
using classes, I prefer to use AttachMsgHandler. In this way you can e.g. define own class for
each PGN and attach/detach them within your program. Example NMEA2000ToNMEA0183 uses AttachMsgHandler.
Due to logic it still has single class and so handles all PGNs.

##### DetachMsgHandler

With DetachMsgHandler you can remove your handler from the handler stack. This is useful,
if you do not want to handle some messages anymore.

##### SetISORqstHandler

Devices on N2k bus may request from your device if it can handle requested PGN. If you want to
respond for ISO request, you should set this handler. The handler will be called by ParseMessages,
if there is ISO request.  

\note When you send request message with SendMsg and it fails, it is your responsibility
to take care of sending response again later. If your sending buffer is large enough, it
is very uncommon that SendMsg fails.

##### GetN2kSource

With this function you can get you device current address on the N2k bus.  
See \ref tNMEA2000::SetMode and \ref tNMEA2000::SetN2kSource

##### SetN2kSource

With this function you can set you device current address on the N2k bus. This is meant to be
use for multi device on basic configuration to restore source address changed by address claiming.  
See \ref tNMEA2000::SetMode and \ref tNMEA2000::SetN2kSource

##### ReadResetAddressChanged

With this function you can check has your device address you initiated with SetMode been
changed after last call. For certified NMEA 2000 devices it is mandatory save changed address
to e.g. EEPROM, for use in next startup.  
See \ref tNMEA2000::SetMode and \ref tNMEA2000::SetN2kSource

##### ReadResetDeviceInformationChanged

With this function you can check has your device device instances or system instances changed.
For certified NMEA 2000 devices it is mandatory save changed info to e.g. EEPROM, for
initialize them in next startup.  

See \ref tNMEA2000::SetDeviceInformationInstances and \ref tNMEA2000::GetDeviceInformation

##### EnableForward

Set true as default. With this you can control if bus messages will be forwarded to forward
stream.  

see \sa Message forwarding.

##### SetForwardSystemMessages

Set true as default. With this you can control if system messages like address
claiming, device information will be forwarded to forward stream.  
If you set this false, system messages will not be forwarded to the stream.

##### SetForwardOnlyKnownMessages

Set false as default. With this you can control if unknown messages will be forwarded to
forward stream. If you set this true, all unknown message will not be forwarded to the stream.

\note This does not effect for own messages. Known messages are listed on library.  

See \ref tNMEA2000::SetSingleFrameMessages, \ref tNMEA2000::SetFastPacketMessages,
\ref tNMEA2000::ExtendSingleFrameMessages and \ref tNMEA2000::ExtendFastPacketMessages.

##### SetForwardOwnMessages

Set true as default. With this you can control if messages your device sends to bus will be
forwarded to forward stream.

##### SetHandleOnlyKnownMessages

Set false as default. With this you can control if unknown messages will be handled at all.
Known messages are listed on library.

See \ref tNMEA2000::SetSingleFrameMessages, \ref tNMEA2000::SetFastPacketMessages,
\ref tNMEA2000::ExtendSingleFrameMessages and \ref tNMEA2000::ExtendFastPacketMessages.

##### SetDebugMode

If you do not have physical N2k bus connection and you like to test your board without
even CAN controller, you can use this function.  
see \sa Debug mode.
