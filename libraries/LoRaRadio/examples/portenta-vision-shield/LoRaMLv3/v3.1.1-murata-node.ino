
#include "LoRaRadio.h"
#include "TimerMillis.h"



TimerMillis myTimer01, myTimer02;  //, myTimer03;

int myBestTx = 14;
//bool mySentIQFalse = false;
bool myReceivedOk = false;




const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char myInArray[myCharMax];
char myBigInArray[64];   // max incoming


void mySomethingWrong(void){
  if (myReceivedOk){  
       Serial.println("ALL GOOD OK ARRIVED");
       delay(20); 
    } else {
       Serial.println("SOMETHIG WRONG!");
       delay(20); 
    }
}



void myTransmitEnd(void){
       LoRaRadio.setIQInverted(false);  // false switch to nodes
       LoRaRadio.receive(5000);    // receive for only 5 seconds      
       myTimer02.start(mySomethingWrong, 5000);  // test if answer in time
       
       Serial.println("OK transmit done");
       delay(20); 
}


void mySerialRead(void){
  int myI, myFinalSize = 0;
  while (Serial.available()) {          
     myBigInArray[myI++] = (char)Serial.read();
   //  if (myI >= myCharMax) {break;}
  }
  
  myFinalSize =  myI;  

  if ( myFinalSize > 0){

    if (myFinalSize == myCharMax-1){  // got 10 bytes so send it
       LoRaRadio.setIQInverted(true);    // sending to main network 
       LoRaRadio.beginPacket();  
       LoRaRadio.write(myBigInArray, sizeof(myInArray)); // myInArray correct size  
       LoRaRadio.endPacket(); 
    // auto set to onTransmit to ask for 5 second reply
       
       Serial.print(String(myCharMax) + " Bytes sent: ");  
       delay(20); 
    //   myWaitingForOk = true;   // get response to switch to node 
    //   myTimer03.start(myNoConnection, 5000); 

       
    } else if (myFinalSize < myCharMax-1) {
        Serial.print("less than 10 bytes, so command: "); 
        delay(20);
        // I gues this could be commands
    } else if (myFinalSize > myCharMax-1) {
        Serial.print("Too big, Ignored: "); //only for 11 bytes not really working
        delay(20);
    }
       
    Serial.println(myBigInArray);
  
    strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars

   // myTimer02.start(myNodeRead, 5000);  //activate timer after x s.
  } 
  //Serial.println("."); 
  //LoRaRadio.receive(0);  // needed to start receiving data again
}


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
    
 //   LoRaRadio.setIQInverted(false);       // true gateway receive, false node receive etc
    LoRaRadio.onTransmit(myTransmitEnd);   // resets to IQ true
    LoRaRadio.onReceive(myReceiveIQFalse);      // just telling it about the callback 
    //LoRaRadio.receive(0);                // is zero infinite, other upto milliseconds

    myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous


}


void loop( void ){
   
}



static void myReceiveIQFalse(void){  // constantly set to receive

   LoRaRadio.setIQInverted(false);       // true gateway receive, false node receive etc

   int myI=0;  
   int myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   int myRssi     = LoRaRadio.packetRssi();
   int mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
   while (LoRaRadio.available() ) {
      myInArray[myI++] = (char)LoRaRadio.read() ;
      if (myI >= myCharMax) {break;}
   }
  if (myI > 0){
   if (myI == 11){  // is it the correct size

      if (myInArray[4] == 'O' && myInArray[5] == 'K'){//expecting: "----OK----"
        myReceivedOk = true;
      }

      
   // Now that info sent you can print what you got   
   Serial.print("Incoming, IQ false: ");  
   delay(20);       
   Serial.println(myInArray);
   delay(20);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );      
   delay(20);


  } else {


    
   Serial.print("Incoming, WRONG SIZE IQ false: ");  
   delay(20);       
   Serial.println(myInArray);
   delay(20);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );      
   delay(20);
    }

  }
}
