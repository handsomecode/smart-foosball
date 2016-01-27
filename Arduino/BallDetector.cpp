#include "BallDetector.h"

BallDetector::BallDetector(byte ldrPin) {
  this->ldrPin = ldrPin;

  lastLdrValue = analogRead(ldrPin);
  bounceTag = millis();
}

bool BallDetector::isGoal() {
  bool result = false;
  int currentLdrValue = analogRead(ldrPin);
  if ( lastLdrValue - currentLdrValue >  HEALTH_TRESHOLD ) {
    unsigned long currentTime = millis();
    if ( currentTime - bounceTag > BOUNCE_INTERVAL) {
      result = true;
    }
    bounceTag = millis();
  }
  lastLdrValue = currentLdrValue;
  return result;
}
