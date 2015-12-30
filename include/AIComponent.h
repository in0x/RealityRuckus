#pragma once
#include "aStarSearch.h"
#include "WeightedGraph.h"
#include "CombatState.h"
#include <string>
#include <memory>

class CombatState;
class AIComponent;
class WeightedGraph;
class aStarSearch;

enum class CommandType {
	None = -1,
	Physical = 0,
	AggSpecial = 1,
	DefSpecial = 2,
	Move = 3
};


struct Command {
public:
	std::vector<CommandType> commands;
	std::vector<sf::Vector2i> targets;
};

struct UnitWithDist {
public:
	int distance;
	Unit* unit;
};

// This is used to temporarly store all necessary resources for an AI calculation
struct AIResources {
public: 
	CombatState* combat;
	WeightedGraph* map;
	aStarSearch* pathFinder;
	Unit* unit;
};

//////// Utility Functions ////////

inline int distance(sf::Vector2i unit1pos, sf::Vector2i unitpos2);

std::vector<UnitWithDist> findUnitsInRange(int range, AIResources& util ,bool enemies = true);
	
inline std::vector<Node> getPath(AIResources& util, sf::Vector2i target);

sf::Vector2i findFieldToMoveTo(sf::Vector2i desired, int leeway, AIResources& util);

void runCommands(Command& actions, Unit* unit);

////////////////
	
	
// Interface for a single skill AI, override for different skill usage
class SkillAI {
public:
	virtual Command evaluate(AIComponent& component) { return{}; }; //Not pure virtual because C++ won't let me pass it as parameter then 
};


// Interface for the units overall AI, which defines which skills it prefers to call 
class UnitAI {
public:
	virtual void run(AIComponent& component) {};
};

class MeleeFighterAI : public UnitAI {
public:
	std::string name = "melee";
	virtual void run(AIComponent& component) override;
};


// Interface for AIComponent, to be overriden for different AI types
class AIComponent {
public:
	// Call this to let the unit evalute and execute its next move
	void runAI(CombatState* combat, WeightedGraph* map, aStarSearch* pathfinder, Unit* unit);

	AIComponent(std::shared_ptr<MeleeFighterAI> eval, std::shared_ptr<SkillAI> physAI, std::shared_ptr<SkillAI> aggSpecAI, 
		std::shared_ptr<SkillAI> defSpecAI, std::shared_ptr<SkillAI> moveAI) :
		ai(eval),
		evalPhysAttack(physAI),
		evalAggSpecial(aggSpecAI),
		evalDefSpecial(defSpecAI),
		evalMove(moveAI) {}

	std::shared_ptr<MeleeFighterAI> ai;
	std::shared_ptr<SkillAI> evalPhysAttack;
	std::shared_ptr<SkillAI> evalAggSpecial;
	std::shared_ptr<SkillAI> evalDefSpecial;
	std::shared_ptr<SkillAI> evalMove;
	
	AIResources util;
};

class AIFactory {
private:
	std::unordered_map<std::string, std::shared_ptr<AIComponent>> aiComps{};
public:
	std::shared_ptr<AIComponent> getAIComponent(std::string key);
};


class MeleeAttack : public SkillAI {
public:
	virtual Command evaluate(AIComponent& component) override;
};


