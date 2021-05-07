/*
 * better confirm by sending same message back but inverted false
 * 
 * 
 * 
 */



#include "LoRaRadio.h"
#include "TimerMillis.h"



TimerMillis myTimer02;    //, myTimer01;  //, myTimer03;

int myBestTx = 1;  // default  14; max 20???
bool mySentIQFalse = false;



const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char myInArray[myCharMax];
char myOutArray[myCharMax];





void setup( void ){
    Serial.begin(9600);
    
    while (!Serial) { }   // non-blocking for the murata module on the Portenta

    LoRaRadio.begin(915000000);

    LoRaRadio.setFrequency(915000000);
    LoRaRadio.setTxPower(myBestTx);    //default 14
    
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true); 
    
    LoRaRadio.setIQInverted(true);       // true gateway receive, false node receive etc

    LoRaRadio.onReceive(myReceiveIQTrue);      // just telling it about the callback 
    LoRaRadio.onTransmit(myTransmitIQFalseEnd);   // resets to IQ true
    LoRaRadio.receive(0);                // is zero infinite, other upto milliseconds

   // myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous


}


void loop( void ){
   
}


void myTransmitIQFalseEnd(void){
       LoRaRadio.setIQInverted(true);  // false switch to nodes
       LoRaRadio.receive(0); 
       Serial.println("Message returned on IQ invert false");
       delay(30); 
  }

void mySendIQFalse(void){ // sends "OK" on Node network to every Gateway message

       //char myOkReply[11]  = "----OK----";  // 10 bytes  
       //using myInArray which is the original message

       LoRaRadio.setIQInverted(false);  // false switch to nodes
       LoRaRadio.beginPacket();  
       LoRaRadio.write(myInArray, sizeof(myInArray));    
       LoRaRadio.endPacket(); 

       // Communicate what you did
       Serial.print("OK packet sent on IQ false: ");  
       delay(20);  
       Serial.println(myInArray);
       delay(20); 
       
     // auto set to onTransmit back to IQ true
   }




static void myReceiveIQTrue(void){  // constantly set to receive

   LoRaRadio.setIQInverted(true);       // true gateway receive, false node receive etc

   int myI=0;  
   int myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   int myRssi     = LoRaRadio.packetRssi();
   int mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
   while (LoRaRadio.available() ) {
      myInArray[myI++] = (char)LoRaRadio.read() ;
      if (myI >= myCharMax) {break;}
   }

   if (myI == 11){    // if something came in send it out again
      //  Good so send it back  on IQ inverted false
      myTimer02.start(mySendIQFalse, 20);  // 0 means continuous

   }

   Serial.print("Incoming, IQ True: ");  
   delay(30);       
   Serial.println(myInArray);
   delay(30);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );      
   delay(30);  
}


