#pragma once
#include "Unit.h"
#include "lvlManager.h"
#include "textureManager.h"
#include "player.h"
#include "DrawableUnit.h"
#include "SoundManager.h"
#include "aStarSearch.h"
#include "UIManager.h"
#include "CombatState.h"

class lvlManager;

class UnitManager {
public:
	UnitManager(lvlManager*, TextureManager*, SoundManager*);
	~UnitManager();
	bool spawnUnit(UnitType, int, int, bool);
	std::vector<DrawableUnit> getUnits();
	std::vector<Unit*> unitList;
	Player* player;
	void isDead(int);
	sf::SoundBuffer dieBuffer;
	sf::Sound die;
	int moveUnit(int x, int y, Unit* unit);
	void damageUnit(Unit * unit, int hp);
	void attackUnit(int unitX, int unitY, Unit* attacker, Unit* unitToAttack);
	void runCombat(CombatState*, CombatEvent);
	void testCombatState();
	void setPlayer(Player* p);
private:
	lvlManager* lvl = nullptr;
	TextureManager* tex = nullptr;
	SoundManager* snd = nullptr;
	aStarSearch pathFinder;
	void removeUnit(Unit*);
};
