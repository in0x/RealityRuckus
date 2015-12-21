#include "ActionManager.h"
#include "CombatEvent.h"
#include "UnitManager.h"


std::vector<CombatEvent> ActionManager::moveUnit(Unit * unit, int x, int y)
{
	std::vector<CombatEvent> events;
	int oldx = unit->x;
	int oldy = unit->y;
	int move = unitManager->moveUnit(x, y, unit);
	if (move)
	{
		CombatEvent c1 = CombatEvent(unit);
		c1.setMove(oldx, oldy, unit->x, unit->y);
		events.push_back(c1);
		CombatEvent c2 = CombatEvent(unit);
		c2.setAPChange(move);
		events.push_back(c2);
	}
	else
	{
		CombatEvent c1 = CombatEvent(unit);
		events.push_back(c1);
	}
	return events;
}

std::vector<CombatEvent> ActionManager::damageUnit(Unit * unit, int x, int y, int hp)
{
	std::vector<CombatEvent> events;
	Unit* damagedUnit = currentCombat->findUnit(x / 128, y / 128);
	if (damagedUnit == nullptr)
	{
		CombatEvent ce = CombatEvent(unit);
		events.push_back(ce);
	}
	else
	{
		unitManager->damageUnit(damagedUnit, hp);
		currentCombat->updateListOfUnits();
		CombatEvent ce = CombatEvent(damagedUnit);
		ce.setHPChange(hp);
		events.push_back(ce);
	}
	std::cout << "Damaged Unit" << std::endl;
	return events;
}

void ActionManager::setCombatState(CombatState* newCombat)
{
	this->currentCombat = newCombat;
}

ActionManager::ActionManager(UnitManager * unitManager)
{
	this->unitManager = unitManager;
}



ActionManager::~ActionManager()
{
}
