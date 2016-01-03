#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"

struct DrawableUnit {
	float x; 
	float y;
	AnimatedSprite* sprite;
};