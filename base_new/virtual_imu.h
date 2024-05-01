//*********************************************************************
// Simple class for virtualization of IMU functionality to
// facilitate R/W capabilites over Bluetooth Low Energy
//
#include <stdint.h>

#ifndef _VIRTUAL_IMU_H_
#define _VIRTUAL_IMU_H_

#define dataNum 14
#define THRESHOLD 6 // Threshold for detecting movement, adjust as needed
#define REP_DELAY 5000 // Delay in milliseconds between reps

class Virtual_IMU {

  public:
    Virtual_IMU();

    ~Virtual_IMU();

    void process();

    float data[dataNum/2];

    enum {
      AX, AY, AZ,
      GX, GY, GZ,
      TEMP
    };

    uint32_t reps = 0, sets = 0, form = 0;
  
  private:


};


#endif // VIRTUAL_IMU_H