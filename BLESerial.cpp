

/*
  BLE UART Service written by SticilFace

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

  _queue = xQueueCreate( _queueSize ,sizeof(uint8_t)); 

  if (!_queue) {
    return false; 
  }

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
  if (!_init && device) {
    BLEDevice::init(device);
    _pServer = BLEDevice::createServer();
    _createdServer = true; 
    return init();
  } else {
    return false;
  }

}

bool BLESerial::begin(BLEServer *server)
{
  if (!_init) {
    _pServer = server;
    return init();
  } else {
    return false;
  }
}

//  unsure if need to stop advertising here.. there is no func to remove advert for service...
//  probably need to delete server... but not if provided externally... some extra work....  
void BLESerial::end()
{
  if (_queue) {
    vQueueDelete(_queue); 
    _queue = nullptr; 
  }

  if (_init && _pServer && _pUARTService) {
    _pTxCharacteristic = nullptr; //  not sure if you need to remove them... no remove characteristic... 
    _pRxCharacteristic = nullptr; 

    _pUARTService->stop();
    _pServer->removeService(_pUARTService);
    _pUARTService = nullptr; 
    _init = false;

    if (_createdServer) {
      BLEDevice::deinit(true); 
      delete _pServer;    //  createServer() calls new, but there is no delete... 
      _pServer = nullptr; 
    }
  }

}

int BLESerial::available(void)
{
  if (_queue) {
    return uxQueueMessagesWaiting(_queue); 
  } else {
    return 0; 
  }
  
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
  if (!pCharacteristic) { 
    log_v("no pCharacteristic");  
    return; 
  }

  if (!_queue) {
    log_v("No Queue");
    return; 
  }

  const std::string & rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
     for (int i = 0; i < rxValue.length(); i++) {
       xQueueSend(_queue, & rxValue[i], 0 ); 
    }
  }
}

bool BLESerial::hasClient()
{
  return _deviceConnected;
}


int BLESerial::peek(void)
{
  uint8_t item; 
  if (_queue) {
    if(xQueuePeek(_queue, &item, 0 )) {
      return item; 
    }
  }
  return -1; 
}

int BLESerial::read(void)
{
  uint8_t item; 
  if (_queue) {
    if (xQueueReceive(_queue, &item, 0 )) {
      return item; 
    }
  }
  return -1; 
}

//  must be called before begin... at the moment... 
size_t BLESerial::setRxBufferSize(size_t len)
{
  if (len == _queueSize) {
    return _queueSize; 
  }
  _queueSize = len;
  
  if (_queue) {
    vQueueDelete(_queue); 
    _queue = nullptr; 
  } 

  _queue = xQueueCreate( _queueSize ,sizeof(uint8_t)); 

  if (_queue) {
    return _queueSize; 
  }
  
  return 0; 
}

