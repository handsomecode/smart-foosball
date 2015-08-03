#include "ControlPanel.h"

ControlPanel::ControlPanel(byte increaseButtonPin, byte decreaseButtonPin) {
  this->increaseButtonPin = increaseButtonPin;
  this->decreaseButtonPin = decreaseButtonPin;

  this->increaseButton = new Button(increaseButtonPin, PULLUP, INVERT, DEBOUNCE_MS);
  this->decreaseButton = new Button(decreaseButtonPin, PULLUP, INVERT, DEBOUNCE_MS);
}

ControlPanel::~ControlPanel() {
  delete increaseButton;
  delete decreaseButton;
}

void ControlPanel::update() {
  increaseButton->read();
  decreaseButton->read();
}

bool ControlPanel::isIncreaseButtonPressed() {
  return increaseButton->wasReleased();
}

bool ControlPanel::isDecreaseButtonPressed() {
  return decreaseButton->wasReleased();
}

bool ControlPanel::isButtonPressedTogether() {
  return decreaseButton->isReleased() && increaseButton->isReleased();
}

bool ControlPanel::isIncreaseButtonLongPressed() {
  return increaseButton->releasedFor(1000);
}
bool ControlPanel::isDecreaseButtonLongPressed() {
  return decreaseButton->releasedFor(1000);
}
bool ControlPanel::isButtonsLongPressedTogether() {
  return increaseButton->releasedFor(1000) && decreaseButton->releasedFor(1000);
}
