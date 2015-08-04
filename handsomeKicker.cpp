
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


//==================================== Detector =====================================

Detector::Detector(int _ldrPin) {
  ldrPin = _ldrPin;
  bounce_tag = 0;
}

void Detector::setup() {
// Serial.begin(9600);
  lastLdrValue = analogRead(ldrPin);
  bounce_tag = millis();
  Serial.println();
  Serial.print("LDR- ");
  Serial.println(lastLdrValue);
}

bool Detector::update() {
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

Team::Team(Display* _teamDisplay, Detector* _teamDetector, ButtonsGroup* buttonsGroup, byte _tonePin) {
  display_ = _teamDisplay;
  detector_ = _teamDetector;
  tonePin = _tonePin;
  buttonsGroup_ = buttonsGroup;
  score=0;
}

void Team::increaseScore() {
  Serial.print("Increase");
  score++;
  display_->displayNumber(score);
}

void Team::decreaseScore() {
  Serial.print("Decrease");
  if (score > 0) {
    score--;
    display_->displayNumber(score);
  }
}
void Team::resetScore() {
  score = 0;
  display_->displayNumber(score);
}


bool Team::update() {
  Serial.println("update");
  bool result = false;
  if (detector_->update()) {
    increaseScore();
    tone(tonePin, 500, 700);
    result = true;
  }
  buttonsGroup_->update();
  if (buttonsGroup_->isIncreasePress()) {
    increaseScore();
  }
  if (buttonsGroup_->isDecreasePress()) {
    decreaseScore();
  }
  return result;
}

//void Team::celebrateVictory() {
//  if (!endGame) {
//    tone(tonePin, 100, 1000);
//    delay(1000);
//    tone(tonePin, 200, 1000);
//    delay(1000);
//    tone(tonePin, 300, 1000);
//    delay(1000);
//    tone(tonePin, 400, 1000);
//    delay(1000);
//    tone(tonePin, 500, 1000);
//    delay(1000);
//    tone(tonePin, 700, 1000);
//    endGame = true;
//  }
//}

void Team::reset() {
  resetScore();
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
  //Serial.println("sdfsdfg");
  //incrButtonValue_ = debouncerIncrButton_->read();
  //decrButtonValue_ = debouncerDecrButton_->read();
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


Referee::Referee(Display* displayA, Display* displayB, Detector* detectorA, Detector* detectorB, ButtonsGroup* buttonsGroupA, ButtonsGroup* buttonsGroupB, byte tonePinA, byte tonePinB){
  displayA_ = displayA;
  displayB_ = displayB;
  detectorA_ = detectorA;
  detectorB_ = detectorB;
  buttonsGroupA_ = buttonsGroupA;
  buttonsGroupB_ = buttonsGroupB;
  tonePinA_ = tonePinA;
  tonePinB_ = tonePinB;
  Team* teamA = new Team(displayA_, detectorA_, buttonsGroupA_, tonePinA_);
  Team* teamB = new Team(displayB_, detectorB_, buttonsGroupB_, tonePinB_);

  currentGame_ = new Game();
  currentGame_->_start();
  settingsMode_ = false;

}

void Referee::setup() {
  displayA_->setup();
  displayB_->setup();
  detectorA_->setup();
  detectorB_->setup();
  buttonsGroupA_->setup();
  buttonsGroupB_->setup();
  pinMode(tonePinA_, OUTPUT);
  pinMode(tonePinB_, OUTPUT);
}

void Referee::update() {
//Serial.println();
  buttonsGroupA_->update();
  buttonsGroupB_->update();

  if (settingsMode_) { // Settings mode on
    if ( buttonsGroupA_->isDoubleLongPress() || buttonsGroupB_->isDoubleLongPress() ) {  // Exit from settings mode
      settingsMode_ = false;
    } else { // Continue settings mode
      // TODO Settings mode ===========

    } // end setings mode
  } else { //Main mode on
    if ( buttonsGroupA_->isDoubleLongPress() || buttonsGroupB_->isDoubleLongPress() ) { //Exit from main mode
      settingsMode_ = true;
    } else { // Continue main mode
//Serial.println("1"); 
      // Main mode ================
      if (currentGame_->isRun()) { // Game submode +++++++++
        //Serial.println("2"); 
        if ( buttonsGroupA_->isDoublePress() || buttonsGroupB_->isDoublePress() ) { // Game restart
          delete currentGame_;
          currentGame_ = new Game();
          currentGame_->_start();
        } else { // Game not restart
          //Serial.println("3"); 
          byte winner = currentGame_->isGameOver(teamA_->getScore(), teamB_->getScore());
          //Serial.print("winner ");  Serial.println(winner); 
          if ( winner != 0  ) { //Game end
            currentGame_->_stop();
            celebrateVictory(winner);
          } else { //Game not end
            //Do nothing or maybe refresh dislay
          //  teamA_->update();
          //  teamB_->update();
              buttonsGroupA_->update();
              buttonsGroupB_->update();
  if (buttonsGroupA_->isIncreasePress()) {
    displayA_->displayNumber(88);
  }
  if (buttonsGroupA_->isDecreasePress()) {
    displayA_->displayNumber(77);
  }
  if (buttonsGroupB_->isIncreasePress()) {
    displayB_->displayNumber(88);
  }
  if (buttonsGroupB_->isDecreasePress()) {
    displayB_->displayNumber(77);
  }



  
          }
        } // end game submode
      } else { // Idle submode +++++++++
        // TODO Banner display
        Serial.println("idle"); 
      }
    } //end main mode
  }
}


void Referee::celebrateVictory(byte winner){
  //if ( winner = 1 )
  
}


//==================================== Game =====================================

Game::Game() {
  gameRun_ = false;
  finalScore_ = 15;
}
Game::Game(byte finalScore) {
  finalScore_ = finalScore;
  gameRun_ = false;
}

void Game::_start() {
  gameRun_ = true;

}
void Game::_stop() {
  gameRun_ = false;
}
byte Game::isGameOver(byte scoreA, byte scoreB) {
  return 0;
  if (scoreA >= finalScore_) {
    if ( scoreA - scoreB > 1 ) {
      return 2;
    }
  }
  if (scoreB >= finalScore_) {
    if ( scoreB - scoreA > 1 ) {
      return 1;
    }
  }

  return 0;
}
bool Game::isRun() {
  return gameRun_;
}



