#pragma once
#include "Unit.h"
#include <algorithm>

//class Unit;

//struct CompareUnitApPtr {
//public:
//	bool operator()(Unit* lhs, Unit* rhs);
//} unitComparer;

class CombatState {
public:
	
	CombatState(std::vector<Unit*>);

	Unit* getFirstUnit();
	Unit* findUnit(int x, int y);

	void updateListOfUnits();
	void listUnits();

	bool isDone();

	std::vector<Unit*> unitsInCombat;
	std::vector<std::tuple<int, Unit*>> replenishUnitAP();

private:

	void removeUnit(Unit* unitToRemove);

	//struct CompareUnitApPtr 
	//{
	//	bool operator()(Unit* lhs, Unit* rhs) {
	//		return lhs->currAP > rhs->currAP;
	//	}
	//} unitComparer; 
};

