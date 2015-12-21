#pragma once
#include "textureManager.h"
#include "Unit.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>


class Player : public Unit {
public:
	Player(TextureManager* texMng, sf::Sprite s, UnitType type, int AP = 20, int HP = 30, int xPos = 0, int yPos = 15);
	~Player();
};
