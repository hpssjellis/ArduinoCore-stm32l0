/* Simple OTAA join for a LoRaWAN or Helium network network
 * Does nothing except connect every hour. No lights no serial print
 * The concept is that you should see some kind of connection on
 * The Helium of TTN network dashboards
 * This example code is in the public domain.
 */

#include "LoRaWAN.h"
#include <CayenneLPP.h>

#include "LoRaRadio.h"
#include "TimerMillis.h"


//#define DEBUG 1

const char *appEui = "0101010101010101";
const char *appKey = "2B7E151628AED2A6ABF7158809CF4F3C";
const char *devEui = "0101010101010101";

CayenneLPP lpp(51);



TimerMillis myTimer01, myTimer02;  //, myTimer03;

int myBestTx = 1;   //default  14;  max might be 20 ????
int myConnectAttempts = 0;
int myMaxAttempts = 3;

bool myReceivedOk = false;
bool myLoadHelium  = false;

const int myCharMax = 11; // allows 10 bytes and extra signal //64;  code breaks if sent bigger than this
char mySendArray[myCharMax];
char myInArray[myCharMax];
char myBigInArray[64];   // max incoming


void setup( void ){

    #ifdef DEBUG
   Serial.begin(9600);
    
    while (!Serial) { }   // non-blocking for the murata module on the Portenta
    #endif 

    LoRaRadio.begin(915000000);

    LoRaRadio.setFrequency(915000000);


             
   #ifdef DEBUG
    LoRaRadio.setTxPower(myBestTx);    //default 14
    
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true); 

  #endif    

    
 //   LoRaRadio.setIQInverted(false);       // true gateway receive, false node receive etc

    LoRaRadio.onReceive(myReceiveIQFalse);      // just telling it about the callback 
    //LoRaRadio.receive(0);                // is zero infinite, other upto milliseconds

    myTimer01.start(mySerialRead, 0, 200);  // 0 means continuous




    LoRaWAN.begin(US915);
    LoRaWAN.setSubBand(1);    // 1 for TTN and 2 for Helium I think ???
    LoRaWAN.setDutyCycle(false);
    LoRaWAN.setAntennaGain(2.0);
    

}

void loop( void ){

if (myLoadHelium){  
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
          //lpp.addAnalogInput(2, 80);

          LoRaWAN.beginPacket();
          LoRaWAN.write(lpp.getBuffer(), lpp.getSize());
         // LoRaWAN.write(mySendArray, sizeof(mySendArray));

          err = LoRaWAN.endPacket(true);
          if (err > 0) {
             // do nothing since everything OK 
          }
        }
    }

    delay(3660000);  // 1000 = 1 second, then 3660000 = 1 hour

}
}







void mySomethingWrong(void){
  if (myReceivedOk){ 
        
    #ifdef DEBUG
       Serial.println("ALL GOOD OK ARRIVED");
       delay(20); 
    #endif   
    } else if (myConnectAttempts >= myMaxAttempts) {
    
    #ifdef DEBUG
          Serial.println("SOMETHIG WRONG MAX RETRIES!");
          delay(20);             
          Serial.println("Sending again at higher power, TX power at: " + String(myBestTx));
          delay(20);           
          Serial.println(String(myMaxAttempts)+", is Max, Present Retires: "+String(myConnectAttempts));
          delay(20);             
          Serial.println("So doing nothing now");
          delay(20);  
     #endif   
          myConnectAttempts = 0; 
          myReceivedOk = true;   // not really just stops this looping
          myLoadHelium = true;
          
          LoRaWAN.setSaveSession(true);
          LoRaWAN.joinOTAA(appEui, appKey, devEui);
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

    #ifdef DEBUG
       Serial.println("SOMETHIG WRONG!");
       delay(20); 
       
       Serial.println("Sending again at higher power, TX power at: " + String(myBestTx));
       delay(20);           
       Serial.println(String(myMaxAttempts)+", is Max, Present Retires: "+String(myConnectAttempts));
       delay(20);  
       Serial.println(String(myCharMax) + " Bytes sent again: ");  
       delay(20);        
       Serial.println(mySendArray);  
       delay(20); 
    #endif 

    }
    myReceivedOk = false;
}



void myTransmitEnd(void){
       LoRaRadio.setIQInverted(false);  // false switch to nodes
       LoRaRadio.receive(5000);    // receive for only 5 seconds      
       myTimer02.start(mySomethingWrong, 5000);  // test if answer in time

    #ifdef DEBUG    
       Serial.println(" OK transmit done");
       delay(20);        
       Serial.println();
       delay(20);
    #endif 

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

    #ifdef DEBUG    
       Serial.print(String(myCharMax) + " Bytes sent: ");  
       delay(20); 
           #endif 

    //   myWaitingForOk = true;   // get response to switch to node 
    //   myTimer03.start(myNoConnection, 5000); 

       
    } else if (myFinalSize < myCharMax-1) {

    #ifdef DEBUG
        Serial.print("less than 10 bytes, so command: "); 
        delay(20);
            #endif 

        // I gues this could be commands
    } else if (myFinalSize > myCharMax-1) {
         
    #ifdef DEBUG
        Serial.print("Too big, Ignored: "); //only for 11 bytes not really working
        delay(20);    
        #endif 

    }

    #ifdef DEBUG       
    Serial.println(myBigInArray);
     #endif 

    strncpy(myBigInArray, "", myFinalSize);  // erase the array of chars

   // myTimer02.start(myNodeRead, 5000);  //activate timer after x s.
  } 
  //Serial.println("."); 
  //LoRaRadio.receive(0);  // needed to start receiving data again
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

    #ifdef DEBUG      
   // Now that info sent you can print what you got   
   Serial.print("Incoming, IQ false: ");  
   delay(20);       
   Serial.println(myInArray);
   delay(20);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );      
   delay(20);
   Serial.println("TX power at: " + String(myBestTx));
   delay(20); 
       #endif 


  } else {



    #ifdef DEBUG   
   Serial.print("Incoming, WRONG SIZE IQ false: ");  
   delay(20);       
   Serial.println(myInArray);
   delay(20);
   Serial.println("parsePacket(): "+String( myIncoming ) + ", RSSI: " + String(myRssi)+", SNR: " + String(mySnr) );      
   delay(20);
       #endif 

    }

  }
}
