# Changes to the Library {#changes}
\tableofcontents

## 09.02.2024

- Fix multiple spelling errors in comments. Thanks to garaminowicz.

## 06.02.2024

- Fix tPriorityRingBuffer. It did not freed values properly. tPriorityRingBuffer is used on tNMEA2000_Teensyx driver.
- Ring buffer documentation update.
- Fixed SetN2kPGN129284 and ParseN2kPGN129284. Changed OriginWaypointNumber and DestinationWaypointNumber parameter to uint32_t. This causes compatibility issue for users used ParseN2kPGN129284 and they have to update reference parameter type. Also if SetN2kPGN129284 was used with related parameters value N2kUint8NA, they must be updated to use N2kUint32NA
- Moved OnOpen call on Open after setting Heartbeat interval and offset. If Heartbeat settings were set on OnOpen, settings were reset.
- Added note to documentation for OnOpen about coming change.
- Added lightning PGNs to fastpacket list.

## 02.02.2024

- Document update. Updates on document sources and code sources.

## 22.01.2024

- Fixed compiler error in case #define N2K_NO_HEARTBEAT_SUPPORT 1 has been used.
- Fixed priority setting test on tN2kGroupFunctionHandler::HandleCommand and tN2kGroupFunctionHandlerForPGN126998::HandleCommand
- Fixed Mode setting on SetN2kPGN129540

## 21.01.2024

- Document update. Updates on document sources and code sources.

## 13.01.2024

- Fixed PGN129539 reader and parser.
- Added #include "N2kTypes.h" to N2kMaretron.h
- Fixed PGN130316 default period information
- Added missing methods to tRingBuffer template
- Updated copyright

## 08.12.2023

- Added entertainment fastpacket PGNs to fastpacket list.
- Added electric fastpcket PGNs 127490, 127491, 127494, 127495 to fastpacket list.
- Added elevator fastpacket PGN 128538 to fastpacket list.
- Moved FindSourceDeviceIndex to public
- Added IsOpen
- Added GetForwardStream
- Document update by trixpan
- Added fluid type enum to N2kMessagesEnumToStr.h
- Added support for NMEA2000_ArduinoCAN to NMEA2000_CAN.h by jboynes

## 04.10.2023

- Fix use N2kMillis instead of millis on N2kDeviceList by nicklasb.

## 12.09.2023

- Fixed priority setting on N2kGroupFunction tN2kGroupFunctionHandler::ParseCommandParams
- Fixed inline void SetN2kPGN129285 on N2kMessages.h to avoid conflict
- Fixed readNHexByte on SeaSmart.cpp to avoid warning.
- Restored dropped GetManufacturerInformation() definition to tNMEA2000::tDevice
- Fix comment for 127233L on NMEA2000.cpp

## 25.07.2023

- Changed INT32_MAX instead of LONG_MAX on N2kTimer.h by Sam Meredith
- CMakeLists update by phatpaul
- Cleaner CMakeLists.txt by Paul Abbott
- Example ActisenseListener update

## 20.05.2023

- Example N2kDataToNMEA0183 update by Andrei Errapart

## 19.05.2023

- Catch update to version 2.13.10 by Andrei Errapart
- Fix N2kMessages/ ParsePGN129029 initialize reference station variables t NA, if no reference station by Andrei Errapart
- CMakeLists compatible as ESP-IDF by Paul Abbott
- N2kCANMsg.h: Changed #include <N2kMSG.h> to use quotes.

## 08.02.2023

- Fixed priority setting on SetN2kPGN130577.
- Fixed "set temperature" field to unsigned on ParseN2kPGN130316 and SetN2kPGN130316.

## 01.02.2023

- Copyright update.
- Document update. Still some lost NMEA document.

## 21.12.2022

- Creating Doxygen based documentation for the library. There will be more documentation updating in future, 
  when I have time to read it through. I also encourage users to read documentation and fix errors, spelling, grammar.
  I prefer to make pure documentation update PRs to keep them separate from new features. +
  Great thanks for matthias-mv for excellent and big job!

## 27.11.2022

- Updated test and added missing source file, thanks to PeterFromSweden.
- Fixed tN2kBatChem - Added ZnO, noted by matthias-mw
- Added optional SID to SetN2kPGN129039, noted by matthias-mw
- Added N2kts_ShaftSealTemperature, noted by gorlanovmax

## 27.10.2022

- *Compatibility change for PGN 129794 setter function:* Replaced tN2kAISTranceiverInfo with tN2kAISTransceiverInformation.
  tN2kAISTranceiverInfo was duplicate for tN2kAISTransceiverInformation. It has been also removed from N2kTypes.h.
- *Compatibility change for PGN 129038 setter function:* Added tN2kAISTransceiverInformation as parameter. I pop up that
  at least some MFD:s does not show class A AIS objects, if transceiver information has not been properly set. +
  Thanks to usauerbrey.
  
## 26.10.2022

- Version confirmed to pass NMEA2000 certification tests.
- Empty incoming frames before starting address claiming on Open.
- Fix: Open returned always false.
- Fix: Group function request turn off transmission for heartbeat (PGN 126993) did no nack.
- Added possibility to select CAN alternative pins for Teensyx.

## 03.10.2022

- *Possible compatibility change:* tN2kGroupFunctionHandler::GetRequestGroupFunctionTransmissionOrPriorityErrorCode
  interface has changed. If you have used that, please update interface.
- *Possible compatibility change:* Removed need of delay function on tNMEA2000::Open(). Now it opens CAN commununication
  and returns true, if that and other configuration succeeds. Final open, when address claiming starts, will be delayed
  and done by ParseMessages. There is new callback OnOpen, which will be called on initial address claiming started. See
  <<SetOnOpen,below>>
- *Possible compatibility change:* For linux users removed default dummy tSocketStream serStream; from NMEA2000_CAN.h
- *Compatibility change:* PGN129285 parameter list SetN2kPGN129285 and SetN2kRouteWPInfo has been changed.
  Compiler could not solve ambiguous problem between compatibility inlines, so users are forced to change
  to new interface.
- Added [[SetOnOpen]]tNMEA2000::SetOnOpen to set callback for communication open. You need this e.g., if you want to
  synchronize your messages with offsets. See how it has been used e.g., on exmample TemperatureMonitor.
- New module N2kTimer to hide platform dependent millis(). Module declares scheduler classes to
  fix problems on simple millis() testing in case of roll over. Class tN2kSyncScheduler is
  for synchronizing message sending. See how it has been used e.g., on example TemperatureMonitor
- Improved: Single test for pending information (tInternalDevice::HasPendingInformation) to avoid multiple tests in every loop.
- Fix: Heartbeat problem caused by millis() roll over after 49 days.
- Fix: Added missing const modifiers. Thanks alexg-k.
- Fix: Limit SendRxPGNList/SendTxPGNList to avoid crash in case of error.
- Fix: PGN 129285 Waypoint list. Thanks aydosj.
- Added: PGN 127252 Heave. Thanks aydosj.
- Changed some parameter order in classes to save small amount of memory.
- Updated examples TemperatureMonitor, WindMonitor, BatteryMonitor and MessageSender to use tN2kSyncScheduler
  and tNMEA2000::SetOnOpen for synchronizating messages.

## 11.08.2022

- Fix: On N2kGroupFunctionDefaultHandlers.cpp tN2kGroupFunctionHandlerForPGN60928 unique number length is 3 bytes.
- Fix: On N2kMsg.cpp changed round(v/precision) to round((v/precision)), since some system round macro behaved unexpectedly. Thaks to ieb.

## 06.07.2022

- Fix: use proper array delete operator in RingBuffer, thanks garaminowicz
- Fix: PGN 127251 rate of turn type, thanks BerndCirotzki and for remind speters.
- Fix: PGN 129794 setter.

## 07.04.2022

- Changed "#include cstring" to "#include string.h" cstring is not available on all environments.
- Fixed proprietary PGN handling on N2kGroupFunction.cpp.
- Fixed N2kMsg.cpp:SetBuf3ByteDouble(). Thanks to mairas.
- Add support for PGN 129041, 130323. Thanks to MattCairns.
- Added support for PGN127237, 127233, 130577 and more detail for 129039. Thanks to aydosj.
- Copyright update.

## 05.09.2021

- Documentation update.

## 23.08.2021

- Added manufacturer bits to engine discrete status ( PGN 127489 )

- Added data limitation handling to tN2kMsg. Now overflow values will be sent as overflow. E.g.
  SetN2kTemperature(N2kMsg,0xff,0,N2kts_MainCabinTemperature,CToKelvin(800)); +
  will send temperature as 0xfffe, which is NMEA2000 standard way to inform that value is out of range.
  Library does not yet handle overflow on reading, but it is under construction.
- Fixed PGNs 126992, 128275, 129029 time reading to UDouble
- Added function tNMEA2000::Restart(). This is preliminary and may change. Read more info on NMEA2000.h.
- Modified tNMEA2000::RespondGroupFunction to allow user add own common group function handler with PGN=0.
- Fixed tNMEA2000::HandleCommandedAddress which should be accepted only by BAM TP.
- Added tN2kGroupFunctionHandler::ChangeTransmissionOrPriorityErrorCode
- Fixed tN2kGroupFunctionHandlerForPGN126993::HandleRequest. System must not respond to pure 126208 request for 126993.
  Also TransmissionInterval setting is limited to 1000 - 60000 ms.
- Modified debug messages.

## 04.03.2021

- Fix for engine discrete status 2.

## 10.02.2021

- Added definition to NMEA2000_CAN.h to select CAN device for Arduino DUE

## 06.02.2021

- Added handlers for PGN 129540 GNSS satellites in view
- Added new necessary types
- Added SetByte for tN2kMsg class.
- Added PGN 129540 handler for test example DataDisplay2
- Redesigned example MessageSender. Now it is more usefull for testing, since each
  message can be individually enabled/disabled.
- Added PGN 129540 handler for test example MessageSender

## 15.01.2021

- Added better inline ParseN2kEngineDynamicParam to N2kMessages
- Copyright update

## 05.01.2021

- Added better engine discrete status handling to N2kMessages

## 08.10.2020

- Fix for 'memcpy' overflow for float reading on N2kMsg.cpp

## 23.08.2020

- Add support for Teensy 4.x

## 05.08.2020

- Add support for Windlass Network Messages PGN 128776, 128777 & 128778 by Paul Reeve

## 16.07.2020

- Fixed setting buffered frame length to min instead of max length on NMEA2000.cpp SendFrame
- Added NMEA2000::IsProprietaryMessage
- Fixed AddGroupFunctionHandler to allow to add handlers after Open
- Added RemoveGroupFunctionHandler to allow dynamically remove handlers on runtime.
- Added proprietary message handling to N2kGroupFunction.cpp
- Improved N2kDeviceList HandleMsg to see devices, which has been off.
- Added FindDeviceByProduct to N2kDeviceList
- Extented list of fast packet messages to all known fast packet messages. This also leads to situation that all
  fast packet messages are known messages and may effect to setting SetHandleOnlyKnownMessages behaviour. To limit
  known messages, one should have provided list with SetFastPacketMessages.

## 27.06.2020

- Fixed PGN 130314 and PGN 130315 pressure type to signed.
- Added pressure enums.

## 28.01.2019

- NOTE! Compatibility change! PGN 127506 TimeRemaining should have been in seconds as it is SI unit.
  If you have used that on your code, provide value in seconds. Parse function also return in seconds now.
- Added Capacity parameter to PGN 127506. I did not made overwrite function without due to above change.
  So it hopefully wakes you up about the changes.
- Fixed MaretronProprietary test on N2kMaretron.cpp
- Some comment fixes.

## 20.10.2019

- Added support for Maretron proprietary PGNs 130823,65286, 65287. See N2kMaretron.h.
  Thanks to Vassilis Bourdakis.
- Added proprietary fast packet message test as default. So now proprietary fast packet
  messages will be automatically parsed right without need to use ExtendFastPacketMessages
- Fixed strings on product information. Unused characters will be filled with 0xff.

## 17.10.2019

- Fixed Rate of turn (PGN 127251) and AIS position report ROT (PGN 129038) value multiplier.
- Separated N2k enums to own file N2kTypes.h
- Fixed PGNs 127251, 127258, 130576 length for sending by padding with reserved. Some devices refuces
  to listen message, if length is wrong.

## 07.07.2019

- Fixed: PGN 127513 was accidently defined also as single frame message.
- Added: Trip fuel consumption, engine PGN 127497.
- Added: More PGN:s to default fast packet list.
- Updated: Examples MessageSender and DataDisplay2, which can be used for testing messages.

## 03.03.2019

- Added: Charger status PGN 127507.
- Added: Possibility to delay ISO address claim. Due to some devices CAN priority, it was sent too fast.
- Added: Count for tDeviceList

## 14.01.2019

- Fix: Device list handler fix. Some tools may use source 254 and that was checked wrong.
- Fix: Message priorities.

## 04.08.2018

- Fix: NMEA2000.h/NMEA2000.cpp, just in parameter N2kSource type change by mrbubble62

## 18.07.2018

- Added: N2kMessagesEnumToStr.h "exhaust gas" string for temperature sources by mrbubble62.

## 09.05.2018

- Fix: NMEA2000_CAN.h, make ESP32 work with ESP-IDF framework, by Sarfata

## 10.04.2018

- Fix: Handling of humidity PGN 130313.

## 06.04.2018

- Added: Support for ESP32. See also NMEA2000_esp32
- Fix: Changed some names to avoid conflicts with some stupid define macros on some environments.

## 04.04.2018

- Fix: NextHeartbeatSentTime initial value.
- Added: New example NMEA2000ToNMEA0183. This has been tested on RPi3B, Arduino DUE, Arduino Mega, Teensy.

## 26.03.2018

- Added: Strings for Magnetic Variation enum by mrbubble62

## 13.03.2018

- Added: PGN129033 Local offset

## 23.02.2018

- Port config ability for SockeCAN by Al Thomason

## 16.02.2018

- SetN2kSource for other devices by jpilet

## 29.01.2018

- Added: PGN130576 Small Craft Status / Trim Tab Position definition by Nicholas Agro

## 15.01.2018

- Fix: ParseN2kPGN129284, Index was not initialized to 0, which caused unpredictable read.

## 07.01.2018

- Fix: Fast packet sequence counter must be related to PGN.
- Fix: TP message priorities.
- Fix: On SetDeviceInformationInstances we need to send ISO address claim, not start adress claim.
- Fix: Do not respond any queries during address claim.
- Fix: Fixed some messages default priority.
- Fix: Responce to Complex Group Function requests.
- Fix: Now compiles with different compiler definitions defined on NMEA2000_CompilerDefns.h
- Added: Support for sending messages by using Transport Protocol. This is mandatory for NMEA 2000
- Updated: License to 2018

## 02.12.2017

- Changes effect only use of tN2kDeviceList and AttachMsgHandler callbacks.
- Added FindDeviceByIDs to tN2kDeviceList
- Fix: Devicelist did not handle right, if device changed its address higher.
- Fix: PNG message handler order problem on tNMEA2000::AttachMsgHandler(tMsgHandler *_MsgHandler);
- Fix: If there was NMEA2000 library device with same name, they handshaked both addresses to null.
  The problem still appears, if both devices will be started at same time. I need to add some
  random start delay
- Cleaned spaces from end of lines on updated code.

## 29.11.2017

- Added PGN 128000 Leeway to N2kMessages.h/.cpp
- Fix: Spelling WaterRefereced -> WaterReferenced on N2kMessages.h/.cpp

## 27.11.2017

- Fix some build errors due to missing include statements (did not affect Arduino, only other platforms)

## 14.11.2017

- Fix: Default responces to Complex Group Function.

## 09.11.2017

- Added library.json, thanks for ronzeiller.

## 31.10.2017

- Fix: fill unused chars on fastpacket messages with 0xff
- Fix: fill unused chars on message fixed sized strings with 0xff. One sample is e.g. PGN 126996, Product Information.

## 14.10.2017

- Changed tActisenseReader class to handle also Actisense N2k request message types. This type will be used
  by applications, which sends data through NGT-1.
- Updated related examples ActisenseListener and ActisenseListenerSender.
- Renamed Min/Max to N2kMin/N2kMax, since Due code had definitions for Min/Max

## 12.10.2017

- Added PGN validity check to the SendMsg()
- Changed max/min -> own Max/Min. max/min defines does not exist on other systems and Arduino does not have std Algorithm for all boards.

## 10.10.2017

- Fix: PGN 126992 TimeSource handling.
- Fix: Added missing wind reference type.

## 02.10.2017

- Added range parameter for depth PGN 128267.
- Added millis() time stamp to N2kMsg clear text Print.
- Now responces also Group Function request for PGN lists PGN 126464
- Now responces also Group Function request for Product Information PGN 126996
- Now responces also Group Function request for Configuration Information PGN 126998
- Fix: spelling Sertification -> Certification
- Fix: on changing SystemInstance reseted DeviceInstance
- Fix: "Group function" responces according to tests with certified Airmar DST800
- Fix: Address claiming could go up to 253 and did not went to "cannot claim". Now, if address cannot be claimed, goes to "cannot claim state" and prevents all message output except ISO address claim.
- Fix: fast packet response for less than 7 data bytes caused two frames.

## 26.09.2017

- Fixed PGN 128259 parser SOG data type.
- DataDisplay2 example update.

## 31.08.2017

- Support for changing configuration information fields InstallationDescription1 and InstallationDescription2 on runtime e.g. with NMEA Reader.
Meaning of those fields is define their "installation description". So if you have two engine monitor devices, you can set e.g. InstallationDescription1 field to
"Port engine" for one and "Starboard engine" for other. So it is not necessary to hardcode those setting. Of coarse your code must support
parameter saving to e.g. EEPROM as with other parameters (see ReadResetInstallationDescriptionChanged, ReadResetAddressChanged and ReadResetDeviceInformationChanged).
I have example under construction for handling parameter changes.
- Changed some indexes to size_t. This may effect compatibility, if you have used those functions.

## 31.07.2017

- New versions of NMEA2000_due and due_can (see. <https://github.com/ttlappalainen/due_can>)

## 29.07.2017

- Fixed setting device instances on N2kGroupFunctionDefaultHandlers
- Fixed wind PGN 130306 output with reserved field.
- New abstract class tNEMA2000:tMsgHandler and functions AttachMsgHandler/DetachMsgHandler. With these you can have multiple
  handlers. It also allow PGN specific handlers. See how it has been used on example DeviceAnalyzer. Other simple example
  is under construction.
- New class tN2kDeviceList. See more on library reference and on example DeviceAnalyzer.
- Improved message type checking. This will be done for every message, so speed in important. For Arduino Mega average test time
  was dropped from about 90 us to 9 us and for Teensy from 3.5 us to 0.9 us.

## 26.06.2017

Example updates

- ActisenseListenerSender can be used to listen and send data to NMEA 2000 bus.
  This is almost same as TeensyActisenseListenerSender, but read and forward
  ports can be chosen with "#define".
- ActisenseListener uses now SetN2kCANReceiveFrameBufSize.
- Removed FromPCToN2k. ActisenseListenerSender replaces this.

## 25.06.2017

Fix and cosmetic changes

- ForwardStream initialization was accidentaly deleted
- Clean code and more debug options.

## 22.06.2017

Fix and cosmetic changes

- Crashed, if ForwardStream was not defined. I accidentaly forgot to comment
  some debug code.
- Definition of tDeviceInformation changed to fixed sized data so that compiler
  can not mix them.
- Added debug definitions to avoid first bug.
- Some cosmetic changes and tests.

## 19.06.2017

Changes due to different revisions of FlexCAN library for Teeansy
boards. NOTE! You must update NMEA2000_Teensy library.
I also forked and developed FlexCAN library from collin80 and also send pull
request for him. Until updated there my fork has more features for use with
NMEA2000 library.

## 13.06.2017

NOTE! Some compatibility changes

- !NOTE compatibility change. `tProductInformation` has been moved inside
  `tNMEA2000` class. If you have defined `tProductInformation` to `PROGMEM` as
  in example `BatteryMonitor`, you need to change definition `const
  tProductInformation`... to `const tNMEA2000::tProductInformation`...  See
  example `BatteryMonitor`.
- Multi device support should work now. So you can show several devices on bus
  with single hw. See example MultiDevice.
- !NOTE compatibility change. tDeviceInformation has been moved inside
  tNMEA2000 class. This was used only internally until 11.06.2017 release.

## 11.06.2017

 Added NMEA 2000 mandatory features. Some bug fixes

- !NOTE compatibility change. PROGMEM configuration information did not work
  and actually wasted RAM.  You should define each configuration information
  string alone as PROGMEM and call changed SetProgmemConfigurationInformation.
  See sample BatteryMonitor
- Due to new mandatory features library requires more RAM and program memory.
  It is possible to squeeze requirements with compiler options. See more info
  on NMEA2000_CompilerDefns.h.
- Added new class tN2kGroupFunctionHandler (N2kGroupFunction.h/.cpp) for NMEA
  2000 group function (PGN 126208) handling.  Group function can be used to
  e.g. to set "temperature instance" or "set temperature" fields on PGN 130316.
- Added automatic Heartbeat, which is mandatory for certified NMEA 2000
  devices. If you do not want it to be sent, you have to set heartbeat interval
  to 0. Added also function SetHeartbeatInterval, GetHeartbeatInterval and
  SendHeartbeat.
- Added group function handling for PGN 60928 (ISO Address) and PGN 126993
  (Heartbeat). Handlers can be found on N2kGroupFunctionDefaultHandlers module.
- Added functions ReadResetDeviceInformationChanged,
  SetDeviceInformationInstances, GetDeviceInformation for checking, setting and
  reading device instance changes. See more info on document.
- Added ISO Multi-packet handling. Changed logic on SetN2kCANBufMsg due this.

## 05.06.2017

- Added PGN 130314 by sarfata.
- Added PGN 127245 rudder parser
- Fixed Device Information, last bit must be set to 1
- Fixed response to ISO Address Claim request. Seems that all new devices
  respond allways with broadcast instead of caller address.

## 28.05.2017

- Changed default NMEA2000 variable definition in NMEA2000_CAN.h to
reference. So now it is possible to refer it in other modules with definition:
```code```
  extern tNMEA2000 &NMEA2000;
```endcode```

## 08.04.2017

- Added Binary status report (PGN 127501) handling. See updated
examples MessageSender and DataDisplay2.

## 09.03.2017

- Added PGN 129539 support and PGN 129283, 129284 parsers by
denravonska.

## 07.03.2017

- Debug mode check for DeviceReady and ParseMessages.

## 05.03.2017

- RPi socketCAN auto selection and MBED compiler portability fix by
thomasonw.

## 08.02.2017

- Fixed Heading PGN 127250 parsing

## 22.01.2017

- Replace pointer casting with memcpy to avoid unaligned access, and
add endian support. Thanks to denravonska.
- Handle for PGN 65240 "Commanded address". E.g. diagnostic device may command
  your device to change address.

## 01.01.2017

- Document and some example fixes to match library portability
changes.

## 20.12.2016

- Added support for PGN 126464L, PGN List (Transmit and Receive).
Library will automatically respond to this message.  You need only add message
lists and call to methods ExtendTransmitMessages and/or ExtendReceiveMessages.
See e.g. example TemperatureMonitor.

## 17.12.2016

- Fixes to avoid compiler warnings

## 16.12.2016

- Portability fixes. Thanks to denravonska and thomasonw!
- NOTE! compatibility issue! There is no more default stream set on library
  constuctor. So in case you are using forwarding, you need to setup it (like
  in examples) NMEA2000.SetForwardStream(&Serial);
- This reduces the Arduino dependency, allowing the library to more easily be
  used on other platforms. Check all changes under
  <https://github.com/ttlappalainen/NMEA2000/pull/3>

## 01.12.2016

- License change to MIT for more permissive
- Also some started to remove platform dependent code.

## 12.11.2016

- PGN129025 parser added and some fixes by KimBP

## 11.11.2016

- Added support for PGN 127258 - magnetic variation by adwuk.

## 18.10.2016

- Added parsing for PGN 130311 by adwuk. Typo fix for system date
comment by sarfata.

## 19.09.2016

- Lot of testing behind - hopefully works now better.
- NOTE! New method SetN2kCANSendFrameBufSize. Added buffer for frames to be
  sent. This takes more RAM and may be critical for low RAM systems.
- If frame sending fails, system now buffers frames to be sent automatically
  and tries to resend them on next call for ParseMessages.  With this feature
  it solved my problem that time to time my MFD could not receive important
  GNSS or SOG/GOG messages and informed error.
- System now also has more reliable response to the Product Information ISO
  request (PGN 126998). Unfortunately if your system does not poll often enough
  incoming messages (ParseMessages), you still may loose the request itself.
  This is specially the case if you system spends some time reading sensors
  like 1-wire system. Even with 1-wire asynchronous read, it may spend 10 ms
  interrupts disabled. Within 10 ms there may be about 30 messages on bus.
- New methods SetConfigurationInformation and
  SetProgmemConfigurationInformation. System can now also handle Configuration
  Information ISO request to (PGN 126998). Default configuration information is
  saved to PROGMEM.
- NOTE! Reload also NMEA2000_due!

## 17.09.2016

- Temporary fix for problem to respond product information ISO
request.

## 12.09.2016

- Thanks for people (usauerbrey, OzOns), who noted below problems
- NOTE! If you are using NMEA2000_can, remember to update that too!
- Fix for ISORequest handling. Now responds allways also for broadcasts.
- Some fixes to avoid compiler warnings.
- Fix for parsing PGN 127257/Attitude

## 09.08.2016

- NOTE! Fixed PGN 130310, PGN 130311 and added
SetHandleOnlyKnownMessages(), which effects backward compatibility. See below.

- NOTE! On PGN 130310 and PGN 130311 description says that "Atmospheric
  pressure in Pascals. Use function mBarToPascal". There was scaling error and
  now they works like description. After update you have to provide value on
  Pascals and really use mBarToPascal, if you have your value in mBar.
- NOTE! Added SetHandleOnlyKnownMessages(). If you have called
  SetForwardOnlyKnownMessages(true), library did not handle unknown messages.
  After update, this effects only message forwarding - as it should have been.
  So call also SetHandleOnlyKnownMessages(true), if you want to disable any
  handling for unknown messages.
- NMEA 2000 Library reference update.
- Added ExtendSingleFrameMessages and ExtendFastPacketMessages. With these one
  can own list of known messages so that it is not necessary to duplicate
  message list as, if used only SetSingleFrameMessages and
  SetFastPacketMessages.
- Added discrete status flags for transmission parameters (PGN 127493), thanks
  for testing Jason.

## 06.08.2016

- Added SetISORqstHandler for setting handler for ISO requests. Thanks
thomasonw.

## 30.07.2016

- NMEA 2000 Library reference update.
- Added example TeensyActisenseListenerSender. Example contains code, schematics
and document.

## 19.07.2016

- Fixed discrete status on engine dynamic parameters (PGN 127489),
thanks Jason.
- Added new PGN 127257, vessel attitude. Only sending has been tested with NMEA
Reader

## 12.07.2016

- Added to API -- Optional message lists by thomasonw

## 25.06.2016

- Corrected Battery Current in ParseN2kPGN127508 by thomasonw.

## 23.03.2016

- Additional PGN 129038, PGN 129039, PGN 129285, PGN 130074 support by
adwuk.

## 13.03.2016

- Fix of using PROGMEM. Now also product information defined to
PROGMEM works right.
- Fix of using PROGMEM. Still does not work right with product
information in PROGMEM. So all changes after 09.03 are still under validation.
- More memory optimization - thanks for thomasonw. Constant message
strings has been marked with F(...) moving them to flash instead of RAM.

Note also that there is new function `void tNMEA2000::SetProductInformation(const tProductInformation *_ProductInformation);` So one can save memory by
defining product information to flash by using syntax:

```
  const tProductInformation BatteryMonitorProductInformation PROGMEM={
  1300,               // N2kVersion
  ...
```

See example BatteryMonitor.ino

## 12.03.2016

- Memory tuning. Currently multi device and user definable message
filters has not been implemented, so I changed buffer sizes to minimum.
- There is also new function void tNMEA2000::SetN2kCANMsgBufSize(const unsigned
char _MaxN2kCANMsgs); to define buffer size for received N2k messages.  Note
that library has to collect fast packet frames, which may arrive fragmented
from different devices, so as default this buffer size has been set to 5.  If
your device is only sending some data (mode is tNMEA2000::N2km_NodeOnly), you
do not need to catch all fast packet messages (if any), so you can set buffer
size smaller.

## 09.03.2016

- Additional PGN 127250, PGN 128275 Support by adwuk.

## 08.03.2016

- AVR CAN support by thomasonw.

## 02.02.2016

- NOTE! Updates, which effects backward compatibility. See list below.
- PGN 127489, SetN2kPGN127489 EngineOilTemp and EngineCoolantTemp is in Kelvins
  as in other temperature functions. So add for call to this
  function CToKelvin(...)
- Some function names withing N2kMessages have been changed. Change function names listed below! +
    SetN2kPGNSystemTime -> SetN2kSystemTime +
    ParseN2kPGNSystemTime -> ParseN2kSystemTime +
    SetN2kPGNTrueHeading -> SetN2kTrueHeading +
    SetN2kPGNMagneticHeading -> SetN2kMagneticHeading
- Variable types has been changed on some functions in N2kMessages. So when you get an compiler error about functions in N2kMessages, check
  carefully all parameter definitions for function from N2kMessages.h.
- If you do not have value for some parameter for functions in N2kMessages, use related N2kxxxxNA constant defined in N2kMsg.h. So e.g. if you only have
  wind speed, call +
  SetN2kWindSpeed(N2kMsg, 1, ReadWindSpeed(),N2kDoubleNA,N2kWind_Apprent);
- If you are reading values from N2k bus, you can now check does some value exist by using function N2kIsNA.
  So if you e.g. call +
  ParseN2kOutsideEnvironmentalParameters(N2kMsg,SID,WaterTemperature,OutsideAmbientAirTemperature,AtmosphericPressure); +
  then check pressure value with +
  if ( !N2kIsNA(AtmosphericPressure) ) { // It is available, so we can show it!
- Added reference document to the documents, which hopefully helps to get started.

## 23.01.2016

- Added PGN 127493 support. NMEA2000_mcp has now interrupt support. Some other fixes.
- Added some comments to samples and several new message readers. Also added support for 130316 extended temperature.
Added new include N2kMessagesEnumToStr.h for translating library enums to clear text. This is now just for preliminary
so I may changes texts in coming future.
Added also new examples DataDisplay2.ini and MessageSender.ino. They are extended versions of DataDisplay.ino and
TemperatureMonitor.ino.

## 05.12.2015

- Added NMEA2000_CAN.h and some fixes. Library has been originally developed with Arduino Software 1.6.5
On Arduino Software 1.6.6 it is possible to include libraries within included files, so now it is possible to just
include one file NMEA2000_CAN.h, which automatically selects right CAN library according. So you can have same code for
different hw. Currently supported CAN libraries are mcp_can, due_can and teensy.
Note! NMEA2000_CAN.h is now used on examples TemperatureMonitor and WindMonitor!
