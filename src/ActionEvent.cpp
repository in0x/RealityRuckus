#pragma once
#include "CombatEvent.h"
#include "ActionManager.h"
#include "ActionEvent.h"



	ActionEvent::ActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage, std::string animation) 
		: name(name), 
		description(description), 
		actionmanager(actionmanager),
		range(range),
		cost(cost), 
		damage(damage),
		animation(animation){}

	std::vector<CombatEvent> ActionEvent::doAction(Unit* sender, int x, int y) { 
		/*std::vector<CombatEvent> events{};
		CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
		sender->loseAP(cost);
		ce.setAPChange(cost);
		events.push_back(ce);
		return events;*/
		actionmanager->currentCombat->updateListOfUnits();
		return{};
	}



	MoveActionEvent::MoveActionEvent(std::string name, std::string description, ActionManager* actionmanager) 
		: ActionEvent(name, description, actionmanager, 0, 0) {}
	
	std::vector<CombatEvent> MoveActionEvent::doAction(Unit* sender, int x, int y) {
		return actionmanager->moveUnit(sender, x, y);
	}
	


	AttackActionEvent::AttackActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage, std::string animation)
		: ActionEvent(name, description, actionmanager, range, cost, damage, animation) {}

	std::vector<CombatEvent> AttackActionEvent::doAction(Unit* sender, int x, int y) {

		// Passing around screen space coordinates is acutally so stupid and annoying
		if (x > 30)
			x /= 128;
		if (y > 30)
			y /= 128;

		name;

		std::vector<CombatEvent> events = actionmanager->damageUnit(sender, x , y, damage, range, cost);
		
		if (events[0].type != CombatEventType::NotValid) {
			if(range == 1)
				actionmanager->playAnimation(animation, sf::Vector2f(x, y), sf::Vector2f(x, y), 2.4);
			else
				actionmanager->playAnimation(animation, sf::Vector2f(sender->x, sender->y), sf::Vector2f(x, y), 0.2);
			
			/*float d_cost = cost;
			sender->applyModifiers(ModifierType::APLoss, d_cost);

			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(d_cost);
			ce.setAPChange(d_cost);
			events.push_back(ce);
			*/
		}

		ActionEvent::doAction(sender, x, y);

		return events;
	}

	APchangeActionEvent::APchangeActionEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float ap, std::string animation) 
		: ActionEvent(name, description, actionmanager, range, cost, ap, animation) {}
	

	std::vector<CombatEvent> APchangeActionEvent::doAction(Unit* sender, int x, int y) {

		if (x > 30)
			x /= 128;
		if (y > 30)
			y /= 128;

		std::vector<CombatEvent> events = actionmanager->changeAP(sender, x, y, damage, range, cost);

		if (events[0].type != CombatEventType::NotValid) {
			if (range == 1)
				actionmanager->playAnimation(animation, sf::Vector2f(x, y), sf::Vector2f(x, y), 2.4);
			else
				actionmanager->playAnimation(animation, sf::Vector2f(sender->x, sender->y), sf::Vector2f(x, y), 0.2);

			float d_cost = cost;
			sender->applyModifiers(ModifierType::APLoss, d_cost);
			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(d_cost);
			ce.setAPChange(d_cost);
			events.push_back(ce);
		}

		ActionEvent::doAction(sender,x,y);

		return events;
	}

	PommelStrikeEvent::PommelStrikeEvent(std::string name, std::string description, ActionManager* actionmanager, int range, int cost, float damage, float ap, std::string animation)
		: ActionEvent(name, description, actionmanager, range, cost, ap, animation) {
		this->ap = ap;
	}

	std::vector<CombatEvent> PommelStrikeEvent::doAction(Unit* sender, int x, int y) {

		if (x > 30)
			x /= 128;
		if (y > 30)
			y /= 128;

		float drain = ap;

		if (rand() % 100 + 1 < 17) {
			drain = 1000;
		}

		std::vector<CombatEvent> events = actionmanager->changeAP(sender, x, y, drain, range, cost);

		/*if (events[0].type != CombatEventType::NotValid) {

			float d_cost = cost;
			sender->applyModifiers(ModifierType::APLoss, d_cost);
			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(d_cost);
			ce.setAPChange(d_cost);
			events.push_back(ce);
		}*/

		auto d_events = actionmanager->damageUnit(sender, x, y, damage, range, cost);

		if (d_events[0].type != CombatEventType::NotValid) {

			if (range == 1)
				actionmanager->playAnimation(animation, sf::Vector2f(x, y), sf::Vector2f(x, y), 2.4);
			else
				actionmanager->playAnimation(animation, sf::Vector2f(sender->x, sender->y), sf::Vector2f(x, y), 0.2);
		}

		for (auto& ev : d_events)
			events.push_back(ev);

		ActionEvent::doAction(sender, x, y);

		return events;
	}

	BuffEvent::BuffEvent(std::string name, std::string description, ActionManager * actionmanager, int range, int cost, Modifier mod, std::string animation)
		: ActionEvent(name, description, actionmanager, range, cost,0, animation) {
		this->mod = mod;
	}

	std::vector<CombatEvent> BuffEvent::doAction(Unit * sender, int x, int y)
	{

		if (x > 30)
			x /= 128;
		if (y > 30)
			y /= 128;

		std::vector<CombatEvent> events = actionmanager->applyBuff(sender, x, y, mod, range, cost);

		if (events[0].type != CombatEventType::NotValid) {
			if (range <= 1)
				actionmanager->playAnimation(animation, sf::Vector2f(x, y), sf::Vector2f(x, y), 2.4);
			else
				actionmanager->playAnimation(animation, sf::Vector2f(sender->x, sender->y), sf::Vector2f(x, y), 0.2);

			/*float d_cost = cost;
			sender->applyModifiers(ModifierType::APLoss, d_cost);

			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(d_cost);
			ce.setAPChange(d_cost);
			events.push_back(ce);
			*/
		}

		ActionEvent::doAction(sender, x, y);

		return events;

	}

	TurnEndingBuffEvent::TurnEndingBuffEvent(std::string name, std::string description, ActionManager * actionmanager, int range, int cost, TurnEndingBuff* func, std::string animation)
		: ActionEvent(name, description, actionmanager, range, cost, 0, animation) {
		this->func = func;
	}

	TurnEndingBuffEvent::~TurnEndingBuffEvent()
	{
		delete func;
	}

	std::vector<CombatEvent> TurnEndingBuffEvent::doAction(Unit * sender, int x, int y)
	{
		if (x > 30)
			x /= 128;
		if (y > 30)
			y /= 128;

		int newCost = sender->currAP;

		std::vector<CombatEvent> events = actionmanager->applyBuff(sender, x, y, func->getModifier(newCost), range, newCost);

		if (events[0].type != CombatEventType::NotValid) {
			if (range <= 1)
				actionmanager->playAnimation(animation, sf::Vector2f(x, y), sf::Vector2f(x, y), 2.4);
			else
				actionmanager->playAnimation(animation, sf::Vector2f(sender->x, sender->y), sf::Vector2f(x, y), 0.2);

			/*float d_cost = cost;
			sender->applyModifiers(ModifierType::APLoss, d_cost);

			CombatEvent ce = CombatEvent(sender, CombatEventType::AP);
			sender->loseAP(d_cost);
			ce.setAPChange(d_cost);
			events.push_back(ce);
			*/
		}

		ActionEvent::doAction(sender, x, y);

		return events;
	}


	