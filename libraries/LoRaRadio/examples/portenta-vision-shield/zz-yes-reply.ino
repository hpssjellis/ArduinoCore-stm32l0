#include "LoRaRadio.h"
#include "TimerMillis.h"

static void myReceive(void);

// Global Variables

TimerMillis timer01,   timer02;
   int myIncoming ;
   int myRssi     ;
   int mySnr      ;
   const int myCharMax = 11; // allows 10 bytes      //64;  code breaks if sent bigger than this
   char myInArray[myCharMax];
   
 // -------------------= "1234567890";     
  char msg[myCharMax]  = "Yes";  





void myPrint(void){
  Serial.println();         
  Serial.println();         
  Serial.println(myInArray);
  Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );

}

void mySend(void){    

  LoRaRadio.beginPacket();  
  LoRaRadio.write(msg, sizeof(msg));    
  LoRaRadio.endPacket();      

  Serial.println(); 
  Serial.println("Sent: "+String(msg)); 

  // timer02.stop();
}



void setup( void ){
    Serial.begin(9600);
    
    while (!Serial) { }   // non-blocking for the murata module on the Portenta

    LoRaRadio.begin(915000000);

    LoRaRadio.setFrequency(915000000);
    LoRaRadio.setTxPower(1);    //default 14
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true);
   
   //    LoRaRadio.setFixedPayloadLength(10);   //testing ???
  
    LoRaRadio.onReceive(myReceive);  // just telling it about the callback 
   // LoRaRadio.receive(0);            // is zero infinite, other upto milliseconds
   // timerOn.start(callbackOn,  3000);
    //timerOn.start(callbackOn, 0, 3000);
}





void loop( void ){
        LoRaRadio.receive(0); 
        delay(500);
}




static void myReceive(void){
  
   int myI=0;  
   myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   myRssi     = LoRaRadio.packetRssi();
   mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
  while (LoRaRadio.available() ) {
    myInArray[myI++] = (char)LoRaRadio.read() ;
    if (myI >= myCharMax) {break;}
  }

                

   timer01.start(myPrint, 50);
   timer02.start(mySend, 70);
}
