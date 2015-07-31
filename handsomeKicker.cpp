
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
  endGame = false;
  buttonsGroup_ = new ButtonsGroup(incrButtonPin,decrButtonPin);
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
  pinMode(tonePin, OUTPUT);
  buttonsGroup_->setup();
  reset();
}


void Team::updateGate() {
  if (teamGate->update()) {
    increaseScore();
    tone(tonePin, 500, 700);
  }
}

void Team::updateButtons() {

  buttonsGroup_->update();
 
  if ( buttonsGroup_->isDoublePress() ) {
    //resetScore();
    reset();
  }
  if (buttonsGroup_->isIncreasePress()) {
    increaseScore();
  }
  if (buttonsGroup_->isDecreasePress()) {
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

//==================================== ButtonsGroup =====================================
ButtonsGroup::ButtonsGroup(byte incrButtonPin, byte decrButtonPin) {
  incrButtonPin_ = incrButtonPin;
  decrButtonPin_ = decrButtonPin;
  debouncerIncrButton_ = new Bounce();
  debouncerDecrButton_ = new Bounce();
}
void ButtonsGroup::setup() {
  debouncerIncrButton_->attach(incrButtonPin_);
  debouncerIncrButton_->interval(BUTTON_BOUNCE_TIME);
  pinMode(incrButtonPin_, INPUT_PULLUP);
  debouncerDecrButton_->attach(decrButtonPin_);
  debouncerDecrButton_->interval(BUTTON_BOUNCE_TIME);
  pinMode(decrButtonPin_, INPUT_PULLUP);
}
void ButtonsGroup::update() {
  debouncerIncrButton_->update();
  debouncerDecrButton_->update();
  int incrButtonValue_ = debouncerIncrButton_->read();
  int decrButtonValue_ = debouncerDecrButton_->read();
}

bool ButtonsGroup::isIncreasePress() {
  if (debouncerIncrButton_->fell()) {
    return true;
  }
  return false;
}
bool ButtonsGroup::isDecreasePress() {
  if (debouncerDecrButton_->fell()) {
    return true;
  }
  return false;
}
bool ButtonsGroup::isDoublePress() {
  if ( incrButtonValue_ == HIGH && decrButtonValue_ == HIGH ) {
    return true;
  }
  return false;
}
bool ButtonsGroup::isIcreaseLongPress() {
  //TODO
  return false;
}
bool ButtonsGroup::isDecreaseLongPress() {
  //TODO
  return false;
}
bool ButtonsGroup::isDoubleLongPress() {
  //TODO
  return false;
}


//==================================== Referee =====================================


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


void Referee::assignWinner(){
  //gameStop
  //toneMusik
}

void Referee::gameStop(){
  //gateA-Stop
  //fgateB-Stop
}

