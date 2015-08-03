#include "Display.h"

Display::Display(byte dataPin, byte latchPin, byte clockPin) {
    this->dataPin = dataPin;
    this->latchPin = latchPin;
    this->clockPin = clockPin;

    initPins();
}

void Display::initPins() {
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void Display::displayNumber(int number) {
    setSymbols(numbers[(number / 10) % 10], numbers[number % 10]);
}

void Display::setSymbols(byte highSymbol,byte lowSymbol) {
    digitalWrite(latchPin, LOW); 
    shiftOut(dataPin, clockPin, MSBFIRST, lowSymbol);
    shiftOut(dataPin, clockPin, MSBFIRST, highSymbol);
    digitalWrite(latchPin, HIGH);
}

void Display::displayDash() {
  setSymbols(DASH, DASH);
}

void Display::switchOff() {
  setSymbols(NONE, NONE);
}

void Display::displayGO() {
  for(byte i = 0; i < 3; i++) {
    switchOff();
    delay(300);
    setSymbols(G, O);
    delay(300);
  }
}

void Display::displayCF() {
  for(byte i = 0; i < 3; i++) {
    switchOff();
    delay(300);
    setSymbols(C, F);
    delay(300);
  }
}

