#include "UIManager.h"
#include <iostream>
UIManager::UIManager()
{
	scale = 1;
}

void UIManager::update(GameEvent e) {
	
}

void UIManager::drawNewFrame(sf::RenderWindow* w, std::vector<DrawableUnit> unitList) {
	w->clear();
	setUnits(unitList);
	drawEverything(w);
	w->display();
}

float UIManager::getX()
{
	return x;
}

float UIManager::getY()
{
	return y;
}

void UIManager::setX(float newX)
{
	if(newX >= -2000 && newX <= 200)
	x = newX;
}

void UIManager::setY(float newY)
{
	if (newY >= -3000 && newY <= 200)
		y = newY;
}

void UIManager::setTerrain(sf::Sprite t)
{
	terrain = t;
}

void UIManager::setUnits(std::vector<DrawableUnit> v)
{
	units = v;
}

void UIManager::setItems(std::vector<DrawableItem> i)
{
	items = i;
}

void UIManager::setAnimations(std::vector<DrawableAnimation> i)
{
	animations = i;
}

void UIManager::setMovement(std::vector<DrawableMovement> i)
{
	movement = i;
}

sf::Sprite* UIManager::getUI()
{
	terrain.setPosition(x, y);
	terrain.setScale(scale, -scale);
	terrain.setOrigin(0, terrain.getLocalBounds().height);
	return &terrain;
}


void UIManager::drawEverything(sf::RenderWindow* window)
{
	window->draw(*getUI());
	drawUnits(window);
	drawItems(window);
	drawAnimations(window);
	drawMovement(window);
	//window->draw(getUnits());
}

void UIManager::drawUnits(sf::RenderWindow* window)
{
	for (std::vector<DrawableUnit>::iterator it = units.begin(); it != units.end(); ++it) {
		bool moving = false;
		for (auto move : movement)
		{
			if (move.sprite->sprite == it->sprite)
			{
				moving = true;
				break;
			}
		}
		if (!moving)
		{
			AnimatedSprite s = *it->sprite;
			s.setPosition((it->x * 128) + x, (it->y * 128) + y);
			window->draw(s);
		}
	}
}

void UIManager::drawItems(sf::RenderWindow* window)
{
	for (std::vector<DrawableItem>::iterator it = items.begin(); it != items.end(); ++it) {
		sf::Sprite s = it->sprite;
		s.setPosition((it->pos.x * 128) + x, (it->pos.y * 128) + y);
		window->draw(s);
	}
}

void UIManager::drawAnimations(sf::RenderWindow* window)
{
	for (std::vector<DrawableAnimation>::iterator it = animations.begin(); it != animations.end(); ++it) {
		AnimatedSprite s = it->sprite;
		s.setPosition((it->pos.x * 128) + x, (it->pos.y * 128) + y);
		window->draw(s);
	}
}

void UIManager::drawMovement(sf::RenderWindow* window)
{
	for (std::vector<DrawableMovement>::iterator it = movement.begin(); it != movement.end(); ++it) {
		AnimatedSprite s = *it->unit->sprite;
		s.setPosition((it->unit->x * 128) + x, (it->unit->y * 128) + y);
		window->draw(s);
	}
}

bool UIManager::isAnimating()
{
	return items.size() + animations.size() + movement.size();	//If there is ANY animation going on -> true
}

UIManager::~UIManager()
{
}
