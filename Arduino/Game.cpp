#include "Game.h"

Game::Game(byte ldrPinA, byte ldrPinB) {
	ballDetectorA = new BallDetector(ldrPinA);
	ballDetectorB = new BallDetector(ldrPinB);
}

Game::~Game() {
	delete ballDetectorA;
	delete ballDetectorB;
}

void Game::update(){
	handleBallDetectors();
}

void Game::handleBallDetectors() {
	if(ballDetectorA->isGoal()) {
    Serial.println(GOAL_DETECTED_BY_SIDE_A);
	}
 
	if(ballDetectorB->isGoal()) {
    Serial.println(GOAL_DETECTED_BY_SIDE_B);
	}
}
