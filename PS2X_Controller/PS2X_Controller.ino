#include "PS2X_lib.h"  //for v1.6
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
extern "C" { 
   #include "user_interface.h" 
}
// Not using hardware SPI so pick your pins
#define PS2_DAT        D6
#define PS2_CMD        D7
#define PS2_SEL        D8
#define PS2_CLK        D5
const char *password = "12345678";
const char* ssid = "WheelyFast";
/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false
// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
IPAddress remoteIPAddress(192, 168, 1, 1);
int remotePortAddress = 8888;

PS2X ps2x; // create PS2 Controller Class

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup(){
  system_update_cpu_freq(80);           // set cpu to 80MHZ or 160MHZ !
  //Serial.begin(57600);
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
  setupWiFi();
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
  }  
  else if(error == 1)
    Serial.println("No controller found");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands.");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
 
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
	  case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
   }
}

void loop() {

  if(error == 1) return;  //skip loop if no controller found

  if(type == 2){ //Guitar Hero Controller 
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    sendMyData();
  }
  checkForIncomingData();
  delay(10);  // 100 updates per second
}
void setupWiFi(){
  Serial.println("");
  WiFi.mode(WIFI_STA); // station mode
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print(".");
  }
  if(UDP.begin(localPort) == 1){
      Serial.println(F("Connection successful"));
      udpConnected = true;
    }
  else{
      Serial.println(F("Connection failed"));
      }
}
void checkForIncomingData(){
  if(udpConnected){
    // if there’s data available, read a packet
    int packetSize = UDP.parsePacket();
    if (packetSize){
      char packetBuffer[packetSize];
      if(packetSize) // have data
      {
        remoteIPAddress = UDP.remoteIP(); // update client details
        remotePortAddress = UDP.remotePort();
        // read the packet into packetBuffer
        UDP.read(packetBuffer,packetSize);
        Serial.print(F("Recieved: "));
        Serial.println(packetBuffer);
        // TODO: process packet here
        }
    }
  }
}

 void sendMyData(){
    // check for 0's
    for (int a = 0; a < 4; a++){
      if (ps2x.PS2data[5 + a] == 0) ps2x.PS2data[5 + a] = 1;
    }
    for (int a = 0; a < 7; a++){
      if (ps2x.PS2data[9 + a] == 0) ps2x.PS2data[9 + a] = 1;
    }
      UDP.beginPacket(remoteIPAddress,remotePortAddress);
      UDP.print((char*)(ps2x.PS2data));
      UDP.endPacket();
 }
