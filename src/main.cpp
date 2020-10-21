#include <Arduino.h>
// Led1 ist bei Pin 14, Led2 ist bei Pin 2 !!!!


// On and Off Times (as int, max=32secs)  // pro micro und Leonardo
const unsigned int onTime1 = 50;
const unsigned int offTime1 = 750;
const unsigned int onTime2 = 740;
const unsigned int offTime2 = 50;


// Tracks the last time event fired
unsigned long previousMillis1=0;
unsigned long previousMillis2=0;
 
// Interval is how long we wait
int interval1 = onTime1;
int interval2 = onTime2;
 
// Used to track if LED should be on or off
boolean Led1 = true;
boolean Led2 = true;
 
int LED1 = 14;
int LED2 = 2;

// Usual Setup Stuff
void setup() {

 Serial.begin(115200); //This pipes to the serial monitor
 Serial1.begin(115200); //This is the UART, pipes to sensors attached to board

pinMode(LED1, OUTPUT); 
pinMode(LED2, OUTPUT); 

digitalWrite(LED1, HIGH);  
digitalWrite(LED2, HIGH);   

}
 
void loop() {

  // Grab snapshot of current time, this keeps all timing
  // consistent, regardless of how much code is inside the next if-statement
  unsigned long currentMillis = millis();

  // Compare to previous capture to see if enough time has passed
  if ((unsigned long)(currentMillis - previousMillis1) >= interval1) {
    // Change wait interval, based on current LED state
    if (Led1) {
      // LED is currently on, set time to stay off
      interval1 = offTime1;
    } else {
      // LED is currently off, set time to stay on
      interval1 = onTime1;
    }
    // Toggle the LED's state, Fancy, eh!?
    Led1 = !(Led1);
    digitalWrite(LED1, Led1); 
    // Save the current time to compare "later"
    previousMillis1 = currentMillis;
  }
   if ((unsigned long)(currentMillis - previousMillis2) >= interval2) {
    // Change wait interval, based on current LED state
    if (Led2) {
      // LED is currently on, set time to stay off
      interval2 = offTime2;
    } else {
      // LED is currently off, set time to stay on
      interval2 = onTime2;
    }
    // Toggle the LED's state, Fancy, eh!?
    Led2 = !(Led2);
    digitalWrite(LED2, Led2); 

    previousMillis2 = currentMillis;
  } 

// hier Code der immer ausgeführt wird!!!!
  
  
}  // ende von loop

