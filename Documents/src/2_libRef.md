# Reference for C++ library {#pgLibRef}

\tableofcontents

## Introduction

NMEA2000 library is object oriented C++ library for developing NMEA2000 bus devices. Library fulfills automatically NMEA 2000 mandatory requirements (see \ref secRefNMEA2000Certification) leaving only interesting data handling for developer. **Library has been used in several commercial certified NMEA2000 devices.**

To use NMEA2000 library you need basic skills for programming. For beginners I have tried to write simple instructions to start from scratch with Arduino IDE. When I started library development and even I had been doing sw and hw development for years, it took some time to dig information from internet to get started with Arduino based boards and do even simple tasks like installing library.

For using NMEA2000 library I prefer Teensy 4.0, 4.1, (or 3.2, 3.5, 3.6 which are end of life) or ESP32 with isolated ISO1050 or unisolated MCP2562 chip. For experienced users and big projects RPi can be also used.

For simple things Arduino Mega with CANBUS shield or schematics found
under [documents](https://github.com/ttlappalainen/NMEA2000/tree/master/Documents) is
ok. I have not tested smaller boards and I prefer to forget any board less than 8 kB RAM.

## History

I started library development 2015 because I wanted to get rid of limitations of expensive devices on market. Also those did not had features I wanted. With my first own device for my yacht, I replaced three devices – NMEA0183 combiner, NMEA0183->NMEA2000 Converter, NMEA2000->PC converter - with single Arduino Due board and few extra chips.

On 2017 library was chosen for first commercial certified device. Later there has been several other commercial devices using my library. That has also made it important to keep it combatible and ready for certificated devices.

Since 2015 my personal work has moved more and more towards NMEA2000. I do commercially different NMEA2000 projects and consulting.

## Start from scratch and make your first NMEA2000 device with Arduino IDE

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

- Open example TemperatureMonitor.ino sketch, which came with NMEA2000 library. Select from
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

#### Change the device and product information

On NMEA2000 bus each device will tell to the bus what it is (see also \ref secRefTermNAME). This can be found on sample under setup() function. Call SetDeviceInformation is important for right
functionality on the bus. If you have only this own device on the bus, you can leave
it like it is. In other case take deeper look in to the explanation for the function.
Note that if you add several own devices to the bus, you have to configure parameters
for this function so that each device will get unique \ref secRefTermNAME as a combination of
these parameters.

Call SetProductInformation is not so important for own boat, but defines nice to know information. If you have some “Multi Function Display” (MFD) on the bus, it will show on the
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

WindMonitor.ino is similar as TemperatureMonitor, but sends wind data to the bus. Only necessary
function names has been changed. So follow the “Try NMEA2000 library with
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

The tNMEA2000 class contains functions for NMEA2000 handling like e.g. sending and reading messages. This is purely an abstract class and to use a real device you must use an appropriate inherited class. Normally you use functions within this class.

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
  #include <NMEA2000_CAN.h>
```

"#include <NMEA2000_CAN.h>" will then automatically select right library according to the board you have selected on IDE (see \ref USE_N2K_CAN). If you want strictly control includes or the board will not be selected right by NMEA2000_CAN.h, use includes described on each inherited class or force selection with define before include. E.g.

```cpp
  #include <Arduino.h>
  #define USE_N2K_CAN 1  // Force mcp_can
  #include <NMEA2000_CAN.h>
```

\note You need to download hardware specific inherit classes separately. See \ref secHWlib

##### Inherited class tNMEA2000_mcp

The tNMEA2000_mcp is class for using library with CANBUS shield or according to schematic
[ArduinoMega_CAN_with_MCP2515_MCP2551.pdf](https://github.com/ttlappalainen/NMEA2000/blob/master/Documents/ArduinoMega_CAN_with_MCP2515_MCP2551.pdf), which I have made for sample.  
To use this class, you need to include  in your project beginning:

```cpp
  #include <SPI.h>
  #include <mcp_can.h> 
  #include <NMEA2000_mcp.h>
  #define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
  #define N2k_CAN_INT_PIN 21 // Use interrupt  and it is connected to pin 21
  tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN);
```
If you use automatic library selection, you need to any define CS pin (default 53) and interrupt pin (default 21), if they differ from defaults.

```cpp
  #include <Arduino.h>
  #define N2k_SPI_CS_PIN 52  // Pin for SPI Can Select
  #define N2k_CAN_INT_PIN 10 // Use interrupt  and it is connected to pin 21
  #include <NMEA2000_CAN.h>
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
  #include <NMEA2000_due.h>
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
  #include <NMEA2000_teensy.h>
  tNMEA2000_teensy NMEA2000;
```

##### Inherited class tNMEA2000_Teensyx

The tNMEA2000_Teensyx class is for using library with Teensy 3.2, 3.5, 3.6, 4.0, 4.1 boards, which has internal
can bus controller. Physical interface is similar as with Arduino Due.  
To use this class, you need to include  in your project beginning:

```cpp
  #include <NMEA2000_Teensyx.h>
  tNMEA2000_teensy NMEA2000;
```

At the moment automatic automatic library selection uses as default NMEA2000_teensy.h for
Teensy 3.2, 3.5, 3.6. NMEA2000_Teensyx works reliably with old boards too so you can force
it for automatic library selection

```cpp
  #include <Arduino.h>
  #define USE_NMEA2000_TEENSYX_FOR_TEENSY_3X // Force NMEA2000_TEENSYX also for Teensy 3.x boards
  #include <NMEA2000_CAN.h>
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

#### Device list collector class tN2kDeviceList

This class can be used to read devices on the connected NMEA 2000 bus. When class has been attached to tNMEA2000 object, it will automatically collect and update information of devices on the bus. On the NMEA 2000 bus all devices (also called nodes) has own source address. The source addressmay be changed due to \ref secRefTermAddressClaiming "address claiming" on power up or when new device will be added to the bus. For this reason source address can not be used as only filter for similar messages like position data. By using tN2kDeviceList source address related to \ref secRefTermNAME can be easily found at any time with tN2kDeviceList functions.


In principle on steady system source will stay unchanged. The safer way is to use device \ref secRefTermNAME which should be unique. So if you e.g., have two speed logs and you want to specify the one you want to use, you can use tN2kDeviceList class to search source address for specified NAME and then read log messages from that source.

See also example DeviceAnalyzer.ino.

### Using tNMEA2000

There are several examples for using library with different functions like for only listen bus
data (ActisenseListener.ino), sending temperature (TemperatureMonitor.ino) or wind data
(WindMonitor.ino) from your own sources or displaying bus data somehow (DisplayData.ino
and DisplayData2.ino).

Library core has all functionalities to communicate with NMEA2000 bus and N2kMessages.h has simple functions to read and set most common messages. And you can add own modules to support missing messages. Rest is up to your imagination.

Below is short description of member functions, which hopefully gives you better knowledge
why something has been used on samples.

#### Device modes {#secDeviceModes}

For historical reasons library offers several message handling modes. One idea was that CAN controller specific drivers could drop some data handling, but it is not implemented on any of them. Practically only modes
- N2km_ListenAndNode for real bus devices.
- N2km_ListenAndSend for message pass trough devices.

would be enough. 

"Listen" in mode definition refers to message forwarding. Mode definitions without "Listen" passes forward functionality. On the other hand forwarding can be enabled or disabled with tNMEA2000::EnableForward(), which then makes other modes less necessary. Also mode can not be changed after open, but forward can be enabled or disabled at any time - this makes listen modes be more usefull. And last since drivers does not limit messages by mode message handlers will be called anyway allowing you to control messages in your code and even build own "message forwarding".

Although there should be no reason to use other than N2km_ListenAndNode or N2km_ListenAndSend modes, below is table of different modes effect.
 - Tx = messages can be sent
 - Rx = messages can be received
 - Dev = acts as active bus device. Has full functionality like address claiming, heartbeat, response to requests.
 - Fen = Message forwarding - tNMEA2000::EnableForward() does have effect
 - S = Setting tNMEA2000::SetForwardSystemMessages does have effect.

 (x) means that feature is not limited by core library, but could have limit on "driver".

| Mode               |  Tx |  Rx | Dev | Fen |  S  |  
|--------------------|:---:|:---:|:---:|:---:|:---:|
| N2km_NodeOnly      |  x  |  x  |  x  |     |     |
| N2km_ListenAndNode |  x  |  x  |  x  |  x  |  x  |
| N2km_ListenAndSend |  x  |  x  |     |  x  |     |
| N2km_ListenOnly    |     |  x  |     |  x  |  x  |
| N2km_SendOnly      |  x  | (x) |     |     |     |

NMEA2000 requires that all devices should act as active device (also called node) on the NMEA2000 bus. Total electrical bus load can be then calculated and there will not be any hidden devices. If you take care that bus load will not be exceeded, it should not be risky to make listen only devices for you own boat.

You can set your device mode on setup with function tNMEA2000::SetMode(). See also tNMEA2000::tN2kMode .

#### Message forwarding {#secMessageforwarding}

Message forwarding offers simple way to forward messages to defined stream in Actisense format. I build this functionality inside library to easily analyze bus messages even device does have its own functionality like temperature sensor. Now when I have mostly used ESP32 with WiFi, I can enable forwarding on the fly to UDP stream for any device and use it as analyzer.

In any listen mode, the device will read all messages from N2k bus and forwards them to the defined forward stream. For forwarding you just define a forward stream - e.g., serial port, UDP stream - with function \ref tNMEA2000::SetForwardStream and enable forward with \ref tNMEA2000::EnableForward(true). Naturally you also need to open a stream first e.g. with Serial.begin(115200);

Messages will be forwarded as default in Actisense format. This format is supported by at least some PC chart plotter applications. You can show Actisence format messages with “NMEA Reader”, which I refer time to. Best program for bus data analyzing is my NMEA Simulator <http://www.kave.fi/Apps/>. For data visualizer you can use e.g., OpenSkipper, on which you can tailor your own displays.  

If message forwarding is enabled, own messages send with tNMEA2000::SendMsg will be forwarded as default even when your device has been set to node only mode. If this disturbs your developing, if you e.g. want to write own clear text messages within your code, you can disable that by either:

- \ref tNMEA2000::EnableForward(false) to disable forwarding totally
- \ref tNMEA2000::SetForwardOwnMessages(false) to disable own message forwarding. In listen mode messages from bus will be still forwarded.

\sa
  - \ref secDeviceModes
  - \ref tNMEA2000::SetForwardStream
  - \ref tNMEA2000::EnableForward
  - \ref tNMEA2000::SetForwardOwnMessages
  - \ref tNMEA2000::SetForwardOnlyKnownMessages
  - \ref tNMEA2000::SetForwardSystemMessages.
  
#### Debug mode {#descDebugMode}

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
Device) and function 132 (Analog to NMEA 2000 Gateway). Anyway I have found that e.g.
B&G Vulcan 7 chartplotter acts as multi device. I do not know are these devices different
chips inside Vulcan or are they within same code as my multi device.  
If you like show different functions as own device on the bus, you can do that with multi
device support. All you need to do is to first set device count (see tNMEA2000::SetDeviceCount), set
device and product information for each device and on sending messages use right device
index. See example MultiDevice.

#### Running library {#descRunningLibrary}

Library has been designed to work without need for multitasking system. After setup it is critical to call tNMEA2000::ParseMessages in loop as fast as possible. By that call library handles many basic required features like address claiming, responding to system requests, heartbeat sending etc.

Fast loop requirement means that you are not allowed to use any delay on your loop or any other library using delay or blocking read. Delays withing loop in worst case causes that other devices on your NMEA2000 bus drops your device from the list and then pops it up again. If you have e.g., configured your MFD to show temperature from your device, it may appear and disappear on the screen.

A practice has shown that random 10-50 ms delay is acceptable. In average loop time should be
less than 2 ms. Also it is important that if you can have up 50 ms random delay, you may get in burst up to 90 frames (=1800 frames/s *0.05 s) during that time. This means that if your receive frame buffer is smaller, your device may loose some critical system messages. In small boat this amount is a bit theory, but anyway there are a lot of large messages just from GPS system so that they may occur time to time at same time. So it is better to prepare your device work in nearly any condition.

\sa
 - \ref tNMEA2000::ParseMessages
 - \ref tNMEA2000::SetN2kCANReceiveFrameBufSize()
 - \ref tNMEA2000::SetN2kCANSendFrameBufSize()

##### Some timing examples

If you use DallasTemperature library as default you may block loop up to 700 ms. By using it "asynchronously", delays are smaller and may be acceptable. Best solution would be to use some kind of hardware based library like esp32-owb for ESP32.

Some ADC libraries blocks loop during conversion, which may be 200-400 ms for high resolution ADC with averaging. ADC:s should be always used so that conversion will be started and then quickly checked, when conversion is ready. Even better if you know conversion time and check it after you expect it should be ready.

Some displays are very slow for writing. Writing some text may take 100 ms causing too long delay and problems with bus. Depending of library, it may also have some kind of buffering to get write request and flag, when it has been finished without blocking call. If there is not, it is often possible write own higher level buffering, which finally writes to display letter by letter and so spent only short time on each call.

With serial line use always available and availableForWrite features to avoid blocking. Also it helps if you can define bigger harware buffers.

In multitasking system someone had defined tNMEA2000::ParseMessages task time to 100 ms. I do not wonder, if there will random strange problems with that device. 1 ms task time would be acceptable.

##### System messages and library internal functionality {#descSystemMessages}

Library has been designed to do automatically as many required features as possible. In this way user can concentrate to his own code and does not need to know everything about NMEA2000 low level. There is several system messages, which are handled automatically and user does not need to take care of.

- PGN 59392 ISO Acknowledgement.
  + Library responds with NAK for unhandled requests.
  .
- PGN 59904 ISO Request.
  + Library handles ISO request for system messages.
  + For other messages library calls ISORequestHandler. See tNMEA2000::SetISORqstHandler.
  .
- PGN 60928 ISO Address Claim.
  + Library starts address claiming procedure automatically and handles it completely.
  + Library responds as required to others address claiming, if necessary. See also tNMEA2000::ReadResetAddressChanged.
  + Library handles normal requests and group function requests to change device or system instances. See also tNMEA2000::ReadResetDeviceInformationChanged.
  + If you need to track devices on the bus e.g., to tie some message to specific device there is automatic module tN2kDeviceList to do work for you.
  .
- PGN 60416 and PGN 60160 ISO Transport Protocol messages.
  + Library hadles automatically message sending and receiving with ISO TP.
  .
- PGN 126208 Group Function message.
  + Library does default handling for system messages.
  + For other messages library forwards handling to class inherited from tN2kGroupFunctionHandler. See tNMEA2000::AddGroupFunctionHandler.
- PGN 126464 Receive/Transmit PGN list. 
  + Library responds automatically for requested PGN lists. User has to take care that own PGNs has been declared at startup with tNMEA2000::SendTxPGNList and tNMEA2000::SendRxPGNList.
- PGN 126993 Heartbeat.
  + Library takes care of heartbeat sending.
  + Library also takes care of heartbeat offset/period change requests.
- PGN 126996 Product information.
  + Library responds automatically to product information requests. User has to setup product information at device start with tNMEA2000::SetProductInformation.
- PGN 126998 Configuration information.
  + Library responds automatically to configuration information requests. User has to setup configuration information at device start with tNMEA2000::SetConfigurationInformation.
  + Library also updates Installation Description 1 and 2 on request. See also tNMEA2000::ReadResetInstallationDescriptionChanged.

#### Member functions

=======================================
##### tNMEA2000::SetProductInformation

\copybrief tNMEA2000::SetProductInformation
\copydetails tNMEA2000::SetProductInformation

=======================================
##### tNMEA2000::SetDeviceInformation

\copybrief tNMEA2000::SetDeviceInformation
\copydetails tNMEA2000::SetDeviceInformation

=======================================
##### tNMEA2000::SetDeviceInformationInstances

\copybrief tNMEA2000::SetDeviceInformationInstances
\copydetails tNMEA2000::SetDeviceInformationInstances

=======================================
##### tNMEA2000::GetDeviceInformation

\copybrief tNMEA2000::GetDeviceInformation
\copydetails tNMEA2000::GetDeviceInformation

=======================================
##### tNMEA2000::SetConfigurationInformation

\copybrief tNMEA2000::SetConfigurationInformation
\copydetails tNMEA2000::SetConfigurationInformation

=======================================
##### tNMEA2000::SetProgmemConfigurationInformation

\copybrief tNMEA2000::SetProgmemConfigurationInformation
\copydetails tNMEA2000::SetProgmemConfigurationInformation

=======================================
##### tNMEA2000::SetMode

\copybrief tNMEA2000::SetMode
\copydetails tNMEA2000::SetMode

Example how to init and save device source address.

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

=======================================
##### tNMEA2000::ExtendTransmitMessages

\copybrief tNMEA2000::ExtendTransmitMessages
\copydetails tNMEA2000::ExtendTransmitMessages

=======================================
##### tNMEA2000::ExtendReceiveMessages

\copybrief tNMEA2000::ExtendReceiveMessages
\copydetails tNMEA2000::ExtendReceiveMessages

=======================================
##### tNMEA2000::SetOnOpen

\copybrief tNMEA2000::SetOnOpen
\copydetails tNMEA2000::SetOnOpen

=======================================
##### tNMEA2000::SendMsg

\copybrief tNMEA2000::SendMsg
\copydetails tNMEA2000::SendMsg

=======================================
##### tNMEA2000::ParseMessages

\copybrief tNMEA2000::ParseMessages
\copydetails tNMEA2000::ParseMessages

=======================================
##### tNMEA2000::SetMsgHandler

\copybrief tNMEA2000::SetMsgHandler
\copydetails tNMEA2000::SetMsgHandler

=======================================
##### tNMEA2000::AttachMsgHandler

\copybrief tNMEA2000::AttachMsgHandler
\copydetails tNMEA2000::AttachMsgHandler

=======================================
##### tNMEA2000::DetachMsgHandler

\copybrief tNMEA2000::DetachMsgHandler
\copydetails tNMEA2000::DetachMsgHandler

=======================================
##### tNMEA2000::SetISORqstHandler

\copybrief tNMEA2000::SetISORqstHandler
\copydetails tNMEA2000::SetISORqstHandler

=======================================
##### tNMEA2000::GetN2kSource

\copybrief tNMEA2000::GetN2kSource
\copydetails tNMEA2000::GetN2kSource

=======================================
##### tNMEA2000::SetN2kSource

\copybrief tNMEA2000::SetN2kSource
\copydetails tNMEA2000::SetN2kSource

=======================================
##### tNMEA2000::ReadResetAddressChanged

\copybrief tNMEA2000::ReadResetAddressChanged
\copydetails tNMEA2000::ReadResetAddressChanged

=======================================
##### tNMEA2000::ReadResetDeviceInformationChanged

\copybrief tNMEA2000::ReadResetDeviceInformationChanged
\copydetails tNMEA2000::ReadResetDeviceInformationChanged

=======================================
##### tNMEA2000::ReadResetInstallationDescriptionChanged

\copybrief tNMEA2000::ReadResetInstallationDescriptionChanged
\copydetails tNMEA2000::ReadResetInstallationDescriptionChanged

=======================================
##### tNMEA2000::SetDeviceCount

\copybrief tNMEA2000::SetDeviceCount
\copydetails tNMEA2000::SetDeviceCount

=======================================
##### tNMEA2000::SetN2kCANMsgBufSize

\copybrief tNMEA2000::SetN2kCANMsgBufSize
\copydetails tNMEA2000::SetN2kCANMsgBufSize

=======================================
##### tNMEA2000::SetN2kCANSendFrameBufSize

\copybrief tNMEA2000::SetN2kCANSendFrameBufSize
\copydetails tNMEA2000::SetN2kCANSendFrameBufSize

=======================================
##### tNMEA2000::SetN2kCANReceiveFrameBufSize

\copybrief tNMEA2000::SetN2kCANReceiveFrameBufSize
\copydetails tNMEA2000::SetN2kCANReceiveFrameBufSize

=======================================
##### tNMEA2000::SetSingleFrameMessages

\copybrief tNMEA2000::SetSingleFrameMessages
\copydetails tNMEA2000::SetSingleFrameMessages

=======================================
##### tNMEA2000::SetFastPacketMessages

\copybrief tNMEA2000::SetFastPacketMessages
\copydetails tNMEA2000::SetFastPacketMessages

=======================================
##### tNMEA2000::ExtendSingleFrameMessages

\copybrief tNMEA2000::ExtendSingleFrameMessages
\copydetails tNMEA2000::ExtendSingleFrameMessages

=======================================
##### tNMEA2000::ExtendFastPacketMessages

\copybrief tNMEA2000::ExtendFastPacketMessages
\copydetails tNMEA2000::ExtendFastPacketMessages

=======================================
##### tNMEA2000::SendIsoAddressClaim

This is automatically used by class. You only need to use this, if you want to write your own
behavior for address claiming.

=======================================
##### tNMEA2000::SendProductInformation

This is automatically used by class. You only need to use this, if you want to write your own
behavior for providing product information.

=======================================
##### tNMEA2000::SendConfigurationInformation

This is automatically used by class. You only need to use this, if you want to write your own
behavior for providing configuration information.

=======================================
##### tNMEA2000::SetHeartbeatIntervalAndOffset

\copybrief tNMEA2000::SetHeartbeatIntervalAndOffset
\copydetails tNMEA2000::SetHeartbeatIntervalAndOffset

=======================================
##### tNMEA2000::GetHeartbeatInterval

\copybrief tNMEA2000::GetHeartbeatInterval
\copydetails tNMEA2000::GetHeartbeatInterval

=======================================
##### tNMEA2000::SendHeartbeat

Library will automatically send heartbeat, if interval is >0. You can also manually send it
any time or force sent, if interval=0;

=======================================
##### tNMEA2000::SetForwardType

\copybrief tNMEA2000::SetForwardType
\copydetails tNMEA2000::SetForwardType

=======================================
##### tNMEA2000::SetForwardStream

\copybrief tNMEA2000::SetForwardStream
\copydetails tNMEA2000::SetForwardStream

=======================================
##### tNMEA2000::Open

\copybrief tNMEA2000::Open
\copydetails tNMEA2000::Open

=======================================
##### tNMEA2000::EnableForward

\copybrief tNMEA2000::EnableForward
\copydetails tNMEA2000::EnableForward

=======================================
##### tNMEA2000::SetForwardSystemMessages

\copybrief tNMEA2000::SetForwardSystemMessages
\copydetails tNMEA2000::SetForwardSystemMessages

=======================================
##### tNMEA2000::SetForwardOnlyKnownMessages

\copybrief tNMEA2000::SetForwardOnlyKnownMessages
\copydetails tNMEA2000::SetForwardOnlyKnownMessages

=======================================
##### tNMEA2000::SetForwardOwnMessages

\copybrief tNMEA2000::SetForwardOwnMessages
\copydetails tNMEA2000::SetForwardOwnMessages

=======================================
##### tNMEA2000::SetHandleOnlyKnownMessages

\copybrief tNMEA2000::SetHandleOnlyKnownMessages
\copydetails tNMEA2000::SetHandleOnlyKnownMessages

=======================================
##### tNMEA2000::SetDebugMode

\copybrief tNMEA2000::SetDebugMode
\copydetails tNMEA2000::SetDebugMode
