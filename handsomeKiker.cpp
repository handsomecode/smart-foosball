
#include "handsomeKiker.h"

//==================================== Display =====================================
Display::Display(int _dataPin, int _latchPin, int _clockPin) {
  dataPin = _dataPin;
  latchPin = _latchPin;
  clockPin = _clockPin;
}

void Display::displayNumber(int number) {
#ifdef DEBUG_MODE
  Serial.print("Display "); Serial.println(number);
#endif
  int numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};
  displaySigns(numbers[number % 10], numbers[(number / 10) % 10]);
}

void Display::displaySigns(int signOne, int signTwo) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, signOne);
  shiftOut(dataPin, clockPin, MSBFIRST, signTwo);
  digitalWrite(latchPin, HIGH);
}

void Display::setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}


//==================================== Gate =====================================

Gate::Gate(int _ldrPin) {
  ldrPin = _ldrPin;
  health_tag = 0;
  bounce_tag = 0;
}

void Gate::setup() {
  Serial.begin(9600);
  lastLdrValue = analogRead(ldrPin);
  bounce_tag = millis();
}

bool Gate::update() {
  bool result = false;
  int currentLdrValue = analogRead(ldrPin);
  if ( lastLdrValue - currentLdrValue >  HEALTH_TRESHOLD ) {
    unsigned long currentTime = millis();
    if ( currentTime - bounce_tag > BOUNCE_INTERVAL) {
      result = true;
#ifdef DEBUG_LDR


      Serial.print(" delta-"); Serial.print(lastLdrValue - currentLdrValue);


#endif
    }
    bounce_tag = millis();
  }
  //#ifdef DEBUG_LDR
  //  Serial.print(" curLdr-"); Serial.print(currentLdrValue);
  //  Serial.print(" lastLdr-"); Serial.print(lastLdrValue);
  //  Serial.print(" delta-"); Serial.print(lastLdrValue - currentLdrValue);
  //  Serial.print(" millis-"); Serial.print(millis());
  //  Serial.print(" result-"); Serial.println(result);
  //#endif

#ifdef DEBUG_MODE
  if (result) {
    Serial.print("Ldr signal");
    Serial.print(" curLdr-"); Serial.print(currentLdrValue);
    Serial.print(" delta-"); Serial.println(lastLdrValue - currentLdrValue);
  }
#endif
  lastLdrValue = currentLdrValue;
  return result;
}

//==================================== Team =====================================

Team::Team(Display* _teamDisplay, Gate* _teamGate, byte _incrButtonPin, byte _decrButtonPin, byte _tonePin) {
  teamDisplay = _teamDisplay;
  teamGate = _teamGate;
  incrButtonPin = _incrButtonPin;
  decrButtonPin = _decrButtonPin;
  tonePin = _tonePin;
  debouncerIncrButton = new Bounce();
  debouncerDecrButton = new Bounce();
  endGame = false;

}

void Team::increaseScore() {
  score++;
#ifdef DEBUG_MODE
  Serial.print("Increase to "); Serial.println(score);
#endif
  teamDisplay->displayNumber(score);
}

void Team::decreaseScore() {
#ifdef DEBUG_MODE
  Serial.print("Decrease from "); Serial.println(score);
#endif
  if (score > 0) {
    score--;

    teamDisplay->displayNumber(score);
  }
}
void Team::resetScore() {
  score = 0;
#ifdef DEBUG_MODE
  Serial.print("Reset to "); Serial.println(score);
#endif
  teamDisplay->displayNumber(score);
}


void Team::setup() {
  teamDisplay->setup();
  teamGate->setup();
  debouncerIncrButton->attach(incrButtonPin);
  debouncerIncrButton->interval(BUTTON_BOUNCE_TIME);
  pinMode(incrButtonPin, INPUT_PULLUP);
  debouncerDecrButton->attach(decrButtonPin);
  debouncerDecrButton->interval(BUTTON_BOUNCE_TIME);
  pinMode(decrButtonPin, INPUT_PULLUP);
  pinMode(tonePin, OUTPUT);
  reset();
}


void Team::updateGate() {
  if (teamGate->update()) {
    increaseScore();
    tone(tonePin, 500, 700);
  }
}

void Team::updateButtons() {

  debouncerIncrButton->update();
  debouncerDecrButton->update();

  int incrButtonValue = debouncerIncrButton->read();
  int decrButtonValue = debouncerDecrButton->read();
  if ( incrButtonValue == HIGH && decrButtonValue == HIGH ) {
#ifdef DEBUG_BUTTON
    Serial.print("incrButton-"); Serial.print(incrButtonValue); Serial.print(" decrButton-"); Serial.println(decrButtonValue);
#endif
    //resetScore();
    reset();
  }
  if (debouncerIncrButton->fell()) {
#ifdef DEBUG_BUTTON
    Serial.print("incrButton-"); Serial.print(incrButtonValue); Serial.print(" decrButton-"); Serial.println(decrButtonValue);
#endif
    increaseScore();
  }
  if (debouncerDecrButton->fell()) {
#ifdef DEBUG_BUTTON
    Serial.print("incrButton-"); Serial.print(incrButtonValue); Serial.print(" decrButton-"); Serial.println(decrButtonValue);
#endif
    decreaseScore();
  }
}

void Team::celebrateVictory() {
  if (!endGame) {
    tone(tonePin, 100, 1000);
    delay(1000);
    tone(tonePin, 200, 1000);
    delay(1000);
    tone(tonePin, 300, 1000);
    delay(1000);
    tone(tonePin, 400, 1000);
    delay(1000);
    tone(tonePin, 500, 1000);
    delay(1000);
    tone(tonePin, 700, 1000);
    endGame = true;
  }
}

void Team::reset() {
#ifdef DEBUG_MODE
  Serial.println("Team reset");
#endif
  resetScore();
  endGame = false;
}

byte Team::getScore() {
  return score;
}



