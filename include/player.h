#pragma once
#include "textureManager.h"
#include "Unit.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

enum class PlayerType {
	armyfighter,
	armyweirdo
};

class Player : public Unit {
public:
	Player(UnitAnimations* anim, UnitType type, PlayerType pType, std::vector<ActionEvent*> actions, int AP = 20, int HP = 25, int xPos = 0, int yPos = 15);
	PlayerType playerType;
	~Player();
};
