
#include "Arduino.h"

#include "TM1638GS.h"

// I/O pins on the Arduino connected to strobe, clock, data
// (power should go to 3.3v and GND)
// TM1638GS LedAndKey(14,12,13);   // D5 D6 D7  WeMos D1R2  
// pinout  https://www.addicore.com/D1-DevBoard-p/ad286.htm

TM1638GS LedAndKey(14,12,13);   // D5 D6 D7     WeMos D1R2
//TM1638GS LedAndKey(5,6,7);    // D5 D6 D7     Leonardo

char string[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtuUVvWwXxYyZz";
int end = sizeof(string) - 8;
char text[9] =  "        ";



void setup() {

Serial.begin(115200);
delay(3000);

Serial.println("___________start_____________");

LedAndKey.reset();

}



void loop() {

  int x = 0;
  
  LedAndKey.set_Chars(0,"12345678");
  LedAndKey.setColorLED(0, 1);
  LedAndKey.setColorLED(1, 2);
  LedAndKey.setColorLED(6, 2);
  LedAndKey.setColorLED(7, 1);
  delay(2000);
  LedAndKey.set_Chars(0,"--------");

  delay(2000);
  LedAndKey.reset();    
  delay(2000);
  
  //const char* pos = string;
  //Chars(pos, 0);
  
  LedAndKey.set_Chars(0,"bright..");

  x = 0;
  do {
  LedAndKey.setColorLED(x, 1);
  LedAndKey.set_Brightness(x);
  LedAndKey.set_Char(7, 48+x);
  delay(400);
  x++;
  } while(x<8); 

  LedAndKey.set_Brightness(3);
  LedAndKey.reset();    

  //delay(2000);
  LedAndKey.clear_digits();
  delay(2000);

     x = 0;
     do {
     LedAndKey.set_Hex(x, x+8);
     x++;
     } while(x<8); 
     delay(2000);
     LedAndKey.clear_digits();
     delay(2000);

    strcpy(string, "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");
    int y = 0;
    while (string[y]) { 
      y++; 
    };
    if (y < 8) y = 8;
    x = 0;
    do {
    char* position = string + x;
    LedAndKey.set_Chars(0, position);
    if (x == 0) delay(2000);
      else  delay(200);
      x++;
    } while (x+7 < y);
      delay(3000);
      LedAndKey.clear_digits();
      delay(2000);


      LedAndKey.setColorLEDs(0xF0, 0x0F);   //<-- left byte green, right byte red
      delay(2000);
      LedAndKey.setColorLEDs(0x55, 0x00);   //<-- left byte green, right byte red
      delay(2000);
      LedAndKey.setColorLEDs(0x00, 0xAA);   //<-- left byte green, right byte red
      delay(2000);
      LedAndKey.setColorLEDs(0x55, 0xAA);   //<-- left byte green, right byte red
      delay(2000);
      LedAndKey.clear_LEDs();
      x = 0;
      do {
      LedAndKey.setColorLED(x, 1);
      delay(150);
      LedAndKey.setColorLED(x, 0);
      x++;
      } while(x<8); 
      
      do {
      x--;  
      LedAndKey.setColorLED(x, 2);
      delay(150);
      LedAndKey.setColorLED(x, 0);
      } while(x != 0); 
  
      x = 0;
      do {
      LedAndKey.setColorLED(x, 3);
      delay(150);
      LedAndKey.setColorLED(x, 0);
      x++;
      } while(x<8); 
       delay(2000);

      unsigned long previousMillis = millis();
      LedAndKey.set_Chars(0,"button  ");
      
      while(1) {
      uint8_t value = LedAndKey.readButtons();
      if (value == 0) LedAndKey.set_Char(7, 32);
      for (uint8_t position = 0; position < 8; position++) {
      LedAndKey.setColorLED(position, value & 1);
       if (value & 1) LedAndKey.set_Char(7, 49+position);
       value = value >> 1;
         }
         delay(50);        
        if ((millis() - previousMillis) >= 20000) break;
      }
      LedAndKey.reset();    
      delay(2000);

}

