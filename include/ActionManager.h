#pragma once
#include "CombatEvent.h"

class UnitManager;
class AnimationManager;
class CombatState;

class ActionManager
{
public:
	~ActionManager();

	std::vector<CombatEvent> moveUnit(Unit* unit, int x, int y);
	std::vector<CombatEvent> damageUnit(Unit* unit, int x, int y, float hp, int range, int cost);
	std::vector<CombatEvent> changeAP(Unit* unit, int x, int y, float ap, int range, int cost);

	void setCombatState(CombatState * newCombat);

	void playAnimation(std::string name, sf::Vector2f from, sf::Vector2f to, float timeToLive);

	ActionManager(UnitManager * unitManager, AnimationManager * animationManager);

	AnimationManager* animationManager;
	UnitManager* unitManager;
	CombatState* currentCombat;
	
private:
	bool checkRange(int range, sf::Vector2i unit1pos, sf::Vector2i unitpos2);
	std::vector<CombatEvent> isValidAction(int x, int y, Unit* unit, int range, int cost);
};

