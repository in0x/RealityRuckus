#pragma once
#include "CombatEvent.h"
#include "ActionManager.h"
#include "ActionEvent.h"


	ActionEvent::ActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost) 
		: name(name), 
		description(description), 
		actionmanager(actionmanager),
		range(range),
		cost(cost) {}

   std::vector<CombatEvent> ActionEvent::doAction(Unit* sender, int x, int y) { return std::vector<CombatEvent>(); }



	MoveActionEvent::MoveActionEvent(std::string name, std::string description, ActionManager* actionmanager) 
		: ActionEvent(name, description, actionmanager, 0, 0) {}
	
	std::vector<CombatEvent> MoveActionEvent::doAction(Unit* sender, int x, int y) {
		return actionmanager->moveUnit(sender, x, y);
	}



	AttackActionEvent::AttackActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost) 
		: ActionEvent(name, description, actionmanager, range, cost) {}

	std::vector<CombatEvent> AttackActionEvent::doAction(Unit* sender, int x, int y) {

		std::vector<CombatEvent> events = actionmanager->damageUnit(sender, x, y, 5, range, cost);
		
		if (events[0].type != CombatEventType::NotValid) {
			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(cost);
			ce.setAPChange(cost);
			events.push_back(ce);
		}

		return events;
	}