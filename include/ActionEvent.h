#pragma once
#include <iostream>
#include "CombatEvent.h"
#include "Modifier.h"

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

	APchangeActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float ap, std::string animation ="");

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;


};

class PommelStrikeEvent : public ActionEvent {
public:

	PommelStrikeEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage, float ap, std::string animation = "");

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;

private:
	float ap;
};

class BuffEvent : public ActionEvent {

public:

	BuffEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, Modifier mod, std::string animation = "");

	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;
private:
	Modifier mod;

};

class TurnEndingBuff
{
public:
	TurnEndingBuff()
	{
		savedAP = new int();
	}
	~TurnEndingBuff()
	{
		delete savedAP;
	}
	Modifier getModifier(int ap)
	{
		*savedAP = ap;
		std::cout << "generated with " << *savedAP << "% chance." << std::endl;
		std::function<void(float&)> func = [&](float& num) {
			std::cout << "attacked, " << *savedAP << "% chance." << std::endl;
			if (rand() % 100 + 1 < *savedAP) {
				num = 0;
				std::cout << "MISSED, " << *savedAP << "% chance." << std::endl;
			}
		};
		float dmg = 25;
		func(dmg);
		Modifier mod = { ModifierType::HPLoss, 1, func };
		return mod;
	}
private:
	int* savedAP;

};

class TurnEndingBuffEvent : public ActionEvent {

public:

	TurnEndingBuffEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, TurnEndingBuff* func, std::string animation = "");
	~TurnEndingBuffEvent();
	virtual std::vector<CombatEvent> doAction(Unit* sender, int x, int y) override;
private:
	TurnEndingBuff* func;

};