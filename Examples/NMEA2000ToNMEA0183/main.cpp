/*
 NMEA0183/NMEA2000 library. NMEA2000 -> NMEA0183
   Reads some messages from NMEA2000 and converts them to NMEA0183
   format to NMEA0183_out (Serial on Arduino or /dev/tnt0 on RPi).
   Also forwards all NMEA2000 bus messages in Actisense format.

   The example is designed for sending N2k data to OpenCPN on RPi with PiCAN2.
   It can be used also on PC with some Arduino/Teensy board to provide
   NMEA0183 (and also NMEA2000) data to PC. Example has been tested on
   RPi3B, ESP32, Arduino DUE, Arduino Mega and Teensy.

 To use this example you need install also:
   - NMEA2000 library
   - NMEA0183 library
   - Related CAN libraries.

 The example works with default settings on Arduino/Teensy boards and on
 Rasberry Pi 3 with PiCAN2 shield.

 On Rasberry Pi you need also Code Block environment and tty0tty (virtual
 null modem cable)
*/

#define N2k_CAN_INT_PIN 21 // Interrupt pin definition for Ardino Mega or other "CAN bus shield" boards.
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kDataToNMEA0183.h"
#include "BoardSerialNumber.h"

#ifdef ARDUINO
#define NMEA0183_Out_Stream_Speed 115200
#define NMEA0183_Out_Stream Serial
#define N2kForward_Stream_Speed 115200
#if defined(__SAM3X8E__)  // Arduino DUE
#define N2kForward_Stream SerialUSB
#elif !defined(ARDUINO_ARCH_ESP32)
#define N2kForward_Stream Serial1
#endif
#else
#include "NMEA0183LinuxStream.h"
tNMEA0183LinuxStream NMEA0183OutStream("/dev/tnt0");
tSocketStream ForwardStream("/dev/tnt2");
#define N2kForward_Stream ForwardStream
#define NMEA0183_Out_Stream NMEA0183OutStream
#endif

// Reading serial number depends of used board. BoardSerialNumber module
// has methods for RPi, Arduino DUE and Teensy. For others function returns
// 0 and then DefaultSerialNumber will be used.
#define DefaultSerialNumber 999999
uint32_t GetSerialNumber() {
  uint32_t Sno=GetBoardSerialNumber();

  return ( Sno!=0?Sno:DefaultSerialNumber );
}

tNMEA0183 NMEA0183_Out;
tN2kDataToNMEA0183 N2kDataToNMEA0183(&NMEA2000, &NMEA0183_Out);

// Set the information for other bus devices, which messages we support
const unsigned long TransmitMessages[] PROGMEM={0};
const unsigned long ReceiveMessages[] PROGMEM={/*126992L,*/127250L,127258L,128259UL,128267UL,129025UL,129026L,129029L,0};

// *****************************************************************************
// Empty stream input buffer. Ports may get stuck, if they get data in and it will
// not be read out.
template <class T> void FlushStreamInput(T &stream) {
#ifdef ARDUINO
  while ( stream.available() ) stream.read();
#endif
}

// *****************************************************************************
void setup() {
  // Setup NMEA2000 system
  #ifndef ARDUINO
  setvbuf (stdout, NULL, _IONBF, 0); // No buffering on stdout, just send chars as they come.
  #endif

  #ifdef ARDUINO
  #ifdef N2kForward_Stream
  N2kForward_Stream.begin(N2kForward_Stream_Speed);
  #endif
  NMEA0183_Out_Stream.begin(NMEA0183_Out_Stream_Speed);
  delay(1000); // Give some time for serial to initialize
  #else
  #endif

  #ifdef N2kForward_Stream
  NMEA2000.SetForwardStream(&N2kForward_Stream);
  #endif

  char SnoStr[33];
  uint32_t SerialNumber=GetSerialNumber();
  snprintf(SnoStr,32,"%lu",(long unsigned int)SerialNumber);

  NMEA2000.SetProductInformation(SnoStr, // Manufacturer's Model serial code
                                 120, // Manufacturer's product code
                                 "N2k->NMEA0183",  // Manufacturer's Model ID
                                 "1.0.0.1 (2018-04-03)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2018-04-03)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(SerialNumber, // Unique number. Use e.g. Serial number.
                                130, // Device function=PC Gateway. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );

  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,25);
  //NMEA2000.EnableForward(false);

  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  NMEA2000.ExtendReceiveMessages(ReceiveMessages);
  NMEA2000.AttachMsgHandler(&N2kDataToNMEA0183);

  NMEA2000.Open();

  // Setup NMEA0183 ports and handlers
  NMEA0183_Out.SetMessageStream(&NMEA0183_Out_Stream);
  NMEA0183_Out.Open();
}

#if defined(__linux__)||defined(__linux)||defined(linux)
#include <unistd.h>
#endif

// *****************************************************************************
// This is preliminary definition. For RPi we need to build some
// event system to minimize cpu usage.
void WaitForEvent(unsigned long /*MaxTime*/) {
#if defined(__linux__)||defined(__linux)||defined(linux)
  usleep(100);
#endif
}

// *****************************************************************************
void loop() {
  WaitForEvent(0);
  NMEA2000.ParseMessages();
  N2kDataToNMEA0183.Update();

  // We need to clear output streams input data to avoid them to get stuck.
  FlushStreamInput(NMEA0183_Out_Stream);
  #ifdef N2kForward_Stream
  FlushStreamInput(N2kForward_Stream);
  #endif
}

#ifndef ARDUINO
// *****************************************************************************
int main(void) {
  setup();
  while ( true ) loop();

  return 0;
}
#endif

