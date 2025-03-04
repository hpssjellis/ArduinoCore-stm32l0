
#include "LoRaRadio.h"
#include "TimerMillis.h"


#include "LoRaWAN.h"
#include <CayenneLPP.h>


const char *devEui = "1010101010101010";    // as per on helium console msb format
const char *appEui = "1010101010101010";              
const char *appKey = "101010101010101010101010101010101";

CayenneLPP lpp(51);


bool myMustRunHelium = false;

TimerMillis myTimer01, myTimer02;  //, myTimer03;

int myBestTx = 1;   //default  14;  max might be 20 ????
int myConnectAttempts = 0;
static int myMaxAttempts = 3;

bool myReceivedOk = false;

const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char mySendArray[myCharMax];
char myInArray[myCharMax];
char myBigInArray[64];   // max incoming



static void myRunHelium(void); // just pre-telling about it

void mySomethingWrong(void){
  if (myReceivedOk){ 
     myMustRunHelium = true;     
      myRunHelium();    // Start LoRaWan
      
       Serial.println("ALL GOOD OK ARRIVED");
       delay(20); 
    } else if (myConnectAttempts >= myMaxAttempts) {

          Serial.println("SOMETHIG WRONG MAX RETRIES!");
          delay(20);             
          Serial.println("Sending again at higher power, TX power at: " + String(myBestTx));
          delay(20);           
          Serial.println(String(myMaxAttempts)+", is Max, Present Retires: "+String(myConnectAttempts));
          delay(20);             
          Serial.println("So doing nothing now");
          delay(20);   
          myConnectAttempts = 0; 
          myReceivedOk = true;   // not really just stops this looping
        } 
        
        // Now do important repeat until get a connection
        
        else   {
       myConnectAttempts +=1;
       myBestTx  +=1; // default 14; start at 1 // crank the Tx power and try again

       if (myBestTx > 20) {myBestTx = 20;}

       
       LoRaRadio.onTransmit(myTransmitEnd);   // resets to IQ true     
       LoRaRadio.setIQInverted(true);    // sending to main network 
       LoRaRadio.beginPacket();  
       LoRaRadio.write(mySendArray, sizeof(mySendArray)); // myInArray correct size  
       LoRaRadio.endPacket(); 

       Serial.println("SOMETHIG WRONG!");
       delay(20); 
       
       Serial.println("Sending again at higher power, TX power at: " + String(myBestTx));
       delay(20);           
       Serial.println(String(myMaxAttempts)+", is Max, Present Retires: "+String(myConnectAttempts));
       delay(20);  
       Serial.print(String(myCharMax) + " Bytes sent again: ");  
       delay(40);        
       Serial.println(mySendArray);  
       delay(20); 
    }
    myReceivedOk = false;
}



void myTransmitEnd(void){
       LoRaRadio.setIQInverted(false);  // false switch to nodes
       LoRaRadio.receive(5000);    // receive for only 5 seconds      
       myTimer02.start(mySomethingWrong, 5000);  // test if answer in time
       
       Serial.println(" OK transmit done");
       delay(20);        
       Serial.println();
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
       strcpy(mySendArray, myBigInArray); 

       LoRaRadio.onTransmit(myTransmitEnd);   // resets to IQ true
       LoRaRadio.setIQInverted(true);    // sending to main network 
       LoRaRadio.beginPacket();  
       LoRaRadio.write(mySendArray, sizeof(mySendArray)); // myInArray correct size  
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

    LoRaRadio.onReceive(myReceiveIQFalse);      // just telling it about the callback 
    //LoRaRadio.receive(0);                // is zero infinite, other upto milliseconds

    myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous


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
   Serial.println("TX power at: " + String(myBestTx));
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



void loop( void ){
if (myMustRunHelium) {
  
if (!LoRaWAN.busy()){
        if (!LoRaWAN.linkGateways()){
            
            LoRaWAN.rejoinOTAA();
        }
        
        if (LoRaWAN.joined())  {
               
          int err;

          // Prepare Cayenne LPP
          lpp.reset();
          lpp.addAnalogInput(1, -18);
          lpp.addAnalogInput(2, -46);
          lpp.addAnalogInput(2, 80);

          LoRaWAN.beginPacket();
          LoRaWAN.write(lpp.getBuffer(), lpp.getSize());

          err = LoRaWAN.endPacket(true);
          if (err > 0) {
             // do nothing since everything OK 
          }
        }
    }

    delay(20000);  // 1000 = 1 second, then 3660000 = 1 hour
 }

}



void myRunHelium(void){

    LoRaRadio.purge();  
    LoRaRadio.end();  // shut down LoRa
    Serial.println("Ending LoRa starting LoRaWan");  
    delay(20);  

 /*     
    LoRaWAN.begin(US915);
 */
    LoRaWAN.setSubBand(2);    // 1 for TTN and 2 for Helium.

    LoRaWAN.setDutyCycle(false);
    
    LoRaWAN.setAntennaGain(2.0);

    LoRaWAN.setDataRate(1);   // not really needed
    
    LoRaWAN.setADR(true);     // not really needed
    
    LoRaWAN.setSaveSession(true);
 
    LoRaWAN.joinOTAA(appEui, appKey, devEui);



}
