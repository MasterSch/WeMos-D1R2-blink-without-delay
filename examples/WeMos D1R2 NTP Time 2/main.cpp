#include <Arduino.h>

// ****************************************************************
// Sketch Esp8266 WiFi NTP Lokalzeit und UTC
// created: Jens Fleischer, 2019-05-05
// last mod: Jens Fleischer, 2020-07-16
// For more information visit: https://fipsok.de
// ****************************************************************
// Hardware: Esp8266
// Software: Esp8266 Arduino Core 2.6.0 - 2.7.4
// Getestet auf: Nodemcu, Wemos D1 Mini Pro, Sonoff Switch, Sonoff Dual
/******************************************************************
  Copyright (c) 2019 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Automatische Umstellung zwischen Sommer- und Normalzeit
/**************************************************************************************/

#include <ESP8266WiFi.h>
#include <TimeLib.h>

struct tm lt;         // http://www.cplusplus.com/reference/ctime/tm/
struct tm utc;

const char* const PROGMEM NTP_SERVER[] = {"fritz.box", "de.pool.ntp.org", "at.pool.ntp.org", "ch.pool.ntp.org", "ptbtime1.ptb.de", "europe.pool.ntp.org"};
const char* const PROGMEM DAY_NAMES[] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
const char* const PROGMEM DAY_SHORT[] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
const char* const PROGMEM MONTH_NAMES[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char* const PROGMEM MONTH_SHORT[] = {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"};

const char* ssid = "zuhause";             // << kann bis zu 32 Zeichen haben
const char* password = "schmitt.obw@web.de";  // << mindestens 8 Zeichen jedoch nicht länger als 64 Zeichen

void setupTime() {                              // deinen NTP Server einstellen (von 0 - 5 aus obiger Liste) alternativ lassen sich durch Komma getrennt bis zu 3 Server angeben
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("\nVerbunden mit: " + WiFi.SSID());
  configTime("CET-1CEST,M3.5.0,M10.5.0/3", NTP_SERVER[1]);  // Zeitzone einstellen https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  // für Core Versionen vor 2.6.0. folgende Konfiguration verwenden.
  //configTime(0, 0, NTP_SERVER[1]);
  //setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  delay(100);  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.printf("\n\nSketchname: %s\nBuild: %s\t\tIDE: %d.%d.%d\n%s\n\n",
                (__FILE__), (__TIMESTAMP__), ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10, ESP.getFullVersion().c_str());
  /*
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("\nVerbunden mit: " + WiFi.SSID());
  */
  setupTime();
  

}

void loop() {
  static char buf[30];                                    // je nach Format von "strftime" eventuell anpassen
  static time_t lastsec {0};
  time_t now = time(&now);
  localtime_r(&now, &lt);
  gmtime_r(&now, &utc);
  if (lt.tm_sec != lastsec) {
    lastsec = lt.tm_sec;
    if (!(time(&now) % 86400)) {                          // einmal am Tag die Zeit vom NTP Server holen o. jede Stunde "% 3600" aller zwei "% 7200"
      setupTime();
    }
    strftime (buf, sizeof(buf), "%d.%m.%Y %T %A", &utc);  // http://www.cplusplus.com/reference/ctime/strftime/
    Serial.printf("Koordinierte  Weltzeit %s\n", buf);
    strftime (buf, sizeof(buf), "%d.%m.%Y %T", &lt);      // http://www.cplusplus.com/reference/ctime/strftime/
    Serial.printf("Mitteleuropäische Zeit %s %s\n", buf, DAY_NAMES[utc.tm_wday]);
    Serial.printf("UTC: %.2d:%.2d:%.2d\n", utc.tm_hour, utc.tm_min, utc.tm_sec);
    Serial.printf("%s: %.2d:%.2d:%.2d %s\n\n", *tzname, lt.tm_hour, lt.tm_min, lt.tm_sec, lt.tm_isdst ? "Sommerzeit" : "Normalzeit");
  }
}



/*
  NTP TZ DST - bare minimum
  NetWork Time Protocol - Time Zone - Daylight Saving Time

  Our target for this MINI sketch is:
  - get the SNTP request running
  - set the timezone
  - (implicit) respect daylight saving time
  - how to "read" time to be printed to Serial.Monitor
  
  This example is a stripped down version of the NTP-TZ-DST (v2)
  And works for ESP8266 core 2.7.4

  by noiasca
  2020-09-22
*/
/*
// Necessary Includes //
#include <ESP8266WiFi.h>            // we need wifi to get internet access
#include <TimeLib.h>                   // time() ctime()


#ifndef STASSID
#define STASSID "zuhause"                            // set your SSID
#define STAPSK  "schmitt.obw@web.de"                        // set your wifi password
#endif

// Configuration of NTP //
#define MY_NTP_SERVER "fritz.box"
//#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   


// Globals //
//time_t now;                         // this is the epoch
tm tm;                              // the structure tm holds time information in a more convient way

void showTime() {
  time_t now();                       // read the current time
  localtime_r(time_t, tm);           // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);      // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);         // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);         // days since Sunday 0-6
  if (tm.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nNTP TZ DST - bare minimum");

  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!

  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ( "." );
  }
  Serial.println("\nWiFi connected");
  // by default, the NTP will be started after 60 secs
}

void loop() {
  showTime();
  delay(1000); // dirty delay
}

*/
