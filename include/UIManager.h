#pragma once
#include <SFML/Graphics.hpp>
#include "drawableUnit.h"
#include <vector>
#include "DrawableUnit.h"
#include "drawableItem.h"
#include "Event.h"


class UIManager : public Observer
{
public:
	std::vector<DrawableUnit> units;
	std::vector<DrawableItem> items;
	std::vector<DrawableAnimation> animations;
	std::vector<DrawableMovement> movement;
	sf::Sprite outsprite;
	sf::Texture outtexture;

	UIManager();
	~UIManager();

	sf::Sprite* getUI();

	void drawEverything(sf::RenderWindow*);
	void drawUnits(sf::RenderWindow* window);
	void drawItems(sf::RenderWindow * window);
	void drawAnimations(sf::RenderWindow * window);
	void drawMovement(sf::RenderWindow * window);
	bool isAnimating();
	void drawNewFrame(sf::RenderWindow* w, std::vector<DrawableUnit> unitList);
	void setX(float newX);
	void setY(float newY);
	void update(GameEvent e);
	void setTerrain(sf::Sprite);
	void setUnits(std::vector<DrawableUnit>);

	void setItems(std::vector<DrawableItem> i);

	void setAnimations(std::vector<DrawableAnimation> i);

	void setMovement(std::vector<DrawableMovement> i);

	float getX();
	float getY();

private:
	float x = 0, y = 0;
	float scale;
	sf::Sprite terrain;
};

