/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 64x48 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/
/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char HTML_text[] PROGMEM = R"=====( <!doctype html>
        <html>
        <header><title>This is title</title></header>
        <body>
        Hello world
        </body>
        </html>
         )=====";

//const int led = 13;



/* Set these to your desired credentials. */
const char *ssid = "WheelyFast";
const char *password = "12345678";
byte clientTimeout = 150;
bool clientStopped = true;
unsigned long nextClientTimeout = 0;

//ESP8266WebServer server(80);
WiFiServer server(80);
WiFiClient client;
//
///* Just a little test message.  Go to http://192.168.4.1 in a web browser
//   connected to this access point to see it.
//*/
//void handleRoot() {
////  digitalWrite(led, 1);
//  server.send(200, "text/html", "<h1>You are connected</h1>");
////  digitalWrite(led, 0);
//}


// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);


#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()
{
  // pinMode ( led, OUTPUT );
  //  digitalWrite ( led, 0 );

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 4);
  display.println("idea");
  display.setCursor(15, 24);
  display.println("Hex");
  display.display();

  Serial.begin(115200);
  Serial.println();

  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Configuring access point...");
  display.display();

  Serial.print("Configuring access point...");
  //  /* You can remove the password parameter if you want the AP to be open. */
  //  WiFi.softAP(ssid, password);

  WiFi.mode(WIFI_AP);

  IPAddress myIP = WiFi.softAPIP();



  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AP IP address: ");
  display.println(myIP);
  display.display();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //  server.on("/", handleRoot);
  server.begin();
  //display.clearDisplay();
  //display.setCursor(0,0);
  display.println("HTTP server started");
  display.display();
  Serial.println("HTTP server started");
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();
}


void loop()
{

  /*
    // server.handleClient();

    if (!client.connected())
    {
    // try to connect to a new client
    client = server.available();
    Serial.println("Client NOT connected");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("No Client");
    display.display();
    }
    else
    {
    // read data from the connected client
    if (client.available() > 0)
    {
      //Serial.write(client.read());
      Serial.println("Client connected");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Client");
      display.display();
    }
    }
  */

  // client functions here
  if (clientStopped)
  {
    client = server.available();
    clientStopped = false;
    nextClientTimeout = millis() + clientTimeout;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Client connected");
    display.display();
  }
  else
  {
    if (!client.connected() || millis() > nextClientTimeout)
    {
      client.stop();
      clientStopped = true;
    }
  }

  if (!client.available())
  {
    return;
  }


  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  Serial.println(client.readString());
  client.flush();
  int indexOfX = req.indexOf("/X");
  int indexOfY = req.indexOf("/Y");
  if (indexOfX != -1 && indexOfY != -1)
  {

    String xOffset = req.substring(indexOfX + 2, indexOfX + 8);
    int dX = xOffset.toInt();
    String yOffset = req.substring(indexOfY + 2, indexOfY + 8);
    int dY = yOffset.toInt();

    //      motors.update(dX, dY);

    //Serial.print(F("DX: "));
    //Serial.print(dX);
    //Serial.print(F(" DY: "));
    //Serial.println(dY);
    //Serial.print(F("Free Ram: "));
    //Serial.println(system_get_free_heap_size());

    //      HeartBeatRcvd = true;               // recieved data, must be connected

  }
  else
  {
    if (req.indexOf("GET / HTTP/1.1") != -1)
    {
      Serial.println(F("Sending Page"));
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Sending Page");
      display.display();

      client.write_P(HTML_text, strlen_P(HTML_text));
      //delay(1);                   // to improve compatability with some browsers
    }
  }
}


