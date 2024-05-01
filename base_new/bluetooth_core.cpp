#include "bluetooth_core.h"


BLE_Core::BLE_Core(){
  
  if(!Serial){
    Serial.begin(115200);
  }

  p_dataBuff8 = ud.dataBuff8;

}


BLE_Core::~BLE_Core(){
  delete p_dataBuff8;
}


void BLE_Core::begin(){

  BLE.begin();

}


//return value of this fucntion if error code
int BLE_Core::scan_connect(void) {
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
  //dataCharacteristic.setEventHandler(BLEWritten, cbi);         //BLEWritten handler
  Serial.println("7.Char and eventhandler(removed) setting");  

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
void BLE_Core::characteristicWritten(BLEDevice peripheral, BLECharacteristic thisChar) {

  digitalWrite(LED_RED, LOW);         // event indicator ON
    
  // wait while data buffer is accessed in main loop  
  while(readingFlag == true) {
  }    
  dataCharacteristic.readValue(ud.dataBuff8, dataNum);  // read data packet 3.5uS  
  ss = peripheral.rssi();                               // read signal strength 5~20mS
    
  long timestampLED = millis();
  while(millis() - timestampLED <= 200);   // Delay to make LED visible
    
  digitalWrite(LED_RED, HIGH);        // event indicator OFF
}


void BLE_Core::connect(){
  err = scan_connect();
}


int BLE_Core::getData(float dataBuff[]){
  //if connected
  if(err == 0 && peripheral.connected() && millis() - timestamp < 80){

    timestamp = millis(); //for timer

    
    readingFlag = true; 
    for(uint32_t i = 0; i < dataNum / 2; i++){
      dataBuff[i] = ud.dataBuff16[i] / 128.;
    }
    readingFlag = false;


    Serial.print("Acceleration: ");
    Serial.print(dataBuff[0]);
    Serial.print(", Y: ");
    Serial.print(dataBuff[1]);
    Serial.print(", Z: ");
    Serial.print(dataBuff[2]);
    Serial.println(" m/s^2");  
    Serial.print("Rotation X: ");
    Serial.print(dataBuff[3]);
    Serial.print(", Y: ");
    Serial.print(dataBuff[4]);
    Serial.print(", z:");
    Serial.print(dataBuff[5]);
    Serial.println(" rad/s");
    Serial.print(dataBuff[6]); 
    Serial.println("");
    delay(500);

    return 1;
  }//if scan and connect
  else if(!peripheral.connected() || err){
    //if disconnected
    Serial.print("Disconnected from the peripheral: ");
    Serial.println(peripheral.address());
    Serial.print("ERROR : ");                 // return value of scan_connect
    Serial.println(err);
    return 0;
  }
  
  return -1;
}


// void (* BLE_Core::charWritten)(BLEDevice peripheral, BLECharacteristic thisChar){

//   digitalWrite(LED_RED, LOW);         // event indicator ON
    
//   // wait while data buffer is accessed in main loop  
//   while(readingFlag == true) {
//   }    
//   dataCharacteristic.readValue(ud.dataBuff8, dataNum);  // read data packet 3.5uS  
//   ss = peripheral.rssi();                               // read signal strength 5~20mS
    
//   long timestampLED = millis();
//   while(millis() - timestampLED <= 200);   // Delay to make LED visible
    
//   digitalWrite(LED_RED, HIGH);        // event indicator OFF
// }