#include "AnimationManager.h"
#include <iostream>


AnimationManager::AnimationManager()
{
}

const double pi = 3.14159265358979323846;

AnimationManager::~AnimationManager()
{
}

void AnimationManager::registerAnimation(std::string name, sf::Texture& tex)
{
	if (tex.getSize().x <= 128)
		registerAnimation(name, sf::Sprite(tex));
	else
	{
		Animation anim = Animation();
		anim.setSpriteSheet(tex);
		for (int i = 0; i < tex.getSize().x; i += 128)
		{
			anim.addFrame(sf::IntRect(i, 0, 128, 128));
		}
		registerAnimation(name, anim);
	}
}

void AnimationManager::registerAnimation(std::string name, sf::Sprite sprite)
{
	items.insert({ name, sprite });
	items[name].setOrigin(64, 64);
}

void AnimationManager::registerAnimation(std::string name, Animation anim)
{
	animations.insert({ name, anim });
	//animations[name].setOrigin(64, 64);
}

void AnimationManager::registerMovement(UnitAnimations* anim, DrawableUnit* unit, std::vector<Node> path, float timePerNode)
{
	screen_movement.push_back(DrawableMovement{ anim,unit,path, path.size()*timePerNode,timePerNode });
}

void AnimationManager::playAnimation(std::string name, sf::Vector2f from, sf::Vector2f to, float timeToLive)
{
	float angle = 0;
	float dist = timeToLive;
	sf::Vector2f dir = sf::Vector2f(0, 0);
	from = { from.x + 0.5f,from.y + 0.5f };
	to = { to.x + 0.5f,to.y + 0.5f };
	if (from != to)
	{
		dist = (getDistance(from, to)*timeToLive);
		dir = (to - from) / dist;
		angle = atan2(dir.y, dir.x) * 180 / pi;
		std::cout << "The angle is " << angle << std::endl;		 
	}
	if (animations.count(name) > 0)
	{
		AnimatedSprite as = AnimatedSprite();
		as.setAnimation(animations[name]);
		as.setOrigin(64, 64);
		as.setRotation(angle);
		screen_animations.push_back(DrawableAnimation{ as,from,dir, dist });
	}
	else
	{
		items[name].setRotation(angle);
		screen_items.push_back(DrawableItem{ items[name],from,dir, dist });
	}
}

float AnimationManager::getDistance(sf::Vector2f from, sf::Vector2f to)
{
	sf::Vector2f AB = to - from;
	return sqrt(pow(AB.x, 2) + pow(AB.y, 2));
}

std::vector<DrawableItem> AnimationManager::getItems()
{
	if (!screen_movement.size())	//only show projectiles and other attackanimations if there is no movement animation going on
		return screen_items;
	return std::vector<DrawableItem>();
}

std::vector<DrawableAnimation> AnimationManager::getAnimations()
{
	if (!screen_movement.size())	//only show projectiles and other attackanimations if there is no movement animation going on
		return screen_animations;
	return std::vector<DrawableAnimation>();
}

std::vector<DrawableMovement> AnimationManager::getMovement()
{
	return screen_movement;
}

void AnimationManager::update(sf::Time time)
{
	//only update projectiles and other animations if there is no movement animation going on
	//enemies register their projectile animations at the same time with their movement, so they need to be paused until the movement is done
	if (!screen_movement.size()) 
	{
		for (std::vector<DrawableItem>::iterator it = screen_items.begin(); it != screen_items.end(); ) {
			it->pos += (it->dir*time.asSeconds());
			it->timeToLive -= time.asSeconds();
			if (it->timeToLive < 0)
			{
				it = screen_items.erase(it);
			}
			else ++it;
		}
		for (std::vector<DrawableAnimation>::iterator it = screen_animations.begin(); it != screen_animations.end(); ) {
			it->pos += (it->dir*time.asSeconds());
			it->timeToLive -= time.asSeconds();
			it->sprite.update(time);
			if (it->timeToLive < 0)
			{
				it->sprite.stop();
				it = screen_animations.erase(it);
			}
			else ++it;
		}
	}
	for (std::vector<DrawableMovement>::iterator it = screen_movement.begin(); it != screen_movement.end(); ) {
		if (it->timeToLive < time.asSeconds())
		{
			it->sprite->stop();
			it = screen_movement.erase(it);
			continue;
		}

		int stepsLeft = it->timeToLive / it->timePerNode;
		float timeLeftOnCurrentNode = it->timeToLive - stepsLeft*it->timePerNode;
		float timeStep = time.asSeconds();
		
		while (timeLeftOnCurrentNode < timeStep)
		{
			it->unit->x = it->path.front().x;
			it->unit->y = it->path.front().y;
			it->path.erase(it->path.begin());

			timeStep -= timeLeftOnCurrentNode;
			it->timeToLive -= timeLeftOnCurrentNode;
			timeLeftOnCurrentNode = it->timePerNode;

			if (!it->path.empty())
			{
				int x = it->path.front().x - it->unit->x;
				int y = it->path.front().y - it->unit->y;
				switch (x)
				{
				case 1:
					it->sprite->play("moveRight");
					break;
				case -1:
					it->sprite->play("moveLeft");
					break;
				case 0:
					switch (y)
					{
					case 1:
						it->sprite->play("moveDown");
						break;
					case -1:
						it->sprite->play("moveUp");
						break;
					case 0:
						break; // THIS SHOULD NEVER HAPPEN (next node = current node)
					}
				}
				//it->unit->sprite = it->sprite->sprite;
			}
			else
			{
				it->sprite->play("moveDown");
				//it->unit->sprite = it->sprite->sprite;
			}

		}
		if (!it->path.empty())
		{
			float progress = timeStep / it->timePerNode;
			float x = it->path.front().x - it->unit->x;
			float y = it->path.front().y - it->unit->y;
			if (x > 0)
				it->unit->x += progress;
			else if (x < 0)
				it->unit->x -= progress;
			else if (y > 0)
				it->unit->y += progress;
			else if (y < 0)
				it->unit->y -= progress;
		}

		it->timeToLive -= timeStep;
		++it;
	}
}