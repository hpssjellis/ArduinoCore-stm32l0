#include "LoRaRadio.h"
#include "TimerMillis.h"

TimerMillis myTimer01, myTimer02;



//Important Globals

//bool myOk = false;
bool myWaitingForOk = false;
bool myNeedToSendOk = false;
//bool myAlwaysReply = true;   // set to false just to test incoming Gateway traffic
int myBestTx = 1;


int myIncoming ;
int myRssi     ;
int mySnr      ;


const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char myInArray[myCharMax];

//const int myBigCharMax = 64;      //64;  code breaks if sent bigger than this
char myBigInArray[myCharMax];

int myFinalSize=0;


void myNodeRead(void){
   
    Serial.println("5000 ms over");
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
   // LoRaRadio.setIQInverted(true);                // true gateway reads, node sends

    LoRaRadio.beginPacket();  
    LoRaRadio.write(myBigInArray, sizeof(myBigInArray));   
    LoRaRadio.endPacket(); 
    
    //LoRaRadio.setIQInverted(true);                // true gateway reads, node sends
      
    Serial.print(String(myCharMax) + " Bytes sent: ");
    
    } else if (myFinalSize < myCharMax-1) {
        Serial.print("less than 10 bytes, so command: "); 
        // I gues this could be commands
    } else if (myFinalSize > myCharMax-1) {
        Serial.print("Too big, Ignored: "); // not really working

    }
    
    
    
    Serial.println(myBigInArray);
  
    strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars
    myNeedToSendOk = true;
    myWaitingForOk = true;   // get response to switch to node 
   // Serial.print("myWaitingForOk = true; "); 
    myTimer02.start(myNodeRead, 5000);  //activate timer after 5 s.
  } 
  //Serial.print("."); 
  LoRaRadio.receive(0);  // needed to start receiving data again
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
    
    LoRaRadio.setIQInverted(true);  // true gateway both read and write, false node
    LoRaRadio.setPublicNetwork(false);  //false private network

  
    LoRaRadio.onReceive(myReceive);  // just telling it about the callback 
    LoRaRadio.receive(0);            // is zero infinite, other upto milliseconds
   // timerOn.start(callbackOn,  3000);
    myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous


}


void loop( void ){

}



void mySendAsNode(void){ // sends "OK" to every Gateway message
   if (myNeedToSendOk){
       char myOkReply[11]  = "----OK----";  // 10 bytes  
       // Serial.println("Cool");
       LoRaRadio.setIQInverted(false);  // false swtich to nodes
       LoRaRadio.beginPacket();  
       LoRaRadio.write(myOkReply, sizeof(myOkReply));    
       LoRaRadio.endPacket(); 
       LoRaRadio.setIQInverted(true);  // true gateway both read and write, false node

       myNeedToSendOk = false;
   }
  }





static void myReceive(void){  // constantly set to receive


   if (myWaitingForOk){ 
      // Serial.println("second myWaitingForOk = true; ");
       LoRaRadio.setIQInverted(false);  // node
      // myWaitingForOk = false; // set node receive off
   } 
        //else          { LoRaRadio.setIQInverted(true); } // Gateway
  
   int myI=0;  
   myIncoming = LoRaRadio.parsePacket();   // must grab before read! 
   myRssi     = LoRaRadio.packetRssi();
   mySnr      = LoRaRadio.packetSnr();
   strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
   while (LoRaRadio.available() ) {
      myInArray[myI++] = (char)LoRaRadio.read() ;
      if (myI >= myCharMax) {break;}
   }

  if (myInArray[4] == 'O' && myInArray[5] == 'K'){//expecting: "----OK----"
       Serial.println("cool OK");
       myWaitingForOk = false;
    }
   
                    
   Serial.println();         
   Serial.println(myInArray);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );

   if(myWaitingForOk){
      mySendAsNode();
   }  


   // will break it until I analyze the output

  //  if (myWaitingForOk){ LoRaRadio.setIQInverted(false);} // Node
  //      else          { LoRaRadio.setIQInverted(true); } // Gateway
  // always reset to Gateway
  LoRaRadio.setIQInverted(true);
  
}
