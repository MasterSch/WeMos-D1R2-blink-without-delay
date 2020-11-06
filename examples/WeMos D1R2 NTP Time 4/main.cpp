#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>


const char *Monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char *Wochentag[7] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

const char* ssid = "zuhause";
const char* password = "schmitt.obw@web.de";


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(50);
  }
  configTime("CET-1CEST,M3.5.0,M10.5.0/3", "fritz.box");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(50);
  }
  Serial.println("");
}

void loop() {
  /*
  time_t now = time(nullptr);
  Serial.print(now);
  Serial.print("   ");
  Serial.println(ctime(&now));
  */
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  char buffer [80];
  strftime (buffer,80,"heute ist  %A, der %e. %B %Y  --- %R:%S Uhr",timeinfo);
  Serial.println(buffer);

  printf ("Current local time and date: %s", asctime(timeinfo));

  int sek = timeinfo->tm_sec;
  int min = timeinfo->tm_min;
  int std = timeinfo->tm_hour;
  int tag = timeinfo->tm_mday;
  int mon = timeinfo->tm_mon;
  int jahr = timeinfo->tm_year + 1900;
  int wtag = timeinfo->tm_wday;
  Serial.print("heute ist:  ");
  Serial.print(Wochentag[wtag]);
  Serial.print(", der ");
  Serial.print(tag);
  Serial.print(". ");
  Serial.print(Monat[mon]);
  Serial.print(" ");
  Serial.print(jahr);
  Serial.print(" --- ");
  Serial.print(std);
  Serial.print(":");
  if (min < 10) Serial.print("0");
  Serial.print(min);
  Serial.print(":");
  if (sek < 10) Serial.print("0");
  Serial.print(sek);
  Serial.println(" Uhr");

  delay(1000);

}