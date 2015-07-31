
#include "handsomeKicker.h"

//==================================== Display =====================================
Display::Display(int _dataPin, int _latchPin, int _clockPin) {
  dataPin = _dataPin;
  latchPin = _latchPin;
  clockPin = _clockPin;
}

void Display::displayNumber(int number) {
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
  bounce_tag = 0;
}

void Gate::setup() {
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
    }
    bounce_tag = millis();
  }
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
  teamDisplay->displayNumber(score);
}

void Team::decreaseScore() {
  if (score > 0) {
    score--;
    teamDisplay->displayNumber(score);
  }
}
void Team::resetScore() {
  score = 0;
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
    //resetScore();
    reset();
  }
  if (debouncerIncrButton->fell()) {
    increaseScore();
  }
  if (debouncerDecrButton->fell()) {
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
  resetScore();
  endGame = false;
}

byte Team::getScore() {
  return score;
}



Referee::Referee(Display* displayA, Display* displayB, Gate* gateA, Gate* gateB, byte incrButtonPinA, byte decrButtonPinA, byte tonePinA, byte incrButtonPinB, byte decrButtonPinB, byte tonePinB) {
  displayA_ = displayA;
  displayB_ = displayB;
  gateA_ = gateA;
  gateB_ = gateB;
  Team* teamA = new Team(displayA_, gateA_, incrButtonPinA, decrButtonPinA, tonePinA);
  Team* teamB = new Team(displayB_, gateB_, incrButtonPinB, decrButtonPinB, tonePinB);

}

void Referee::setup() {
  Serial.begin(9600);
  teamA_->setup();
  teamB_->setup();
  finalScore = 15;
}
void Referee::update() {
  Serial.println();
  teamA_ -> updateButtons();
  teamB_ -> updateButtons();
  teamA_ -> updateGate();
  teamB_ -> updateGate();
  byte scoreA = teamA_->getScore();
  byte scoreB = teamB_->getScore();
  if (scoreA >= finalScore) {
    if ( scoreA - scoreB > 1 ) {
      teamB_->celebrateVictory();
    }
  }

  if (scoreB >= finalScore) {
    if ( scoreB - scoreA > 1 ) {
      teamA_->celebrateVictory();
    }
  }



}

