#include "Game.h"

#define BAUD_RATE 115200
#define LDR_PIN_A 1
#define LDR_PIN_B 0

Game* game;

void setup() {
  Serial.begin(BAUD_RATE);
  game = new Game(LDR_PIN_A, LDR_PIN_B);
}


void loop() {
  game->update();
}

