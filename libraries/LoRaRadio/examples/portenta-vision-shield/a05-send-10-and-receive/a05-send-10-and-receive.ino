/*
*
*   TESTING 
*
*
*
*
*/





#include "LoRaRadio.h"
#include "TimerMillis.h"

static void myReceive(void);

// Global Variables

TimerMillis myTimer01;

int myIncoming ;
int myRssi     ;
int mySnr      ;

const int myCharMax = 11; // allows 10 bytes      //64;  code breaks if sent bigger than this
char myInArray[myCharMax];


static void myReceive(void);
static void myPrint(void);




void myPrint(void){
  Serial.println();         
  Serial.println();         
  Serial.println(myInArray);
  Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );

}


void setup( void )
{
    Serial.begin(9600);
    
    while (!Serial) { }  // non-blocking on Portenta murata module

    LoRaRadio.begin(915000000);

    LoRaRadio.setFrequency(915000000);
    LoRaRadio.setTxPower(1);                      // smallest try -1, default 14,  max ~20
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);     // Bandwidth:        BW_125   BW_250   BW_500
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7); // Spreading Factor: SF_7   SF_8   SF_9   SF_10   SF_11   SF_12	
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);    // Coding Rate:      CR_4_5   CR_4_6   CR_4_7   CR_4_8	
    LoRaRadio.setLnaBoost(true);
       
    LoRaRadio.onReceive(myReceive);  // just telling it about the callback 
   // LoRaRadio.receive(0);            // is zero infinite, other upto milliseconds
   // timerOn.start(callbackOn,  3000);
    //timerOn.start(callbackOn, 0, 3000);

    
    
   //    LoRaRadio.setFixedPayloadLength(10);   //testing ???

}

void loop( void ){


 //String msg = Serial.readStringUntil('\n');
 // ---------------------"1234567890"                       
 char msg[myCharMax]  =  "Cool sends";   // strangely 10 not 11   


  LoRaRadio.beginPacket();  
  LoRaRadio.write(msg, sizeof(msg));    
  LoRaRadio.endPacket();
  
  LoRaRadio.receive(1000);    // if 1000 receinve for only 1 second. if 0 receive continuously
  //Serial.println("Message sent: "+String(msg));
  
  delay(10000);              // wait x seconds, 1000 = 1 second  
}




static void myReceive(void){
  
   int myI=0;  
   myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   myRssi     = LoRaRadio.packetRssi();
   mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);       // erase the array of chars
   
  while (LoRaRadio.available() ) {
    myInArray[myI++] = (char)LoRaRadio.read() ;
    if (myI >= myCharMax) {break;}
  }                   

   myTimer01.start(myPrint, 5);   // only calls once after 5 ms,  if  0, 1000 would continuously call every 1 second.
}


