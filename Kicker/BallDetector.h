#include "Arduino.h"

#ifndef DETECTOR_H
#define DETECTOR_H

//Порог фоторезистора считающийся как "Нет сигнала"
#define HEALTH_TRESHOLD 50
#define BOUNCE_INTERVAL 500

class BallDetector {
  
  public:
    BallDetector (byte ldrPin);
    bool isGoal();
  
  private:
    byte ldrPin;
    int lastLdrValue;
    unsigned long bounce_tag;
};

#endif
