#include "AIController.h"

void AIController::runNextAIMove() {
	combat.getFirstUnit();
};

void AIController::setNewCombat(CombatState& combat) {
	this->combat = combat;
}

void AIController::setNewMap(WeightedGraph& map) {
	this->map = map;
}