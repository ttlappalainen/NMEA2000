# Additional Information about NMEA2000

\tableofcontents

## NMEA2000 instances

Instances are used in an NMEA 2000 network to identify multiple similar products connected on the same network.

As an example, take a system with two battery monitors (one for the main battery bank, and another for the hydraulic-thruster bank) and also a Quattro inverter/charger. All three of those devices will send their battery voltage measurements out on the N2K network. For the displays to show these values at the right place, they need to know which voltage belongs to what battery. That is what instances are for.

### Different types of instances

There various types of instances, and for marine systems are two that matter: the **Device instance**, **Data instance** and **System instance**. Device and System instance is stored in tNMEA2000::tDeviceInformation.

#### Device instance

The device instance is sent in PGN 60928, ISO Address Claim, as a combined field of Device Instance Lower (ISO ECU Instance) and Device Instance Upper (ISO Function Instance). The Device instance is e.G used by Victron chargers (Skylla-i/-IP44, VE.Can MPPTs) to configure them in the same group and synchronize them.

#### Data instances

These instances belong to data on the bus (e.G. Battery Instance, DC Detailed Instance, Switch bank instance, etc.) and are embedded in the different PGN’s. For changing these instances through a complex write, PGN 126208, Complex Request Group Function Code 5, write fields.

#### System instance

The system instance is also sent in PGN 60928, field 8. 

\sa
- [Victron Technical-Information-Data-communication](https://www.victronenergy.com/upload/documents/Technical-Information-Data-communication-with-Victron-Energy-products_EN.pdf)
- [Victron changing NMEA2000 Instances](https://www.victronenergy.com/live/ve.can:changing_nmea2000_instances)

## Sequence ID

SID – The sequence identifier field is used to tie related PGNs together. For example, the GPS200 will transmit identical SIDs for 126992 (System Time), 128259 (Speed), 129026 (COG and SOG, Rapid Update), 129029 (GNSS Position Data), 129539 (GNSS DOPs), and 129540 (GNSS Satellites in View) to indicate that the readings are linked together (i.e., the data from each PGN was taken at the same time although they are reported at slightly different times).

## Manufacturer Code

The Manufacturer Code is part of the tNMEA2000::tDeviceInformation which is mandatory for each NMEA2000 device.

[List of all Manufacturer Codes](https://www.nmea.org/Assets/20190623%20manu%20code%20and%20product%20code.pdf)

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
