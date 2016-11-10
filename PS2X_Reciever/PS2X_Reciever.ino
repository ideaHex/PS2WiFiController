#include "PS2X_lib.h"
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Ticker.h>
extern "C" { 
   #include "user_interface.h" 
 }
String AP_Name = "WheelyFast";
const char *password = "12345678";
byte vibrate = 0;
PS2X ps2x; // create PS2 Controller Class
Ticker PPSTest;
int PPS = 0;

// UDP variables
unsigned int localPort = 8888;
WiFiUDP UDP;
boolean udpConnected = false;
IPAddress remoteIPAddress;
int remotePortAddress = 0;

void setup(){
  system_update_cpu_freq(80);           // set cpu to 80MHZ or 160MHZ !
  Serial.begin(250000);
  delay(300);
  setupWiFi();
  PPSTest.attach_ms(5000,outPutPPS);
}

void loop() {
    if(!checkForIncomingData())return;
    if(!udpConnected || !remotePortAddress) return;
    //DualShock Controller    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");      

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }   

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if(ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if(ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if(ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if(ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if(ps2x.Button(PSB_TRIANGLE))
        Serial.println("Triangle pressed");        
    }

    if(ps2x.ButtonPressed(PSB_CIRCLE))               //will be TRUE if button was JUST pressed
      Serial.println("Circle just pressed");
    if(ps2x.NewButtonState(PSB_CROSS))               //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");
    if(ps2x.ButtonReleased(PSB_SQUARE))              //will be TRUE if button was JUST released
      Serial.println("Square just released");     

    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) { //print stick values if either is TRUE
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY), DEC); //Left stick, Y axis. Other options: LX, RY, RX  
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_LX), DEC); 
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC); 
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC); 
    }
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);
  char AP_NameChar[AP_Name.length() + 1];
  AP_Name.toCharArray(AP_NameChar,AP_Name.length() + 1);

  int channel = random(1,13 + 1);               // have to add 1 or will be 1 - 12
  IPAddress subnet(255, 255, 255, 0);
  IPAddress apIP(192, 168, 1, 1);
  WiFi.softAPConfig(apIP, apIP, subnet);
  //wifi_set_phy_mode(PHY_MODE_11N);
  WiFi.softAP(AP_NameChar, password , channel , 0 );
  Serial.println("");
  if(UDP.begin(localPort) == 1){
      Serial.println(F("Ready for Controller to connect"));
      udpConnected = true;
    }
  else{
      Serial.println(F("Connection failed"));
      }
}

boolean checkForIncomingData(){
  if(udpConnected){
    // if there’s data available, read a packet
    int packetSize = UDP.parsePacket();
    if (packetSize){
      char packetBuffer[packetSize];
      if (!remotePortAddress || remotePortAddress != UDP.remotePort())Serial.println("Controller Connected, Port: " + String(UDP.remotePort()));
      if(packetSize) // have data
      {
        remoteIPAddress = UDP.remoteIP(); // update client details
        remotePortAddress = UDP.remotePort();
        // read the packet into packetBuffer
        UDP.read(packetBuffer,packetSize);
        // send to library
        for (int a = 0; a < 21; a++){
          ps2x.PS2data[a] = packetBuffer[a];
          }
        ps2x.last_buttons = ps2x.buttons; //store the previous buttons states
        ps2x.buttons = (ps2x.PS2data[4] << 8) + ps2x.PS2data[3]; //store as one value for multiple functions
        //Serial.print(F("Free Ram: "));
        //Serial.println(system_get_free_heap_size());
        PPS++;
        return true;
        }
    }
  }
  return false;
}
void outPutPPS(){
  Serial.println("Packets Per Second: " + String(PPS / 5));
  PPS=0;
}

