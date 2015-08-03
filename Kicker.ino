#include <Bounce2.h>
#include "handsomeKicker.h"

//First display pins
int dataPinA = 2;
int latchPinA = 3;
int clockPinA = 4;
//Second display pins
int dataPinB = 7;
int latchPinB = 8;
int clockPinB = 9;
//Serail pins from light resistor (gate)
int ldrPinA = 1;
int ldrPinB = 0;
//Pins buttons
int incrButtonPinA = 5;
int decrButtonPinA = 6;
int incrButtonPinB = 10;
int decrButtonPinB = 11;
//Pins speakers
int tonePinA = 13;
int tonePinB = 12;

//=====================================================================

Display* displayA = new Display(dataPinA, latchPinA, clockPinA);
Display* displayB = new Display(dataPinB, latchPinB, clockPinB);
Detector* detectorA = new Detector(ldrPinA);
Detector* detectorB = new Detector(ldrPinB);
ButtonsGroup* buttonsGroupA = new ButtonsGroup(incrButtonPinA, decrButtonPinA);
ButtonsGroup* buttonsGroupB = new ButtonsGroup(incrButtonPinB, decrButtonPinB);

Referee* referee = new Referee(displayA, displayB, detectorA, detectorB, buttonsGroupA, buttonsGroupB, tonePinA, tonePinB);

void setup() {
  referee->setup();
}

void loop()
{
  referee->update();
}

