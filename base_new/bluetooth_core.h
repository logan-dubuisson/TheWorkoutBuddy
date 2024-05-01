#include <ArduinoBLE.h>

#ifndef _BLUETOOTH_CORE_H_
#define _BLUETOOTH_CORE_H_


// Characteristic UUID
#define myUUID(val) ("0dd7eb5a-" val "-4f45-bcd7-94c674c3b25f")
// #define myUUID "0dd7eb5a-0010-4f45-bcd7-94c674c3b25f"

// const String servUUID = "0dd7eb5a-0000-4f45-bcd7-94c674c3b25f";
// const String charUUID = "0dd7eb5a-0010-4f45-bcd7-94c674c3b25f";

#ifndef dataNum
#define dataNum 14
#endif // dataNum

//BLECharacteristic globalDataCharacteristic(myUUID("0010"), BLEWrite | BLENotify, dataNum);
static void (*cb_charWritten)(BLEDevice peripheral, BLECharacteristic thisChar);


class BLE_Core {

  public:
    BLE_Core();

    ~BLE_Core();

    void begin();

    int scan_connect(void);

    void characteristicWritten(BLEDevice peripheral, BLECharacteristic thisChar);

    void connect();

    int getData(float dataBuff[]);
    
    BLECharacteristic dataCharacteristic = BLECharacteristic(myUUID("0010"), BLEWrite | BLENotify, dataNum);

    BLEDevice peripheral;

    bool isReading() {return readingFlag;};

    uint8_t* p_dataBuff8 = nullptr;

    int ss = 0;                         // peripheral signal strength

  private:
    bool readingFlag = false;           // data buffer in use flag
    int err = 0;                        // error code of scan_connect function
    bool LED_state = false;             // LED ON/OFF state 
    union unionData {                   // Union for bit convertion 16 <--> 8
      int16_t   dataBuff16[dataNum/2];
      uint8_t   dataBuff8[dataNum];
    };
    union unionData ud;
    long timestamp = 0;
    bool isConnected = false;

    //BLEService        AttService(myUUID("0000"));
    //BLECharacteristic dataCharacteristic(myUUID("0010"), BLEWrite | BLENotify, dataNum);

    static void (*cb_charWritten)(BLEDevice peripheral, BLECharacteristic thisChar); //{
    //   ((BLE_Core *)p) -> characteristicWritten(BLEDevice peripheral, BLECharacteristic thisChar);
    // }
  
};


#endif // BLUETOOTH_CORE_H