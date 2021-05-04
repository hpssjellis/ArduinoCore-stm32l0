#include "LoRaRadio.h"
#include "TimerMillis.h"

TimerMillis myTimer01, myTimer02, myTimer03;


//Important Globals

bool myWaitingForOk = false;
bool myOkGood = false;
int myBestTx = 3;
int myLastTx = 1;
int myTxSpread = 3;


int myIncoming ;
int myRssi     ;
int mySnr      ;


const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char myInArray[myCharMax];

const int myBigCharMax = 64;      //64;  code breaks if sent bigger than this
char myBigInArray[myBigCharMax];

int myFinalSize=0;




void myNoConnection(void){
    LoRaRadio.setIQInverted(true);   
    if (myOkGood){  
       Serial.println("5000 ms over, all good");
       myOkGood = false; 
     //  if (LastTx < myBestTx - myTxSpread  ) {
     //    myBestTx-1
     //    Serial.println("Reducing Best. Now TX: "+string(myLastTx)+ ", Best is: "+ String(myBestTx));    
     //   }   else {
      // Serial.println("TX is: "+string(myLastTx)+ ", Best is: "+ String(myBestTx));
      //  }
       
    } else {
       Serial.println("5000 ms over, This is bad");
        //myBestTx  myLastTx
    //   if (LastTx > myBestTx + myTxSpread  ) { }
       myLastTx += 1;   
       if (myLastTx > 20){myLastTx = 20;}
       Serial.println("Increasing Tx. was: "+String(myLastTx)+ ", Best is: "+ String(myBestTx));
    
       LoRaRadio.setTxPower(myLastTx);    //default 14
       
      // Serial.println("IResending");
    }

}

void myNodeRead(void){  
    LoRaRadio.setIQInverted(false);  // true gateway both read and write, false node
    LoRaRadio.receive(0); 
}


void mySerialRead(void){
  int myI=0;
  while (Serial.available()) {          
     myBigInArray[myI++] = (char)Serial.read();
   //  if (myI >= myCharMax) {break;}
  }
  
  myFinalSize =  myI;  

  if ( myFinalSize > 0){

    if (myFinalSize == myCharMax-1){  // got 10 bytes so send it
       if (myWaitingForOk){ 
             LoRaRadio.setIQInverted(false);  // node
       } else  { LoRaRadio.setIQInverted(true); } // Gateway
       
       LoRaRadio.beginPacket();  
       LoRaRadio.write(myBigInArray, sizeof(myInArray)); // myInArray correct size  
       LoRaRadio.endPacket(); 
    
    //LoRaRadio.setIQInverted(true);                // true gateway reads, node sends
      
       Serial.print(String(myCharMax) + " Bytes sent: ");   
       myWaitingForOk = true;   // get response to switch to node 
       myTimer03.start(myNoConnection, 5000); 

       
    } else if (myFinalSize < myCharMax-1) {
        Serial.print("less than 10 bytes, so command: "); 
        // I gues this could be commands
    } else if (myFinalSize > myCharMax-1) {
        Serial.print("Too big, Ignored: "); //only for 11 bytes not really working
    }
       
    Serial.println(myBigInArray);
  
    strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars

   // myTimer02.start(myNodeRead, 5000);  //activate timer after x s.
  } 
  //Serial.println("."); 
  LoRaRadio.receive(0);  // needed to start receiving data again
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
    
    LoRaRadio.setIQInverted(true);       // true gateway receive, false node receive etc

    LoRaRadio.onReceive(myReceive);      // just telling it about the callback 
    LoRaRadio.onTransmit(myNodeRead);   // should happen once transmitting is over
    LoRaRadio.receive(0);                // is zero infinite, other upto milliseconds

    myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous


}


void loop( void ){

}



void mySendAsNode(void){ // sends "OK" on Node network to every Gateway message

       char myOkReply[11]  = "----OK----";  // 10 bytes  

       LoRaRadio.setIQInverted(false);  // false switch to nodes
       LoRaRadio.beginPacket();  
       LoRaRadio.write(myOkReply, sizeof(myOkReply));    
       LoRaRadio.endPacket(); 
  
       LoRaRadio.setIQInverted(true);  // true back to gateway  
  }


static void myReceive(void){  // constantly set to receive

   if (myWaitingForOk){ 
             LoRaRadio.setIQInverted(false);  // node
   } else  { LoRaRadio.setIQInverted(true); } // Gateway
  
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

   if (myWaitingForOk){ 
      if (myInArray[4] == 'O' && myInArray[5] == 'K'){//expecting: "----OK----"
          Serial.println("cool OK");
          myOkGood = true; 
          myWaitingForOk = false;         
          LoRaRadio.setIQInverted(true);   
      }
   } else {                
     //  mySendAsNode();
         myTimer02.start(mySendAsNode, 20);  // 0 means continuous
   }
}
