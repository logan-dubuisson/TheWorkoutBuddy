#include "bluetooth_core.h"
#include "virtual_imu.h"
#include "tft.h"


static void cbi(BLEDevice peripheral, BLECharacteristic thisChar);


// INIT OBJECTS
//// virtual imu
Virtual_IMU imu = Virtual_IMU();
//// ble
BLE_Core ble = BLE_Core();
//// tft
TFT tft = TFT();

void setup() {
  // put your setup code here, to run once:
  if(!Serial) Serial.begin(115200);

  uint32_t waitTime = millis();
  tft.bootScreen();

  // INIT BLE
  ble.dataCharacteristic.setEventHandler(BLEWritten, cbi);
  ble.begin();

  while(millis() - waitTime < 5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  tft.connectScreen();
  ble.connect();
  while(!ble.getData(imu.data));

  // GET DATA
  while(ble.getData(imu.data)){
    imu.process();
    tft.update();
  }
  // SCREEN SET/REP DATA WRITE

} //end of loop









//*****************************************************************************************************
// Rep & set counting function
//
int countRnS(uint32_t &repCounter, uint32_t &setCounter, uint16_t dataBuff[]){

  ;

  return 1;
}

static void cbi(BLEDevice peripheral, BLECharacteristic thisChar){
  digitalWrite(LED_RED, LOW);         // event indicator ON
    
  // wait while data buffer is accessed in main loop  
  while(ble.isReading() == true) {
  }    
  ble.dataCharacteristic.readValue(ble.p_dataBuff8, dataNum);  // read data packet 3.5uS  
  ble.ss = ble.peripheral.rssi();                               // read signal strength 5~20mS
    
  long timestampLED = millis();
  while(millis() - timestampLED <= 200);   // Delay to make LED visible
    
  digitalWrite(LED_RED, HIGH);        // event indicator OFF
}