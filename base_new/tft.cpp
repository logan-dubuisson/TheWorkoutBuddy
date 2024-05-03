#include "tft.h"


TFT::TFT(){
  
  if(!Serial){
    Serial.begin(115200);
  }

  tft.init(240, 320);

  tft.setRotation(3);

  Serial.println(F("Initialized"));

  currentScreen = NONE;

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

}


TFT::~TFT(){}


void TFT::bootScreen(){

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
  currentScreen = BOOT;

}


void TFT::mainScreen(uint32_t reps, uint32_t sets, uint32_t form){

  tft.fillScreen(ST77XX_BLACK);
  // LCD rep, set, form display
  tft.setTextWrap(false);
  tft.setCursor(60, 40);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(4);
  tft.print("Rep: ");
  tft.println(reps);
  tft.setCursor(60,100);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(4);
  tft.print("Set: ");
  tft.println(sets);
  tft.setCursor(60,160);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(4);
  tft.print("Form: ");
  tft.println(form);
  currentScreen = MAIN;

}

void TFT::update(uint32_t reps, uint32_t sets, uint32_t form){
  if(currentScreen != MAIN){
    mainScreen(reps, sets, form);
  }else{
    // UPDATE AT SPECIFIC CURSOR POSITIONS
  }
}


void TFT::connectScreen(){

  tft.setCursor(5,100);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.print("Connecting...");
  currentScreen = CONNECT;
  
}


void TFT::wipe(){
  tft.fillScreen(ST77XX_BLACK);
}