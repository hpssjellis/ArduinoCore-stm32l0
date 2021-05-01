/*
*
* LoRaML for Machine Learning 10 byte communication application of LoRa/LoRaWan technologies
*
* General use: A machine Learning program sends a pre-configured 10 Byte code through LoRa to the web.
*
*
*
* Attempt to make the PortentaH7 with LoRa Vision Shield more useful
* for situations when a LoRaWan Gateway is not nearby, or when a busines does not want to use 
* a public network unless the device goes off-site.
* Note: Not security interested.
*
*
* V-0.1.x  Serial3 read then SerialUSB write from the (LoRa) Murata Module to the (Portenta) M7 core
*          Showing fixed 10 Byte, Private, InvertIQ true (Gateway listening) nearby LoRa Activity. 
*          Needs code that just broadcasts
*
* V-0.2.x  Above but showing on a WiFi webpage or Adafruit.io MQTT or (Ethernet if using the Portenta Breakout board)
*
* V-0.3.x  Back to just serial display but from M7 core being able to send 10 bytes to Murata Module to broadcast as a Private, 
*          InvertIQ false(Node send)
*
* V-0.4.x  Above but in Gateway mode being able to reply with "----ok----"
*

* V-0.5.x  Above but being able to receive and acknowledge the "----ok----"
*
*
* V-0.6.x  Above but being able to increase TX Strength if no reply. Note: if reply "Yes-------" then reduce TX strength???
*
*
* V-0.7.x  Above but if Strength is at Maximum, being able to switch to Helium Network. (Choose any LoRaWan Network)
*
*
*
* V-0.8.x  Above but if no connection to Helium try to connect to TTN
*
*
*
* V-0.9.x  Above but if no connecction to any Network then sleep one hour and try entire process again from V-0.6.x with High TX strength
*
*
*
*
*/


