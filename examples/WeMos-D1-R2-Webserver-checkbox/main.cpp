//---------------------------------------------------------------------
//ESP866 HTML Demo 02
//---------------------------------------------------------------------
// Author  : Hubert Baumgarten
//---------------------------------------------------------------------
#include <Arduino.h>
#include <ESP8266WiFi.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


#define DEBUG 1

void WiFi_Start_STA(void);
void WiFi_Start_AP(void);
void WiFI_Traffic(void);
void make_HTML01(void);
void send_not_found(void);
void send_HTML(void);
void set_colgroup(int, int, int, int, int);
void set_colgroup1(int);
void strcati(char*, int);
void strcati2(char*, int);
int Pick_Parameter_Zahl(const char *, char *);
int Find_End(const char *, const char *);
int Find_Start(const char *, const char *);
int Pick_Dec(const char *, int);
int Pick_N_Zahl(const char *, char, byte);
int Pick_Hex(const char *, int);
void Pick_Text(char *, char  *, int);
char HexChar_to_NumChar( char);
void exhibit(const char *, int);
void exhibit(const char *, unsigned int);
void exhibit(const char *, unsigned long);
void exhibit(const char *, const char *);


//---------------------------------------------------------------------
// WiFi

byte my_WiFi_Mode = 0;  // WIFI_STA = 1 = Workstation  WIFI_AP = 2  = Accesspoint

const char * ssid_sta     = "zuhause";
const char * password_sta = "schmitt.obw@web.de";

const char * ssid_ap      = "WeMos_Webserver";
const char * password_ap  = "";    // alternativ :  = "12345678";

WiFiServer server(80);
WiFiClient client;

#define MAX_PACKAGE_SIZE 2048
static char HTML_String[2500];
static char HTTP_Header[100];

//---------------------------------------------------------------------
// Allgemeine Variablen

byte status = 0;
int Aufruf_Zaehler = 0;

#define ACTION_OK 1
#define ACTION_NOTOK 2
#define ACTION_SET_DATE_TIME 3
#define ACTION_SET_NAME 4
#define ACTION_LIES_AUSWAHL 5
#define ACTION_LIES_VOLUME 6

int action;

// Vor- Nachname
char Vorname[20] = "Guenter";
char Nachname[20] = "Schmitt";

// Uhrzeit Datum
byte Uhrzeit_HH = 16;
byte Uhrzeit_MM = 47;
byte Uhrzeit_SS = 0;
byte Datum_TT = 9;
byte Datum_MM = 2;
int Datum_JJJJ = 2016;

// checkboxen
char Wochentage_tab[7][3] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
byte Wochentage = 0;

// Radiobutton
char Jahreszeiten_tab[4][15] = {"Fr&uuml;hling", "Sommer", "Herbst", "Winter"};
byte Jahreszeit = 0;

// Combobox
char Wetter_tab[4][10] = {"Sonne", "Wolken", "Regen", "Schnee"};
byte Wetter;


// Slider
byte Volume = 15;

char tmp_string[20];
//---------------------------------------------------------------------
void setup() {
#ifdef DEBUG
  Serial.begin(115200);

  for (int i = 5; i > 0; i--) {
    Serial.print("Warte ");
    Serial.print(i);
    Serial.println(" sec");
    delay(1000);
  }
  Serial.println("WeMos D1R1 Server");
#endif

  //---------------------------------------------------------------------
  // WiFi starten

  WiFi_Start_STA();
  if (my_WiFi_Mode == 0) WiFi_Start_AP();

}

//---------------------------------------------------------------------
void loop() {

  WiFI_Traffic();
  delay(10);
}

//---------------------------------------------------------------------
void WiFi_Start_STA() {
  unsigned long timeout;

  WiFi.mode(WIFI_STA);   //  Workstation
  WiFi.begin(ssid_sta, password_sta);
  
  timeout = millis() + 12000L;
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
    delay(10);
  }

  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    my_WiFi_Mode = WIFI_STA;

#ifdef DEBUG
    Serial.print("Connected IP - Address : ");
    for (int i = 0; i < 3; i++) {
      Serial.print( WiFi.localIP()[i]);
      Serial.print(".");
    }
    Serial.println(WiFi.localIP()[3]);
#endif


  } else {
    WiFi.mode(WIFI_OFF);
#ifdef DEBUG
    Serial.println("WLAN-Connection failed");
#endif

  }
}

//---------------------------------------------------------------------
void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  WiFi.softAP(ssid_ap, password_ap);
  server.begin();
  IPAddress myIP = WiFi.softAPIP();
  my_WiFi_Mode = WIFI_AP;

#ifdef DEBUG
  Serial.print("Accesspoint started - Name : ");
  Serial.print(ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(myIP);
#endif
}
//---------------------------------------------------------------------
void WiFI_Traffic() {
  char my_char;
  int htmlPtr = 0;
  int myIndex;
  unsigned long my_timeout;

  // Check if a client has connected
  client = server.available();
  if (!client)  {
    return;
  }

  my_timeout = millis() + 250L;
  while (!client.available() && (millis() < my_timeout) ) delay(10);
  delay(10);
  if (millis() > my_timeout)  {
    return;
  }
  //---------------------------------------------------------------------
  htmlPtr = 0;
  my_char = 0;
  while (client.available() && my_char != '\r') {
    my_char = client.read();
    HTML_String[htmlPtr++] = my_char;
  }
  client.flush();
  HTML_String[htmlPtr] = 0;
#ifdef DEBUG
  exhibit ("Request : ", HTML_String);
#endif

  Aufruf_Zaehler++;



  if (Find_Start ("/?", HTML_String) < 0 && Find_Start ("GET / HTTP", HTML_String) < 0 ) {
    send_not_found();
    return;
  }

  //---------------------------------------------------------------------
  // Benutzereingaben einlesen und verarbeiten
  //---------------------------------------------------------------------
int temp1 = 0;
int status1 = 0;
myIndex = (Find_Start ("rel=", HTML_String));
if (myIndex > -1) {                                // rel= gefunden weiter ansonst Homepage senden
  do  {
  exhibit("myIndex = ", myIndex);
  my_char = HTML_String[myIndex + 4];
  exhibit("mychar = ", my_char);
  temp1 = my_char - 49;
  exhibit("temp1 = ", temp1);
  my_char = HTML_String[myIndex + 5];
  exhibit("mychar = ", my_char);
  if (temp1 < 9) bitSet(status1, temp1);
  myIndex+=6;
  } while (my_char == 38);
  status = status1;
  }















  //---------------------------------------------------------------------
  //Antwortseite aufbauen

  make_HTML01();

  //---------------------------------------------------------------------
  // Header aufbauen
  strcpy(HTTP_Header, "HTTP/1.1 200 OK\r\nContent-Length: ");
  strcati(HTTP_Header, strlen(HTML_String));
  strcat(HTTP_Header, "\r\nContent-Type: text/html\r\nConnection: close\r\n");
  strcat(HTTP_Header, "\r\n");

#ifdef DEBUG
  exhibit("Header : ", HTTP_Header);
  Serial.print("Header :  ");
  Serial.println(HTTP_Header);
  exhibit("Laenge Header : ", strlen(HTTP_Header));
  exhibit("Laenge HTML...: ", strlen(HTML_String));
#endif

  client.print(HTTP_Header);
  delay(10);

  send_HTML();

}

//---------------------------------------------------------------------
// HTML Seite 01 aufbauen
//---------------------------------------------------------------------
void make_HTML01() {

  strcpy( HTML_String, "<!DOCTYPE html>");
  strcat( HTML_String, "<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  strcat( HTML_String, "<meta http-equiv=\"refresh\" content=\"10\">");
  strcat( HTML_String, "</head>");
  strcat( HTML_String, "<body bgcolor=\"#adcede\">");
  strcat( HTML_String, "<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">");
  strcat( HTML_String, "<br><center><h1>WeMos D1R2 Webserver</h1></center>");
  //-----------------------------------------------------------------------------------------
  // Checkboxen

  strcat( HTML_String, "<form method = get><center><br>");
  
  strcat( HTML_String, "<div>Relay 1 <input type=\"checkbox\" name=\"rel\" value= \"1\" onclick=\"submit();\""); 
  if (bitRead(status,0) == 1) strcat( HTML_String, "checked=\"checked\"");
  strcat( HTML_String, "> &nbsp;&nbsp;");
  strcat( HTML_String, "<input type=\"checkbox\" name=\"rel\" value= \"2\" onclick=\"submit();\"");
  if (bitRead(status,1) == 1) strcat( HTML_String, "checked=\"checked\""); 
  strcat( HTML_String, "> Relay 2</div><br><br>");
    
  strcat( HTML_String, "<div>Relay 3 <input type=\"checkbox\" name=\"rel\" value= \"3\" onclick=\"submit();\""); 
  if (bitRead(status,2) == 1) strcat( HTML_String, "checked=\"checked\"");
  strcat( HTML_String, "> &nbsp;&nbsp;");
  strcat( HTML_String, "<input type=\"checkbox\" name=\"rel\" value= \"4\" onclick=\"submit();\"");
  if (bitRead(status,3) == 1) strcat( HTML_String, "checked=\"checked\""); 
  strcat( HTML_String, "> Relay 4</div><br><br>");

  strcat( HTML_String, "<div>Relay 5 <input type=\"checkbox\" name=\"rel\" value= \"5\" onclick=\"submit();\""); 
  if (bitRead(status,4) == 1) strcat( HTML_String, "checked=\"checked\"");
  strcat( HTML_String, "> &nbsp;&nbsp;");
  strcat( HTML_String, "<input type=\"checkbox\" name=\"rel\" value= \"6\" onclick=\"submit();\"");
  if (bitRead(status,5) == 1) strcat( HTML_String, "checked=\"checked\""); 
  strcat( HTML_String, "> Relay 6</div><br><br>");

  strcat( HTML_String, "<div>Relay 7 <input type=\"checkbox\" name=\"rel\" value= \"7\" onclick=\"submit();\""); 
  if (bitRead(status,6) == 1) strcat( HTML_String, "checked=\"checked\"");
  strcat( HTML_String, "> &nbsp;&nbsp;");
  strcat( HTML_String, "<input type=\"checkbox\" name=\"rel\" value= \"8\" onclick=\"submit();\"");
  if (bitRead(status,7) == 1) strcat( HTML_String, "checked=\"checked\""); 
  strcat( HTML_String, "> Relay 8</div><br><br>");
  strcat( HTML_String, "<input type=\"checkbox\" name=\"rel\" value= \"9\" checked=\"checked\"> Checkbox Test");
  strcat( HTML_String, "</center></form>"); 

  strcat( HTML_String, "</body>");
  strcat( HTML_String, "</html>");
}

//--------------------------------------------------------------------------
void send_not_found() {
#ifdef DEBUG
  Serial.println("Sende Not Found");
#endif
  client.print("HTTP/1.1 404 Not Found\r\n\r\n");
  delay(10);
  client.stop();
}

//--------------------------------------------------------------------------
void send_HTML() {
  char my_char;
  int  my_len = strlen(HTML_String);
  int  my_ptr = 0;
  int  my_send = 0;

  //--------------------------------------------------------------------------
  // in Portionen senden
  while ((my_len - my_send) > 0) {
    my_send = my_ptr + MAX_PACKAGE_SIZE;
    if (my_send > my_len) {
      client.print(&HTML_String[my_ptr]);
      delay(10);
#ifdef DEBUG
      Serial.println(&HTML_String[my_ptr]);
#endif
      my_send = my_len;
    } else {
      my_char = HTML_String[my_send];
      // Auf Anfang eines Tags positionieren
      while ( my_char != 'y') my_char = HTML_String[--my_send];
      HTML_String[my_send] = 0;
      client.print(&HTML_String[my_ptr]);
      delay(20);
#ifdef DEBUG
      Serial.println(&HTML_String[my_ptr]);
#endif
      HTML_String[my_send] =  my_char;
      my_ptr = my_send;
    }
  }
 
 // client.println(HTML_String); 
  client.stop();
}

//----------------------------------------------------------------------------------------------
void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
  strcat( HTML_String, "<colgroup>");
  set_colgroup1(w1);
  set_colgroup1(w2);
  set_colgroup1(w3);
  set_colgroup1(w4);
  set_colgroup1(w5);
  strcat( HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
  if (ww == 0) return;
  strcat( HTML_String, "<col width=\"");
  strcati( HTML_String, ww);
  strcat( HTML_String, "\">");
}


//---------------------------------------------------------------------
void strcati(char* tx, int i) {
  char tmp[8];

  itoa(i, tmp, 10);
  strcat (tx, tmp);
}

//---------------------------------------------------------------------
void strcati2(char* tx, int i) {
  char tmp[8];

  itoa(i, tmp, 10);
  if (strlen(tmp) < 2) strcat (tx, "0");
  strcat (tx, tmp);
}

//---------------------------------------------------------------------
int Pick_Parameter_Zahl(const char * par, char * str) {
  int myIdx = Find_End(par, str);

  if (myIdx >= 0) return  Pick_Dec(str, myIdx);
  else return -1;
}
//---------------------------------------------------------------------
int Find_End(const char * such, const char * str) {
  int tmp = Find_Start(such, str);
  if (tmp >= 0)tmp += strlen(such);
  return tmp;
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
//---------------------------------------------------------------------
int Pick_Dec(const char * tx, int idx ) {
  int tmp = 0;

  for (int p = idx; p < idx + 5 && (tx[p] >= '0' && tx[p] <= '9') ; p++) {
    tmp = 10 * tmp + tx[p] - '0';
  }
  return tmp;
}
//----------------------------------------------------------------------------
int Pick_N_Zahl(const char * tx, char separator, byte n) {

  int ll = strlen(tx);
  byte anz = 1;
  byte i = 0;
  while (i < ll && anz < n) {
    if (tx[i] == separator)anz++;
    i++;
  }
  if (i < ll) return Pick_Dec(tx, i);
  else return -1;
}

//---------------------------------------------------------------------
int Pick_Hex(const char * tx, int idx ) {
  int tmp = 0;

  for (int p = idx; p < idx + 5 && ( (tx[p] >= '0' && tx[p] <= '9') || (tx[p] >= 'A' && tx[p] <= 'F')) ; p++) {
    if (tx[p] <= '9')tmp = 16 * tmp + tx[p] - '0';
    else tmp = 16 * tmp + tx[p] - 55;
  }

  return tmp;
}

//---------------------------------------------------------------------
void Pick_Text(char * tx_ziel, char  * tx_quelle, int max_ziel) {

  int p_ziel = 0;
  int p_quelle = 0;
  int len_quelle = strlen(tx_quelle);

  while (p_ziel < max_ziel && p_quelle < len_quelle && tx_quelle[p_quelle] && tx_quelle[p_quelle] != ' ' && tx_quelle[p_quelle] !=  '&') {
    if (tx_quelle[p_quelle] == '%') {
      tx_ziel[p_ziel] = (HexChar_to_NumChar( tx_quelle[p_quelle + 1]) << 4) + HexChar_to_NumChar(tx_quelle[p_quelle + 2]);
      p_quelle += 2;
    } else if (tx_quelle[p_quelle] == '+') {
      tx_ziel[p_ziel] = ' ';
    }
    else {
      tx_ziel[p_ziel] = tx_quelle[p_quelle];
    }
    p_ziel++;
    p_quelle++;
  }

  tx_ziel[p_ziel] = 0;
}
//---------------------------------------------------------------------
char HexChar_to_NumChar( char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 55;
  return 0;
}

#ifdef DEBUG
//---------------------------------------------------------------------
void exhibit(const char * tx, int v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned int v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, unsigned long v) {
  Serial.print(tx);
  Serial.println(v);
}
//---------------------------------------------------------------------
void exhibit(const char * tx, const char * v) {
  Serial.print(tx);
  Serial.println(v);
}
#endif

/*
  action = Pick_Parameter_Zahl("ACTION=", HTML_String);

  // Vor und Nachname
  if ( action == ACTION_SET_NAME) {

    myIndex = Find_End("VORNAME=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(Vorname, &HTML_String[myIndex], 20);
#ifdef DEBUG
      exhibit ("Vorname  : ", Vorname);
#endif
    }

    myIndex = Find_End("NACHNAME=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(Nachname, &HTML_String[myIndex], 20);
#ifdef DEBUG
      exhibit ("Nachname  : ", Nachname);
#endif
    }
  }
  // Uhrzeit und Datum
  if ( action == ACTION_SET_DATE_TIME) {
    // UHRZEIT=12%3A35%3A25
    myIndex = Find_End("UHRZEIT=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(tmp_string, &HTML_String[myIndex], 8);
      Uhrzeit_HH = Pick_N_Zahl(tmp_string, ':', 1);
      Uhrzeit_MM = Pick_N_Zahl(tmp_string, ':', 2);
      Uhrzeit_SS = Pick_N_Zahl(tmp_string, ':', 3);
#ifdef DEBUG
      Serial.print("Neue Uhrzeit ");
      Serial.print(Uhrzeit_HH);
      Serial.print(":");
      Serial.print(Uhrzeit_MM);
      Serial.print(":");
      Serial.println(Uhrzeit_SS);
#endif
    }
    // DATUM=2015-12-31
    myIndex = Find_End("DATUM=", HTML_String);
    if (myIndex >= 0) {
      Pick_Text(tmp_string, &HTML_String[myIndex], 10);
      Datum_JJJJ = Pick_N_Zahl(tmp_string, '-', 1);
      Datum_MM = Pick_N_Zahl(tmp_string, '-', 2);
      Datum_TT = Pick_N_Zahl(tmp_string, '-', 3);
#ifdef DEBUG
      Serial.print("Neues Datum ");
      Serial.print(Datum_TT);
      Serial.print(".");
      Serial.print(Datum_MM);
      Serial.print(".");
      Serial.println(Datum_JJJJ);
#endif
    }
  }


  if ( action == ACTION_LIES_AUSWAHL) {
    Wochentage = 0;
    for (int i = 0; i < 7; i++) {
      strcpy( tmp_string, "WOCHENTAG");
      strcati( tmp_string, i);
      strcat( tmp_string, "=");
      if (Pick_Parameter_Zahl(tmp_string, HTML_String) == 1)Wochentage |= 1 << i;
    }
    Jahreszeit = Pick_Parameter_Zahl("JAHRESZEIT=", HTML_String);
    Wetter = Pick_Parameter_Zahl("WETTER=", HTML_String);

  }

  if ( action == ACTION_LIES_VOLUME) {
    Volume = Pick_Parameter_Zahl("VOLUME=", HTML_String);
  }

*/
