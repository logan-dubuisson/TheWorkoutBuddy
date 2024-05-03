#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#ifndef _TFT_H_
#define _TFT_H_

//LCD module pin assignments
#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         7


class TFT {

  public:
    TFT();

    ~TFT();

    void bootScreen();

    void mainScreen(uint32_t reps, uint32_t sets, uint32_t form);

    void update(uint32_t reps = 0, uint32_t sets = 0, uint32_t form = 0);

    void connectScreen();

    void wipe();

  private:
    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

    enum {
      NONE,
      BOOT,
      CONNECT,
      MAIN
    } currentScreen;

};


#endif // TFT_H