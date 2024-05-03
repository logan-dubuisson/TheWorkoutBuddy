#include <Wire.h>
#include <ArduinoBLE.h> 
//#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DS3TRC.h>

#define dataNum 14

Adafruit_LSM6DS3TRC sensor__imu;

uint8_t readData;                   // for reading IMU register
uint32_t timestamp;                 // for delay function
union unionData {                   //Union for bit convertion 16 <--> 8
  int16_t   dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;

#define myUUID(val) ("0dd7eb5a-" val "-4f45-bcd7-94c674c3b25f")
BLEService        IMUService(myUUID("0000"));
BLECharacteristic dataCharacteristic(myUUID("0010"), BLERead | BLENotify, dataNum);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
  delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Sensor System test!");

  // Try to initialize!
  if (!sensor__imu.begin_I2C()) {
    Serial.println("Failed to find IMU chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("IMU Sensor Found!");

  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  if (!BLE.begin()) {
    Serial.println("starting BLE module failed!");
    while (1);
  }

  // set the local name
  BLE.setLocalName("TWB Sensor");
  // set the device name
  BLE.setDeviceName("Sensor System");
  // set the UUID for the service
  BLE.setAdvertisedService(IMUService);
  // add the characteristic to the service  
  IMUService.addCharacteristic(dataCharacteristic);
  // add service  
  BLE.addService(IMUService);

  // start advertising
  BLE.setAdvertisingInterval(160);    //0.625mS * 160 = 100mS
  BLE.setConnectionInterval(6, 3200); //1.25mS * 6 = 7.5mS, 1.25mS * 3200 = 4S
  BLE.advertise();
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors_event_t a, g, temp;
  sensor__imu.getEvent(&a, &g, &temp);

  // connect the sentral
  Serial.println("Connecting to Central ........");
  BLEDevice central = BLE.central();

  if(central){
    Serial.println("Base System Found");

    uint32_t time = millis();

    while(central.connected()){

      sensor__imu.getEvent(&a, &g, &temp);

      //Converting float data to uint16_t data
      ud.dataBuff16[0] = (int16_t)(a.acceleration.x * 128);
      ud.dataBuff16[1] = (int16_t)(a.acceleration.y * 128);
      ud.dataBuff16[2] = (int16_t)(a.acceleration.z * 128);
      ud.dataBuff16[3] = (int16_t)(g.gyro.x * 128);
      ud.dataBuff16[4] = (int16_t)(g.gyro.y * 128);
      ud.dataBuff16[5] = (int16_t)(g.gyro.z * 128);
      ud.dataBuff16[6] = (int16_t)(temp.temperature * 128);
    
      if(millis() - 500 > time){

        /* Print out the values */
        Serial.print("Acceleration X: ");
        Serial.print(a.acceleration.x);
        Serial.print(", Y: ");
        Serial.print(a.acceleration.y);
        Serial.print(", Z: ");
        Serial.print(a.acceleration.z);
        Serial.println(" m/s^2");

        Serial.print("Rotation X: ");
        Serial.print(g.gyro.x);
        Serial.print(", Y: ");
        Serial.print(g.gyro.y);
        Serial.print(", Z: ");
        Serial.print(g.gyro.z);
        Serial.println(" rad/s");

        Serial.print("Temperature: ");
        Serial.print(temp.temperature);
        Serial.println(" degC");

        Serial.println("");
        delay(500);
      }

      // write to Characteristic as byte data        
      dataCharacteristic.writeValue(ud.dataBuff8, dataNum);
    } //end while loop

  } //end if loop

} //end void loop
