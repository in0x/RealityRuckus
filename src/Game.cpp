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

	pUnitMng = new UnitManager(pLvlMng, pTexMng, pSoundMng, &aiFactory);
	
	pActionMng = new ActionManager(pUnitMng);

	p = new Player(pTexMng, sf::Sprite(), UnitType::player);

	/*
	
		SUPER IMPORTANT, IF YOU SEE AND THIS NEGLECT IT AND IT CAUSES A BUG ILL SMACK YA
		A UNITS ACTIONS NEED TO ALWAYS BE INSERTED INTO THE VECTOR IN A CERTAIN ORDER:

		1. PHYSICAL ATTACK (I.E. SWORD ATTACK, RIFLE SHOT)
		2. AGGRESSIVE SPECIAL  -> THIS IS ALWAYS SOMETHING THATS USED ON THE ENEMY
		3. DEFENSIVE SPECIAL -> THIS IS ALWAYS USED ON A FRIENDLY
		4. MOVE

		THE AI NEEDS THIS ORDER TO CORRECTLY EXECUTE ITS ACTIONS WITHOUT SPECIFICALLY KNOWING ABOUT THEM

	*/

	std::vector<ActionEvent*> playerActions = { 
		new AttackActionEvent("Shoot (Plasma Rifle)", "Use your plasma rifle to shoot your enemy. Has a chance to do extra burn damage on hit.\n\nRange: 3\tCost: 5\tDamage: 5",pActionMng, 3, 5),
		new ActionEvent("Pommel Strike", "Hit an enemy with the stock of your rifle. Does much less damage, but has a chance to stun on hit.",pActionMng,0,0),
		new ActionEvent("Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0),
		new MoveActionEvent("Move", "Move to a specific position.",pActionMng)
	};

	pUnitMng->actions = playerActions;

	pLvlMng->genMap(pTexMng, pUnitMng);

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
	// playerVector = { p };
	pGuiMng->initExplorationGUI(playerVector, pLvlMng->map);

	cursorAttack.loadFromFile("cursor_attack.png");
	cursorRegular.loadFromFile("cursor_regular.png");

	sf::IntRect cursorRect = sf::IntRect(0, 0, 65, 65);
	cursor.setTexture(cursorRegular);
	cursor.setTextureRect(cursorRect);

	state = GameState::exploration;

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

/* enum CombatEventType { None = 0, Move = 1, AP = 2, HP = 4 }; These are powers of 2 so we can later add them and transport multiple messagess at once */

// Gets called in main when the player has chosen a target location after selecting an ability 
std::tuple<bool, std::string> Game::updateActionManager(ActionManagerInput inputEvent) {
	
	auto events = playerVector[inputEvent.playerIndex]->learnedActions[inputEvent.actionIndex]->doAction(playerVector[inputEvent.playerIndex], inputEvent.mouseX, inputEvent.mouseY);
	
	currentCombat->updateListOfUnits();

	// TODO: return false if the player has made an invalid selection

	if (events[0].type == CombatEventType::NotValid)
		return std::tuple<bool, std::string>{false, events[0].message};

	pGuiMng->handleCombatEvents(events, playerVector);

	return std::tuple<bool, std::string>{true, ""};
}

// This function gets called by the GUImanager when the player has chosen an ability to execute
void Game::getGUIevent(int playerIndex, int abilityIndex) {
	inputState = InputState::WaitingForActionInput;
	userActionInput = new ActionManagerInput(playerIndex, abilityIndex);
	cursor.setTexture(cursorAttack);
}

void Game::update() {
	int mousex, mousey = 0;

	std::vector<Unit*> visibleUnits;
	
	window.setMouseCursorVisible(false);

	auto isPlayer = [](auto vec, auto unit) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == unit)
				return true;
		}
		return false;
	};

	// Main Game Loop
	while (window.isOpen())
	{
		bool aiTurn = false;

		// Selects whether player or AI makes next turn
		if (state == GameState::combat) {
			Unit* npc = currentCombat->getFirstUnit();

			if (!isPlayer(playerVector, npc)) {
				std::cout << "npc turn" << std::endl;
				npc->aiComponent->runAI(currentCombat, &pLvlMng->createGraph(), &pUnitMng->pathFinder, npc);
				aiTurn = true;
			}
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (aiTurn) break;

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

				//Move around in overworld
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
						//pGuiMng->displayMessage("Player moved", sf::Color{255,0,0,255});
					}
				}
			}
			// This state is entered when the player has selected an ability. Lets the player choose a target location to use the ab. on
			else if (inputState == InputState::WaitingForActionInput) {

				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
					window.close();

				if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
				{
					bool actionTookPlace = true;

					int xPos = sf::Mouse::getPosition(window).x - pUiMng->getX();
					int yPos = sf::Mouse::getPosition(window).y - pUiMng->getY();
					//click pos are screen space coordinates
					
					auto wasValidAction = updateActionManager(ActionManagerInput(userActionInput->playerIndex, userActionInput->actionIndex, xPos, yPos));
					
					// If this occurs the player either selected an Action that is not in range or costs more AP than he has
					// In this case we continue wait for valid input
					if (!std::get<0>(wasValidAction)) { 
						pGuiMng->displayMessage(std::get<1>(wasValidAction), sf::Color{ 255,0,0,255 });
						actionTookPlace = false;
					}

					delete userActionInput;
					userActionInput = nullptr;
					cursor.setTexture(cursorRegular);
					inputState = InputState::normal;

					//Update the GUI with player ap replenishments, without popups
					if (actionTookPlace) {
						auto gains = currentCombat->replenishUnitAP();
						std::vector<CombatEvent> apEvents{};
						for (auto gainEv : gains) {

							CombatEvent e{ std::get<1>(gainEv), CombatEventType::AP };

							// Since the ActionManager returns loses as positive values I implemented the GUI to asume updates as values
							// So for displaying gains we need to currently send the negative value (gui adds --2 for example)
							e.setAPChange(-std::get<0>(gainEv));

							apEvents.push_back(e);
						}

						pGuiMng->handleCombatEvents(apEvents, playerVector, false);
					}
				}
			}
		}

		pGuiMng->handleEvent(event);

		//One or more enemies are in range and no combat is happening right now -> enter new combat
		if (visibleUnits.size() > 1 && currentCombat == nullptr) {

			state = GameState::combat;
			currentCombat = new CombatState(visibleUnits);
			pActionMng->setCombatState(currentCombat);
			pGuiMng->initCombatGUI(*currentCombat, playerVector, pLvlMng->map);

		}

		// Combat is over, go back to overworld exploration
		if (state == GameState::combat && currentCombat->isDone()) {

			pGuiMng->returnToExploration(playerVector.size());
			delete currentCombat;
			currentCombat = nullptr;
			visibleUnits.clear();
			state = GameState::exploration;

		}

		//Camera movement
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

		if (state == GameState::combat) {
			pGuiMng->updateWindowOffset(pUiMng->getX(), pUiMng->getY(), currentCombat->unitsInCombat.size());
		}

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
