#include "Arduino.h"
#include "Display.h"
#include "BallDetector.h"
#include "ControlPanel.h"
#include "WinnerMelody.h"

#ifndef GAME_H
#define GAME_H

class Game {

public:
  Game();
  ~Game();
  void update();

private:
  void handleControlPanels();
  void handleBallDetectors();
  void increaseScoreA();
  void increaseScoreB();
  void decreaseScoreA();
  void decreaseScoreB();
  void restart();
  void showNextGameMode();
  void applyChosenGameMode();
  byte getGameModesCount();
  void blinkScore();

  byte maxScore;
  byte scoreA;
  byte scoreB;
  bool gameStopped;
  bool configModeEnabled;
  byte modeIndex;
  byte victoryCountA;
  byte victoryCountB;

  const byte gameModes[5] = {7, 10, 15, 20, 35};

  Display* displayA; 
  Display* displayB;
  BallDetector* ballDetectorA;
  BallDetector* ballDetectorB;
  ControlPanel* controlPanelA; 
  ControlPanel* controlPanelB;

	/*hardware config*/
	//Пин подключен к SER входу SN74HC595N
  byte dataPinA = 2;

  //Пин подключеный к SRCLR входу SN74HC595N
  byte latchPinA = 3;

  //Пин подключеный к SRCLK входу SN74HC595N
  byte clockPinA = 4;

  //Пин подключен к SER входу SN74HC595N
  byte dataPinB = 7;

  //Пин подключеный к SRCLR входу SN74HC595N
  byte latchPinB = 8;

  //Пин подключеный к SRCLK входу SN74HC595N
  byte clockPinB = 9;

  byte ldrPinA = 1;
  byte ldrPinB = 0;

  byte incrButtonPinA = 5;
  byte decrButtonPinA = 6;

  byte incrButtonPinB = 10;
  byte decrButtonPinB = 11;

  byte tonePinA = 13;
  byte tonePinB = 12;
};

#endif
