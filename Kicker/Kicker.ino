#include "Bounce2.h"
#include "handsomeKicker.h"

static volatile int finalScore;

//Пин подключен к SER входу SN74HC595N
int dataPinA = 2;
//Пин подключеный к SRCLR входу SN74HC595N
int latchPinA = 3;
//Пин подключеный к SRCLK входу SN74HC595N
int clockPinA = 4;
//Пин подключен к SER входу SN74HC595N
int dataPinB = 7;
//Пин подключеный к SRCLR входу SN74HC595N
int latchPinB = 8;
//Пин подключеный к SRCLK входу SN74HC595N
int clockPinB = 9;

int ldrPinA = 1;
int ldrPinB = 0;

int incrButtonPinA = 5;
int decrButtonPinA = 6;

int incrButtonPinB = 10;
int decrButtonPinB = 11;

int tonePinA = 13;
int tonePinB = 12;


//=====================================================================

Display* displayA = new Display(dataPinA, latchPinA, clockPinA);
Display* displayB = new Display(dataPinB, latchPinB, clockPinB);

Gate* gateA = new Gate(ldrPinA);
Gate* gateB = new Gate(ldrPinB);

Team* teamA = new Team(displayA, gateA, incrButtonPinA, decrButtonPinA, tonePinA);
Team* teamB = new Team(displayB, gateB, incrButtonPinB, decrButtonPinB, tonePinB);

void setup() {
  Serial.begin(9600);
  teamA->setup();
  teamB->setup();
  finalScore = 15;

}


void loop()
{

  Serial.println();
  teamA -> updateButtons();
  teamB -> updateButtons();
  teamA -> updateGate();
  teamB -> updateGate();
  byte scoreA = teamA->getScore();
  byte scoreB = teamB->getScore();
  if (scoreA >= finalScore) {
    if ( scoreA - scoreB > 1 ) {
      teamB->celebrateVictory();
    }
  }

  if (scoreB >= finalScore) {
    if ( scoreB - scoreA > 1 ) {
      teamA->celebrateVictory();
    }
  }



}

