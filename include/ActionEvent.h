#pragma once
#include <iostream>

class ActionManager;
class CombatEvent;
class Unit;

class ActionEvent {
public:
	std::string name;
	std::string description;
	ActionManager* actionmanager;
	ActionEvent(std::string name, std::string description, ActionManager* actionmanager);
	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y);
};
	
class MoveActionEvent :public ActionEvent {
public:
	MoveActionEvent(std::string name, std::string description, ActionManager* actionmanager);
	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y);
};

class AttackActionEvent : public ActionEvent {
public:
	AttackActionEvent(std::string name, std::string description, ActionManager* actionmanager);
	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y);
};