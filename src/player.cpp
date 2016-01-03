#pragma once
#include "player.h"


Player::Player(UnitAnimations* anim, UnitType type, PlayerType pType, std::vector<ActionEvent*> actions, int AP, int HP, int xPos, int yPos) : Unit(AP, HP, anim, nullptr, xPos, yPos, type)
{ 	
	playerType = pType;

	learnedActions = actions;
}

Player::~Player() {}

