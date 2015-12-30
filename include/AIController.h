#pragma once
#include "CombatState.h"
#include "WeightedGraph.h"
class AIController {
public:
	AIController(CombatState& combat, WeightedGraph& map) : combat(combat), map(map) {}
	void runNextAIMove();
	void setNewCombat(CombatState& combat);
	void setNewMap(WeightedGraph& map);
private:
	CombatState& combat;
	WeightedGraph& map;
};