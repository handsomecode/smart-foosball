#include "BallDetector.h"

BallDetector::BallDetector(byte ldrPin) {
  this->ldrPin = ldrPin;

  lastLdrValue = analogRead(ldrPin);
  bounce_tag = millis();
}

bool BallDetector::isGoal() {
  bool result = false;
  int currentLdrValue = analogRead(ldrPin);
  Serial.println(currentLdrValue);
  if ( lastLdrValue - currentLdrValue >  HEALTH_TRESHOLD ) {
    unsigned long currentTime = millis();
    if ( currentTime - bounce_tag > BOUNCE_INTERVAL) {
      result = true;
      Serial.println("goal");
    }
    bounce_tag = millis();
  }
  lastLdrValue = currentLdrValue;
  return result;
}
