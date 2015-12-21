// FinalFrontier.cpp : Defines the entry point for the console application.
//

#include "Event.h"

void Observable::addObserver(Observer* o) {
	observers.push_back(o);
}

void Observable::removeObserver(Observer* o) {

	std::vector<Observer*>::iterator it;

	for (it = observers.begin(); it != observers.end(); it++) {
		if (*it == o)
			break;
	}
		
	if (*it != o) {
		return;
	}

	observers.erase(it);
}

void Observable::updateObservers(GameEvent e) {
	for (auto observer : observers)
		observer->update(e);
}

 