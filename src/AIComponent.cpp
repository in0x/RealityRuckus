#include "AIComponent.h"
#include "CombatEvent.h"

void operator+(std::vector<CombatEvent> &lhv, std::vector<CombatEvent> &rhv)
{
	for(auto ev : rhv) {
		lhv.push_back(ev);
	}
}

std::shared_ptr<AIComponent> AIFactory::getAIComponent(std::string key) {
	int i = 0;

	while (key[i])
	{
		char c = key[i];
		key[i] = tolower(c);
		i++;
	}

	try {
		std::shared_ptr<AIComponent> ref = aiComps.at(key);
		return ref;
	}

	catch (std::exception) {

		if (key == "meleefighter") {
			std::shared_ptr<AIComponent> val = std::make_shared<AIComponent>(
				std::make_shared<MeleeFighterAI>(), 
				std::make_shared<MeleeAttack>(),
				std::make_shared<CleaveAttack>(), 
				std::make_shared<ApplySelfBuff>([](float& val) {val -= 4;}), // here we pass the modifier the unit can apply as a lambda, i.e. incoming damage is reduced by 4 
				std::make_shared<Reposition>(3, 20, 3));
			aiComps.emplace(key, val);
			return aiComps.at(key);
		}

		if (key == "rangedcaster") {
			std::shared_ptr<AIComponent> val = std::make_shared<AIComponent>(
				std::make_shared<RangedCasterAI>(),
				std::make_shared<MeleeAttack>(),
				std::make_shared<ApplyAPDebuff>([](float& val) {val *= 2.5;}),
				std::make_shared<HealFriendly>(),
				std::make_shared<Reposition>(1, 50, 5));
			aiComps.emplace(key, val);
			return aiComps.at(key);
		}
	}

	return nullptr;

}

inline int AIUtility::countUnitsInVicinity(std::vector<UnitWithDist> vec, sf::Vector2i pos, int distance) {
	int total = 0;
	for (auto& el : vec)
		if (el.distance <= distance)
			total++;
	return total;
}

// Enemies is true per default, set to false to get all allies in range
std::vector<UnitWithDist> AIUtility::getUnitsDistance(AIResources& util, bool enemies) {
	std::vector<UnitWithDist> unitsInRange{};

	for (auto& other : util.combat->unitsInCombat) {

		int d = distance(sf::Vector2i{ other->x, other->y }, sf::Vector2i{ util.unit->x, util.unit->y });

		//if (other != util.unit) {

			if (enemies) {
				if (other->type == UnitType::player) {
					unitsInRange.push_back({ d, other });
				}
			}

			else
				if (other->type != UnitType::player)
					unitsInRange.push_back({ d, other });
		//}
	}

	auto comp = [](UnitWithDist& lhv, UnitWithDist rhv) -> bool {
		return lhv.distance < rhv.distance;
	};

	std::sort(unitsInRange.begin(), unitsInRange.end(), comp);

	return unitsInRange;
}

inline std::vector<Node> AIUtility::getPath(AIResources& util, sf::Vector2i target) {
	std::vector<Node> path = util.pathFinder->findPath(Node{ util.unit->x, util.unit->y }, Node{ target.x, target.y }, *util.map);

	return path;
}

inline int AIUtility::distance(sf::Vector2i unit1pos, sf::Vector2i unitpos2) {
	int xDistance = abs(unit1pos.x - unitpos2.x);
	int yDistance = abs(unit1pos.y - unitpos2.y);
	return (xDistance + yDistance);
}

// Leeway defines the maximum distance a move position can be away from the target
sf::Vector2i AIUtility::findFieldToMoveTo(sf::Vector2i desired, int leeway, AIResources& util, int minDistance) {
	auto path = AIUtility::getPath(util, desired);
	sf::Vector2i current = { util.unit->x, util.unit->y };
		
	Node end = path.back();

	if (end.x == desired.x && end.y == desired.y)
		return { end.x, end.y };

	else {
		for (int x = -leeway/2; x <= leeway / 2; x++) {
			for (int y = -leeway/2; y <= leeway / 2; y++) {

				if (abs(x) < minDistance|| abs(y) < minDistance) // Continue if field gets too close to unit
					continue;

				sf::Vector2i newTarget = { desired.x + x, desired.y + y };

				if (newTarget.x > 0 && newTarget.y > 0 && newTarget.x < 30 && newTarget.y < 30) {

					if (util.map->map[desired.x + x][desired.y + y].accessible == true && util.map->map[desired.x + x][desired.y + y].occupied == true) {
						if (getPath(util, { desired.x + x, desired.y + y }).size() < 40)
							return{ desired.x + x, desired.y + y };
					}
				}
			}
		}
	}

	return{ -1,-1 };

}


std::vector<CombatEvent> AIComponent::runAI(CombatState* combat, WeightedGraph* map, aStarSearch* pathFinder, Unit* unit) {
	util = { combat, map, pathFinder, unit };
	return ai->run(*this);
}

std::vector<CombatEvent> AIUtility::runCommands(Command& actions, Unit* unit) {
	
	std::vector<CombatEvent> events{};

	for (int i = 0; i < actions.commands.size(); i++) {
		auto ev = unit->learnedActions[(int)actions.commands[i]]->doAction(unit, actions.targets[i].x, actions.targets[i].y);
		for (auto& e : ev)
			events.push_back(e);
	}
	return events;
}

std::vector<CombatEvent> MeleeFighterAI::run(AIComponent& component) {

	// In this case a cleave is more valuable than a regular phys, so we try to use it first

	// If at less than 20% health we want to evade the enemy instead of attacking

	
	Command actions{};
	/*
	if (component.util.unit->currHP / component.util.unit->maxHP >= 0.2) {
	*/
	actions = component.evalOffSpecial->evaluate(component);

	if (actions.commands[0] != CommandType::None) {
		return AIUtility::runCommands(actions, component.util.unit);
	}

	actions = component.evalPhysAttack->evaluate(component);

	if (actions.commands[0] != CommandType::None) {
		return AIUtility::runCommands(actions, component.util.unit);
	}
	//}

	// If neither a physical attack or a cleave are viable we have to evaluate 
	// if it is ok to cast the self buff in this position or if we should move

	// A melee fighter does not want to have more than 2 enemies in his vicinity
	auto enemies = AIUtility::getUnitsDistance(component.util);
	int inRange = 0;

	for (auto& enemy : enemies)
		if (enemy.distance <= 3)
			inRange++;

	if (inRange > 2) {
		actions = component.evalMove->evaluate(component);
		if (actions.commands[0] != CommandType::None) {
			return AIUtility::runCommands(actions, component.util.unit);
		}
	}

	if (rand() % 100 + 1 < 20) {
		if (component.util.unit->modifiers.size() < 1)
			actions = component.evalDefSpecial->evaluate(component);
	}

	if (actions.commands[0] != CommandType::None) {
		return AIUtility::runCommands(actions, component.util.unit);
	}

	return{ { component.util.unit, CombatEventType::Skip} };
}

// This defines the AI of a melee attack. It is completely decoupled from all other factors, it only does its own thing
Command MeleeAttack::evaluate(AIComponent& component) {

	// Index 0 is physical attack
	int range = component.util.unit->learnedActions[0]->range;
	int cost = component.util.unit->learnedActions[0]->cost;

	Command actions = { { CommandType::None } };
	AIResources& util = component.util;
	Unit* target;

	// Sorted by distance, ascending
	auto enemies = AIUtility::getUnitsDistance(component.util);

	// Attack the first unit in range
	if (enemies[0].distance <= range) {
		target = enemies[0].unit;
		auto moveTo = AIUtility::findFieldToMoveTo({ target->x, target->y - (range/2) }, range, util);

		if (moveTo.x != -1) { //field found

			actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
			return actions;
		}
	}

	//Otherwise look for one that can be attacked with a move
	else {
		for (auto& other : enemies) {
			//auto path = AIUtility::getPath(util, { other.unit->x, other.unit->y });

			auto moveTo = AIUtility::findFieldToMoveTo({ other.unit->x, other.unit->y - 1 }, range, util);

			auto path = AIUtility::getPath(util, { moveTo });

			auto end = path.back();

			if (cost + path.size() <= component.util.unit->currAP) {

				target = other.unit;

				auto moveTo = AIUtility::findFieldToMoveTo({ other.unit->x, other.unit->y - (range/2) }, range, util, (range/2));

				if (moveTo.x != -1) { //field found

					actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
					return actions;
				}

			}
		}
	}

	return{ { CommandType::None } };
}



Command CleaveAttack::evaluate(AIComponent& component) {

	// Index 1 is offensive special 
	int range = component.util.unit->learnedActions[1]->range;
	int cost = component.util.unit->learnedActions[1]->cost;
	int damage = component.util.unit->learnedActions[1]->damage;

	Command actions = { { CommandType::None } };
	AIResources& util = component.util;
	
	Unit* target = nullptr;
	sf::Vector2i moveTo{};

	// Sorted by distance, ascending
	auto enemies = AIUtility::getUnitsDistance(component.util);


	if (enemies[0].distance <= range) {
	
		auto loc = AIUtility::findFieldToMoveTo({ enemies[0].unit->x, enemies[0].unit->y - 1 }, range, util);

		if (loc.x != -1) {
			moveTo = loc;
			target = enemies[0].unit;
			actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
			return actions;
		}
	}

	for (auto& other : enemies) {
		//auto path = AIUtility::getPath(util, { other.unit->x, other.unit->y });

		auto moveTo = AIUtility::findFieldToMoveTo({ other.unit->x, other.unit->y - 1 }, range, util);

		auto path = AIUtility::getPath(util, moveTo);

		auto end = path.back();

		if (cost + path.size() <= component.util.unit->currAP) {
			
			//auto moveTo = AIUtility::findFieldToMoveTo({ other.unit->x, other.unit->y - 1 }, range, util);

			if (moveTo.x != -1) { // Field found 

				if (target == nullptr) { // If we haven't found a suitable target yet, select this one
					target = other.unit;
					actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
				}

				if (other.unit->currHP <= damage) { // We can kill this unit, so we go for it first
					target = other.unit;
					actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
					return actions;
				}

			}

			return actions;
		}
	}

	return{ { CommandType::None } };

	/*
		TODO Cleave Behaviour (Go again if the unit was killed)
	*/
}

Command ApplySelfBuff::evaluate(AIComponent& component) {
	// Take 3 damage less for 1 round
	Unit* u = component.util.unit;

	u->addModifier({ ModifierType::HPLoss, 1, buff });

	float cost = u->learnedActions[2]->cost;
	u->applyModifiers(ModifierType::APLoss, cost);

	u->loseAP(cost); // We need to trigger the AP loss here since the action doesnt reach the unitmanager
	return{ { CommandType::None } };
}


Command Reposition::evaluate(AIComponent& component) {

	Unit* unit = component.util.unit;

	int _maxUnitsInRange = maxUnitsInRange;

	if (unit->currHP / unit->maxHP <= criticalHealthRatio) // If at less than critical Health, we don't want to have any Units in Range
		_maxUnitsInRange = 0;

	auto enemies = AIUtility::getUnitsDistance(component.util);

	// Check if there are too many enemies in vicinity, if not, don't move
	if (AIUtility::countUnitsInVicinity(enemies, { unit->x, unit->y }, desiredDistance) <= _maxUnitsInRange) return{ { CommandType::None } };
	
	std::vector<sf::Vector2i> moveDirs = {
		{1,0},
		{0,1},
		{-1,0},
		{0,-1},
		{1,1},
		{-1,-1},
		{1,-1},
		{-1,1}
	};

	for (auto& dir : moveDirs) {
		sf::Vector2i newPos = {unit->x + dir.x, unit->y + dir.y};
		
		int steps = 0;
		
		while (steps <= 6 && newPos.x > 0 && newPos.y > 0 && newPos.x < 30 && newPos.y < 30) {
			
			if (AIUtility::countUnitsInVicinity(enemies, newPos, desiredDistance) <= _maxUnitsInRange) 
				return{ { CommandType::Move } ,{ newPos } };
	
			newPos += dir;
			steps++;
		}

	}

	return{ {CommandType::None} };
}

Command ApplyAPDebuff::evaluate(AIComponent& component) {
	
	int range = component.util.unit->learnedActions[0]->range;
	int cost = component.util.unit->learnedActions[0]->cost;

	Command actions = { { CommandType::None } };
	AIResources& util = component.util;
	Unit* target;

	// Sorted by distance, ascending
	auto enemies = AIUtility::getUnitsDistance(component.util);

	// If a unit is already in range, apply the debuff to it
	if (enemies[0].distance <= range) {
		target = enemies[0].unit;
		auto moveTo = AIUtility::findFieldToMoveTo({ target->x, target->y - (range / 2) }, range, util);

		if (moveTo.x != -1) { //field found

			actions = { { CommandType::AggSpecial },{ { target->x, target->y } } };
			return actions;
		}
	}

	//Otherwise look for one that can be attacked with a move
	else {

		for (auto& other : enemies) {

			auto path = AIUtility::getPath(util, { other.unit->x, other.unit->y });

			if (cost + path.size() <= component.util.unit->currAP) {

				target = other.unit;

				auto moveTo = AIUtility::findFieldToMoveTo({ other.unit->x, other.unit->y - range }, range, util);

				if (moveTo.x != -1) { //field found
				
					actions = { { CommandType::Move, CommandType::AggSpecial },{ moveTo,{ target->x, target->y } } };
					return actions;
				}

			}
		}
	}

	return Command{ {CommandType::None} };
}

Command HealFriendly::evaluate(AIComponent& component) {

	Unit* unit = component.util.unit;
	int range = component.util.unit->learnedActions[2]->range;
	int cost = component.util.unit->learnedActions[2]->cost;
	Command actions{ {CommandType::None} };

	if (unit->currHP / unit->maxHP <= 0.4) { // If at less than 40% health, self heal
		return { { CommandType::DefSpecial },{ { unit->x, unit->y } } };
	}

	auto friendly = AIUtility::getUnitsDistance(component.util, false);

	for (auto& ally : friendly) {
		
		if (ally.unit->currHP / ally.unit->maxHP <= 0.8) {
			
			if (ally.distance <= range) {
				actions = { {CommandType::DefSpecial}, {{ally.unit->x, ally.unit->y}} };
			}

			else {
				
				auto moveTo = AIUtility::findFieldToMoveTo({ ally.unit->x, ally.unit->y - range }, range, component.util);

				if (moveTo.x != -1) { //field found

					auto path = AIUtility::getPath(component.util, { ally.unit->x, ally.unit->y });

					if (cost + path.size() <= unit->currAP) {
						actions = { { CommandType::DefSpecial },{ moveTo } };
					}
				}
			}
		}
	}

	return actions;
}

std::vector<CombatEvent> RangedCasterAI::run(AIComponent& component) {

	Command actions{};

	srand(time(NULL));

	auto enemies = AIUtility::getUnitsDistance(component.util);
	auto friendly = AIUtility::getUnitsDistance(component.util, false);

	for (auto& ally : friendly) {
		if (ally.unit->currHP / ally.unit->maxHP <= 0.5)
			actions = component.evalDefSpecial->evaluate(component);
	}

	if (component.util.unit->currHP / component.util.unit->maxHP >= 0.4) {

		if (rand() % 100 + 1 < 30) { // Only cast AP drain with a 30% probability to make the behaviour a bit more dynamic

			try {
				actions = component.evalOffSpecial->evaluate(component);
			}
			catch (std::exception) {
			
			}

			if (actions.commands[0] != CommandType::None) {
				return AIUtility::runCommands(actions, component.util.unit);
			}
		}

		actions = component.evalPhysAttack->evaluate(component);

		if (actions.commands[0] != CommandType::None) {
			return AIUtility::runCommands(actions, component.util.unit);
		}
	}

	int inRange = 0;

	for (auto& enemy : enemies)
		if (enemy.distance <= 3)
			inRange++;

	if (inRange > 2) {
		actions = component.evalMove->evaluate(component);

		if (actions.commands[0] != CommandType::None) {
			return AIUtility::runCommands(actions, component.util.unit);
		}
	}

	actions = component.evalDefSpecial->evaluate(component);

	if (actions.commands[0] != CommandType::None) {
		return AIUtility::runCommands(actions, component.util.unit);
	}

	return{ { component.util.unit, CombatEventType::Skip } };
}