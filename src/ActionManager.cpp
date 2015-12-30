#include "ActionManager.h"
#include "CombatEvent.h"
#include "UnitManager.h"

bool ActionManager::checkRange(int range, sf::Vector2i unit1pos, sf::Vector2i unitpos2) {
	int xDistance = abs(unit1pos.x - unitpos2.x);
	int yDistance = abs(unit1pos.y - unitpos2.y);
	return (xDistance + yDistance) <= range;
}

std::vector<CombatEvent> ActionManager::moveUnit(Unit * unit, int x, int y)
{
	std::vector<CombatEvent> events;
	int oldx = unit->x;
	int oldy = unit->y;
	int move = unitManager->moveUnit(x, y, unit);
	if (move)
	{
		CombatEvent c1 = CombatEvent(unit, CombatEventType::Move);
		c1.setMove(oldx, oldy, unit->x, unit->y);
		events.push_back(c1);
		CombatEvent c2 = CombatEvent(unit, CombatEventType::AP);
		c2.setAPChange(move);
		events.push_back(c2);
		unitManager->removeAP(unit, move);
	}
	else
	{
		CombatEvent c1 = CombatEvent(unit, CombatEventType::None);
		events.push_back(c1);
	}
	return events;
}

//unit is the the Unit that wants to do something, x and y define where the affected Unit is
std::vector<CombatEvent> ActionManager::damageUnit(Unit * unit, int x, int y, int hp, int range, int cost)
{
	std::vector<CombatEvent> events;

	if (!checkRange(range, sf::Vector2i{ x, y }, sf::Vector2i{ unit->x, unit->y })) {
		int distance = abs(unit->x - x ) + abs(unit->y - y );

		events.push_back(CombatEvent{unit, CombatEventType::NotValid, 
			"You need to move closer for this action\nDistance from target: " + std::to_string(distance) +  "\tSkill Range: " + std::to_string(range)
		});

		return events;
	}

	if (cost > unit->currAP) {

		events.push_back(CombatEvent{ unit, CombatEventType::NotValid, 
			"You don't have enough AP for this action.\nYour AP: " + std::to_string(unit->currAP) + "\tRequired AP: " +  std::to_string(cost)
		});

		return events;
	}

	Unit* damagedUnit = currentCombat->findUnit(x, y);

	if (damagedUnit == nullptr)
	{
		events.push_back(CombatEvent(unit, CombatEventType::NotValid, "No Target here"));
		return events;
	}
	else
	{
		unitManager->damageUnit(damagedUnit, hp);
		currentCombat->updateListOfUnits();
		CombatEvent ce = CombatEvent(damagedUnit, CombatEventType::HP);
		ce.setHPChange(hp);
		events.push_back(ce);
	}
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
