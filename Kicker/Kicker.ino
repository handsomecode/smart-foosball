#include "Button.h"
#include "Game.h"

  Game* game;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); // your esp's baud rate might be different
  game = new Game();
}


void loop() {
  game->update();
}

