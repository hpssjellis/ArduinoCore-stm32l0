// crashing







#include "LoRaRadio.h"
#include "TimerMillis.h"

TimerMillis timerOn;




int myIncoming ;
int myRssi     ;
int mySnr      ;

const int myCharMax = 10; // allows 10 bytes      //64;  code breaks if sent bigger than this
char myInArray[myCharMax];

const int myBigCharMax = 100; // allows 10 bytes      //64;  code breaks if sent bigger than this
char myBigInArray[myBigCharMax];

int myFinalSize=0;

//String myInString="";


//static void myReceive(void);

/*
 * 
 * 
 *   while (LoRaRadio.available() ) {
    myInArray[myI++] = (char)LoRaRadio.read() ;
    if (myI >= myCharMax) {break;}
  }
 * 
 */


void mySerialRead(void){
  int myI=0;
  while (Serial.available()) {          
   //  Serial.write(Serial.read());  
     myBigInArray[myI++] = (char)Serial.read();
  }
  myFinalSize =  myI;
     
  //  myInString = Serial.readString();

    if (myFinalSize == myCharMax){  // got 10 bytes so send it
    LoRaRadio.setIQInverted(true);                // true gateway reads, node sends

    LoRaRadio.beginPacket();  
    LoRaRadio.write(myBigInArray, sizeof(myBigInArray));    
    LoRaRadio.endPacket(); 
    
    LoRaRadio.setIQInverted(true);                // true gateway reads, node sends
      
    Serial.println("10 Bytes");
    }
    
  if (myFinalSize > 0){
    Serial.println(myBigInArray);
  }
  strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars
  myFinalSize = 0;
    
  
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
    
    LoRaRadio.setIQInverted(true);  // true gateway reads, node sends 
    LoRaRadio.setPublicNetwork(false);  //false private network

  
    LoRaRadio.onReceive(myReceive);  // just telling it about the callback 
    LoRaRadio.receive(0);            // is zero infinite, other upto milliseconds
   // timerOn.start(callbackOn,  3000);
    timerOn.start(mySerialRead, 0, 200);


}


void loop( void ){

}


static void myReceive(void){  // constantly set to receive
  
   int myI=0;  
   myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   myRssi     = LoRaRadio.packetRssi();
   mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
   while (LoRaRadio.available() ) {
      myInArray[myI++] = (char)LoRaRadio.read() ;
      if (myI >= myCharMax) {break;}
   }
                         
   Serial.println();         
   Serial.println(myInArray);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );

}
