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
	Move = 3,
	Skip = 4
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


class AIUtility {
public:
	static inline int distance(sf::Vector2i unit1pos, sf::Vector2i unitpos2);

	static std::vector<UnitWithDist> getUnitsDistance(AIResources& util, bool enemies = true);

	static inline std::vector<Node> getPath(AIResources& util, sf::Vector2i target);

	static sf::Vector2i findFieldToMoveTo(sf::Vector2i desired, int leeway, AIResources& util, int minDistance = 0);

	static std::vector<CombatEvent> runCommands(Command& actions, Unit* unit);

	static inline int countUnitsInVicinity(std::vector<UnitWithDist>, sf::Vector2i pos, int distance);

};

	
	
// Interface for a single skill AI, override for different skill usage
class SkillAI {
public:
	virtual Command evaluate(AIComponent& component) { return{}; }; //Not pure virtual because C++ won't let me pass it as parameter then 
};


// Interface for the units overall AI, which defines which skills it prefers to call 
class UnitAI {
public:
	virtual std::vector<CombatEvent> run(AIComponent& component) { return{}; };
};

class MeleeFighterAI : public UnitAI {
public:
	virtual std::vector<CombatEvent> run(AIComponent& component) override;
};

class RangedCasterAI : public UnitAI {
public:
	virtual std::vector<CombatEvent> run(AIComponent& component) override;
};


// Interface for AIComponent, to be overriden for different AI types
class AIComponent {
public:
	// Call this to let the unit evalute and execute its next move
	std::vector<CombatEvent> runAI(CombatState* combat, WeightedGraph* map, aStarSearch* pathfinder, Unit* unit);

	AIComponent(std::shared_ptr<UnitAI> eval, std::shared_ptr<SkillAI> physAI, std::shared_ptr<SkillAI> offSpecAI, 
		std::shared_ptr<SkillAI> defSpecAI, std::shared_ptr<SkillAI> moveAI) :
		ai(eval),
		evalPhysAttack(physAI),
		evalOffSpecial(offSpecAI),
		evalDefSpecial(defSpecAI),
		evalMove(moveAI) {}

	std::shared_ptr<UnitAI> ai;
	std::shared_ptr<SkillAI> evalPhysAttack;
	std::shared_ptr<SkillAI> evalOffSpecial;
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

class CleaveAttack : public SkillAI {
public:
	virtual Command evaluate(AIComponent& component) override;
};

class ApplySelfBuff : public SkillAI {
public:
	ApplySelfBuff(std::function<void(float&)> buff) : buff(buff) {}
	virtual Command evaluate(AIComponent& component) override;
private:
	std::function<void(float&)> buff;
};

class Reposition : public SkillAI {
public:
	Reposition(int unitsInRange, int healthRt, int dist) : maxUnitsInRange(unitsInRange), criticalHealthRatio(healthRt), desiredDistance(dist) {}
	virtual Command evaluate(AIComponent& component) override;
private:
	int maxUnitsInRange;
	int desiredDistance;
	float criticalHealthRatio;
};


class ApplyAPDebuff : public SkillAI {
public:
	ApplyAPDebuff(std::function<void(float&)> debuff) : debuff(debuff) {}
	virtual Command evaluate(AIComponent& component) override;
private:
	std::function<void(float&)> debuff;
};

class HealFriendly : public SkillAI {
public:
	virtual Command evaluate(AIComponent& component) override;
};


