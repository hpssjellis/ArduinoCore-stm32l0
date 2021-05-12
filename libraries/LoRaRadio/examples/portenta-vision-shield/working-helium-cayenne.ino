/* Simple OTAA join for a LoRaWAN or Helium network network
 * Does nothing except connect every hour. No lights no serial print
 * The concept is that you should see some kind of connection on
 * The Helium of TTN network dashboards
 * This example code is in the public domain.
 */

#include "LoRaWAN.h"
#include <CayenneLPP.h>

String devEui = "0101010101010101";
String appEui = "0101010101010101";    // get from https://console.helium.com/devices
String appKey = "01010101010101010101010101010101";  // you have to change this from https://console.helium.com/devices

CayenneLPP lpp(51);



void setup( void ){

    LoRaWAN.begin(US915);
    LoRaWAN.setSubBand(2);    // 1 for TTN and 2 for Helium I think ???
    LoRaWAN.setDutyCycle(false);
    LoRaWAN.setAntennaGain(2.0);
    
    LoRaWAN.setSaveSession(true);
    LoRaWAN.joinOTAA(appEui, appKey, devEui);

}

void loop( void ){
  
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

    delay(3660000);  // 1000 = 1 second, then 3660000 = 1 hour
}
