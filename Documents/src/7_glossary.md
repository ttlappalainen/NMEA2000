# Additional Information about NMEA2000

\tableofcontents

## NAME {#secRefTermNAME}

On NMEA2000 NAME (written all capital) means data defined by device information. Since device information will be sent by PGN 60928, NAME is also 8 byte content of than message. NAME can be used to identify devices sending some information. For this reason it is important that NAME is unique for all devices on the bus.

Normally unique number and manufacturer code within device information should be enough to define unique device. Some manufactures does not do that and their devices may require modification for device instance when same devices appears on the bus.

Safest way to fullfill unique NAME requirement is to keep unique number unique. For commercial production that requires some kind of register for manufactured devices.

On library you set device information by using function tNMEA2000::SetDeviceInformation and if necessary with function tNMEA2000::SetDeviceInformationInstances.

## Address claiming {#secRefTermAddressClaiming}

NMEA2000 bus can have maximum 252 devices. Each device will get device source address (0-251), which
can be predefined on device start, but may be changed by address claiming at any time.

On start device starts address claiming using previously saved source address by sending its \ref secRefTermNAME to the bus by using message ISO address claim PGN 60928. If there is no other device using same source address, device can start normal operation after 250 ms delay. If there is another device using same address there are two possible cases.
1. Other device NAME has lower priority, so it will increment its own address and start address claiming. This device can keep its source address.
2. Other device NAME has higher priority and responds with ISO address claim. This device will increment its source address and start address claiming again with new address.

If bus is full of devices, devices with lowest priority will get source address 254 and they can not send anything to bus.

Library takes care of address claiming and required silence delay automatically. To get information possible source address changes, use funtion tNMEA2000::ReadResetAddressChanged().

## NMEA2000 instances

Instances are used in an NMEA 2000 network to identify multiple similar products connected on the same network.

As an example, take a system with two battery monitors (one for the main battery bank, and another for the hydraulic-thruster bank) and also a Quattro inverter/charger. All three of those devices will send their battery voltage measurements out on the N2K network. For the displays to show these values at the right place, they need to know which voltage belongs to what battery. That is what instances are for.

### Different types of instances

There are various types of instances: **Device instance**, **Data instance** and **System instance**. Device and System instance is stored in tNMEA2000::tDeviceInformation and so they are also part of \ref secRefTermNAME. See also tNMEA2000::SetDeviceInformationInstances.

#### Device instance

The device instance is sent in PGN 60928, ISO Address Claim, as a combined field of Device Instance Lower (ISO ECU Instance) and Device Instance Upper (ISO Function Instance). The Device instance is e.g., used by Victron chargers (Skylla-i/-IP44, VE.Can MPPTs) to configure them in the same group and synchronize them.

#### Data instances

These instances belong to data on the bus (e.g., Battery Instance, DC Detailed Instance, Switch bank instance, etc.) and are embedded in the different PGN’s. For changing these instances through a complex write, PGN 126208, Complex Request Group Function Code 5, write fields.

#### System instance

The system instance is also sent in PGN 60928, field 8. System instance indicates device occurence in other like redundant or parallel NMEA2000 networks. For small and simple vessels this is always 0.

\sa
- [Victron Technical-Information-Data-communication](https://www.victronenergy.com/upload/documents/Technical-Information-Data-communication-with-Victron-Energy-products_EN.pdf)
- [Victron changing NMEA2000 Instances](https://www.victronenergy.com/live/ve.can:changing_nmea2000_instances)

## SID {#secRefTermSID}

The sequence identifier field is used to tie different PGNs data together to same sampling or calculation time. In other words same SID in different PGNs sent by same device defines that values on PGN:s has been sampled or calculated at same time. Devices reading those messages can use in calculations PGNs data provided with same SID and get more accurate results.

For example, the GPS200 will transmit identical SIDs for 126992 (System Time), 128259 (Speed), 129026 (COG and SOG, Rapid Update), 129029 (GNSS Position Data), 129539 (GNSS DOPs), and 129540 (GNSS Satellites in View) to indicate that the readings are linked together (i.e., the data from each PGN was taken at the same time although they are reported at slightly different times).

E.g., with temperature data I do not see any use for SID. Temperature changes are 
normally anyway slow and 1.5-2 s difference on sampling does not have meaningfull effect. 
I would also think that there is no device using SID for temperature, pressure and humidity data.

On sending in most cases you can use just 0xff for SID. On receiving you can normally just forget it.

If you like to use SID, you have to increase value synchronized with sampling and sending. SID value
has to be on range 0-252. 255 (0xff) means data not available and 253-254 are reserved.
So use it e.g., with code 
``` SensorSID++; if ( SensorSID>252) SensorSID=0;``` 
If you e.g., sample your multisensor every 2 s, 
but PGNs will be sent every 1 s you can increase SID after each sampling and there would be 
2 sends having same SID indicating that they have been sampled at same time.
If you instead sample faster than send, you have to increase SID after all related messages
has been sent. It would be even more complex if your devices a lot different messages and their
sampling is grouped and requires different SID. Then you have to also take care that messages
which are not linked to each other never has same SID.

If you use multitasking system, you should also lock data 
during sampling and sending so that they would not change during task. If you e.g., 
have task sending sensor values from some kind of globals, both task should use some 
data locking system:


Sampling task:

```shell
SampleSensors(Sensor1,Sensor2);
LockSend();
Sensor1Out=Sensor1;
Sensor2Out=Sensor2;
SensorsSID++; if ( SensorsSID>252 ) SensorsSID=0;
ReleaseSend();
```

Sending task:

```shell
LockSend();
double S1=Sensor1Out;
double S2=Sensor2Out;
SID=SensorsSID;
ReleaseSend();
SendSensorsData(SID,S1,S2);
```

## Manufacturer Code {#secRefTermManufacturerCode}

The Manufacturer Code is part of the \ref secRefTermNAME (see tNMEA2000::tDeviceInformation). Manufacturer code is mandatory for each NMEA2000 device and it has to be requested from NMEA organization.

Note that NMEA2000 devices does not have way to request manufacturer name. Manufacturer code is link to hardcoded name. So if you have old MFD and sersor from new manufacturer, sensor will be listed with name "unknown manufacturer".

Some manufacturers hardcodes which manufacturer sensors they accept. So in case e.g., MFD document claims to support e.g., PGN 130316, but sensor data will not be shown or not shown properly, check your MFD manufacturer that your sensor manufacturer is listed in their database. As opposite if you make device for your own vessel only, you can setup your device to use accepted manufacturer code.

\ref secRefMfgCodes

## NMEA2000 certification {#secRefNMEA2000Certification}

I have advertised library to be certification ready. There are some functionalities left for developer, since there is no common way do that. Good example is saving possibly changed source address or device/system instances. It is developer responsibility to choose method for saving. It can be e.g., EEPROM, file, flash. To fullfill all certification requirements you have to add some functionality.
- Your device has to be electrically isolated as I have descibed on \ref pageHWSetUp.
- Save device source address and restore it on startup by using saved value on tNMEA2000::SetMode(). Track possible changes by using function tNMEA2000::ReadResetAddressChanged().
- Save device and system instances and restore them on startup with function tNMEA2000::SetDeviceInformationInstances().Track possible changes by using function tNMEA2000::ReadResetDeviceInformationChanged().
- Write ISO request handler to respond requests for all PGNs you are transmitting. Register handler with tNMEA2000::SetISORqstHandler().
- Inherit tN2kGroupFunctionHandler and write handler, which responds all messages your device is transmitting. You can write own handler for all your messages or write common handler. See N2kGroupFunction.h. Note that if you accept message period or offset change, you should also save them and restore them on startup. Also if you handle PGN data instance change, you need to save that too. Register handler with tNMEA2000::AddGroupFunctionHandler().
- Save both installation descriptions and restore them on startup with function tNMEA2000::SetConfigurationInformation(). Track possible changes by using function tNMEA2000::ReadResetInstallationDescriptionChanged().

For commercial certified devices you need also (prices at 2024 are for members):
- Join as member to NMEA organization - new member 475 $, yearly renewal 300 $.
- Request \ref secRefTermManufacturerCode from NMEA organization - 1200 $.
- Request ProductCode from NMEA organization - 450 $.
- Run certification tests for you device. For that you need specific certification tool. You can ask help from me. Costs depends of certification test provider - I know some providers. Costs 500 $ - 2000 $.
- Certification verification - 850 $.

So total costs for certified device for new member is about 4000 $.

## Relation between CAN ID and NMEA2000

The SAE J1939 protocol requires a specific format for the CAN message’s identifer. The ID can consist of 11 bits, or optionally 29 bits in the extended format. At NMEA2000 only the extended format ist supported.

\image html saeJ1939canID.png

- Priority (28-26)- establishes the arbitration priority, highest priority being 0 and lowest is seven
- EDP (25) - Extended Data Page: combined with DP to identify different message definitions
- DP (24) - Data Page: J1939 set to Page 0, Page 1 for NMEA2000
- PDU Format (23-16) - helps to define the parameter group the message belongs in. Distinguishes what type of data is being sent
- PDU Specific (15-8) - the address of the message’s destination device
- Source Address (7-0) - the address of the message’s source device

\sa
  [J1939 PGN vs. 29-Bit CAN ID - Online Converter](https://www.csselectronics.com/pages/j1939-pgn-conversion-tool)

## List of registered manufacturer codes  {#secRefMfgCodes}

| Code | Manufacturer name |
|--------|--------|
| 78 | Fw Murphy |
| 80 | Twin Disc |
| 85 | Kohler Power Systems |
| 88 | Hemisphere |
| 116 | BEP Marine |
| 135 | Airmar |
| 137 | Maretron |
| 140 | Lowrance |
| 144 | Mercury Marine |
| 147 | Nautibus Electronic Gmbh |
| 148 | Blue Water Data |
| 154 | Westerbeke |
| 161 | Offshore Systems UK |
| 163 | Evinrude/Brp Bombardier |
| 165 | CPAC Systems Ab |
| 168 | Xantrex Technology |
| 172 | Yanmar Marine |
| 174 | Ab Volvo/Volvo Penta |
| 176 | Carling Technologies |
| 185 | Beede Electrical |
| 192 | Floscan Instrument Co., Inc. |
| 193 | Nobeltec |
| 198 | Mystic Valley Communications |
| 199 | Actia Corporation |
| 201 | Disenos Y Technologia |
| 211 | Digital Switching Systems |
| 215 | Xintex/Atena |
| 224 | Emmi Network |
| 228 | Zf Marine Electronics |
| 229 | Garmin |
| 233 | Yacht Monitoring Solutions |
| 235 | Sailormade Marine Telemetry |
| 243 | Eride |
| 257 | Honda Motor Company |
| 272 | Groco |
| 273 | Actisense |
| 274 | Amphenol Ltw Technology |
| 275 | Navico |
| 283 | Hamilton Jet |
| 285 | Sea Recovery |
| 286 | Coelmo Srl Italy |
| 295 | BEP Marine |
| 299 | Garmin |
| 304 | Trigentic/EmpirBus |
| 305 | Novatel |
| 306 | Sleipner Motor As |
| 307 | Mbw Technologies |
| 315 | ICOM |
| 328 | Qwerty |
| 329 | Dief |
| 341 | Böning Automationstechnologie GmbH |
| 345 | Korea Maritime University |
| 351 | Thrane And Thrane |
| 355 | Mastervolt |
| 356 | Fischer Panda Generators |
| 358 | Victron Energy |
| 370 | Rolls Royce Marine |
| 373 | Electronic Design |
| 374 | Northern Lights |
| 378 | Glendinning |
| 381 | B&G |
| 384 | Rose Point Navigation Systems |
| 385 | Johnson Outdoors Marine Electronics |
| 394 | Capi 2 |
| 396 | Beyond Measure |
| 400 | Livorsi Marine |
| 404 | ComNav |
| 419 | Fusion Electronics |
| 421 | Standard Horizon |
| 422 | True Heading |
| 426 | Egersund Marine Electronics As |
| 427 | Em-Trak Marine Electronics Ltd |
| 431 | Tohatsu Co Jp |
| 437 | Digital Yacht |
| 438 | Comar Systems Limited |
| 440 | Cummins |
| 443 | VDO |
| 451 | Parker Hannifin |
| 459 | Alltek Marine Electronics Corp |
| 460 | San Giorgio S.E.I.N. |
| 466 | Veethree Electronics |
| 467 | Humminbird Marine Electronics |
| 470 | SI-TEX Marine Electronics |
| 471 | Sea Cross Marine Ab |
| 475 | GME/Standard Communications |
| 476 | Humminbird Marine Electronics |
| 478 | Ocean Sat Bv |
| 481 | Chetco Digitial Instruments |
| 493 | Watcheye |
| 499 | Lcj Capteurs |
| 502 | Attwood Marine |
| 503 | Naviop S.R.L. |
| 504 | Vesper Marine |
| 510 | Marinesoft Co. LTD |
| 517 | NoLand Engineering |
| 518 | Transas USA |
| 529 | National Instruments Korea |
| 532 | Onwa Marine |
| 540 | Webasto |
| 571 | Marinecraft Co. |
| 573 | McMurdo Group |
| 578 | Advansea |
| 579 | KVH |
| 580 | San Jose Technology |
| 583 | Yacht Control |
| 586 | Suzuki Motor Corporation |
| 591 | USCG |
| 595 | ShipModul |
| 600 | Aquatic AV |
| 605 | Aventics GmbH |
| 606 | Intellian |
| 612 | SamwonIT |
| 614 | Arlt Tecnologies |
| 637 | Bavaria Yacts |
| 641 | Diverse Yacht Services |
| 644 | KUS Manufacturer |
| 645 | Garmin |
| 658 | Shenzhen Jiuzhou Himunication |
| 688 | Rockford Corporation |
| 704 | JL Audio |
| 715 | Autonnic |
| 717 | Yacht Devices |
| 734 | REAP Systems |
| 735 | Au Electronics Group |
| 739 | LxNav |
| 740 | JL Audio |
| 743 | DaeMyung Elevator |
| 744 | Woosung |
| 773 | Clarion US |
| 776 | HMI Systems |
| 777 | Ocean Signal |
| 778 | Seekeeper |
| 781 | Poly Planar |
| 785 | Fischer Panda DE |
| 795 | Broyda Industries |
| 796 | Canadian Automotive |
| 797 | Tides Marine |
| 798 | Lumishore |
| 799 | Still Water Designs and Audto |
| 802 | SPBI SA Ets BJTechnologie |
| 803 | Gill Sensors |
| 811 | Blue Water Desalination |
| 815 | Flir Systems |
| 824 | Undheim Systems |
| 838 | TeamSurv |
| 844 | Fell Marine |
| 847 | Oceanvolt |
| 862 | Prospec |
| 868 | Data Panel Corp |
| 890 | L3 Technologies Maritime Systems |
| 894 | Rhodan Marine Systems |
| 896 | Nexfour Solutions |
| 905 | ASA Electronics |
| 909 | Marines Co (South Korea) |
| 911 | Nautic-On, A Brunswick Company |
| 930 | Ecotronix Corp |
| 944 | Zonisa Marine |
| 951 | Exor International S.p.A. |
| 962 | Timbolier Industries |
| 968 | Cox Powertrain |
| 969 | Blue Sea |
| 981 | Kobelt |
| 1008 | Lintest SmartBoat |
| 1011 | Soundmax |
| 1020 | Onyx Marine Automation s.r.l |
| 1021 | Entratech |
| 1022 | ITC Inc. |
| 1023 | PEAK-System Technik GmbH |
| 1029 | The Marine Guardian LLC |
| 1034 | Siren Marine |
| 1047 | Sonic Corporation |
| 1051 | ProNav |
| 1053 | Stillwater Designs / Kicker |
| 1059 | Boatrax |
| 1062 | ComNav |
| 1065 | CALYPSO Instruments |
| 1066 | Spot Zero Water |
| 1070 | Quick-teck Electronics Ltd |
| 1075 | Uniden America |
| 1083 | Nauticoncept |
| 1084 | Shadow-Caster LED lighting LLC |
| 1088 | E-T-A Circuit Breakers |
| 1092 | Scheiber |
| 1095 | Wetsounds |
| 1140 | Across Ocean Systems Ltd. |
| 1305 | Dragonfly Energy |
| 1850 | Teleflex |
| 1851 | Raymarine |
| 1852 | Navionics |
| 1853 | Japan Radio Co |
| 1854 | Northstar Technologies |
| 1855 | Furuno |
| 1856 | Trimble |
| 1857 | Simrad - Navico |
| 1858 | Litton |
| 1859 | Kvasar Ab |
| 1860 | Mmp |
| 1861 | Vector Cantech |
| 1862 | Sanshin Industries/Yamaha Marine |
| 1863 | Faria Instruments |
