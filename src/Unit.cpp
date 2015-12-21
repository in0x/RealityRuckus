#pragma once
#include "Unit.h"
#include <iostream>
// unit with highest ratio of max to current AP, if same, moves are simultaneously.
Unit::Unit(int mAP, int mHP, sf::Sprite s, TextureManager* texMng,int xPos, int yPos, UnitType u)
{
	maxAP = mAP;
	currAP = mAP;
	maxHP = mHP;
	currHP = mHP;
	
	sprite = s;

	sf::Sprite sp;

	switch (u) {
		case standard:
			sp.setTexture(texMng->textureTable.at("EnemyChar"));
			break;

		case doge:
			sp.setTexture(texMng->textureTable.at("shibe"));
			break;

		case turtle:
			sp.setTexture(texMng->textureTable.at("turtle"));
			break;
		case robotdude:
			sp.setTexture(texMng->textureTable.at("robotdude"));
			break;
		case robotfly:
			sp.setTexture(texMng->textureTable.at("robotfly"));
			break;
		/*case robot:
			sp.setTexture(texMng->textureTable.at("robot"));
			break;*/
		case player:
			sp.setTexture(texMng->textureTable.at("coolarmy"));
	}

	if (s.getTexture() == NULL)
	 	sprite = sp;
	
	x = xPos;
	y = yPos;
	type = u;
}


Unit::Unit(int mAP, int mHP, sf::Sprite s, TextureManager* texMng, int xPos, int yPos, UnitType u, std::vector<ActionEvent*> actions) {

	maxAP = mAP;
	currAP = mAP;
	maxHP = mHP;
	currHP = mHP;
	
	sprite = s;

	sf::Sprite sp;

	switch (u) {
		case standard:
			sp.setTexture(texMng->textureTable.at("EnemyChar"));
			break;

		case doge:
			sp.setTexture(texMng->textureTable.at("shibe"));
			break;

		case turtle:
			sp.setTexture(texMng->textureTable.at("turtle"));
			break;
		case robotdude:
			sp.setTexture(texMng->textureTable.at("robotdude"));
			break;
		case robotfly:
			sp.setTexture(texMng->textureTable.at("robotfly"));
			break;
		/*case robot:
			sp.setTexture(texMng->textureTable.at("robot"));
			break;*/
		case player:
			sp.setTexture(texMng->textureTable.at("coolarmy"));
	}

	if (s.getTexture() == NULL)
	 	sprite = sp;
	
	x = xPos;
	y = yPos;
	type = u;
	learnedActions = actions;
}

Unit::Unit() {
	learnedActions = std::vector<ActionEvent*>();
	maxAP = 10;
	currAP = 10; //! should be 10, for test purposes
	maxHP = 15;
	currHP = 15;
	x = 29; 
	y = 29;
	type = standard;
}

Unit::~Unit()
{
}

