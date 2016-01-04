#pragma once
#include "Unit.h"
#include "lvlManager.h"
#include "textureManager.h"
#include "DrawableUnit.h"
#include "SoundManager.h"
#include "aStarSearch.h"
#include "UIManager.h"
#include "CombatState.h"
#include "AIComponent.h"
#include "ActionEvent.h"
#include "OrthogonalLineOfSight.h"

class lvlManager;
class OrthogonalLineOfSight;
class AIFactory;

class UnitManager {
public:
	UnitManager(lvlManager*, TextureManager*, SoundManager*, AIFactory* aiFactory);
	~UnitManager();

	std::vector<DrawableUnit> getUnits();
	std::vector<Unit*> unitList;
	 
	//Player* player;
	aStarSearch pathFinder;
	ActionManager* pActionMng;

	sf::SoundBuffer dieBuffer;
	sf::Sound die;

	std::vector<Node> moveUnit(int x, int y, Unit* unit, OrthogonalLineOfSight* los = nullptr, bool lookForUnits = false);

	Unit* spawnUnit(UnitType, int, int, bool);

	bool isDead(int);
	void update(sf::Time time);
	bool damageUnit(Unit * unit, float hp);
	void setPlayer(Unit* p);
	void removeAP(Unit* unit, int ap);
	std::string getRandomName();

private:

	lvlManager* lvl = nullptr;
	TextureManager* tex = nullptr;
	SoundManager* snd = nullptr;
	AIFactory* aiFactory;
	std::vector<std::string> nameList{};
	void removeUnit(Unit*);
};
