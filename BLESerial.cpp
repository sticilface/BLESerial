

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
  if (_init && _pServer && _pUARTService) {
    _pTxCharacteristic = nullptr; //  not sure if you need to remove them... no remove characteristic... 
    _pRxCharacteristic = nullptr; 

    _pUARTService->stop();
    _pServer->removeService(_pUARTService);
    _pUARTService = nullptr; 
    _init = false;

    if (_createdServer) {
      _pServer.deinit(true); 
      delete _pServer;    //  createServer() calls new, but there is no delete... 
    }
  }

}

int BLESerial::available(void)
{
  return _data.size() ; //result;
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
  if (!pCharacteristic) { Serial.println("***********************CRASH PREVENTED***************");  return; }
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0) {
    for (int i = 0; i < rxValue.length(); i++) {
      if (_data.size() == 512) {
        _data.pop();
      }
      _data.push(rxValue[i]);
    }
  }
}

bool BLESerial::hasClient()
{
  return _deviceConnected;
}


int BLESerial::peek(void)
{
  if (_data.size()) {
    return _data.front();
  } else {
    return -1;
  }
}

int BLESerial::read(void)
{
  if (_data.size()) {
    uint8_t c = _data.front();
    _data.pop();
    return c;
  } else {
    return -1;
  }
}


