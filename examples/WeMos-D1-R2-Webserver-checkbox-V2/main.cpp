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
static char request[80], request1[80];
int value = LOW, base = 0x40, Relay1 = 0;
byte relayState = 0, status1 = 0, keyOld = 0, keyNew = 0, keyPressTime = 0;
unsigned long starttime, prevTime = 0;
int state = 0, waitTime = 0, x = 0, end = 0;
char displayBuffer[20] = "", Relay90 = '0';
const char* pos;

 
void WiFi_Traffc(void); 
void sendHomepage(void);
void sendErrpage(void);
void setRelay(void);
void DoIt(void);
void processKey(void);
int Find_Start(const char *, const char *);


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
 
 while (millis() - prevTime > 50)     // each 50ms tm1638 check
    prevTime += 50;
  

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  IPAddress ip = WiFi.localIP();
  sprintf(displayBuffer, "IP = %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  Serial.println(displayBuffer);
  state = 1;
 
}
 
void loop() {

 WiFi_Traffc();
 
 if (millis() - prevTime > 50)  {   // each 50ms tm1638 check
 //prevTime = millis();    // timer restart
 prevTime += 50;
 if (state > 0)   DoIt();
 processKey();
 }

}     //    end loop


void WiFi_Traffc()  {
  char my_char;
  int htmlPtr = 0;
  int myIndex;
  int temp1 = 0;
  unsigned long my_timeout;

  // Check if a client has connected
  client = server.available();
  if (!client)  {
    return;
  }

  my_timeout = millis() + 40L;
  while (!client.available() && (millis() < my_timeout) ) delay(3);
  delay(3);
  if (millis() > my_timeout)  {
    return;
  }
  //---------------------------------------------------------------------
  htmlPtr = 0;
  my_char = 0;
    // Read the first line of the request
  while (client.available() && my_char != '\r') {
    my_char = client.read();
    request[htmlPtr++] = my_char;
  }
  client.flush();
  request[htmlPtr] = 0;

  Serial.print("request: ");
  Serial.println(request);
 
 if(strcmp(request, request1) == 0) {
      Serial.print("refresh!!!"); 
      sendHomepage();
     return;
 }

  // Match the request
    if (Find_Start("/favicon", request) > -1) {
    sendErrpage();
    } else {
if (Find_Start("GET / ", request) > -1) {
     sendHomepage();
   }
   else {
 myIndex = Find_Start("/?rel", request);
 if (myIndex > -1) {
   status1 = 0;
do  {
  Serial.print("myIndex = ");
  Serial.println(myIndex);
  my_char = request[myIndex + 6];
  Serial.print("relay # = ");
  Serial.println(my_char);
  temp1 = my_char - 48;
  if ((temp1 > 9) || (temp1 < 0))
  {
    Serial.println("--- error--- ");
    sendErrpage();
    return;
  }
  Serial.print("temp1 = ");
  Serial.println(temp1);
  my_char = request[myIndex + 7];
  Serial.print("trenner = ");
  Serial.println(my_char);
  if ((temp1 < 9) && (temp1 > 0)) bitSet(status1, temp1 - 1);
  myIndex+=6;
  } while (my_char == 38);   // = &
  temp1 = (relayState ^ status1);
  Serial.print("xor = ");
  Serial.println(temp1);
  htmlPtr = 0;
  while (temp1 > 0) {
    temp1 = temp1 >> 1;
    htmlPtr += 1;
  }
  Serial.print("Relay ");
  Serial.print(htmlPtr);
  sprintf(displayBuffer, "rELAy %d", htmlPtr);
  if (bitRead(status1, htmlPtr - 1) == 1)
   {
     Serial.println(" ein ");
     strcat(displayBuffer, " On");
   } else {  
     Serial.println(" aus ");
     strcat(displayBuffer, " Off");
   }
  state = 1;
  relayState = status1;
  setRelay();
  strcpy(request1, request);
  if (Relay90 == '0') Relay90 = '9'; else Relay90 = '0';
     sendHomepage();
     } else    sendErrpage();
    }  
  }
}  


void sendErrpage()
 {
 Serial.println("");
 Serial.println("Sende 404 not found Fehler....");
 client.print(F("HTTP/1.1 404 Not found\r\n Content-Type:text/html\r\n Connection:close\r\n\r\n"));    
 client.print(F("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
 client.print(F("<title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>"));
 }

//---------------------------------------------------------------------
int Find_Start(const char * such, const char * str) {
  int tmp = -1;
  int ww = strlen(str) - strlen(such);
  int ll = strlen(such);

  for (int i = 0; i <= ww && tmp == -1; i++) {
    if (strncmp(such, &str[i], ll) == 0) tmp = i;
  }
  return tmp;
}

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
 state = 0;
break;

   }
}

void processKey() {
keyNew = LedAndKey.get_buttons();
if (keyNew == 0) {
   if ((keyPressTime > 0) && (keyPressTime < 8)) {  // short keypress
   strcpy(displayBuffer,"Relay ");
   x = 0;
   while (x < 8)  {
    if (bitRead(keyOld, x) == 1) break;
   x++;
   }
  sprintf(displayBuffer, "rELAy %d", x+1);
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
if (debug) Serial.println("Header sent"); 
    client.print(F("<!DOCTYPE html>"));
    client.print(F("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
    client.print(F("<meta http-equiv=\"refresh\" content=\"10\"></head>"));  // geä
    client.print(F("<body bgcolor=\"#adcede\"><font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">"));
    client.print(F("<br><center><h1>WeMos D1R2 Webserver</h1></center>"));
  //-----------------------------------------------------------------------------------------
  // Checkboxen"));
    client.print(F("<form method = get><center><body><br>"));
if (debug) Serial.println("sending Buttons....");
char j = ' ';
int i = 1; 
while (i < 8) {
client.print(F("<div>Relay "));
j = i + 48;
client.print(j);
client.print(F(" <input type=\"checkbox\" name=\"rel\" value= \""));
client.print(j);
client.print(F("\" onclick=\"submit();\""));
if (bitRead(relayState, i - 1) == 1) client.print(F("checked=\"checked\""));
client.print(F("> &nbsp;&nbsp;"));
i++;
j++;
client.print(F("<input type=\"checkbox\" name=\"rel\" value= \""));
client.print(j);
client.print(F("\" onclick=\"submit();\""));
if (bitRead(relayState, i - 1) == 1) client.print(F("checked=\"checked\""));
client.print(F("> Relay "));
client.print(j);
client.print(F("</div><br><br>"));
i++;
j++; 
} 
client.print(F("<div>with <input type=\"checkbox\" name=\"rel\" value= \""));
client.print(Relay90);
client.print(F("\" checked=\"checked\"> checkboxes</div></form>"));

if (debug) Serial.println("sending bottom.....");
client.print(F("<br><hr><a id=\"google\" href=\"http://google.de\">Google.de</a>\r\n\r\n</center></body></html>"));
if (debug) Serial.println("homepage sent");     
         
}

