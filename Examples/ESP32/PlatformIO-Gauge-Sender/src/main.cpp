#include <Arduino.h>

#define USE_N2K_CAN 7 // 1=Due CAN, 2=Teensy CAN, 3=TeensyX CAN, 4=AVR CAN, 5=SocketCAN, 6=MBED CAN, 7=ESP32 CAN, 8=Arduino CAN
// Set CAN pins for ESP32
#define ESP32_CAN_TX_PIN GPIO_NUM_16
#define ESP32_CAN_RX_PIN GPIO_NUM_4

#include <N2kMessages.h>
#include <N2kMsg.h>
#include <NMEA2000_CAN.h>

// Input pin variables
const uint8_t fuel_level_pin = 36;
const uint8_t trim_percent_pin = 39;

// Synchronization schedulers for each PGN.  Parameters are: Enable, Period [ms], Offset [ms]. 
// The period and offset values are based on typical transmission rates for these PGNs, but can be adjusted as needed.
tN2kSyncScheduler FuelLevelScheduler(false, 2500, 500); // normally transmitted every 2500 milliseconds.
tN2kSyncScheduler TrimPercentScheduler(false, 100, 505); // normally transmitted every 100 milliseconds.

// Static input variables
double FuelCapacity = 246;  // Fuel capacity in liters. This is a static variable that should be set to the actual fuel capacity of the boat. It is used in the fuel level message to indicate the total capacity of the fuel tank.

// Define Streams for NMEA 2000
Stream *read_stream = &Serial1;
Stream *forward_stream = &Serial1;

// Function that runs once when the NMEA 2000 bus is successfullyopened
void OnN2kOpen() {
  FuelLevelScheduler.UpdateNextTime();      // Start all the schedulers
  TrimPercentScheduler.UpdateNextTime();
}

// Function that runs when a message is received from the NMEA 2000 bus
void HandleStreamN2kMsg(const tN2kMsg &message) {
  message.Print(&Serial);
}

// Variables to hold placeholder values.  These shouldn't be needed once the sensor logic is in place.
double FuelLevel = 0;
double TrimPercent = 0;

// Callback functions to read the input values. This is all placeholder code.  Will need to add sensor code later.
float read_fuel_level_callback() {
  FuelLevel += 0.01F;
  if (FuelLevel >= 1.0F) {
    FuelLevel = 0;
  }
  return FuelLevel;
}

float read_trim_percent_callback() {
  TrimPercent += 0.001F;
  if (TrimPercent >= 1.0F) {
    TrimPercent = 0;
  }
  return TrimPercent;
}

// Functions to send the NMEA 2000 messages. These will be called in the main loop and will check if it's time to send 
// the message based on the schedulers.
void SendN2kFuelLevel() {
  tN2kMsg N2kMsg;   // Create a new N2kMsg object to hold the message data

  if ( FuelLevelScheduler.IsTime() ) {        // Check if it's time to send the fuel level message
    FuelLevelScheduler.UpdateNextTime();      // Update the next time to send the message based on the scheduler settings
    FuelLevel = read_fuel_level_callback();   // Read the fuel level from the callback function
    SetN2kFluidLevel(                         // Set the data of the message using the helper function for PGN 127505 (Fluid Level)
      N2kMsg,                                 // Reference to the N2kMsg object to set the data on
      0,                                      // Instance, set to 0 for single instance
      N2kft_Fuel,                             // Fluid type, set to N2kft_Fuel for fuel level
      FuelLevel * 100,                        // Level, convert to percentage (0-100)
      FuelCapacity                            // Capacity, set to the fuel capacity variable (in liters)
    );
    NMEA2000.SendMsg(N2kMsg);                 // Send the message on the NMEA 2000 bus 
  }
}

void SendN2kTrimPercent() {
  tN2kMsg N2kMsg;   // Create a new N2kMsg object to hold the message data

  if ( TrimPercentScheduler.IsTime() ) {        // Check if it's time to send the trim percent message  
    TrimPercentScheduler.UpdateNextTime();      // Update the next time to send the message based on the scheduler settings
    TrimPercent = read_trim_percent_callback(); // Read the trim percent from the callback function
    SetN2kEngineParamRapid(                     // Set the data of the message using the helper function for PGN 127488 (Engine Parameters Rapid Update)
      N2kMsg,                                   // Reference to the N2kMsg object to set the data on
      0,                                        // Engine instance, set to 0 for single instance
      0,                                        // Engine Speed, I don't have this data, so set to 0
      0,                                        // Engine Boost Pressure, I don't have this data, so set to 0 
      TrimPercent * 100                         // Trim Percent, convert from decimal to percentage (0-100)
    );
    NMEA2000.SendMsg(N2kMsg);                   // Send the message on the NMEA 2000 bus
  }
}

void setup() {
  Serial.begin(115200);

  // Set up NMEA 2000 buffers
  NMEA2000.SetN2kCANSendFrameBufSize(250);
  NMEA2000.SetN2kCANReceiveFrameBufSize(250);

  // Set product information
  NMEA2000.SetProductInformation(
      "20260502",               // Manufacturer's Model serial code (max 32 chars)
      100,                      // Manufacturer's product code
      "Base NMEA2k Template",   // Manufacturer's Model ID (max 33 chars)
      "0.1.0.0",                // Manufacturer's Software version code (max 40 chars)
      "0.1.0.0"                 // Manufacturer's Model version (max 24 chars)
  );

  // Set device information
  NMEA2000.SetDeviceInformation(
      985621,     // Unique number. Use e.g. Serial number.
      170,        // Device function=Analog to NMEA 2000 Gateway. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
      35,         // Device class=Inter/Intranetwork Device. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
      122         // Just choose a free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  );

  // Configure NMEA2000 instance
  NMEA2000.SetForwardStream(forward_stream);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show bus data in clear text
  NMEA2000.SetForwardOwnMessages(false);  // do not echo own messages.
  NMEA2000.SetMsgHandler(HandleStreamN2kMsg); // Attach a message handler to print received messages to Serial
  NMEA2000.SetOnOpen(OnN2kOpen); // Attach a function that will run once the NMEA 2000 bus is successfully opened
  NMEA2000.Open(); // Finally open the NMEA 2000 bus. This will start the communication and call the OnN2kOpen function when successful.
}

unsigned long parse_time = millis();  // Time variable to control how often we call ParseMessages in the main loop. We don't need to call it more often than every 1ms, so we can save some CPU time by controlling it with this variable.

void loop() { 
  // Call ParseMessages every 1ms to read messages from the NMEA 2000 bus and call the message handler for each received message.
  if (millis() - parse_time > 1) {
    parse_time = millis();
    NMEA2000.ParseMessages();
  }
  // Call the functions to send the NMEA 2000 messages. These functions will check if it's time to send the message based on the schedulers.
  SendN2kFuelLevel();
  SendN2kTrimPercent();
}