#include "LoRaRadio.h"
#include "TimerMillis.h"

TimerMillis timerOn;


int myIncoming ;
int myRssi     ;
int mySnr      ;

//const int myMaxBytes = 10;
const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char myInArray[myCharMax];

//const int myBigCharMax = 64;      //64;  code breaks if sent bigger than this
char myBigInArray[myCharMax];

int myFinalSize=0;



void mySerialRead(void){
  int myI=0;
  while (Serial.available()) {          
     myBigInArray[myI++] = (char)Serial.read();
   //  if (myI >= myCharMax) {break;}
  }
  
  myFinalSize =  myI;  

  if ( myFinalSize > 0){

    if (myFinalSize == myCharMax-1){  // got 10 bytes so send it
   // LoRaRadio.setIQInverted(true);                // true gateway reads, node sends
    //myBigInArray[10] = '*';
    //char array needs to be the corect size
  //  strcpy(myInArray, myBigInArray);  


    LoRaRadio.beginPacket();  
    LoRaRadio.write(myBigInArray, sizeof(myBigInArray));   
  //  LoRaRadio.write(myBigInArray, myCharMax);     
    //LoRaRadio.write(myBigInArray);    
    LoRaRadio.endPacket(); 
    
    //LoRaRadio.setIQInverted(true);                // true gateway reads, node sends
      
        Serial.print(String(myCharMax) + " Bytes sent: ");
    
    } else if (myFinalSize < myCharMax-1) {
        Serial.print("less than 10 bytes, so command: "); 
        // I gues this could be commands
    } else if (myFinalSize > myCharMax-1) {
        Serial.print("Too big, Ignored: "); 

    }
    
    
    
    Serial.println(myBigInArray);
  
    strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars
    //myFinalSize = 0;
     // LoRaRadio.receive(0);            // is zero infinite, other upto milliseconds
  } 
  //Serial.print("."); 
  LoRaRadio.receive(0);  
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
