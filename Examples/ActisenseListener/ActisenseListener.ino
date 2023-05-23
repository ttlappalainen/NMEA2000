// Demo: NMEA2000 library. Bus listener. Sends all bus data to serial in Actisense format.
// Note! If you use Arduino Mega, it can not handle lot of messages without using interrupt.
// enable interrupt by defining pin you have used for MCP2515 interrupt pin. See below
// definition #define N2k_CAN_INT_PIN 21
//
// Now demo also blinks led for each message and starts slow blink, if no messages has been received.
// Do not confuse with led blinking code

#define N2k_CAN_INT_PIN 21
#include <Arduino.h>
#include <NMEA2000_CAN.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN 0xff
#endif

// Forward declarations for led blinking
void LedOn(unsigned long OnTime);
void UpdateLedState();
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg);

//*****************************************************************************
void setup() {
  if ( LED_BUILTIN!=0xff ) pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetForwardStream(&Serial);  // PC output on due native port
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  // NMEA2000.EnableForward(false);                 // Disable all msg forwarding to USB (=Serial)
  NMEA2000.SetMsgHandler(HandleNMEA2000Msg);
  NMEA2000.Open();
}

//*****************************************************************************
void loop() {
  NMEA2000.ParseMessages();
  // Dummy to empty input buffer to avoid board to stuck with e.g. NMEA Reader
  if ( Serial.available() ) { Serial.read(); } 
  UpdateLedState();
}

// Code below is just for handling led blinking.

#define LedOnTime 2
#define LedBlinkTime 1000
unsigned long TurnLedOffTime=0;
unsigned long TurnLedOnTime=millis()+LedBlinkTime;

//*****************************************************************************
void LedOn(unsigned long OnTime) {
  if ( LED_BUILTIN!=0xff ) digitalWrite(LED_BUILTIN, HIGH);
  TurnLedOffTime=millis()+OnTime;
  TurnLedOnTime=0;
}

//*****************************************************************************
void UpdateLedState() {
  if ( TurnLedOffTime>0 && TurnLedOffTime<millis() ) {
    if ( LED_BUILTIN!=0xff ) digitalWrite(LED_BUILTIN, LOW);
    TurnLedOffTime=0;
    TurnLedOnTime=millis()+LedBlinkTime;
  }
  
  if ( TurnLedOnTime>0 && TurnLedOnTime<millis() ) LedOn(LedBlinkTime);
}

//*****************************************************************************
//NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &/*N2kMsg*/) {
  LedOn(LedOnTime);
}


