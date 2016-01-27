#include "Arduino.h"

#ifndef DETECTOR_H
#define DETECTOR_H

//Photoresistor treshold for state "no signal"
#define HEALTH_TRESHOLD 5
#define BOUNCE_INTERVAL 2000

class BallDetector {
  
  public:
    BallDetector (byte ldrPin);
    bool isGoal();
  
  private:
    byte ldrPin;
    int lastLdrValue;
    unsigned long bounceTag;
};

#endif
