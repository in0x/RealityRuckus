#include "AIComponent.h"

std::ostream& operator<<(std::ostream& os, const Unit& unit) {
	os << "Unit: ( Type = " + std::to_string(unit.type) + " ) (AP: " + std::to_string(unit.currAP) + " ) " + " ( x: " + std::to_string(unit.x) + " , y: " + std::to_string(unit.x) + " )";
	return os;
}

// Enemies is true per default, set to false to get all allies in range
std::vector<UnitWithDist> findUnitsInRange(int range, AIResources& util, bool enemies) {
	std::vector<UnitWithDist> unitsInRange{};

	for (auto& other : util.combat->unitsInCombat) {

		int d = distance(sf::Vector2i{ other->x, other->y }, sf::Vector2i{ util.unit->x, util.unit->y });

		if (d <= range) {

			if (other != util.unit) {

				if (enemies) {
					if (other->type == UnitType::player) {
						unitsInRange.push_back({ d, other });
						std::cout << *other << std::endl;
					}
				}

				else
					if (other->type != UnitType::player)
						unitsInRange.push_back({ d, other });
			}
		}

	}

	auto comp = [](UnitWithDist& lhv, UnitWithDist rhv) -> bool {
		return lhv.distance < rhv.distance;
	};

	std::sort(unitsInRange.begin(), unitsInRange.end(), comp);

	return unitsInRange;
}

inline std::vector<Node> getPath(AIResources& util, sf::Vector2i target) {
	std::vector<Node> path = util.pathFinder->findPath(Node{ util.unit->x, util.unit->y }, Node{ target.x, target.y }, *util.map);

	return path;
}

inline int distance(sf::Vector2i unit1pos, sf::Vector2i unitpos2) {
	int xDistance = abs(unit1pos.x - unitpos2.x);
	int yDistance = abs(unit1pos.y - unitpos2.y);
	return (xDistance + yDistance);
}

// Leeway defines the maximum distance a move position can be away from the target
sf::Vector2i findFieldToMoveTo(sf::Vector2i desired, int leeway, AIResources& util) {
	auto path = getPath(util, desired);
	sf::Vector2i current = { util.unit->x, util.unit->y };
		
	Node end = *path.end();
	if (end.x == desired.x && end.y == desired.y)
		return{ end.x, end.y };

	else {
		for (int x = -leeway/2; x <= leeway / 2; x++) {
			for (int y = -leeway/2; y <= leeway / 2; y++) {

				sf::Vector2i newTarget = { desired.x + x, desired.y + y };

				if (newTarget.x > 0 && newTarget.y > 0 && newTarget.x < 30 && newTarget.y < 30) {

					if (util.map->map[desired.x + x][desired.y + y].accessible == true) {
						return{ desired.x + x, desired.y + y };
					}
				}
			}
		}
	}

	return{ -1,-1 };

}


void AIComponent::runAI(CombatState* combat, WeightedGraph* map, aStarSearch* pathFinder, Unit* unit) {
	util = { combat, map, pathFinder, unit };
	ai->run(*this);
}

void runCommands(Command& actions, Unit* unit) {
	for (int i = 0; i < actions.commands.size(); i++) {
		unit->learnedActions[(int)actions.commands[i]]->doAction(unit, actions.targets[i].x, actions.targets[i].y);
	}
}

void MeleeFighterAI::run(AIComponent& component) {

	// A melee fighter wants to hit stuff, so he always tries to use his physical attack first
	Command actions = component.evalPhysAttack->evaluate(component);

	if (actions.commands[0] != CommandType::None) {
		runCommands(actions, component.util.unit);
		return;
	}

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
	auto enemies = findUnitsInRange(range, component.util);

	// Attack the first unit in range
	if (enemies[0].distance <= range)
		target = enemies[0].unit;

	//Otherwise look for one that can be attacked with a move
	else {
		for (auto& other : enemies) {
			auto path = getPath(util, { other.unit->x, other.unit->y });

			if (cost + path.size() <= component.util.unit->currAP) {

				target = other.unit;

				auto moveTo = findFieldToMoveTo({ other.unit->x, other.unit->y - 1 }, range, util);

				if (moveTo.x != -1) { //field found

					actions = { { CommandType::Move, CommandType::Physical },{ moveTo,{ target->x, target->y } } };
					return actions;
				}

			}
		}
	}

	return{ { CommandType::None } };
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
				std::make_shared<MeleeFighterAI>(), std::make_shared<MeleeAttack>(), 
				std::make_shared<SkillAI>(), std::make_shared<SkillAI>(), std::make_shared<SkillAI>());
			aiComps.emplace(key, val);
			return aiComps.at(key);
		}
	}

	return nullptr;

}


