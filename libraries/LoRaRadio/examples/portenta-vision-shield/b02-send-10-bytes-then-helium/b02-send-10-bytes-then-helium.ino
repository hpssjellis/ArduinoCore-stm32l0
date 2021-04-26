/*
* TESTING IN PROGRESS
*
*
*/



#include "LoRaRadio.h"

   int myTxPower = 1;
   int myIncoming ;
   int myRssi     ;
   int mySnr      ;
   const int myCharMax = 11; // allows 10 bytes      //64;  code breaks if sent bigger than this
   char myInArray[myCharMax];




const int myCharMax = 11;   // strangley only allows 10 characters

void setup( void )
{
    Serial.begin(9600);
    
    while (!Serial) { }  // non-blocking on Portenta murata module

    LoRaRadio.begin(915000000);

    LoRaRadio.setFrequency(915000000);
    LoRaRadio.setTxPower(myTxPower);    // smallest try -1, default 14,  max ~20
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true);
       
   //    LoRaRadio.setFixedPayloadLength(10);   //testing ???

  

  
}








void loop( void ){


 //String msg = Serial.readStringUntil('\n');

 char msg[myCharMax]  =  "1234567890";   // strangely 10 not 11   
 //char msg[myCharMax] = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";


  //Serial.println();
  //Serial.println();


  LoRaRadio.beginPacket();  
  LoRaRadio.write(msg, sizeof(msg));    
  LoRaRadio.endPacket(); 
  
  LoRaRadio.onTransmit(myTransmit);  // just telling it about the callback 
  LoRaRadio.transmit(5000);            // is zero infinite, other up to milliseconds
   // timerOn.start(callbackOn,  3000);
   //timerOn.start(callbackOn, 0, 3000);
  //Serial.println("Message sent: "+String(msg));
  
  delay(30000);              // wait x seconds  
  
  if ( !LoRaRadio.busy() ){
        // Receive timed out
        // Do new stuff here 
      myTxPower += 1;
     if (myTxPower >=20){myTxPower = 20;} // stop increasing power at max  
     LoRaRadio.setTxPower(myTxPower);    // smallest try -1, default 14,  max ~20
  }
  
}






static void myTransmit(void){
  
   int myI=0;  
   myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   myRssi     = LoRaRadio.packetRssi();
   mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
  while (LoRaRadio.available() ) {
    myInArray[myI++] = (char)LoRaRadio.read();    // char forces alphaNumerics, HEX would be different 
    if (myI >= myCharMax) {break;}
  }

  //byte lastChar = strlen(myInArray)-1;                                  
  //myInArray[lastChar]= '\0';                   


}

