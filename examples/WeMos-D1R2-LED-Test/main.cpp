#include <Arduino.h>

int ontime = 100;
int x = 0;


void setup() {
  // put your setup code here, to run once:

/*
x = 1;
do {
pinMode(x, OUTPUT);
digitalWrite(x, 0);
x++;
} while (x < 7);

do {
x = 1;
do {
digitalWrite(x, 1);
delay(200);
digitalWrite(x, 0);
x++;
} while (x < 7);
} while(1);
*/





pinMode(16, OUTPUT);  //D0
pinMode(5, OUTPUT);  //D1
pinMode(4, OUTPUT);  //D2
pinMode(0, OUTPUT);  //D3
pinMode(2, OUTPUT);  //D4
pinMode(14, OUTPUT);  //D5
pinMode(12, OUTPUT);  //D6
pinMode(13, OUTPUT);  //D7
pinMode(15, OUTPUT);  //D8


digitalWrite(16, 0);
digitalWrite(5, 0);
digitalWrite(4, 0);
digitalWrite(0, 0);
digitalWrite(2, 0);
digitalWrite(14, 0);
digitalWrite(12, 0);
digitalWrite(13, 0);
digitalWrite(15, 0);

}

void loop() {
  // put your main code here, to run repeatedly:
do {
digitalWrite(16, 1);
delay(ontime);
digitalWrite(16, 0);
digitalWrite(5, 1);
delay(ontime);
digitalWrite(5, 0);
digitalWrite(4, 1);
delay(ontime);
digitalWrite(4, 0);
digitalWrite(0, 1);
delay(ontime);
digitalWrite(0, 0);
digitalWrite(2, 1);
delay(ontime);
digitalWrite(2, 0);
digitalWrite(14, 1);
delay(ontime);
digitalWrite(14, 0);
digitalWrite(12, 1);
delay(ontime);
digitalWrite(12, 0);
digitalWrite(13, 1);
delay(ontime);
digitalWrite(13, 0);
digitalWrite(15, 1);
delay(ontime);
digitalWrite(15 , 0);
digitalWrite(13, 1);
delay(ontime);
digitalWrite(13, 0);
digitalWrite(12, 1);
delay(ontime);
digitalWrite(12, 0);
digitalWrite(14, 1);
delay(ontime);
digitalWrite(14, 0);
digitalWrite(2, 1);
delay(ontime);
digitalWrite(2, 0);
digitalWrite(0, 1);
delay(ontime);
digitalWrite(0, 0);
digitalWrite(4, 1);
delay(ontime);
digitalWrite(4, 0);
digitalWrite(5, 1);
delay(ontime);
digitalWrite(5, 0);

} while (1);   

}
