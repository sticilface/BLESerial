/*

        Example BLE Uart Sketch
*/

#include <BLESerial.h>

BLESerial bleserial; 
uint32_t timeout = 0; 

void setup() {
  Serial.begin(115200); 
  
  // put your setup code here, to run once:
bleserial.begin("UART");  //  create device and server  

// ******* or *********  

//BLEDevice::init("UART");
//BLEServer *pServer = BLEDevice::createServer();
//bleserial.begin(pServer);  //  init with an already created device and Server 

// **********************


  Serial.println("Ready"); 
}

void loop() {

  if (bleserial.available()) {
    while (bleserial.available()) {
      Serial.write(bleserial.read()); 
    }
  }

  if (millis() - timeout > 1000) {
    bleserial.println("hello"); 
    timeout = millis(); 
  }
}
