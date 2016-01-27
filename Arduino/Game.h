#include "Arduino.h"
#include "BallDetector.h"

#ifndef GAME_H
#define GAME_H

#define GOAL_DETECTED_BY_SIDE_A false
#define GOAL_DETECTED_BY_SIDE_B true

class Game {

public:
  Game(byte ldrPinA, byte ldrPinB);
  ~Game();
  void update();

private:
  void handleBallDetectors();

  BallDetector* ballDetectorA;
  BallDetector* ballDetectorB;
};

#endif
