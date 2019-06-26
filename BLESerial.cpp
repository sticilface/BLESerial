

/*

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "BLESerial.h"

BLESerial::BLESerial()
{

}

bool BLESerial::init()
{
    if (!_pServer) { return false; }

    _pServer->setCallbacks(this);

    _pUARTService = _pServer->createService(UART_SERVICE_UUID);

    if (!_pUARTService) { return false; }

    // Create a BLE Characteristic
    _pTxCharacteristic = _pUARTService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                        
    _pTxCharacteristic->addDescriptor(new BLE2902());

    _pRxCharacteristic = _pUARTService->createCharacteristic(
                         CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                      );

    _pRxCharacteristic->setCallbacks(this);

    _pServer->getAdvertising()->addServiceUUID(UART_SERVICE_UUID);

    _pUARTService->start();

    _pServer->getAdvertising()->start();

    _init = true; 

    return true; 
}

bool BLESerial::begin(const char * device)
{
    BLEDevice::init(device);

    _pServer = BLEDevice::createServer();

    return init(); 

}



bool BLESerial::begin(BLEServer *server)
{
    _pServer = server; 

    return init(); 
}



void BLESerial::end()
{

}

size_t BLESerial::setRxBufferSize(size_t size){
    // if(_uart) {
    //     _rx_size = uart_resize_rx_buffer(_uart, size);
    // } else {
    //     _rx_size = size;
    // }
     return 0;//_rx_size;
}


int BLESerial::available(void)
{
    // int result = static_cast<int>(uart_rx_available(_uart));
    // if (!result) {
    //     optimistic_yield(10000);
    // }
    return 0 ; //result;
}

void BLESerial::flush()
{

}

void BLESerial::onConnect(BLEServer* pServer) 
{
  _deviceConnected = true; 
}

void BLESerial::onDisconnect(BLEServer* pServer) 
{
  _deviceConnected = false; 
}

void BLESerial::onWrite(BLECharacteristic *pCharacteristic)
{
   std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }   
}

bool BLESerial::hasClient()
{
  return _deviceConnected; 
}
