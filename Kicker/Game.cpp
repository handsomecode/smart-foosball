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

  if(Serial1.available()) // check if the esp is sending a message 
  {
    while(Serial1.available())
    {
      // The esp has data so display its output to the serial window 
      response = Serial1.readString(); // read the next character.
//      Serial.println(response);
    }  
  }
    
  if(response.substring(response.length()-2, response.length()-1) == ">") {
          sendPostRequest();
          response = "";
   }
      
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
//				tone(tonePinA, 950, 500);
				scoreA = scoreB = 0;
				blinkScore();
			} else {
//				playStarWarsTheme(tonePinA);
				gameStopped = true;
			}

		} else if(scoreB >= maxScore && (scoreB -scoreA) > 1) {

      //check a game mode, if there is a 3/5 mode, handle it
			if(modeIndex == getGameModesCount() - 1 && victoryCountB < 1) {
				victoryCountB++;
//				tone(tonePinB, 950, 500);
				scoreA = scoreB = 0;
				blinkScore();
			} else {
//				playImperialMarch(tonePinB);
				gameStopped = true;
			}
		}	
	}
}

//**********************************************
void Game::createPostRequest() {
  Serial1.println(COMMAND_START_TCP_CONNECTION);
  isSending = true;

  //log
//        Serial.println(COMMAND_START_TCP_CONNECTION);

   delay(300);

   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   request = getPostRequestString();
   Serial1.println(request.length());
}

void Game::sendPostRequest() {
  Serial1.println(request);
  Serial1.flush();
}

String Game::getPostRequestString(){
  String body = SCORE_1;
  body += String(scoreA);
  body += AMP;
  body += SCORE_2;
  body += String(scoreB);

  String postRequest = POST_PREFIX;
  postRequest += String(body.length());
  postRequest += CRLF;
  postRequest += CRLF;
  postRequest += body;
  return postRequest;
}


String Game::buildRequestString(String side){
  String body = SIDE;
  body += side;

  String postRequest = POST_PREFIX;
  postRequest += String(body.length());
  postRequest += CRLF;
  postRequest += CRLF;
  postRequest += body;
  return postRequest;
}
void Game::createRequest(String side) {
  Serial1.println(COMMAND_START_TCP_CONNECTION);
   delay(300);
   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   request = buildRequestString(side);
   Serial1.println(request.length());
}

void Game::createRestartRequest() {
  Serial1.println(COMMAND_START_TCP_CONNECTION);

  //log
//        Serial.println(COMMAND_START_TCP_CONNECTION);

   delay(300);

   Serial1.print(COMMAND_SEND_MESSAGE_SIZE);
   request = getRestartRequestString();
   Serial1.println(request.length());
}

String Game::getRestartRequestString() {
  String body = "restart=";
  body += "true";

  String postRequest = POST_PREFIX;
  postRequest += String(body.length());
  postRequest += CRLF;
  postRequest += CRLF;
  postRequest += body;
  return postRequest;
}
//**********************************************

void Game::handleBallDetectors() {
	if(ballDetectorA->isGoal()) {
		increaseScoreA();
//    createPostRequest();
//    tone(tonePinA, 950, 400);
//    createRequest("A");
    boolean b = false;
    Serial.print(b);
//		tone(tonePinA, 500, 300);
    
	}
	if(ballDetectorB->isGoal()) {
		increaseScoreB();
//		createPostRequest();
//    tone(tonePinB, 950, 400);
    createRequest("B");
//		tone(tonePinB, 500, 300);
    
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
  createRestartRequest();
}

void Game::increaseScoreA() {
	scoreA++;
}

void Game::increaseScoreB() {
	scoreB++;
  boolean b = true;
    Serial.print(b);
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
