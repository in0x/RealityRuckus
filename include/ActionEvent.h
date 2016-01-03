#pragma once
#include <iostream>
#include "CombatEvent.h"

class ActionManager;
class CombatEvent;
class Unit;
class AIComponent;

class ActionEvent {
public:

	ActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage = 0, std::string animation = "");

	std::string name;
	std::string description;
	std::string animation;

	int range;
	int cost;
	float damage;

	ActionManager* actionmanager;

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y);
};
	
class MoveActionEvent :public ActionEvent {

public:

	MoveActionEvent(std::string name, std::string description, ActionManager* actionmanager);

	//Add the override key word to overriden functions to show that you are replacing a function, not declaring a new one
	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;
};

class AttackActionEvent : public ActionEvent {

public:

	AttackActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage, std::string animation = "");

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;
};

class APchangeActionEvent : public ActionEvent {

public:

	APchangeActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float ap);

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;


};

