#include <ArduinoBLE.h>
#include "LSM6DS3.h"
#include "Wire.h"

LSM6DS3 imu;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("The Workout Buddy - Sensor System");

  BLE.setLocalName("SENSOR");
  // start scanning for peripherals
  BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral1 = BLE.available();
  BLEDevice peripheral2 = BLE.available();

  if (peripheral1) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral1.address());
    Serial.print(" '");
    Serial.print(peripheral1.localName());
    Serial.print("' ");
    Serial.print(peripheral1.advertisedServiceUuid());
    Serial.println();

    if (peripheral1.localName() != "BASE") {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlAccel(peripheral1);
    controlGyro(peripheral2);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19B10000-E8F2-537E-4F6C-D104768A1214");

  }
}

//Void loop for Accel data
void controlAccel(BLEDevice peripheral1) {

  // connect to peripheral 1
  Serial.println("Connecting to Acceloremeter");

  if (peripheral1.connect()) {
    Serial.println("Connected to Accel");
  } else {
    Serial.println("Failed to connect to Accel");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral1.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral1.disconnect();
    return;
  }

  // retrieve the Accel characteristic
  BLECharacteristic accelCharacteristic = peripheral1.characteristic("19B10001-E8F2-537E-4F6C-D104768A1214");

  byte accelData[6];

  while (peripheral1.connected()) {

    // while the peripheral is connected
    int16_t accelX = imu.readRawAccelX();
    int16_t accelY = imu.readRawAccelY();
    int16_t accelZ = imu.readRawAccelZ();

    // Fill the accelData array with accelerometer data
    memcpy(accelData, &accelX, 2);
    memcpy(accelData + 2, &accelY, 2);
    memcpy(accelData + 4, &accelZ, 2);

    // Send data over Bluetooth
    accelCharacteristic.setValue(accelData, sizeof(accelData));
    delay(1000);
  }

  Serial.println("Peripheral 1 disconnected");
}

//Void loop for Gyro data
void controlGyro(BLEDevice peripheral2) {
  // connect to peripheral 2
  Serial.println("Connecting to Gyro");

  if (peripheral2.connect()) {
    Serial.println("Connected to Gyro");
  } else {
    Serial.println("Failed to connect to Gyro");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral2.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral2.disconnect();
    return;
  }

  // retrieve the Gyro characteristic
  BLECharacteristic gyroCharacteristic = peripheral2.characteristic("19B10002-E8F2-537E-4F6C-D104768A1214");

  byte gyroData[6];

  while (peripheral2.connect()) {

    // while the peripheral is connected

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
    delay(1000);
  }

  Serial.println("Peripheral 2 disconnected");
}
