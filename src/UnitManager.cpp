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
#include <fstream>

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

	std::ifstream fileStream{ "names.txt" };

	if (!fileStream) {
		std::cout << "Failed to open file to read. Check if path is correct" << std::endl;
		throw std::invalid_argument("");
	}

	std::string name;
	
	while(fileStream >> name) { 
		nameList.push_back(name);
	}

	fileStream.clear();
	fileStream.close();
}

Unit* UnitManager::spawnUnit(UnitType u, int x, int y, bool isAccessible) {
	if (!isAccessible) 
	 	return false;

	Unit* unit  = new Unit();

	switch(u) {	
		case standard:
			break;
		case doge:	//(int mAP, int mHP, sf::Sprite s, TextureManager* texMng,int xPos, int yPos, UnitType u)
			unit = new Unit(1000, 50, new UnitAnimations(tex->textureTable["coolarmymove"]), nullptr, x, y, doge);
			break;
		case turtle:	
			unit = new Unit(10, 10, new UnitAnimations(tex->textureTable["coolarmymove"]), nullptr, x, y, turtle);
		case robotdude:
			unit = new Unit(25, 25, new UnitAnimations(tex->textureTable["robotdudemove"]), aiFactory->getAIComponent("meleefighter"), x, y, robotdude);
			unit->learnedActions = {
				new AttackActionEvent("Medium Melee Attack", "\"I'll smack ya. Beep Boop.\"\n\nRange: 1\tDamage: 5",pActionMng, 1, 3, 5, "explosion"),
				new AttackActionEvent("Strong Melee (Cleave)" , "Now it's angry. Robot falls into a rage. If it kills a Unit with this skill, it can attack another straight away.\n\nRange: 3\tDamage: 10",pActionMng,1, 8, 10, "cleave"),
				new ActionEvent("Brace", "Robot is ready to take some punishment. It takes reduced damage until next round.\n\nRange: -\tReduction: 3", pActionMng, 0 ,3),
				new MoveActionEvent("Move", "Robots, on the move.",pActionMng)
			};
			break;
		case robotfly:
			unit = new Unit(30, 25, new UnitAnimations(tex->textureTable["robotflymove"]), aiFactory->getAIComponent("rangedcaster"), x, y, robotfly);
			unit->learnedActions = {
				new AttackActionEvent("Light Ranged Attack", "It hurts, just enough to be annoying.\n\nRange: 6\tDamage: 3",pActionMng, 6, 2, 3, "bullet"),
				new APchangeActionEvent("Draining Shot", "This stuff is potent.\n\nRange: 4\tAP loss: 8",pActionMng,4,8,8),
				new AttackActionEvent("Healing Shot", "Fix me up, Doc.\n\nRange: 5\tHeal: 6", pActionMng, 5 , 5 , -6, "heal"), // Use an Attack with a negative dmg val to fake a heal
				new MoveActionEvent("Move", "Robots, on the move.",pActionMng)
			};
			break;
	};

	unit->nickName = std::string(nameList[rand() % nameList.size()]);
	std::cout << unit->nickName << std::endl;

	unitList.push_back(unit);
	return unit; //rm
}

std::string UnitManager::getRandomName() {
	return std::string(nameList[rand() % nameList.size()]);
};

UnitManager::~UnitManager() {
	/*for (auto u : unitList)
		delete u;*/
}

	
std::vector<DrawableUnit> UnitManager::getUnits() {
	std::vector<DrawableUnit> dUnits;
	
	for (auto u : unitList) {
		AnimatedSprite* sprtPtr = u->sprite->sprite;
		dUnits.push_back(DrawableUnit{ (float)u->x, (float)u->y, sprtPtr });
	}

    return dUnits;
}

std::vector<Node> UnitManager::moveUnit(int x, int y, Unit* unit, OrthogonalLineOfSight* los, bool lookForUnits) {
	if (x > 30)
		x /= 128;
	if (y > 30)
		y /= 128;

	std::vector<Node> path;

	if (!(lvl->isAccessible(x, y))) {
		return path;
	}

	else if ((lvl->isOccupied(x, y))) {
		return path;
	}

	path = pathFinder.findPath(Node{ unit->x, unit->y }, Node{ x, y }, lvl->createGraph());

	Node end = path.back();

	if (end.x == 0 && end.y == 0)
		return std::vector<Node>();

	if (lookForUnits) {

		std::vector<Node> newPath{};

		for (auto& node : path) {

			newPath.push_back(node);
			
			unit->x = node.x;
			unit->y = node.y;

			if (los->getVisibleUnits(lvl, this, unit).size() > 1) {
				
				end = newPath.back();

				return newPath;

			}
		}
	
	}
	
	lvl->moveUnitInMap(end.x, end.y, unit);

	unit->x = end.x;
	unit->y = end.y;

	return path;
}

// Returns true if attacked unit died
bool UnitManager::damageUnit(Unit* unit, float hp)
{
	//lvl->setAccessTile(unitList[index]->x, unitList[index]->y, true);
	unit->loseHP(hp);

	if (unit->currHP <= 0.99)
	{
		removeUnit(unit);
		return true;
	}

	return false;
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

void UnitManager::setPlayer(Unit * p)
{
	unitList.push_back(p);
}

bool UnitManager::isDead(int index) {
	if (unitList[index]->currHP <= 0) {
		lvl->setAccessTile(unitList[index]->x, unitList[index]->y, true);
		unitList.erase(unitList.begin() + index, unitList.begin() + index + 1);
		die.play();
		return true;
	}
	return false;
}

void UnitManager::update(sf::Time time)
{
	for (auto u : unitList)
	{
		u->sprite->update(time);
	}
}