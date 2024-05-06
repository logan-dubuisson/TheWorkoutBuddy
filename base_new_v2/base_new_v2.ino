#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <math.h>

//LCD module pin assignments
#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         7  //8

#define dataNum 14
#define THRESHOLD 7 // Threshold for detecting movement, adjust as needed
#define REP_DELAY 5000 // Delay in milliseconds between reps

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
bool changeFlag = true;

unsigned long lastRepTime = 0; // Time of the last detected rep
bool repInProgress = false; // Flag to track if a repetition is in progress
int repCount = 0; // Variable to store the repetition count
int repTotal = 15;
int setCount = 0;
bool countFlag = true;

bool repState = false;

float ax, ay, az;
float gx, gy, gz;
float temp;

// Logan rep code variables
bool inited = false;
float grav[3] = {0, 0, 0};
float gTotal = 9.8;
float angle[3] = {0, 0, 0};
float ax_prev, ay_prev, az_prev;
float speed = 0., height = 0., alpha = 0.4;
//

int ss;                             // peripheral signal strength
bool readingFlag = false;           // data buffer in use flag
int err;                            // error code of scan_connect function
bool LED_state;                     // LED ON/OFF state 
union unionData {                   // Union for bit convertion 16 <--> 8
  int16_t   dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;

// char buffer
// char buf[20];

// Characteristic UUID
#define myUUID(val) ("0dd7eb5a-" val "-4f45-bcd7-94c674c3b25f")
//BLEService        AttService(myUUID("0000"));
BLECharacteristic dataCharacteristic(myUUID("0010"), BLEWrite | BLENotify, dataNum);

BLEDevice peripheral;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  tft.init(240, 320);

  tft.setRotation(3);

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(40, 50);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.println("The Workout");
  tft.setCursor(100,90);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.println("Buddy");
  tft.setCursor(50,150);
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.println("Brought to you by:");
  tft.setCursor(75,180);
  tft.println("Bravo Builders");
  delay(5000);
  tft.fillScreen(ST77XX_BLACK);


  // initialize BLE
  BLE.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  err = scan_connect();

  if(changeFlag){
    changeFlag = false;
    //Display connecting to LCD module
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(5, 100); 
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.print("Connecting...");
  }

   //if connected
  if(err == 0){
    speed = 0.;
    height = 0.;
    inited = false;
    changeFlag = true;

    long timestamp = millis();        //for debug timer
    uint64_t measuredTime = micros(); //for measurement scaling timer

    while(peripheral.connected()){

      
      readingFlag = true;   
        ax = ud.dataBuff16[0] /128.;
        ay = ud.dataBuff16[1] /128.;
        az = ud.dataBuff16[2] /128.;
        gx = ud.dataBuff16[3] /128.;
        gy = ud.dataBuff16[4] /128.;
        gz = ud.dataBuff16[5] /128.;
        temp = ud.dataBuff16[6] /128.;
      readingFlag = false;

      //call function for process
      float timeScale = (micros() - measuredTime) / 1000000.; // for scaling based on time since last measurement
      float accelMeasured = process(grav, angle) * timeScale;
      measuredTime = micros();
      speed += (fabs(accelMeasured) > 0.01) ? accelMeasured * timeScale : -speed * alpha;
      height += speed * 100;  // in cm from m/s speed
      reorient(grav, angle);

      if(millis() - timestamp > 100){
        Serial.print("Acceleration X: ");
        Serial.print(ax);
        Serial.print(", Y: ");
        Serial.print(ay);
        Serial.print(", Z: ");
        Serial.print(az);
        Serial.print("  (G = ");
        Serial.print(gTotal);
        Serial.println(" m/s^2");  
        Serial.print("Rotation X: ");
        Serial.print(gx);
        Serial.print(", Y: ");
        Serial.print(gy);
        Serial.print(", Z:");
        Serial.print(gz);
        Serial.println(" rad/s");

        Serial.print("Vertical acceleration: ");
        Serial.print(accelMeasured);
        Serial.println(" m/s^2");
        Serial.print("Speed: ");
        Serial.print(speed);
        Serial.print(" m/s  |  Height: ");
        Serial.print(height);
        Serial.println(" cm\n");
        
        timestamp = millis();
      }

      //Rep code

      //end rep code
      if(countFlag){
        countFlag = false;
        if(changeFlag){
          changeFlag = false;
          tft.fillScreen(ST77XX_BLACK);
        }

        tft.setTextWrap(false);
        // Reps
        tft.setCursor(60, 40);
        tft.setTextColor(ST77XX_RED);
        tft.setTextSize(4);
        tft.println("Rep: ");
        // Clean previous value
        tft.setTextColor(ST77XX_BLACK);
        tft.setCursor(240, 40);
        tft.print(repCount - 1);
        // Write new value
        tft.setTextColor(ST77XX_RED);
        tft.setCursor(240, 40);
        tft.print(repCount);
        // Sets
        tft.setCursor(60,100);
        tft.setTextColor(ST77XX_YELLOW);
        tft.setTextSize(4);
        tft.println("Set: ");
        // Clean previous value
        tft.setTextColor(ST77XX_BLACK);
        tft.setCursor(240, 100);
        tft.print(setCount - 1);
        // Write new value
        tft.setTextColor(ST77XX_YELLOW);
        tft.setCursor(240,100);
        tft.print(setCount);
        // Form
        tft.setCursor(60,160);
        tft.setTextColor(ST77XX_GREEN);
        tft.setTextSize(4);
        tft.println("Form: ");
      }
 
    } //while connected

    changeFlag = true;
  }//if scan and connect

  //if disconnected 
  Serial.print("Disconnected from the peripheral: ");
  Serial.println(peripheral.address());
  Serial.print("ERROR : ");                 // return value of scan_connect
  Serial.println(err);

  // LCD rep, set, form display


} //end of loop

//return value of this fucntion if error code
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
    
  if (peripheral.localName() != "TWB Sensor") {
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



void reorient(float gravVec[], float ang[]){
  enum {
    X, Y, Z
  };

  if(inited){
    ang[X] += gx;
    ang[Y] += gy;
    ang[Z] += gz;

    gravVec[X] += gTotal * cos(gz)*sin(gy);
    gravVec[Y] += gTotal * cos(gx)*sin(gz);
    gravVec[Z] += gTotal * cos(gy)*sin(gx);
    return;
  }
  else{
    gTotal = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));
    ang[X] = atan(az/ay);
    ang[Y] = atan(az/ax);
    ang[Z] = atan(ay/ax);

    gravVec[X]=ax;
    gravVec[Y]=ay;
    gravVec[Z]=az;
    return;
  }

}


float process(float gravVec[], float ang[]){

  enum {
    X, Y, Z
  };

  float accel = (ax - gravVec[X])*(gravVec[X]/gTotal) + (ay - gravVec[Y])*(gravVec[Y]/gTotal) + (az - gravVec[Z])*(gravVec[Z]/gTotal);
  return accel;
}