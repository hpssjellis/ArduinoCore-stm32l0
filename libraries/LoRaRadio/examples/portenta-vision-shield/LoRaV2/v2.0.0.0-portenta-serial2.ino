/*
 * Code on the Portenta looks something like
 *  Note these Portenta defines
 *
 * From the schematics for the Portenta https://content.arduino.cc/assets/Pinout-PortentaH7_latest.pdf
 * UART0 TX PA_0
 * UART0 RX PI_9
 *
 * UART1 TX PA_9
 * UART1 RX PA_10
 
 * UART2 TX PG_14
 * UART2 RX PG_9
 
 * UART3 TX PJ_8
 * UART3 RX PJ_9
 *
 * 
 *
*/


#include <Arduino.h>


//#include "mbed.h"
//#include "rtos.h"

//using namespace mbed;  // sometimes needed
//using namespace rtos;

//int myLastUart = -1;
//Thread thread;

//UART mySerial3(PJ_8,  PJ_9,  NC, NC); //TX, TR, RTS, CTS  NOTE: NC means not connected

#define Serial2 SerialLoRa


/*
void myLedBlue_thread(){
   while (true) {
      digitalWrite(LEDB, !digitalRead(LEDB));   //switch on / off
      ThisThread::sleep_for(1000);

      Serial.println(".");

   }
}


*/

void setup(){
    pinMode(LEDB, OUTPUT);   // LEDB = blue, LEDG or LED_BUILTIN = green, LEDR = red 
    Serial.begin(115200);
    Serial2.begin(9600);   // not 115200 does not work on Murata module
    digitalWrite(LEDB, 0);   // internal LED on
    //thread.start(myLedBlue_thread);
}

void loop(){

  if (Serial2.available()) {          // If anything comes in Serial3 
      Serial.write(Serial2.read());   // Read it and send it out Serial (USB)
  }

  if (Serial.available()) {          // If anything comes in Serial (USB) 
      Serial2.write(Serial.read());   // Read it and send it out Serial3
  }

     
}
