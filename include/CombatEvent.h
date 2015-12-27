#pragma once
#include "Unit.h"

class Unit;

enum CombatEventType {
	NotValid = -1,
	None = 0,
	Move = 1,
	AP = 2,
	HP = 4
};

class CombatEvent {
public:
	Unit* affected;

	CombatEventType type;

	CombatEvent(Unit* sender, CombatEventType type, std::string msg = "") : affected(sender), type(type), message(msg) { }

	void setMove(int oldX, int oldY, int newX, int newY) {
		this->oldX = oldX;
		this->oldY = oldY;
		this->newX = newX;
		this->newY = newY;
		type = (CombatEventType) (type | Move);
	}

	void setAPChange(int APChange)
	{
		this->APChange = APChange;
		type = (CombatEventType)(type | AP);
	}

	void setHPChange(int HPChange)
	{
		this->HPChange = HPChange;
		type = (CombatEventType)(type | HP);
	}

	int oldX = 0;
	int oldY = 0;
	int newX = 0;
	int newY = 0;
	int APChange;
	int HPChange;

	std::string message;

};