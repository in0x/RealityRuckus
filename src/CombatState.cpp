#pragma once
#include "CombatState.h"
#include <iostream>

CombatState::CombatState(std::vector<Unit*> combatUnitPtrs) {
	unitsInCombat = combatUnitPtrs;
	std::sort(unitsInCombat.begin(), unitsInCombat.end(), unitComparer); 
}

void CombatState::updateListOfUnits() {
	std::sort(unitsInCombat.begin(), unitsInCombat.end(), unitComparer);
	std::vector<Unit*> unitsToRemove = std::vector<Unit*>();
	for (auto unit : unitsInCombat) {
		if (unit->currHP <= 0) {
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
	return unitsInCombat.size() == 1;
}

void CombatState::listUnits() {
	for (auto& unit : unitsInCombat)
		std::cout << "Unit at: (x: " << unit->x << " | y: " << unit->y << ")" << std::endl;
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

Unit* CombatState::findUnit(int x, int y) {
	for (auto unit : unitsInCombat)
		if (unit->x == x && unit->y == y)
			return unit;
	return nullptr;
}