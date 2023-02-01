// Demo: NMEA2000 library. Sends NMEA2000 to WiFi in NMEA0183 and SeaSmart format.
//
// The code has been tested with ESP32.

#include <NMEA2000_CAN.h>
#include <Seasmart.h>
#include <WiFi.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <memory>
#include "N2kDataToNMEA0183.h"
#include "BoardSerialNumber.h"
#include "List.h"

// Define your default settings here
const char* ssid     = "Guest";
const char* password = "Guest";
const uint16_t ServerPort=2222; // Define the port, where served sends data. Use this e.g. on OpenCPN
const char *ServerIP="192.168.40.33"; // Define the IP, what server will use. This has to be within your local network. Leave empty for DHCP

const size_t MaxClients=10;
bool SendNMEA0183Conversion=true; // Do we send NMEA2000 -> NMEA0183 consverion
bool SendSeaSmart=true; // Do we send NMEA2000 messages in SeaSmart format
bool ResetWiFiSettings=true; // If you have tested other code in your module, it may have saved settings and have difficulties to make connection.

WiFiServer server(ServerPort,MaxClients);

using tWiFiClientPtr = std::shared_ptr<WiFiClient>;
LinkedList<tWiFiClientPtr> clients;

tN2kDataToNMEA0183 tN2kDataToNMEA0183(&NMEA2000, 0);

// Set the information for other bus devices, which messages we support
const unsigned long TransmitMessages[] PROGMEM={0};
const unsigned long ReceiveMessages[] PROGMEM={/*126992L,*/ // System time
                                              127250L, // Heading
                                              127258L, // Magnetic variation
                                              128259UL,// Boat speed
                                              128267UL,// Depth
                                              129025UL,// Position
                                              129026L, // COG and SOG
                                              129029L, // GNSS
                                              130306L, // Wind
                                              0};

// Forward declarations
void LedOn(unsigned long OnTime);
void UpdateLedState();
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg);
void SendNMEA0183Message(const tNMEA0183Msg &NMEA0183Msg);
void InitNMEA2000();

#include <nvs.h>
#include <nvs_flash.h>

void ResetWiFiSettingsOnNvs() {
  int err;
  err=nvs_flash_init();
  Serial.println("nvs_flash_init: " + err);
  err=nvs_flash_erase();
  Serial.println("nvs_flash_erase: " + err); 
}

//*****************************************************************************
void setup() {
  if ( ResetWiFiSettings ) ResetWiFiSettingsOnNvs();

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password);

  size_t WaitCount=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    WaitCount++;
    if ( WaitCount>80 ) {
      Serial.println();
      WaitCount=0;
    }
  }

  if ( ServerIP!=0 && ServerIP[0]!=0 ) { // Try to force ip.
    IPAddress local_IP; 
    if ( local_IP.fromString(ServerIP) && !WiFi.config(local_IP,WiFi.gatewayIP(),WiFi.subnetMask()) ) { //, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();

  InitNMEA2000();
}

//*****************************************************************************
void AddClient(WiFiClient &client) {
  Serial.println("New Client.");
  clients.push_back(tWiFiClientPtr(new WiFiClient(client)));
}

//*****************************************************************************
void StopClient(LinkedList<tWiFiClientPtr>::iterator &it) {
  Serial.println("Client Disconnected.");
  (*it)->stop();
  it=clients.erase(it);
}

//*****************************************************************************
void CheckConnections() {
 WiFiClient client = server.available();   // listen for incoming clients

  if ( client ) AddClient(client);

  for (auto it=clients.begin(); it!=clients.end(); it++) {
    if ( (*it)!=NULL ) {
      if ( !(*it)->connected() ) {
        StopClient(it);
      } else {
        if ( (*it)->available() ) {
          char c = (*it)->read();
          if ( c==0x03 ) StopClient(it); // Close connection by ctrl-c
        }
      }
    } else {
      it=clients.erase(it); // Should have been erased by StopClient
    }
  }
}

//*****************************************************************************
void loop() {
  CheckConnections();
  NMEA2000.ParseMessages();
  tN2kDataToNMEA0183.Update();
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
  digitalWrite(LED_BUILTIN, HIGH);
  TurnLedOffTime=millis()+OnTime;
  TurnLedOnTime=0;
}

//*****************************************************************************
void UpdateLedState() {
  if ( TurnLedOffTime>0 && TurnLedOffTime<millis() ) {
    digitalWrite(LED_BUILTIN, LOW);
    TurnLedOffTime=0;
    TurnLedOnTime=millis()+LedBlinkTime;
  }
  
  if ( TurnLedOnTime>0 && TurnLedOnTime<millis() ) LedOn(LedBlinkTime);
}

// Reading serial number depends of used board. BoardSerialNumber module
// has methods for RPi, Arduino DUE and Teensy. For others function returns
// 0 and then DefaultSerialNumber will be used.
#define DefaultSerialNumber 999999
//*****************************************************************************
uint32_t GetSerialNumber() {
  uint32_t Sno=GetBoardSerialNumber();

  return ( Sno!=0?Sno:DefaultSerialNumber );
}

//*****************************************************************************
void InitNMEA2000() {
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
//  NMEA2000.SetForwardStream(&Serial);  // PC output on due native port
//  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text
  // NMEA2000.EnableForward(false);                 // Disable all msg forwarding to USB (=Serial)

  char SnoStr[33];
  uint32_t SerialNumber=GetSerialNumber();
  snprintf(SnoStr,32,"%lu",(long unsigned int)SerialNumber);

  NMEA2000.SetProductInformation(SnoStr, // Manufacturer's Model serial code
                                 130, // Manufacturer's product code
                                 "N2k->NMEA0183 WiFi",  // Manufacturer's Model ID
                                 "1.0.0.1 (2018-04-08)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2018-04-08)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(SerialNumber, // Unique number. Use e.g. Serial number.
                                130, // Device function=PC Gateway. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                25, // Device class=Inter/Intranetwork Device. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );

  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,32);
  //NMEA2000.EnableForward(false);

  NMEA2000.ExtendTransmitMessages(TransmitMessages);
  NMEA2000.ExtendReceiveMessages(ReceiveMessages);
  NMEA2000.AttachMsgHandler(&tN2kDataToNMEA0183); // NMEA 2000 -> NMEA 0183 conversion
  NMEA2000.SetMsgHandler(HandleNMEA2000Msg); // Also send all NMEA2000 messages in SeaSmart format
  
  tN2kDataToNMEA0183.SetSendNMEA0183MessageCallback(SendNMEA0183Message);

  NMEA2000.Open();
}

//*****************************************************************************
void SendBufToClients(const char *buf) {
  for (auto it=clients.begin() ;it!=clients.end(); it++) {
    if ( (*it)!=NULL && (*it)->connected() ) {
      (*it)->println(buf);
    }
  }
}

#define MAX_NMEA2000_MESSAGE_SEASMART_SIZE 500 
//*****************************************************************************
//NMEA 2000 message handler
void HandleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  LedOn(LedOnTime);

  if ( !SendSeaSmart ) return;
  
  char buf[MAX_NMEA2000_MESSAGE_SEASMART_SIZE];
  if ( N2kToSeasmart(N2kMsg,millis(),buf,MAX_NMEA2000_MESSAGE_SEASMART_SIZE)==0 ) return;
  SendBufToClients(buf);
}

#define MAX_NMEA0183_MESSAGE_SIZE 100
//*****************************************************************************
void SendNMEA0183Message(const tNMEA0183Msg &NMEA0183Msg) {
  if ( !SendNMEA0183Conversion ) return;
  
  char buf[MAX_NMEA0183_MESSAGE_SIZE];
  if ( !NMEA0183Msg.GetMessage(buf,MAX_NMEA0183_MESSAGE_SIZE) ) return;
  SendBufToClients(buf);
}

