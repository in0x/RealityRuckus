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
	//scale = t.getScale().x;
	//terrain.setScale(scale, -scale);
	//terrain.setOrigin(0, terrain.getLocalBounds().height);
}

void UIManager::setUnits(std::vector<DrawableUnit> v)
{
	units = v;
	//scale = t.getScale().x;
	//units.setScale(scale, -scale);
	//units.setOrigin(0, units.getLocalBounds().height);
}

sf::Sprite* UIManager::getUI()
{
	terrain.setPosition(x, y);
	//units.setPosition(x, y);
	terrain.setScale(scale, -scale);
	//units.setScale(scale, -scale);
	terrain.setOrigin(0, terrain.getLocalBounds().height);
	//sf::RenderTexture texture;
	//texture.create(3840, 3840);
	//texture.draw(terrain);
	
	//texture.draw(units);
	//outtexture = texture.getTexture();
	//outsprite.setTexture(outtexture);
	//outsprite.setPosition(x, y);
	//outsprite.setScale(scale, -scale);
	
	//return &outsprite;
	return &terrain;
}


void UIManager::drawEverything(sf::RenderWindow* window)
{
	window->draw(*getUI());
	drawUnits(window);
	//window->draw(getUnits());
}

void UIManager::drawUnits(sf::RenderWindow* window)
{
	for (std::vector<DrawableUnit>::iterator it = units.begin(); it != units.end(); ++it) {
		sf::Sprite s = *it->sprite;
		s.setPosition((it->x * 128)+x, (it->y * 128)+y);
		window->draw(s);
	}
}

UIManager::~UIManager()
{
}
