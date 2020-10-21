//    WeMos D1 R2 STA Webserver


//    WeMos D1 R2 _Webserver_Jul2019_new V50
//    tm1638 library 
//    but now TM1638GS
//    relays are active low
//    with refresh

//    WeMos D1 R2 mit LED Module an MC23017 Port A
//    LED an bei output Low
//    I2C bus address is 0x20
//    an SDA und SCL

//    TM1638 LED&KEY pins
//    STB      WeMos D1 R2  PIN D5
//    CLK      WeMos D1 R2  PIN D6
//    DAT      WeMos D1 R2  PIN D7


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Wire.h"
#include <TM1638GS.h>

#define debug true    // true means debug messages on serial monitor

const char* ssid = "zuhause";
const char* password = "schmitt.obw@web.de";
char href[15] = "<a href=\"?rel ";

int value = LOW, base = 0x40, Relay1 = 0;
byte relayState = 0, keyOld = 0, keyNew = 0, keyPressTime = 0;
unsigned long starttime, prevTime;
int state = 0, waitTime = 0, x = 0, end = 0, repeat = 0;
char displayBuffer[20] = "";
const char* pos;

 
void sendHomepage(void);
void setRelay(void);
void DoIt(void);
void processKey(void);

WiFiServer server(80);
// Initialize the client library
WiFiClient client;

// I/O pins on the Arduino connected to strobe, clock, data
// (power should go to 3.3v and GND)
TM1638GS LedAndKey(14, 12, 13);   // D5 D6 D7     WeMos D1R2

void setup() {
  Serial.begin(115200);
  delay(10);
 
  LedAndKey.reset();
  LedAndKey.set_Brightness(2);
  LedAndKey.set_Chars(0,"StArt...");

  Wire.begin(); // wake up I2C bus
                // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  
  setRelay();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  IPAddress ip = WiFi.localIP();
  sprintf(displayBuffer, "IP = %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(displayBuffer);
  repeat = 2;
  state = 1;
 
}
 
void loop() {
  // Check if a client has connected
  client = server.available();
  if (client) {
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.print("request: ");
  Serial.println(request);
  client.flush();
 
  // Match the request
 if (request.indexOf("/favicon") == -1) {
 int index = request.indexOf("/?rel");
 if (index != -1) {
   Serial.print("index = ");
   Serial.println(index);
 int Relay = request[index + 5];  
 int onoff = request[index + 6];  
  if (Relay != Relay1)
   {
   Relay1 = Relay;
   base ^= 0x20;    // XOR toggle Bit
   Relay &= 0x0f;
   Serial.print("Relay = ");
   Serial.println(Relay);
   Serial.print("on off = ");
   Serial.println(onoff);
  if ((Relay > 0) && (Relay < 9))
       {
   sprintf(displayBuffer, "Relay %d", Relay);   
  if (onoff == 43){
   bitSet(relayState, Relay -1);
   strcat(displayBuffer, " On");
    }
  if (onoff == 45) {
   bitClear(relayState, Relay -1);
   strcat(displayBuffer, " Off");
            }  
   setRelay(); 
   state = 1;
     }
   }
 }
 sendHomepage();
} else  {
 Serial.println("");
 Serial.println("Sende 404 not found Fehler....");
 client.print(F("HTTP/1.1 404 Not found\r\n Content-Type:text/html\r\n Connection:close\r\n\r\n"));    
 client.print(F("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
 client.print(F("<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>"));
         }
  }
    if (millis() - prevTime > 50)  {   // each 50ms tm1638 check
    prevTime = millis();    // timer restart
    if (state > 0)   DoIt();
    processKey();

   }
}     //    end loop


void DoIt() {

switch (state)  {

case 1:     // init and display message
x = 0;
end = 0;
while (displayBuffer[x] != '\0')
x++;
if (x > 7) {
  end = x - 7;
  x = 0;
}
  else end = x;
LedAndKey.set_Chars(0, displayBuffer);
waitTime = 10;     // 20 * 50ms = 1 sec.
state++;
break;

case 2:     // wait
if (waitTime > 0) waitTime--;
else state++; 
break;

case 3:     //  shift left if needed
if (waitTime > 0) waitTime--; 
else {
waitTime = 5;
 
    if (x == end) {
    waitTime = 40;     // 40 * 50ms = 1 Sec.
    state++; 
    } else {
    pos = displayBuffer + x;
    LedAndKey.set_Chars(0, pos);
    x++;
    }
}
break;

case 4:     // wait again
 if (waitTime > 0) waitTime--;
 else state++; 
break;

case 5:     //  finished
 LedAndKey.clear_digits();
 if (repeat > 0) {
   repeat--;
   waitTime = 40;     // 40 * 50ms = 1 Sec. 
   state++;
 } 
 else state = 0;
break;

case 6:     // wait again
 if (waitTime > 0) waitTime--;
 else state=1; 
break;
   }
}

void processKey() {
keyNew = LedAndKey.get_buttons();
if (keyNew == 0) {
   if ((keyPressTime > 0) && (keyPressTime < 8)) {  // short keypress
   strcpy(displayBuffer,"Relay ");
   x = 0;
   do
   {
    if (bitRead(keyOld, x) == 1) break;
   x++;
   } while (x < 8);
  sprintf(displayBuffer, "Relay %d", x+1);
   if (bitRead(relayState, x) == 1) 
      strcat(displayBuffer, " Off");
   else    strcat(displayBuffer, " On");
   state = 1;
   relayState ^= keyOld;    // XOR toggle Bit
   setRelay();
     }
  keyOld = 0;       
  keyPressTime = 0;
} else {          // key s(till) pressed
if (keyPressTime == 9) {
IPAddress ip = WiFi.localIP();
sprintf(displayBuffer, "IP = %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
state = 1;
}
keyPressTime++;
keyOld = keyNew;
}   

}


 void setRelay() {
 Wire.beginTransmission(0x20);
 Wire.write(0x12); // GPIOA
 Wire.write(~relayState); // port data
 Wire.endTransmission();
 LedAndKey.setColorLEDs(0x00, relayState);  // <--  green
}
 

void sendHomepage()  { 

//clearSerialBuffer();
if (debug) Serial.println("...............sending homepage..........");
            
if (debug) Serial.println("sending Header.....");    

   
    client.print(F("HTTP/1.1 200 OK\r\n Server: Arduino\r\n Content-Type:text/html\r\n Connection:close\r\n\r\n"));
    client.print(F("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
    client.print(F("<meta http-equiv=\"refresh\" content=\"10\">"));  // geä
    client.print(F("<style>* {color:#ff0} h1 {background:#000} button{width:130px;height:40px; font-size:16px; border:none;"));
    client.print(F("cursor:pointer;border-radius:5px; margin-right: 15px;} .an1{background:#4CAF50; color:white;}"));
    client.print(F(".an0{background:silver;color:black;} #google{color:blue;font-size:26px;}</style></head>"));
    client.print(F("<body><center><br><br><h1>ESP8266 WiFi Web Server V50 </h1><br><br>"));

if (debug) Serial.println("Header sent");     

if (debug) Serial.println("sending Buttons....");

int i = 1; 
do {            
href[13] = base + i;
client.print(href);
if (bitRead(relayState, i - 1) == 1) client.print('-');
     else client.print('+');
client.print(F("\"><button class=\"an"));
if (bitRead(relayState, i - 1) == 1) client.print('1');
     else client.print('0');
client.print(F("\">Relay "));
client.print(i);
if (bitRead(relayState, i - 1) == 1) client.print(" ON ");
     else client.print(" OFF");
client.print(F("</button></a>"));
i++;

href[13] = base + i;
client.print(href);
if (bitRead(relayState, i - 1) == 1) client.print('-');
     else client.print('+');
client.print(F("\"><button class=\"an"));
if (bitRead(relayState, i - 1) == 1) client.print('1');
     else client.print('0');
client.print(F("\">Relay "));
client.print(i);
if (bitRead(relayState, i - 1) == 1) client.print(" ON ");
     else client.print(" OFF");
client.print(F("</button></a><br><br>")); 
  i++; 
} while (i < 8);
    if (debug) Serial.println("sending bottom.....");
    client.print(F("<br><br><hr><a id=\"google\" href=\"http://google.de\">Google.de</a>\r\n\r\n"));
    if (debug) Serial.println("homepage sent");     
         
}

