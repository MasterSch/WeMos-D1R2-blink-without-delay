#include <Arduino.h>



#include <ESP8266WiFi.h>
#include <TimeLib.h>

const char* ssid       = "zuhause";
const char* password   = "schmitt.obw@web.de";

const char* ntpServer1 = "fritz.box";
//const char* ntpServer2 = "1.de.pool.ntp.org";
//const char* ntpServer3 = "2.de.pool.ntp.org";

const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char *Monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char *Wochentag[7] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

unsigned long previousMillis=0, currentMillis=0, intervallMillis = 100 * 60 *60;
int interval = 1000;

struct tm timeinfo;


void printLocalTime()
{
  //struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println(&timeinfo, "%A, %d. %B %Y %H:%M:%S Uhr  ");
  Serial.print(Wochentag[timeinfo.tm_wday]);
  Serial.print(", der ");
  Serial.print(&timeinfo, "%d. ");
  Serial.print(Monat[timeinfo.tm_mon]);
  Serial.println(&timeinfo, " %Y  Zeit: %H:%M:%S Uhr  ");

}

void syncTime()
  {
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(50);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", ntpServer1);
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();
  getLocalTime(&timeinfo);
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  currentMillis = millis();
  while ((currentMillis - previousMillis) >= interval) 
    {
      previousMillis += interval;
    }
  }



void setup()
{

  Serial.begin(115200);

    
  syncTime();
  printLocalTime();

}

void loop()
{
  currentMillis = millis();
  if (currentMillis >= intervallMillis) syncTime();
  if ((currentMillis - previousMillis) >= interval) 
  {
    previousMillis += interval;
 
    getLocalTime(&timeinfo);
    
    printLocalTime();       

  }
}


/*


void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{


delay(10000);

}

*/