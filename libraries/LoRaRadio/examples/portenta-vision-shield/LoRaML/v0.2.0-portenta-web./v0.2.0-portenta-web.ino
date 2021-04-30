
#include "Arduino.h"
#include "WiFi.h"

#include "mbed.h"
#include "rtos.h"

//using namespace mbed;  // sometimes needed
using namespace rtos;

const int myCharMax = 10; // allows 10 bytes      //64;  code breaks if sent bigger than this
//char myInArray[myCharMax];
int myCharMax2=0;
String myInArray;


// Choose either the following arduino_secrets.h tab and bracket out the next 2 lines after it
// That route is more secure.
// Or just change the lines below for your Network and Password. Eaier but not as secure
// Since if you share this file it will have your info on it

//#include "arduino_secrets.h"   // more safe to use this file with below
#define SECRET_SSID "xxxxxxxxxxx"    // your SSID here
#define SECRET_PASS "yyyyyyyyyyyyy"    // Your password here


Thread thread;

UART mySerial3(PJ_8,  PJ_9,  NC, NC); //TX, TR, RTS, CTS  NOTE: NC means not connected




void myLedBlue_thread(){
   while (true) {
   //   digitalWrite(LEDB, !digitalRead(LEDB));   //switch on / off
   //   ThisThread::sleep_for(1000);

    //  Serial.println(".");
           int myI = 1;
            if (mySerial3.available()) {          // If anything comes in Serial0 
               // Serial.write(mySerial3.read());   // Read it and send it out Serial (USB)
 
                myInArray = mySerial3.readString() ;
               // if (myI >= myCharMax) {break;}
              // myCharMax2 = myI;
              Serial.print(myInArray);
            }


              // Serial.print(myInArray);
    
  /*          
            if (mySerial3.available()) {          // If anything comes in Serial0 
                Serial.write(mySerial3.read());   // Read it and send it out Serial (USB)
            }


 * 
 *    strncpy(myInArray, "", myCharMax);  // erase the array of chars
   
  while (LoRaRadio.available() ) {
    myInArray[myI++] = (char)LoRaRadio.read() ;
    if (myI >= myCharMax) {break;}
  }
 * 
 * */

            

   }
}








char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;    
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  Serial.println("Wait a bit sometimes this takes a while");
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
  pinMode(LEDB, OUTPUT);      // set the LED pin mode
  digitalWrite(LEDB, LOW);  

  mySerial3.begin(9600);   // not 115200 does not work on Murata module
   
  thread.start(myLedBlue_thread);

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  digitalWrite(LEDB, HIGH);  
  digitalWrite(LED_BUILTIN, LOW); 
}


void loop() {



  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
           //client.println("{answer:42}");
            client.println();
            client.println("<br>");


            client.println(myInArray);             
            Serial.println(myInArray);  
                  
            client.println("<br>");           
            client.println();
           // strncpy(myInArray, "", myCharMax2);
            //myInArray = ""; 
            
            // the content of the HTTP response follows the header:
            client.print("<input type=button value='LED_BUILTIN Off' onclick='{location=\"/H\"}'>");
            client.print("<input type=button value='LED_BUILTIN On' onclick='{location=\"/L\"}'>");

            // The HTTP response ends with another blank line:
            client.println();    
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);  
    Serial.println("");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}





