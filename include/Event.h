#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

enum class EventType {
	movePlayer
};

class GameEvent {
public:
	GameEvent() {}
	GameEvent(sf::Event* e, sf::RenderWindow* w, EventType t) : pEvent(e), window(w), type(t) {}
	sf::Event* pEvent = nullptr;
	sf::RenderWindow* window = nullptr;
	EventType type;
};


class Observer {
public:
	virtual void update(GameEvent e) { throw std::exception("Not implementd"); }
};

class Observable {
public: 
	void addObserver(Observer*);
	void removeObserver(Observer*);
	void updateObservers(GameEvent e);
private:
	std::vector<Observer*> observers;
};
