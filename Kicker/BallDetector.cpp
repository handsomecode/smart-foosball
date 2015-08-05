#include "BallDetector.h"

BallDetector::BallDetector(byte ldrPin) {
  this->ldrPin = ldrPin;

  lastLdrValue = analogRead(ldrPin);
  bounce_tag = millis();
}

bool BallDetector::isGoal() {
  bool result = false;
  int currentLdrValue = analogRead(ldrPin);
  if ( lastLdrValue - currentLdrValue >  HEALTH_TRESHOLD ) {
    unsigned long currentTime = millis();
    if ( currentTime - bounce_tag > BOUNCE_INTERVAL) {
      result = true;
    }
    bounce_tag = millis();
  }
  lastLdrValue = currentLdrValue;
  return result;
}
