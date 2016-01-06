#pragma once
#include "Game.h"

int main() {
	
	bool restart = true;

	while (restart) {
		Game game;
		game.init();
		restart = game.update();
	}

	return 0;
}
