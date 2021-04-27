

#include "LoRaRadio.h"
#include "TimerMillis.h"

static void myReceive(void);

// Global Variables

TimerMillis myTimer01, myTimer02;

int myIncoming ;
int myRssi     ;
int mySnr      ;

const int myCharMax = 11;    // allows 10 bytes      //64;  code breaks if sent bigger than this
char myInArray[myCharMax];   // for incoming messages

 // ---------------------"1234567890"                       
 char msg[myCharMax]  =  "Cool send6";   // strangely 10 not 11  


static void myReceive(void);
static void myPrint(void);
static void mySend(void);




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
       
  myTimer01.start(myPrint, 5000);   // only calls once after 5 ms,  if  0, 1000 would continuously call every 1 second.


  Serial.println(); 
  Serial.println("Sending: " + String(msg)); 

  // timer02.stop();
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

    myTimer02.start(mySend, 0, 7000);  // continuous send every x seconds.1000 = 1 second
    
 // LoRaRadio.setIQInverted(true);   //note false is standard, true Gateway
    

}

void loop( void ){
  
  LoRaRadio.receive(0); 
  delay(1000);
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
                  
 

}
