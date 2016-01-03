#pragma once
#include "SFML\Graphics.hpp"
#include <unordered_map>
#include "drawableItem.h"
#include "AnimatedSprite.hpp"
#include "drawableUnit.h"

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();
	void registerAnimation(std::string name, sf::Texture& tex);
	void registerAnimation(std::string name, sf::Sprite sprite);
	void registerAnimation(std::string name, Animation sprite);
	void registerMovement(UnitAnimations * anim, DrawableUnit * unit, std::vector<Node> path, float timePerNode);
	void playAnimation(std::string name, sf::Vector2f from, sf::Vector2f to, float timeToLive);
	std::vector<DrawableItem> getItems();
	std::vector<DrawableAnimation> getAnimations();
	std::vector<DrawableMovement> getMovement();
	void update(sf::Time time);


private:
	std::unordered_map<std::string,sf::Sprite> items;	//all fake animations (simple objects moving)
	std::unordered_map<std::string, Animation> animations;	//all real animations
	std::vector<DrawableItem> screen_items;						//all items (animations) currently on screen
	std::vector<DrawableAnimation> screen_animations;
	std::vector<DrawableMovement> screen_movement;

	float getDistance(sf::Vector2f from, sf::Vector2f to);
};

