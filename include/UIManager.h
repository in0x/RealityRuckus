#pragma once
#include <SFML/Graphics.hpp>
#include "drawableUnit.h"
#include <vector>
#include "DrawableUnit.h"
#include "Event.h"


class UIManager : public Observer
{
public:
	std::vector<DrawableUnit> units;
	sf::Sprite outsprite;
	sf::Texture outtexture;
	UIManager();
	~UIManager();
	void setTerrain(sf::Sprite);
	void setUnits(std::vector<DrawableUnit>);
	sf::Sprite* getUI();
	void drawEverything(sf::RenderWindow*);
	void drawUnits(sf::RenderWindow* window);
	void drawNewFrame(sf::RenderWindow* w, std::vector<DrawableUnit> unitList);
	float getX();
	float getY();
	void setX(float newX);
	void setY(float newY);
	void update(GameEvent e);
private:
	float x = 0, y = 0;
	sf::Sprite terrain;
	float scale;
};

