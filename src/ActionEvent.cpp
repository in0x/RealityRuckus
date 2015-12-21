#pragma once
#include "CombatEvent.h"
#include "ActionManager.h"
#include "ActionEvent.h"


	ActionEvent::ActionEvent(std::string name, std::string description, ActionManager* actionmanager) : name(name), description(description), actionmanager(actionmanager) {}
   std::vector<CombatEvent> ActionEvent::doAction(Unit* sender, int x, int y) { std::cout << "ActionEvent::doAction()" << std::endl; return std::vector<CombatEvent>(); }


	MoveActionEvent::MoveActionEvent(std::string name, std::string description, ActionManager* actionmanager) : ActionEvent(name, description, actionmanager) {}
	std::vector<CombatEvent> MoveActionEvent::doAction(Unit* sender, int x, int y) {
		return actionmanager->moveUnit(sender, x, y);
		//return std::vector<CombatEvent>();
	}


	AttackActionEvent::AttackActionEvent(std::string name, std::string description, ActionManager* actionmanager) : ActionEvent(name, description, actionmanager) {}
	std::vector<CombatEvent> AttackActionEvent::doAction(Unit* sender, int x, int y) {
		std::vector<CombatEvent> events = actionmanager->damageUnit(sender, x, y, 5);
		CombatEvent ce = CombatEvent(sender);
		ce.setAPChange(5);
		events.push_back(ce);
		return events;
		//return std::vector<CombatEvent>();
	}