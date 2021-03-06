#pragma once
#include "CombatState.h"
#include <iostream>

struct CompareUnitApPtr
{
	bool operator()(Unit* lhs, Unit* rhs) {
		//return (lhs->currAP / lhs->maxAP) > (rhs->currAP / rhs->maxAP);

		if ((lhs->currAP / lhs->maxAP) == (rhs->currAP / rhs->maxAP))
			return lhs->currAP > rhs->currAP;

		return (lhs->currAP / lhs->maxAP) > (rhs->currAP / rhs->maxAP);
	}
} unitComparer;


CombatState::CombatState(std::vector<Unit*> combatUnitPtrs) {
	unitsInCombat = combatUnitPtrs;
	std::sort(unitsInCombat.begin(), unitsInCombat.end(), unitComparer);
}

void CombatState::cycleUnitModifiers() {
	for (auto& unit : unitsInCombat)
		unit->cycleModifiers();
}

void CombatState::skipTurn(Unit* skipper) {
	if (skipper->currAP >= 5)
		skipper->currAP -= 5;
	else
		skipper->currAP = 0;
	updateListOfUnits();
}

std::ostream& operator<<(std::ostream& os, const Unit& unit) {
	os << unit.nickName + ": ( Type = " + unit.name + " ) (APR: " + std::to_string(unit.currAP / unit.maxAP) + " , AP: " + std::to_string(unit.currAP) + " ) " + " (HP: " + std::to_string(unit.currHP) + " ) " + " ( x: " + std::to_string(unit.x) + " , y: " + std::to_string(unit.y) + " )";
	return os;
}

std::vector<CombatEvent> CombatState::endTurn()
{
	std::vector<CombatEvent> events;
	for (auto& unit : unitsInCombat)
	{
		CombatEvent ev = CombatEvent(unit, CombatEventType::AP);
		ev.setAPChange(-unit->replenishAP());
		events.push_back(ev);
	}
	cycleUnitModifiers();
	updateListOfUnits();
	return events;
}

void CombatState::updateListOfUnits() {
	//std::cout << "before" << std::endl;
	//listUnits();
	std::sort(unitsInCombat.begin(), unitsInCombat.end(), unitComparer);
	//std::cout << "after" << std::endl;
	//listUnits();
	std::vector<Unit*> unitsToRemove = std::vector<Unit*>();
	for (auto unit : unitsInCombat) {
		if (unit->currHP <= 0.99) {
			unitsToRemove.push_back(unit);
			
		}
	}

	for (auto unit : unitsToRemove) {
		removeUnit(unit);
	}

}

Unit* CombatState::getFirstUnit() {
	return unitsInCombat.front();
}

bool CombatState::isDone() {
	for (int i = 0; i < unitsInCombat.size(); i++) {
		if (unitsInCombat[i]->type != player)
			return false;
	}
	return true;
}

void CombatState::listUnits() {
	for (auto unit : unitsInCombat)
		std::cout << *unit << std::endl; std::cout << "\n" << std::endl;
}

void CombatState::removeUnit(Unit* unitToRemove) {

	std::vector<Unit*>::iterator it;

	for (it = unitsInCombat.begin(); it != unitsInCombat.end(); ++it) {
		if (*it == unitToRemove)
			break;
	}

	if (*it != unitToRemove) {
		return;
	}

	unitsInCombat.erase(it);
}

/*std::vector<std::tuple<int, Unit*>> CombatState::replenishUnitAP() {

	std::vector<std::tuple<int, Unit*>> apGains{};

	for (auto& unit : unitsInCombat) {
		if (unit->type == UnitType::player)
		apGains.push_back(std::tuple<int, Unit*>{unit->replenishAP(), unit});
	}

	return apGains;
}*/

Unit* CombatState::findUnit(int x, int y) {

	for (auto unit : unitsInCombat)
		if (unit->x == x && unit->y == y)
			return unit;
	return nullptr;
}

void CombatState::addUnitsToCombat(std::vector<Unit*> allVisibleUnits) {
	for (int i = 0; i < allVisibleUnits.size(); i++) {
		for (int j = 0; j < unitsInCombat.size(); j++) {
			if (allVisibleUnits.size() > 0) {
				if (allVisibleUnits[i]->x == unitsInCombat[j]->x && allVisibleUnits[i]->y == unitsInCombat[j]->y) {
					allVisibleUnits.erase(allVisibleUnits.begin() + i);
					i--;
					break;
				}
			}
		}
	}
	if (allVisibleUnits.size() > 0) {
		for (int i = 0; i < allVisibleUnits.size(); i++) {
			if(allVisibleUnits[i]->type != UnitType::player)
				unitsInCombat.push_back(allVisibleUnits[i]);
		}
	}
}
