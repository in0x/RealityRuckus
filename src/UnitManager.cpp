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

UnitManager::UnitManager(lvlManager* lvlMng, TextureManager* texMng, SoundManager* s) {
	unitList = std::vector<Unit*>();
	lvl = lvlMng;
	tex = texMng;
	snd = s;

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
			unit = new Unit(1000, 50, sf::Sprite(), tex, x, y, doge); 
			break;
		case turtle:	
			unit = new Unit(10, 10, sf::Sprite(), tex, x, y, turtle); 
		case robotdude:
			unit = new Unit(10, 10, sf::Sprite(), tex, x, y, robotdude);
			break;
		case robotfly:
			unit = new Unit(10, 10, sf::Sprite(), tex, x, y, robotfly);
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
	if (!(lvl->isAccessible(x, y))) {
		return 0;
	}
	else if ((lvl->isOccupied(x/128, y/128))) {
		return 0;
	}

	x /= 128;
	y /= 128;

	std::vector<Node> path = pathFinder.findPath(Node{unit->x, unit->y}, Node{ x, y }, lvl->createGraph());

	std::cout << path.size() << std::endl;
	
	Node end = path.back();
	std::cout << end.x << "  " << end.y << std::endl;
	
	if (end.x == 0 && end.y == 0)
		return 0;

	lvl->moveUnitInMap(end.x, end.y, unit);
	
	unit->x = end.x;
	unit->y = end.y;
	return path.size();
}

void UnitManager::damageUnit(Unit* unit, int hp)
{
	//lvl->setAccessTile(unitList[index]->x, unitList[index]->y, true);
	unit->currHP -= hp;
	if (unit->currHP <= 0)
	{
		removeUnit(unit);
	}
}

//Mock Combat: when entering Combat, update() sends an attack at the position at which you shift right click.  
void UnitManager::attackUnit(int unitX, int unitY, Unit* attackerUnit, Unit* unitToAttack) {
	std::cout << "Attacking x: " << unitX << " y: " << unitY << std::endl;
	if (!lvl->isOccupied(unitX, unitY))
		return;

	int attackValue = (rand() % 10) + 1;

	unitToAttack->currHP -= attackValue;
	std::cout << "Attack. Damage: " << attackValue << " Enemy at " << unitToAttack->currHP << " hp" << std::endl;
	std::cout << unitToAttack->currHP << std::endl;

	if (unitToAttack->currHP <= 0)
		removeUnit(unitToAttack);
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


void UnitManager::runCombat(CombatState* combatState, CombatEvent event) {
	/*if (event.eventType == ActionEvent)
		return;

	Unit* firstUnit = combatState->getFirstUnit();

	if (event.eventType == ActionEvent::attack) {

		int attackX = (event.targetX > 30) ? (event.targetX / 128) : event.targetX;
		int attackY = (event.targetY > 30) ? (event.targetY / 128) : event.targetY;

		Unit* unitToAttack = combatState->findUnit(attackX, attackY);
		std::cout << "Looking for Unit at " << attackX << " " << attackY << std::endl;

		if (unitToAttack != nullptr)
			attackUnit(attackX, attackY, event.sender, unitToAttack);
		else
			std::cout << "No Unit to attack here" << std::endl;

	}*/

	combatState->updateListOfUnits();

	combatState->listUnits();
}



void UnitManager::testCombatState() {
	std::vector<Unit*> units;

	for (int i = 0; i < 4; i++) {
		Unit* u = new Unit{ i, i, sf::Sprite(), tex, turtle };
		units.push_back(u);
	}

	Unit* p = new Unit{ 30, 30, sf::Sprite(), tex, UnitType::player };
	Unit* d = new Unit{ 15, 15, sf::Sprite(), tex, doge };

	units.push_back(p);
	units.push_back(d);

	CombatState state(units);
	std::cout << "testCombatState()" << std::endl;

	for (auto uPtr : units)
		delete uPtr;
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

