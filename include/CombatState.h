#pragma once
#include "Unit.h"
#include <algorithm>

class CombatState {
public:
	
	CombatState(std::vector<Unit*>);

	Unit* getFirstUnit();
	Unit* findUnit(int x, int y);

	void updateListOfUnits();
	void listUnits();
	void cycleUnitModifiers();
	void skipTurn(Unit * skipper);
	void addUnitsToCombat(std::vector<Unit*>);
	
	std::vector<CombatEvent> endTurn();
	
	bool isDone();

	std::vector<Unit*> unitsInCombat;
	//std::vector<std::tuple<int, Unit*>> replenishUnitAP();
	
private:

	void removeUnit(Unit* unitToRemove);
 
};

