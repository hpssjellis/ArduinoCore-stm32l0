/*
*
* To install core, just add a "hardware" folder beside your "libraries" folder in linux "$sketchbook" folder
* on windows use documents/arduino/  and make a hardware folder.
* Then unzip this repository in that "hardware" folder
* Note: not all the code runs on windows but you can at least see the methods
* Alternate install is to install https://github.com/GrumpyOldPizza/ArduinoCore-stm32l0 "TleraCorp" using the json file
*
* https://grumpyoldpizza.github.io/ArduinoCore-stm32l0/package_stm32l0_boards_index.json
*
* and then replace all those files with the files from this repository at https://github.com/hpssjellis/ArduinoCore-stm32l0
*
* For farthest LoRa sending max at 11 BYTES or 88 bits
* Thats 11 letters perfect enough to send Machine Learning Coded information
* or 88 bits if you want to Hexadecimal exchange data.  
*
* 1. I want to send data from a node and then wait for a confirmation
* The confirmation can then effect the next data send
*
* 2. If no confirmation I want the node to try again but with
* a stronger signal etc
* If still no reply I want to connect to 
* TTN (TheThingsNetwork) first if no connection then
* Helium.org next
*
* Presntly working on getting a confirmation response.
*
*
*
*
*
*
*
*
*/
