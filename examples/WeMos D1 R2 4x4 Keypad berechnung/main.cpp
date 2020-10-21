#include <Arduino.h>





void setup() {
  // put your setup code here, to run once:

#define C1 0
#define C2 R2
#define C3 R2 + R3
#define C4 R3 + R3 + R4
#define Z1 R5 + R6 + R7
#define Z2 R6 + R7
#define Z3 R7
#define Z4 0



Serial.begin(115200);
delay(3000);

Serial.println("Start......");

}


void loop() {
  // put your main code here, to run repeatedly:



float R1 = 15000;
float R2 = 1500, R3 = 1500, R4 = 15000;
float R5 = 4700, R6 = 4700, R7 = 2200;


/*
float R1 = 4700;
float R2 = 390, R3 = 390, R4 = 390;
float R5 = 1500, R6 = 1500, R7 = 1500;
*/

/*
float R1 = 3300;
float R2 = 390, R3 = 390, R4 = 390;
float R5 = 1500, R6 = 1500, R7 = 1500;
*/

float Vin = 5;
float Vout;


Serial.print("Vout = ");
Vout = Vin /(R1 + C1 + Z1) * (C1 + Z1);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C2 + Z1) * (C2 + Z1);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C3 + Z1) * (C3 + Z1);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C4 + Z1) * (C4 + Z1);
Serial.println(Vout);
Serial.println();

  
Serial.print("Vout = ");
Vout = Vin /(R1 + C1 + Z2) * (C1 + Z2);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C2 + Z2) * (C2 + Z2);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C3 + Z2) * (C3 + Z2);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C4 + Z2) * (C4 + Z2);
Serial.println(Vout);
Serial.println();

  
Serial.print("Vout = ");
Vout = Vin /(R1 + C1 + Z3) * (C1 + Z3);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C2 + Z3) * (C2 + Z3);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C3 + Z3) * (C3 + Z3);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C4 + Z3) * (C4 + Z3);
Serial.println(Vout);
Serial.println();


    
Serial.print("Vout = ");
Vout = Vin /(R1 + C1 + Z4) * (C1 + Z4);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C2 + Z4) * (C2 + Z4);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C3 + Z4) * (C3 + Z4);
Serial.print(Vout);

  
Serial.print("  ");
Vout = Vin /(R1 + C4 + Z4) * (C4 + Z4);
Serial.println(Vout);
Serial.println();
Serial.println();

delay(5000);

}

