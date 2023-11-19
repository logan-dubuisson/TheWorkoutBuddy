#include <ArduinoBLE.h>
#include "LSM6DS3.h"
#include "Wire.h"

LSM6DS3 imu;  
void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  BLE.setLocalName("Sensor-System");

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");

  //Connecting message
  Serial.println("Attmepting to establish connection");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral1 = BLE.available();
  BLEDevice peripheral2 = BLE.available();

  // retrieve the LED characteristic
  BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214");
  BLECharacteristic gyroCharacteristic = peripheral1.characteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLENotify);
  BLECharacteristic accelCharacteristic = peripheral2.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLENotify);

  if(peripheral1 && peripheral2) {
    Serial.print("Connected to Base System");

}


  // Declare data array outside the while loop
  byte gyroData[6];
  byte accelData[6];

  while (peripheral1.connected()&& peripheral2.connect()) {
    // while the peripheral is connected

    int16_t accelX = imu.readRawAccelX();
    int16_t accelY = imu.readRawAccelY();
    int16_t accelZ = imu.readRawAccelZ();

    // Fill the accelData array with accelerometer data
    memcpy(accelData, &accelX, 2);
    memcpy(accelData + 2, &accelY, 2);
    memcpy(accelData + 4, &accelZ, 2);
    
    // Read data from source and send it
    int16_t gyroX = imu.readRawGyroX();
    int16_t gyroY = imu.readRawGyroY();
    int16_t gyroZ = imu.readRawGyroZ();

    // Create a byte array to hold the gyroscope data
    memcpy(gyroData, &gyroX, 2);
    memcpy(gyroData + 2, &gyroY, 2);
    memcpy(gyroData + 4, &gyroZ, 2);

    // Send data over Bluetooth
    gyroCharacteristic.setValue(gyroData, sizeof(gyroData));
    accelCharacteristic.setValue(accelData, sizeof(accelData));
    delay(1000);
  }

  Serial.println("Base System Disconnected");

}
