#include "ActionManager.h"
#include "CombatEvent.h"
#include "UnitManager.h"
#include "AnimationManager.h"

bool ActionManager::checkRange(int range, sf::Vector2i unit1pos, sf::Vector2i unitpos2) {
	int xDistance = abs(unit1pos.x - unitpos2.x);
	int yDistance = abs(unit1pos.y - unitpos2.y);
	return sqrt(pow(xDistance,2) + pow(yDistance,2)) <= range;
}

std::vector<CombatEvent> ActionManager::moveUnit(Unit * unit, int x, int y)
{
	std::vector<CombatEvent> events;
	int oldx = unit->x;
	int oldy = unit->y;
	std::vector<Node> path = unitManager->moveUnit(x, y, unit);

	if (path.size() && path.size() < 90) // Pathfinder seems to sometimes return 101 for some reason, so this is a quick and dirty fix
	{
		if (path.size() > unit->currAP) {
			events.push_back(CombatEvent{ unit, CombatEventType::NotValid,
				"You don't have enough AP for this action.\nYour AP: " + std::to_string(unit->currAP) + "\tRequired AP: " + std::to_string(path.size())
			});

			return events;
		}

		animationManager->registerMovement(unit->sprite, new DrawableUnit{ (float)unit->x, (float)unit->y, unit->sprite->sprite }, path, 0.5);
		
		CombatEvent c1 = CombatEvent(unit, CombatEventType::Move);
		c1.setMove(oldx, oldy, unit->x, unit->y);
		events.push_back(c1);

		float cost = path.size();
		unit->applyModifiers(ModifierType::APLoss, cost);


		float d_cost = cost;
		unit->applyModifiers(ModifierType::APLoss, d_cost);
		unit->loseAP(d_cost);
		CombatEvent c2 = CombatEvent(unit, CombatEventType::AP);
		c2.setAPChange(d_cost);
		events.push_back(c2);
	}
	else
	{
		CombatEvent c1 = CombatEvent(unit, CombatEventType::NotValid);
		events.push_back(c1);
	}
	return events;
}


std::vector<CombatEvent> ActionManager::isValidAction(int x, int y, Unit* unit, int range, int cost) {

	std::vector<CombatEvent> events;

	if (!checkRange(range, sf::Vector2i{ x, y }, sf::Vector2i{ unit->x, unit->y })) {
		int distance = ceil(sqrt(pow(unit->x - x, 2) + pow(unit->y - y, 2)));

		events.push_back(CombatEvent{ unit, CombatEventType::NotValid,
			"You need to move closer for this action\nDistance from target: " + std::to_string(distance) + "\tSkill Range: " + std::to_string(range)
		});
		return events;
	}

	if (cost > unit->currAP) {

		events.push_back(CombatEvent{ unit, CombatEventType::NotValid,
			"You don't have enough AP for this action.\nYour AP: " + std::to_string(unit->currAP) + "\tRequired AP: " + std::to_string(cost)
		});
		return events;
	}


	return {{ unit, CombatEventType::None }};

}

//unit is the the Unit that wants to do something, x and y define where the affected Unit is
std::vector<CombatEvent> ActionManager::damageUnit(Unit * unit, int x, int y, float hp, int range, int cost)
{
	std::vector<CombatEvent> events;

	Unit* damagedUnit = currentCombat->findUnit(x, y);
	if (damagedUnit == nullptr)
	{
		events.push_back(CombatEvent(unit, CombatEventType::NotValid, "No Target here"));
		return events;
	}
	
	events = isValidAction(x, y, unit, range, cost);

	if (events[0].type == CombatEventType::NotValid)
		return events;

	events = {};


	//else
	{
		damagedUnit->applyModifiers(ModifierType::HPLoss, hp);
		bool unitDied = unitManager->damageUnit(damagedUnit, hp);
		currentCombat->updateListOfUnits();
		CombatEvent ce = { damagedUnit, CombatEventType::HP };
		ce.setHPChange(hp);
		events.push_back(ce);

		float mod_cost = cost;
		unit->loseAP(mod_cost);
		unit->applyModifiers(ModifierType::APLoss, mod_cost);
		ce = { unit, CombatEventType::AP };
		ce.setAPChange(mod_cost);
		events.push_back(ce);
		
		if (unitDied) {
			ce = {damagedUnit, CombatEventType::UnitDied};
			ce.affected = damagedUnit;
			events.push_back(ce);
		}
	}

	return events;
}

std::vector<CombatEvent> ActionManager::changeAP(Unit * unit, int x, int y, float ap, int range, int cost)
{
	std::vector<CombatEvent> events;

	events = isValidAction(x, y, unit, range, cost);

	if (events[0].type == CombatEventType::NotValid)
		return events;

	events = {};

	Unit* affectedUnit = currentCombat->findUnit(x, y);

	if (affectedUnit == nullptr)
	{
		events.push_back(CombatEvent(unit, CombatEventType::NotValid, "No Target here"));
		return events;
	}
	else
	{
		affectedUnit->applyModifiers(ModifierType::APLoss, ap);

		float d_cost = cost;
		affectedUnit->applyModifiers(ModifierType::APLoss, d_cost);
		affectedUnit->loseAP(d_cost);
		currentCombat->updateListOfUnits();
		CombatEvent ce = { affectedUnit, CombatEventType::AP };
		ce.setAPChange(d_cost);
		events.push_back(ce);
	}

	return events;
}

std::vector<CombatEvent> ActionManager::applyBuff(Unit * unit, int x, int y, Modifier mod, int range, int cost)
{
	std::vector<CombatEvent> events;

	events = isValidAction(x, y, unit, range, cost);

	if (events[0].type == CombatEventType::NotValid)
		return events;

	events = {};

	Unit* affectedUnit = currentCombat->findUnit(x, y);

	if (affectedUnit == nullptr)
	{
		events.push_back(CombatEvent(unit, CombatEventType::NotValid, "No Target here"));
		return events;
	}
	else
	{
		affectedUnit->addModifier(mod);

		float d_cost = cost;
		affectedUnit->applyModifiers(ModifierType::APLoss, d_cost);
		affectedUnit->loseAP(d_cost);
		currentCombat->updateListOfUnits();
		CombatEvent ce = { affectedUnit, CombatEventType::AP };
		ce.setAPChange(d_cost);
		events.push_back(ce);
	}

	return events;

}



void ActionManager::setCombatState(CombatState* newCombat)
{
	this->currentCombat = newCombat;
}

void ActionManager::playAnimation(std::string name, sf::Vector2f from, sf::Vector2f to, float timeToLive)
{
	this->animationManager->playAnimation(name, from, to, timeToLive);
}

ActionManager::ActionManager(UnitManager * unitManager, AnimationManager* animationManager)
{
	this->unitManager = unitManager;
	this->animationManager = animationManager;
}



ActionManager::~ActionManager()
{
}
