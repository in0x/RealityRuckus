#pragma once
#include <SFML/Graphics.hpp>
#include "unitType.h"
#include "textureManager.h"
#include "ActionEvent.h"
#include <queue>
#include <memory>
#include <iostream>
#include "UnitAnimations.hpp"
#include "AIComponent.h"

class AIComponent;

enum class ModifierType {
	HPLoss,
	HPGain,
	APLoss,
	APGain,
	Movement
};

struct Modifier {
public:
	ModifierType type;
	int rounds;
	std::function<void(float&)> func;
};

class Unit
{

public:
	float maxAP;
	float currAP;
	float maxHP;
	float currHP;
	int x; 
	int y;
	UnitType type;
	UnitAnimations* sprite;
	Unit(float mAP, float mHP, UnitAnimations*, std::shared_ptr<AIComponent> aiComp, int xPos = 29, int yPos = 29, UnitType u = standard);
	Unit(float mAP, float mHP, UnitAnimations*, int xPos, int yPos, UnitType u, std::vector<ActionEvent*>, std::shared_ptr<AIComponent> aiComp);
	Unit();
	~Unit();
	std::vector<ActionEvent*> learnedActions{};
	std::string name = "";
	std::string nickName = "";
	std::shared_ptr<AIComponent> aiComponent = nullptr;
	std::vector<Modifier> modifiers{};

	void setAPGainPerRound(float ap);
	void loseAP(float amount);
	void loseHP(float amount);
	void gainAP(float amount);
	void gainHP(float amount);

	void cycleModifiers();
	void addModifier(Modifier modifier);
	void applyModifiers(ModifierType type, float& val);

	int replenishAP();

private:
	float apGain = 0.5f;
};

