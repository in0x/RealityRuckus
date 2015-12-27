#pragma once
#include "CombatEvent.h"

class UnitManager;
class CombatState;

class ActionManager
{
public:
	ActionManager(UnitManager* unitManager);
	~ActionManager();

	std::vector<CombatEvent> moveUnit(Unit* unit, int x, int y);
	std::vector<CombatEvent> damageUnit(Unit* unit, int x, int y, int hp, int range, int cost);
	
	void setCombatState(CombatState * newCombat);

	UnitManager* unitManager;
	CombatState* currentCombat;
	
private:
	bool checkRange(int range, sf::Vector2i unit1pos, sf::Vector2i unitpos2);
};

