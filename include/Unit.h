#pragma once
#include <SFML/Graphics.hpp>
#include "unitType.h"
#include "textureManager.h"
#include "ActionEvent.h"
#include <queue>

class ActionEvent;

class Unit
{

public:
	int maxAP;
	int currAP;
	int maxHP;
	int currHP;
	int x; 
	int y;
	UnitType type;
	sf::Sprite sprite;
	Unit(int, int, sf::Sprite, TextureManager*, int xPos = 29, int yPos = 29, UnitType u = standard);
	Unit(int, int, sf::Sprite, TextureManager*, int xPos, int yPos, UnitType u, std::vector<ActionEvent*>);
	Unit();
	~Unit();
	std::vector<ActionEvent*> learnedActions;
	std::string name = "";

	void setAPGainPerRound(int ap);
	void loseAP(int amount);
	void loseHP(int amount);
	void gainAP(int amount);
	void gainHP(int amount);
	int replenishAP();
private:
	int apGain = 2;
};

