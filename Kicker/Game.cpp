#include "Game.h"

Game::Game() {
	displayA = new Display(dataPinA, latchPinA, clockPinA);
	displayB = new Display(dataPinB, latchPinB, clockPinB);

	ballDetectorA = new BallDetector(ldrPinA);
	ballDetectorB = new BallDetector(ldrPinB);

	controlPanelA = new ControlPanel(incrButtonPinA, decrButtonPinA);
	controlPanelB = new ControlPanel(incrButtonPinB, decrButtonPinB); 

	maxScore = 15;
	scoreA = scoreB = 0;
	victoryCountA = victoryCountB = 0;

	gameStopped = false;
	configModeEnabled = false;
	
	modeIndex = 0;
}

Game::~Game() {
	delete displayA;
	delete displayB;
	delete ballDetectorA;
	delete ballDetectorB;
	delete controlPanelA;
	delete controlPanelB;
}

void Game::update(){
	handleControlPanels();
	handleBallDetectors();

	if(!configModeEnabled) {
		displayA->displayNumber(scoreA);
		displayB->displayNumber(scoreB);
	}

	if(!gameStopped) {
		if(scoreA >= maxScore && (scoreA - scoreB) > 1 ) {

      //check a game mode, if there is a 3/5 mode, handle it
			if(modeIndex == getGameModesCount() - 1 && victoryCountA < 1) {
				victoryCountA++;
				tone(tonePinA, 950, 500);
				scoreA = scoreB = 0;
				blinkScore();
			} else {
				playStarWarsTheme(tonePinA);
				gameStopped = true;
			}

		} else if(scoreB >= maxScore && (scoreB -scoreA) > 1) {

      //check a game mode, if there is a 3/5 mode, handle it
			if(modeIndex == getGameModesCount() - 1 && victoryCountB < 1) {
				victoryCountB++;
				tone(tonePinB, 950, 500);
				scoreA = scoreB = 0;
				blinkScore();
			} else {
				playImperialMarch(tonePinB);
				gameStopped = true;
			}
		}	
	}
}

void Game::handleBallDetectors() {
	if(ballDetectorA->isGoal()) {
		increaseScoreA();
		tone(tonePinA, 500, 700);
	}
	if(ballDetectorB->isGoal()) {
		increaseScoreB();
		tone(tonePinB, 500, 700);
	}
}

void Game::handleControlPanels() {
	controlPanelA->update();
	controlPanelB->update();

	if(controlPanelB->isButtonsLongPressedTogether() || controlPanelA->isButtonsLongPressedTogether()) {
		if(!configModeEnabled) {
			gameStopped = true;
			configModeEnabled = true;

			displayB->displayCF();
			displayA->displayCF();
			
			displayA->displayNumber(gameModes[modeIndex]);
			displayB->displayNumber(gameModes[modeIndex]);
		} 
	}

	if(controlPanelA->isButtonPressedTogether() || controlPanelB->isButtonPressedTogether()) {
		restart();
	}

	if(controlPanelB->isIncreaseButtonPressed()) {
		if(configModeEnabled) {
			showNextGameMode();
		} else {
			increaseScoreB();  
		}
	}

	if(controlPanelB->isDecreaseButtonPressed()) {
		if(configModeEnabled) {
			applyChosenGameMode();
			configModeEnabled = false;
			restart();
		} else {
			decreaseScoreB();  
		}
	}

	if(controlPanelA->isIncreaseButtonPressed()) {
		if(configModeEnabled) {
			showNextGameMode();
		} else {
			increaseScoreA();  
		}
	}

	if(controlPanelA->isDecreaseButtonPressed()) {
		if(configModeEnabled) {
			applyChosenGameMode();
			configModeEnabled = false;
			restart();
		} else {
			decreaseScoreA();  
		}
	}

}

void Game::showNextGameMode() {
	modeIndex++;
	if(modeIndex == getGameModesCount()) {
		modeIndex = 0;
	}
	displayA->displayNumber(gameModes[modeIndex]);
	displayB->displayNumber(gameModes[modeIndex]);
}

void Game::applyChosenGameMode() {
	if(modeIndex == getGameModesCount() - 1) {
		maxScore = gameModes[modeIndex] % 10;
	} else {
		maxScore = gameModes[modeIndex];
	}
	displayB->displayGO();
	displayA->displayGO();
}

void Game::blinkScore() {
	for(byte i = 0; i < 3; i++) {
		displayA->switchOff();
		displayB->switchOff();
		delay(500);
		displayA->displayNumber(scoreA);
		displayB->displayNumber(scoreB);
		delay(500);
	}
}

byte Game::getGameModesCount() {
	return sizeof(gameModes)/sizeof(gameModes[0]);
}

void Game::restart(){
	scoreA = scoreB = 0;
	victoryCountA = victoryCountB = 0;
	gameStopped = false;
}

void Game::increaseScoreA() {
	scoreA++;
}

void Game::increaseScoreB() {
	scoreB++;
}

void Game::decreaseScoreA() {
	if(scoreA > 0) {
		scoreA--;  
	}
}

void Game::decreaseScoreB() {
	if(scoreB > 0) {
		scoreB--;  
	}
}