#include <ArduinoBLE.h>
#include "LSM6DS3.h"
#include "Wire.h"

//Create a instance of class LSM6DS3
LSM6DS3 imu;   

BLEService sensorService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

BLEByteCharacteristic accelCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic gyroCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);


void setup() {
    // put your setup code here, to run once:

Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Base-System");
  BLE.setAdvertisedService(sensorService);

  // add the characteristic to the services
  sensorService.addCharacteristic(accelCharacteristic);
  sensorService.addCharacteristic(gyroCharacteristic);

  //add service
  BLE.addService(sensorService);
  
  Serial.println("Searching for Connection");
}

void loop() {
    // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address()); 
    
    while(central.connected()){
      if (accelCharacteristic.written()) {
        byte accelData[6];
        accelCharacteristic.readValue(accelData, sizeof(accelData));

        int16_t accelX = *((int16_t*)&accelData[0]);
        int16_t accelY = *((int16_t*)&accelData[2]);
        int16_t accelZ = *((int16_t*)&accelData[4]);

        Serial.print("Accelerometer Data: ");
        Serial.print("X: "); Serial.print(accelX);
        Serial.print(", Y: "); Serial.print(accelY);
        Serial.print(", Z: "); Serial.println(accelZ);
      }

      // Read gyroscope data
      if (gyroCharacteristic.written()) {
        byte gyroData[6];
        gyroCharacteristic.readValue(gyroData, sizeof(gyroData));

        int16_t gyroX = *((int16_t*)&gyroData[0]);
        int16_t gyroY = *((int16_t*)&gyroData[2]);
        int16_t gyroZ = *((int16_t*)&gyroData[4]);

        Serial.print("Gyroscope Data: ");
        Serial.print("X: "); Serial.print(gyroX);
        Serial.print(", Y: "); Serial.print(gyroY);
        Serial.print(", Z: "); Serial.println(gyroZ);
      }
    }
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from Sensor System: "));
    Serial.println(central.address()); 
  }
}
