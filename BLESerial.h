
/*

 */

#ifndef BLESerial_h
#define BLESerial_h

#include <inttypes.h>
#include "Stream.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2904.h>

#define UART_SERVICE_UUID      "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BLESerial: public Stream, public BLEServerCallbacks, public BLECharacteristicCallbacks
{
  private:
    bool init(); 
    void onConnect(BLEServer* pServer); 
    void onDisconnect(BLEServer* pServer);
    void onWrite(BLECharacteristic *pCharacteristic); 
    bool _init{false}; 
    BLEServer *_pServer{nullptr};
    BLEService *_pUARTService{nullptr}; 
    BLECharacteristic * _pTxCharacteristic{nullptr};
    BLECharacteristic * _pRxCharacteristic{nullptr};

public:
    BLESerial();
    virtual ~BLESerial() {}

    bool begin(const char * device); 
    bool begin(BLEServer *server); 

    void end();

    size_t setRxBufferSize(size_t size);
    bool hasClient(void);

    int available(void) override;

    int peek(void) override
    {
        // this may return -1, but that's okay
        return 0; 
    }

    int read(void) override
    {
        // this may return -1, but that's okay
        return 0; 
    }


    void flush(void) override;

    size_t write(uint8_t c) override
    {
        return 0;
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t) n);
    }
    size_t write(const uint8_t *buffer, size_t size)
    {
        return 0; //uart_write(_uart, (const char*)buffer, size);
    }
    size_t write(const char *buffer)
    {
        return 0; //buffer? uart_write(_uart, buffer, strlen(buffer)): 0;
    }
    operator bool() const
    {
        return _init;//_uart != 0;
    }

protected:

    size_t _rx_size;
    bool _deviceConnected{false}; 
};


#endif