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
#include "AIComponent.h"
#include "ActionEvent.h"

class lvlManager;

class UnitManager {
public:
	UnitManager(lvlManager*, TextureManager*, SoundManager*, AIFactory* aiFactory);
	~UnitManager();

	std::vector<DrawableUnit> getUnits();
	std::vector<Unit*> unitList;
	 
	//
	std::vector<ActionEvent*> actions;
	//

	Player* player;
	aStarSearch pathFinder;
	
	sf::SoundBuffer dieBuffer;
	sf::Sound die;

	int moveUnit(int x, int y, Unit* unit);

	bool spawnUnit(UnitType, int, int, bool);

	void isDead(int);
	void damageUnit(Unit * unit, int hp);
	void setPlayer(Player* p);
	void removeAP(Unit* unit, int ap);

private:

	lvlManager* lvl = nullptr;
	TextureManager* tex = nullptr;
	SoundManager* snd = nullptr;
	AIFactory* aiFactory;
	void removeUnit(Unit*);

	/* THIS IS FOR TEST PURPOSES ONLY, DONT DO THIS */
};
