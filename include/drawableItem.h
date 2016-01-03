#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"
#include "UnitAnimations.hpp"
#include "drawableUnit.h"
#include "Node.h"

struct DrawableItem {
	sf::Sprite sprite;
	sf::Vector2f pos;
	sf::Vector2f dir;
	float timeToLive;
};

struct DrawableAnimation {
	AnimatedSprite sprite;
	sf::Vector2f pos;
	sf::Vector2f dir;
	float timeToLive;
};

struct DrawableMovement {
	UnitAnimations* sprite;
	DrawableUnit* unit;
	std::vector<Node> path;
	float timeToLive;
	float timePerNode;
};