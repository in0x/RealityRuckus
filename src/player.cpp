#pragma once
#include "player.h"


Player::Player(TextureManager* texMng, sf::Sprite s, UnitType type, int AP, int HP, int xPos, int yPos) : Unit(AP, HP, s, texMng, xPos, yPos, type) {}

Player::~Player() {}

