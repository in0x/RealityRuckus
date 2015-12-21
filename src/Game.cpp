#pragma once
#include "Game.h"
#include "ActionManager.h"
#include <TGUI/TGUI.hpp>

Game::Game() {

}

void Game::init() {

	state = GameState::loading;

	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Corgi Movement Simulator");
	
	pTexMng = new TextureManager();
	pTexMng->getFiles();
	pTexMng->createTextures();

	pLvlMng = new lvlManager();

	pSoundMng = new SoundManager();
	pSoundMng->getFiles();
	pSoundMng->createSounds();

	pUiMng = new UIManager();

	pUnitMng = new UnitManager(pLvlMng, pTexMng, pSoundMng);
	pLvlMng->genMap(pTexMng, pUnitMng);

	/*
	ActionEvent("Move", "Move to a specific position."),
	ActionEvent("Block", "Forfeit Combat this round to take a defensive stance. Increases your chance to block incoming damage."),
	ActionEvent("Attack (Long Sword)", "Attack an enemy with your Longsword. Has a range of 1 field."),
	ActionEvent("Half-sword", "Grip the blade of your sword and use the guard to attack. Deals less damage, but has a chance to stun your enemy.")
	*/

	pActionMng = new ActionManager(pUnitMng);


	p = new Player(pTexMng, sf::Sprite(), UnitType::player);
	std::vector<ActionEvent*> playerActions = { 
		new MoveActionEvent("Move", "Move to a specific position.",pActionMng),
		new AttackActionEvent("Shoot (Plasma Rifle)", "Use your plasma rifle to shoot your enemy. Has a chance to do extra burn damage on hit.",pActionMng),
		new ActionEvent("Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.",pActionMng),
		new ActionEvent("Pommel Strike", "Hit an enemy with the stock of your rifle. Does much less damage, but has a chance to stun on hit.",pActionMng)
	};

	p->learnedActions = playerActions;
	pUnitMng->setPlayer(p);

	pLvlMng->genDrawable(&terrain);
	pUiMng->setTerrain(terrain);
	
	pLoS = new OrthogonalLineOfSight();

	gameFont = new sf::Font();
	if (!gameFont->loadFromFile("Menlo-Regular.ttf"))
		std::cout << "could not load font" << std::endl;

	pGuiMng = new GUImanager(WINDOW_WIDTH, WINDOW_HEIGHT, &window, this);
	playerVector = { p, p, p};
	//playerVector = { p };
	pGuiMng->initExplorationGUI(playerVector, pLvlMng->map);

	cursorAttack.loadFromFile("cursor_attack.png");
	cursorRegular.loadFromFile("cursor_regular.png");

	sf::IntRect cursorRect = sf::IntRect(0, 0, 65, 65);
	cursor.setTexture(cursorRegular);
	cursor.setTextureRect(cursorRect);

	state = GameState::exploration;

	//addObserver(pUiMng);
}

Game::~Game() {
	delete pLvlMng;
	delete pUnitMng;
	delete pSoundMng;
	delete pTexMng;
	delete pUiMng;
	delete pLoS;
	delete p;
	delete gameFont;
	delete pGuiMng;
}

/* enum CombatEventType { None = 0, Move = 1, AP = 2, HP = 4 }; */
void Game::updateActionManager(ActionManagerInput inputEvent) {
	
	auto find = [](auto vec, auto x) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == x)
				return i;
		}
		return -1;
	};

	auto events = playerVector[inputEvent.playerIndex]->learnedActions[inputEvent.actionIndex]->doAction(playerVector[inputEvent.playerIndex], inputEvent.mouseX, inputEvent.mouseY);
	for (auto& e : events) {
		switch (e.type) {
			case 0:
				break;
			case 1:
				pGuiMng->updatePlayerPositionMap(e.oldX, e.oldY, e.newX, e.newY);
				break;
			case 2: 
				int index = find(playerVector, e.affected);
				if (index != -1)
				pGuiMng->updateAPbar(index, e.APChange);
				break;
			
			default:
				break;
		}
	}
}


void Game::getGUIevent(int playerIndex, int abilityIndex) {
	inputState = InputState::WaitingForActionInput;
	userActionInput = new ActionManagerInput(playerIndex, abilityIndex);
	cursor.setTexture(cursorAttack);
}

void Game::update() {
	int mousex, mousey = 0;

	std::vector<Unit*> visibleUnits;

	CombatState* currentCombat = nullptr;
	
	window.setMouseCursorVisible(false);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (inputState == InputState::normal) {
				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
					window.close();
				if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && event.type == sf::Event::MouseMoved)
				{
					mousex = sf::Mouse::getPosition(window).x;
					mousey = sf::Mouse::getPosition(window).y;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && event.type == sf::Event::MouseMoved)
				{
					pUiMng->setX(pUiMng->getX() + event.mouseMove.x - mousex);
					pUiMng->setY(pUiMng->getY() + event.mouseMove.y - mousey);
					mousex = event.mouseMove.x;
					mousey = event.mouseMove.y;
				}

				if (state == GameState::exploration) {

					if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)
					{
						int xPos = sf::Mouse::getPosition(window).x - pUiMng->getX();
						int yPos = sf::Mouse::getPosition(window).y - pUiMng->getY();
						int oldX = p->x;
						int oldY = p->y;
						if (pUnitMng->moveUnit(xPos, yPos, p))
						{
							pGuiMng->updatePlayerPositionMap(oldX, oldY, p->x, p->y);
							visibleUnits = pLoS->getVisibleUnits(pLvlMng, pUnitMng);
						}
					}
				}
			}
			else if (inputState == InputState::WaitingForActionInput) {

				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
					window.close();

				if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
				{
					int xPos = sf::Mouse::getPosition(window).x - pUiMng->getX();
					int yPos = sf::Mouse::getPosition(window).y - pUiMng->getY();
					//click pos are screen space coordinates
					updateActionManager(ActionManagerInput(userActionInput->playerIndex, userActionInput->actionIndex, xPos, yPos));
					delete userActionInput;
					userActionInput = nullptr;
					cursor.setTexture(cursorRegular);
					inputState = InputState::normal;
					currentCombat->listUnits();
				}
			}
		}

		pGuiMng->handleEvent(event);

		if (visibleUnits.size() > 1 && currentCombat == nullptr) {
			state = GameState::combat;
			//visibleUnits.push_back(p);
			currentCombat = new CombatState(visibleUnits);
			pActionMng->setCombatState(currentCombat);
			pGuiMng->initCombatGUI(*currentCombat, playerVector, pLvlMng->map);
		}

		
		if (state == GameState::combat && currentCombat->isDone()) {
			std::cout << "Combat done, returning to exploration" << std::endl;
			pGuiMng->returnToExploration(playerVector.size());
			delete currentCombat;
			currentCombat = nullptr;
			visibleUnits.clear();
			state = GameState::exploration;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			pUiMng->setY(pUiMng->getY() + 1);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			pUiMng->setY(pUiMng->getY() - 1);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			pUiMng->setX(pUiMng->getX() + 1);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			pUiMng->setX(pUiMng->getX() - 1);

		if (sf::Mouse::getPosition(window).x < 20)
			pUiMng->setX(pUiMng->getX() + 1);

		if (sf::Mouse::getPosition(window).x > 1900)
			pUiMng->setX(pUiMng->getX() - 1);

		if (sf::Mouse::getPosition(window).y < 20)
			pUiMng->setY(pUiMng->getY() + 1);

		if (sf::Mouse::getPosition(window).y > 1060)
			pUiMng->setY(pUiMng->getY() - 1);


		sf::Time frameTime = frameClock.restart();

		window.clear();
		cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
		pUiMng->setUnits(pUnitMng->getUnits());
		pUiMng->drawEverything(&window);
		pGuiMng->draw();
		window.draw(cursor);
		window.display();

	}
}

void Game::end() {

}
