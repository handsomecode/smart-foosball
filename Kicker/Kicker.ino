#include "Button.h"
#include "Game.h"

  Game* game;

void setup() {
  game = new Game();
}


void loop() {
  game->update();
}

