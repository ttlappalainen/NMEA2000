// Demo: NMEA2000 library. Read some data and their show values.
// See also DataDisplay2.ino, which handles more data.

// #define USE_DUE_CAN 1
#define N2K_SOURCE 15

#include <Arduino.h>
#include <Time.h>
#include <N2kMsg.h>
#include <NMEA2000.h>
#include <N2kMessages.h>

// Uncomment these for using Arduino Due internal CAN
#include <due_can.h>  // https://github.com/collin80/due_can
#include <NMEA2000_due.h>
tNMEA2000_due NMEA2000;
// Uncomment these for using Arduino Due internal CAN

// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip
//#include <SPI.h>
// CAN_BUS_shield libraries will be originally found on https://github.com/Seeed-Studio/CAN_BUS_Shield
// There is improved library, which branch can be found on https://github.com/peppeve/CAN_BUS_Shield
// That works also with Maple mini and 8 MHz clock. Hopefully these improvements will be applied to
// original library
//#include <mcp_can.h> 
//#include <NMEA2000_mcp.h>
//#define N2k_SPI_CS_PIN 53  // Pin for SPI Can Select
//tNMEA2000_mcp NMEA2000(N2k_SPI_CS_PIN);
// Uncomment these for using Arduino Mega and external MCP2551 CAN bus chip

typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg); 
} tNMEA2000Handler;

void FluidLevel(const tN2kMsg &N2kMsg);
void WaterDepth(const tN2kMsg &N2kMsg);

tNMEA2000Handler NMEA2000Handlers[]={
  {127505L,&FluidLevel},
  {128267L,&WaterDepth},
  {0,0}
};

void setup() {
  Serial.begin(115200);
  // Do not forward bus messages at all
  NMEA2000.EnableForward(false);
  NMEA2000.SetMsgHandler(HandleNMEA2000Msg);
  NMEA2000.Open();
}

void WaterDepth(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    double DepthBelowTransducer;
    double Offset;

    if (ParseN2kWaterDepth(N2kMsg,SID,DepthBelowTransducer,Offset) ) {
      if (Offset>0) {
        Serial.print("Water depth:");
      } else {
        Serial.print("Depth below keel:");
      }
      Serial.println(DepthBelowTransducer+Offset);
    }
}

void FluidLevel(const tN2kMsg &N2kMsg) {
    unsigned char Instance;
    tN2kFluidType FluidType;
    double Level=0;
    double Capacity=0;

    if (ParseN2kFluidLevel(N2kMsg,Instance,FluidType,Level,Capacity) ) {
      switch (FluidType) {
        case N2kft_Fuel:
          Serial.print("Fuel level :");
          break;
        case N2kft_Water:
          Serial.print("Water level :");
          break;
        case N2kft_GrayWater:
          Serial.print("Gray water level :");
          break;
        case N2kft_LiveWell:
          Serial.print("Live well level :");
          break;
        case N2kft_Oil:
          Serial.print("Oil level :");
          break;
        case N2kft_BlackWater:
          Serial.print("Black water level :");
          break;
      }
      Serial.print(Level); Serial.print("%"); 
      Serial.print(" ("); Serial.print(Capacity*Level/100); Serial.print("l)");
      Serial.print(" capacity :"); Serial.println(Capacity);
    }
}

//NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  int iHandler;
  // Find handler
  for (iHandler=0; NMEA2000Handlers[iHandler].PGN!=0 && !(N2kMsg.PGN==NMEA2000Handlers[iHandler].PGN); iHandler++);
  if (NMEA2000Handlers[iHandler].PGN!=0) {
    NMEA2000Handlers[iHandler].Handler(N2kMsg); 
  }
}

void loop() {
  NMEA2000.ParseMessages();
}

