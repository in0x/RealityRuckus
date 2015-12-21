#pragma once
#include "CombatEvent.h"

class UnitManager;
class CombatState;

class ActionManager
{
public:
	UnitManager* unitManager;
	CombatState* currentCombat;
	std::vector<CombatEvent> moveUnit(Unit* unit, int x, int y);
	std::vector<CombatEvent> damageUnit(Unit* unit, int x, int y, int hp);
	void setCombatState(CombatState * newCombat);
	ActionManager(UnitManager* unitManager);
	~ActionManager();
};

