#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

//LCD module pin assignments
#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define dataNum 14
#define THRESHOLD 6 // Threshold for detecting movement, adjust as needed
#define REP_DELAY 5000 // Delay in milliseconds between reps

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

float ax, ay, az;
float gx, gy, gz;
float temp;

int ss;                             // peripheral signal strength
bool readingFlag = false;           // data buffer in use flag
int err;                            // error code of scan_connect function
bool LED_state;                     // LED ON/OFF state 
union unionData {                   // Union for bit convertion 16 <--> 8
  int16_t   dataBuff16[dataNum/2];
  uint8_t   dataBuff8[dataNum];
};
union unionData ud;

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

   //if connected
   if(err == 0){

    while(peripheral.connected()){

      long timestamp = millis(); //for timer

      
  readingFlag = true;   
    ax = ud.dataBuff16[0] /128.;
    ay = ud.dataBuff16[1] /128.;
    az = ud.dataBuff16[2] /128.;
    gx = ud.dataBuff16[3] /128.;
    gy = ud.dataBuff16[4] /128.;
    gz = ud.dataBuff16[5] /128.;
    temp = ud.dataBuff16[6] /128.;
  readingFlag = false;


  Serial.print("Acceleration: ");
  Serial.print(ax);
  Serial.print(", Y: ");
  Serial.print(ay);
  Serial.print(", Z: ");
  Serial.print(az);
  Serial.println(" m/s^2");  
  Serial.print("Rotation X: ");
  Serial.print(gx);
  Serial.print(", Y: ");
  Serial.print(gy);
  Serial.print(", z:");
  Serial.print(gz);
  Serial.println(" rad/s");
  Serial.print(temp); 
  Serial.println("");
  delay(500);       

 while(millis() - timestamp < 80);     // wait for loop time 80mS

    } //while connected


   }//if scan and connect

   //if disconnected 
  Serial.print("Disconnected from the peripheral: ");
  Serial.println(peripheral.address());
  Serial.print("ERROR : ");                 // return value of scan_connect
  Serial.println(err);

  // LCD rep, set, form display
  tft.setTextWrap(false);
  tft.setCursor(60, 40);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(4);
  tft.println("Rep: ");
  tft.setCursor(60,100);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(4);
  tft.println("Set: ");
  tft.setCursor(60,160);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(4);
  tft.println("Form: ");

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