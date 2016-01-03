#pragma once
#include "Unit.h"
#include <iostream>
// unit with highest ratio of max to current AP, if same, moves are simultaneously.
Unit::Unit(float mAP, float mHP, UnitAnimations* sprite, std::shared_ptr<AIComponent> aiComp, int xPos, int yPos, UnitType u)
{
	maxAP = mAP;
	currAP = mAP;
	maxHP = mHP;
	currHP = mHP;
	
	this->sprite = sprite;


	switch (u) {
		case standard:
			//sp.setTexture(texMng->textureTable.at("EnemyChar"));
			break;

		case doge:
			//sp.setTexture(texMng->textureTable.at("shibe"));
			break;

		case turtle:
			//sp.setTexture(texMng->textureTable.at("turtle"));
			break;
		case robotdude:
			//sp.setTexture(texMng->textureTable.at("robotdude"));
			name = "Attack Droid";
			break;
		case robotfly:
			//sp.setTexture(texMng->textureTable.at("robotfly"));
			name = "Robot Fly";
			break;
		case player:
			//sp.setTexture(texMng->textureTable.at("coolarmy"));
			name = "Player";
			break;
	}

	aiComponent = aiComp;
	
	x = xPos;
	y = yPos;
	type = u;
}


Unit::Unit(float mAP, float mHP, UnitAnimations* sprite, int xPos, int yPos, UnitType u, std::vector<ActionEvent*> actions, std::shared_ptr<AIComponent> aiComp) {

	maxAP = mAP;
	currAP = mAP;
	maxHP = mHP;
	currHP = mHP;

	this->sprite = sprite;

	switch (u) {
		case standard:
			//sp.setTexture(texMng->textureTable.at("EnemyChar"));
			break;
		case doge:
			//sp.setTexture(texMng->textureTable.at("shibe"));
			break;
		case turtle:
			//sp.setTexture(texMng->textureTable.at("turtle"));
			break;
		case robotdude:
			//sp.setTexture(texMng->textureTable.at("robotdude"));
			name = "Attack Droid";
			break;
		case robotfly:
			//sp.setTexture(texMng->textureTable.at("robotfly"));
			name = "Robot Fly";
			break;
		case player:
			//sp.setTexture(texMng->textureTable.at("coolarmy"));
			name = "Player";
			break;
	}

	aiComponent = aiComp;
	
	x = xPos;
	y = yPos;
	type = u;
	learnedActions = actions;
}

Unit::Unit() {
	learnedActions = std::vector<ActionEvent*>();
	sprite = new UnitAnimations();
	maxAP = 10;
	currAP = 10; 
	maxHP = 15;
	currHP = 15;
	x = 29; 
	y = 29;
	type = standard;
}

Unit::~Unit()
{
	delete sprite;
}

void Unit::addModifier(Modifier modifier) {
	modifiers.push_back(modifier);
}

// Use this function after a round, to reduce the duration off all mods by 1
void Unit::cycleModifiers() {
	for (auto& mod : modifiers) {
		mod.rounds--;
	}
}

void Unit::applyModifiers(ModifierType type, float& val) {
	for (auto& mod : modifiers) {
		if (mod.type == type)
			mod.func(val);
	}
}


void Unit::loseAP(float amount) {

	currAP -= amount;

	if (currAP < 0)
		currAP = 0;

}

void Unit::loseHP(float amount) {

	currHP -= amount;

	if (currHP < 0)
		currHP = 0;
}

void Unit::gainAP(float amount) {

	currAP += amount;

	if (currAP > maxAP) {
		currAP = maxAP;
	}
}

void Unit::gainHP(float amount) {

	currHP += amount;

	if (currHP > maxHP) {
		currHP = maxHP;
	}
}

void Unit::setAPGainPerRound(float ap) {
	apGain = ap;
}

int Unit::replenishAP() {
	gainAP(apGain);
	return apGain;
}