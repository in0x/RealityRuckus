#pragma once
#include "Unit.h"
#include <algorithm>


class CombatState {
public:
	CombatState(std::vector<Unit*>);
	Unit* getFirstUnit();
	void updateListOfUnits();
	bool isDone();
	void listUnits();
	Unit* findUnit(int x, int y);
	std::vector<Unit*> unitsInCombat;
private:
	void removeUnit(Unit* unitToRemove);
	struct CompareUnitApPtr 
	{
		bool operator()(Unit* lhs, Unit* rhs) {
			return lhs->currAP > rhs->currAP;
		}
	} unitComparer; 
};

