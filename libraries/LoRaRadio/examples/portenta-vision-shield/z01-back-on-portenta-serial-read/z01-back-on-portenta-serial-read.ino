/*
 * Now you can if you want double press the Portenta button to enter bootloader mode (Don't if fine)
 * 
 * This part can be done on a windows machine if you want
 *
 * This program allows reading from the lora/murata module but from the M7 core 
 * Serial3 on Portenta is Serial on the murata module, so they can talk!
 *
 *
 *
 *
*/


#include <Arduino.h>


#include "mbed.h"
#include "rtos.h"

//using namespace mbed;  // sometimes needed
using namespace rtos;

//int myLastUart = -1;
Thread thread;

UART mySerial3(PJ_8,  PJ_9,  NC, NC); //TX, TR, RTS, CTS  NOTE: NC means not connected




void myLedBlue_thread(){
   while (true) {
      digitalWrite(LEDB, !digitalRead(LEDB));   //switch on / off
      ThisThread::sleep_for(1000);

      Serial.println(".");

   }
}


void setup(){
    pinMode(LEDB, OUTPUT);   // LEDB = blue, LEDG or LED_BUILTIN = green, LEDR = red 
    Serial.begin(115200);
    mySerial3.begin(9600);   // not 115200 does not work on Murata module
   
    thread.start(myLedBlue_thread);
}

void loop(){

  if (mySerial3.available()) {          // If anything comes in Serial0 
     Serial.write(mySerial3.read());   // Read it and send it out Serial (USB)
  }


     
}
