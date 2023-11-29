//----------------------------------------------------------------------------------------------
//Board Library : Seeed nRF52 mbed-enable Borads 2.7.2
//Board Select  : Seeed nRF52 mbed-enable Borads / Seeed XIAO BLE - nRF52840
//2022/10/08
//----------------------------------------------------------------------------------------------

#include <Wire.h>
#include <Adafruit_GFX.h>         // Adafruit GFX Library 1.10.14 by Adafruit
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSerif9pt7b.h"
#include <Adafruit_SSD1306.h>     // Adafruit SSD1306 2.5.3 by Adafruit
#include <ArduinoBLE.h>           // ArduinoBLE 1.3.2 by Arduino

#define VBAT_LOWER  3.5           // battery voltage lower limit
#define VBAT_UPPER  4.2           // battery voltage upper limit
#define dataNum 14                 // receive data number : roll, pitch, yaw, Vbatt (4 data 8 byte) (And accel. so 14 bytes total)

Adafruit_SSD1306 display(128, 64, &Wire, -1);   //SSD1306

const char* versionNumber = "0.90"; // version number
float roll, pitch, yaw;             // attitude
float ax, ay, az;                   // acceleration
float Vbatt;                        // battery voltage
int ss;                             // peripheral signal strength
bool readingFlag = false;           // data buffer in use flag
int err;                            // error code of scan_connect function
bool LED_state;                     // LED ON/OFF state 
    float threshold = -0.19;
    float rep = 0;
union unionData {                   // Union for bit convertion 16 <--> 8
  int16_t   dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;

//
//
//// Tracking data for integration and detection
//const uint16_t trackNum = 50;
//float distance = 0.;
//
//struct accelVect {
//  float x;
//  float y;
//  float z;
//};
//
//accelVect accelTrack[trackNum] = {};
//  accelVect nullAccel;
//    nullAccel.x = 0.;
//    nullAccel.y = 0.;
//    nullAccel.z = 0.;
//  for(unsigned char i = 0; i < trackNum; i++){
//    accelTrack[i] = nullAccel;
//  }
//uint16_t trackCounts = 0;
//accelVect gravVect;
//  gravVect.x = 0.;
//  gravVect.y = 0.;
//  gravVect.z = 1.;
//
//
//
//// Check count and integration function
//uint8_t checkTrack(float& dist, const accelVect& currAccel){
//  if(trackCounts < trackNum){
//    accelTrack[trackCounts] = currAccel;
//    return 1;
//  }else{
//    trackCounts = 0;
//    
//    return 0;
//  }
//}
//


// Characteristic UUID
#define myUUID(val) ("0dd7eb5a-" val "-4f45-bcd7-94c674c3b25f")
//BLEService        AttService(myUUID("0000"));
BLECharacteristic dataCharacteristic(myUUID("0010"), BLEWrite | BLENotify, dataNum);

BLEDevice peripheral;

void setup()
{
  //initialize serial port
  Serial.begin(115200);
  while (!Serial) { delay(1); }

  //set I/O pins
  pinMode(LED_RED, OUTPUT);       // LOW:LED ON, HIGH:LED OFF
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);

  // initialize SSD1306
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 initialize error");
  }
  display.clearDisplay();
  display.setFont(&FreeSerif9pt7b);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15);
  display.println("Workout Buddy");
  display.setCursor(0, 31);
  display.println("     BLE Central");
  display.setCursor(0, 47);
  display.print("Ver   : ");
  display.println(versionNumber);
  display.display();
  delay(2000);

  // initialize BLE
  BLE.begin();

  // Init gravity basis vector
  gravVect.x = ax;
  gravVect.y = ay;
  gravVect.z = az;
}

//******************************************************************************************
// IMU data is received every 80mS
//
void loop() {
  // scan and connect the peripheral
  // return value of this function is an error code 
  err = scan_connect();
  
  // if connected? 
  if(err == 0) {            
    // main task while connected to the peripheral 60~78mS
    while (peripheral.connected()) {     
      LED_state = !LED_state;
      digitalWrite(LED_GREEN, (LED_state ? LOW : HIGH));   // connect indicator blinking
      
      long timestamp = millis();    // for timer
      
      // restoration of received data
      // prohibit event handler access while data buffer is in use          
      readingFlag = true;   
        roll = ud.dataBuff16[0] * 180.0 / 32768.0;
        pitch = ud.dataBuff16[1] * 180.0 / 32768.0;
        yaw = ud.dataBuff16[2] * 360.0 / 32768.0;
        Vbatt = ud.dataBuff16[3] / 1000.0;
        ax = ud.dataBuff16[4];
        ay = ud.dataBuff16[5];
        az = ud.dataBuff16[6];
      readingFlag = false;

      // for serial plotter
      Serial.print(roll); Serial.print(", ");
      Serial.print(pitch); Serial.print(", ");
      Serial.print(yaw); Serial.println();

      Serial.print(ax); Serial.print(", ");
      Serial.print(ay); Serial.print(", ");
      Serial.print(az); Serial.println();
      Serial.println();

    
     //rep and set count
     if (roll <= threshold){
      ++rep;
     }
   
      // display attitude, battery voltage, signal strength
      display.clearDisplay();      
      display.setCursor(0, 15);
      display.print("Roll   "); display.println(roll);
      display.setCursor(0, 31);
      display.print("Pitch  "); display.print(pitch);
      display.setCursor(0, 47);
      display.print("Yaw  "); display.println(yaw);    
      display.setCursor(0, 63);
      display.print("Rep "); display.print(rep);
      display.setCursor(96, 63);
      display.println(ss);      
      display.display();

      while(millis() - timestamp < 80);     // wait for loop time 80mS
    } //While connected
      
  } //if scan & connect
    
  // if disconnected from the peripheral
  digitalWrite(LED_GREEN, HIGH);            // connect indicator OFF
  Serial.print("Disconnected from the peripheral: ");
  Serial.println(peripheral.address());
  Serial.print("ERROR : ");                 // return value of scan_connect
  Serial.println(err);
      
} //loop

//**************************************************************************************************
// scan and connect the peripheral
// return value of this function is an error code
//
int scan_connect(void) {
  // scanning peripherals
  BLE.scanForUuid(myUUID("0000"));  
  Serial.println("1.SCANNING ................");

  peripheral = BLE.available();
  
  if (!peripheral) {
    Serial.println("2x.Peripheral unavailable");
    return 2;
  }
  Serial.println("2.Peripheral is available");
    
  if (peripheral.localName() != "Att_Monitor") {
    Serial.println("3x.Peripheral local name miss match");
    return 3;
  }
  Serial.println("3.Got the right peripheral");

  // stop scanning, connect the peripheral
  BLE.stopScan();
  Serial.println("4.Stop scanning");
  
  Serial.println("5.CONNECTING ................");
  if (!peripheral.connect()) {
    Serial.println("5x.Can't connect");
    return 5;
  } 
  Serial.println("5.Connected");
  
  if (!peripheral.discoverAttributes()) {
    Serial.println("6x.Didn't discover attributes");
    peripheral.disconnect();
    return 6;
  }
  Serial.println("6.Discovered attributes");

  dataCharacteristic = peripheral.characteristic(myUUID("0010"));         //dataCaracteristic UUID
  dataCharacteristic.setEventHandler(BLEWritten, characteristicWritten);  //BLEWritten handler
  Serial.println("7.Char and eventhandler setting");  

  if (!dataCharacteristic.canSubscribe()) {
    Serial.println("8x.Can't subscribe");
    peripheral.disconnect();
    return 8;
  }
  Serial.println("8.Can subscribe");

  if (!dataCharacteristic.subscribe()) {
    Serial.println("9x.Can't Subscribe");
    peripheral.disconnect();
    return 9;
  }
  Serial.println("9.Subscribed");

  Serial.println("10.Success scanning and connecting");
  return 0;
}

//****************************************************************************************************
// Characteristic written event handler
//
void characteristicWritten(BLEDevice peripheral, BLECharacteristic thisChar) {

  digitalWrite(LED_RED, LOW);         // event indicator ON
    
  // wait while data buffer is accessed in main loop  
  while(readingFlag == true) {
  }    
  dataCharacteristic.readValue(ud.dataBuff8, dataNum);  // read data packet 3.5uS  
  ss = peripheral.rssi();                               // read signal strength 5~20mS
    
  long timestamp = millis();
  while(millis() - timestamp <= 1);   // Delay to make LED visible
    
  digitalWrite(LED_RED, HIGH);        // event indicator OFF
}
