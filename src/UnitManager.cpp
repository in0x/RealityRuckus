#pragma once
#include "UnitManager.h"
#include <vector>
#include "Unit.h"
#include <iostream>
#include "ActionUnit.h"
#include "WeightedGraph.h"
#include "Node.h"
#include "CombatEvent.h"
#include <thread>
#include <chrono>

UnitManager::UnitManager(lvlManager* lvlMng, TextureManager* texMng, SoundManager* s, AIFactory* aiFactory) {
	unitList = std::vector<Unit*>();
	lvl = lvlMng;
	tex = texMng;
	snd = s;
	this->aiFactory = aiFactory;
	
 	dieBuffer = s->soundTable.at("die");
	die = sf::Sound();
	die = sf::Sound(dieBuffer);
	die.setVolume(100);
}

bool UnitManager::spawnUnit(UnitType u, int x, int y, bool isAccessible) {
	if (!isAccessible) 
	 	return false;

	Unit* unit  = new Unit();

	switch(u) {	
		case standard:
			break;
		case doge:	//(int mAP, int mHP, sf::Sprite s, TextureManager* texMng,int xPos, int yPos, UnitType u)
			unit = new Unit(1000, 50, sf::Sprite(), tex, nullptr, x, y, doge);
			break;
		case turtle:	
			unit = new Unit(10, 10, sf::Sprite(), tex, nullptr, x, y, turtle);
		case robotdude:
			unit = new Unit(19, 10, sf::Sprite(), tex, aiFactory->getAIComponent("meleefighter"), x, y, robotdude);
			unit->learnedActions = actions;
			break;
		case robotfly:
			unit = new Unit(19, 10, sf::Sprite(), tex, aiFactory->getAIComponent("meleefighter"), x, y, robotfly);
			unit->learnedActions = actions;
			break;
	};

	unitList.push_back(unit);
	return true; //rm
}

UnitManager::~UnitManager() {
	/*for (auto u : unitList)
		delete u;*/
}

	
std::vector<DrawableUnit> UnitManager::getUnits() {
	std::vector<DrawableUnit> dUnits;
	
	for (auto u : unitList) {
		sf::Sprite* sprtPtr = &(u->sprite);
		dUnits.push_back(DrawableUnit{ u->x, u->y, sprtPtr });
	}

    return dUnits;
}

int UnitManager::moveUnit(int x, int y, Unit* unit) {

	if (x > 30)
		x /= 128;
	if (y > 30)
		y /= 128;

	if (!(lvl->isAccessible(x, y))) {
		return 0;
	}

	else if ((lvl->isOccupied(x, y))) {
		return 0;
	}

	std::vector<Node> path = pathFinder.findPath(Node{unit->x, unit->y}, Node{ x, y }, lvl->createGraph());

	Node end = path.back();
	
	if (end.x == 0 && end.y == 0)
		return 0;

	lvl->moveUnitInMap(end.x, end.y, unit);

	std::cout << "Moved Unit ( Type " << unit->type << " ) from x " << unit->x << " y " << unit->y << " to x " << end.x << " y " << end.y << std::endl;

	unit->x = end.x;
	unit->y = end.y;

	return path.size();
}

void UnitManager::damageUnit(Unit* unit, int hp)
{
	//lvl->setAccessTile(unitList[index]->x, unitList[index]->y, true);
	unit->loseHP(hp);
	if (unit->currHP == 0)
	{
		removeUnit(unit);
	}
}

void UnitManager::removeAP(Unit* unit, int ap) {
	unit->loseAP(ap);
}


void UnitManager::removeUnit(Unit* unitToRemove) {
	std::vector<Unit*>::iterator it;

	for (it = unitList.begin(); it != unitList.end(); ++it) {
		if (*it == unitToRemove)
			break;
	}

	if (*it != unitToRemove) {
		return;
	}

	lvl->setOccupied(unitToRemove->x, unitToRemove->y, false);
	unitList.erase(it);
	die.play();
}


void UnitManager::setPlayer(Player * p)
{
	player = p;
	unitList.push_back(p);
}

void UnitManager::isDead(int index) {
	if (unitList[index]->currHP <= 0) {
		lvl->setAccessTile(unitList[index]->x, unitList[index]->y, true);
		unitList.erase(unitList.begin() + index, unitList.begin() + index + 1);
		die.play();
	}
}

