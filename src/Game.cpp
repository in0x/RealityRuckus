#pragma once
#include "Game.h"
#include "ActionManager.h"
#include <TGUI/TGUI.hpp>
#include <thread>
#include <chrono>

/*
SUPER IMPORTANT, IF YOU SEE THIS AND NEGLECT IT AND IT CAUSES A BUG ILL SMACK YA
A UNITS ACTIONS NEED TO ALWAYS BE INSERTED INTO THE VECTOR IN A CERTAIN ORDER:

1. PHYSICAL ATTACK (I.E. SWORD ATTACK, RIFLE SHOT)
2. OFFENSIVE SPECIAL  -> THIS IS ALWAYS SOMETHING THATS USED ON THE ENEMY
3. DEFENSIVE SPECIAL -> THIS IS ALWAYS USED ON A FRIENDLY
4. MOVE

THE AI NEEDS THIS IN ORDER TO CORRECTLY EXECUTE ITS ACTIONS WITHOUT SPECIFICALLY KNOWING ABOUT THEM
*/

Game::Game() {

}

void Game::init() {

	state = GameState::loading;

	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "REALITY RUCKUS", sf::Style::Close);//sf::Style::Fullscreen);
	
	pTexMng = new TextureManager();
	pTexMng->getFiles();
	pTexMng->createTextures();

	pLvlMng = new lvlManager();

	pSoundMng = new SoundManager();
	pSoundMng->getFiles();
	pSoundMng->createSounds();

	pUiMng = new UIManager();
	pAnimationMng = new AnimationManager();
	pAnimationMng->registerAnimation("bullet", pTexMng->textureTable["bullet"]);
	pAnimationMng->registerAnimation("plasma", pTexMng->textureTable["plasma"]);
	pAnimationMng->registerAnimation("explosion", pTexMng->textureTable["explosion"]);
	pAnimationMng->registerAnimation("drain", pTexMng->textureTable["drain"]);
	pAnimationMng->registerAnimation("heal", pTexMng->textureTable["heal"]);
	pAnimationMng->registerAnimation("cleave", pTexMng->textureTable["cleave"]);
	pAnimationMng->registerAnimation("pommelstrike", pTexMng->textureTable["pommelstrike"]);
	pAnimationMng->registerAnimation("defensive", pTexMng->textureTable["defensive"]);

	pUnitMng = new UnitManager(pLvlMng, pTexMng, pSoundMng, &aiFactory);
	
	pActionMng = new ActionManager(pUnitMng, pAnimationMng);

	pUnitMng->pActionMng = pActionMng;

	spawnPlayer(PlayerType::armyfighter, 0, 15);
	spawnPlayer(PlayerType::armyweirdo, -100, -100);

	pLvlMng->genMap(pTexMng, pUnitMng);

	pLvlMng->genDrawable(&terrain);
	pUiMng->setTerrain(terrain);
	
	pLoS = new OrthogonalLineOfSight();

	gameFont = new sf::Font();
	if (!gameFont->loadFromFile("Menlo-Regular.ttf"))
		std::cout << "could not load font" << std::endl;

	pGuiMng = new GUImanager(WINDOW_WIDTH, WINDOW_HEIGHT, &window, this);
	
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

void Game::setActivePlayer(int index) {
	activeP = index;
}

// This function gets called by the GUImanager when the player has chosen an ability to execute
void Game::getGUIevent(int playerIndex, int abilityIndex) {
	inputState = InputState::WaitingForActionInput;
	userActionInput = new ActionManagerInput(playerIndex, abilityIndex);
	cursor.setTexture(cursorAttack);
}



void Game::spawnPlayer(PlayerType type, int x, int y) {

	if (pCharacters.size() == 3 ) return; //No more than 4 pcs
	std::vector<ActionEvent*> playerActions{};
	UnitAnimations* playerAnimations;

	/*std::function<Modifier(int)> func = [&](int ap) {
		std::cout << "generated with " << ap << "% chance." << std::endl;
		std::function<void(float&)> func = [&](float& num) {
			std::cout << "attacked, " << ap << "% chance." << std::endl;
			if (rand() % 100 + 1 < ap) {
				num = 0;
				std::cout << "MISSED, " << ap << "% chance." << std::endl;
			}
		};
		float dmg = 25;
		func(dmg);
		Modifier mod = { ModifierType::HPLoss, 1, func };
		return mod;
	};


	/*std::function<void(float&)> func = [&](float& num) {
		if (rand() % 100 + 1 < 33) {
			num = 0;
			std::cout << "MISSED LOL" << std::endl;
		}
	};
	Modifier mod = { ModifierType::HPLoss, 1, func };*/

	switch (type) {
	case PlayerType::armyfighter:
		playerActions = {
			new AttackActionEvent("Q. Shoot (Plasma Rifle)", "Use your plasma rifle to shoot your enemy. Has a chance to do extra burn damage on hit.\n\nRange: 3\tCost: 5\tDamage: 5",pActionMng, 3, 5, 5, "plasma"),
			new PommelStrikeEvent("W. Pommel Strike", "Hit an enemy with the stock of your rifle. Does much less damage, but has a chance to stun on hit.",pActionMng,1, 6, 3, 5, "pommelstrike"),
			//new ActionEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0),
			//new BuffEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0,mod,"defensive"),
			new TurnEndingBuffEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0,new TurnEndingBuff(),"defensive"),
			new MoveActionEvent("R. Move", "Move to a specific position.",pActionMng)
		};
		playerAnimations = new UnitAnimations(pTexMng->textureTable["coolarmymove"]);
		break;
	case PlayerType::armyweirdo:
		playerActions = {
			new AttackActionEvent("Q. Shoot (Plasma Rifle)", "Use your plasma rifle to shoot your enemy. Has a chance to do extra burn damage on hit.\n\nRange: 3\tCost: 5\tDamage: 5",pActionMng, 3, 5, 5, "plasma"),
			new PommelStrikeEvent("W. Pommel Strike", "Hit an enemy with the stock of your rifle. Does much less damage, but has a chance to stun on hit.",pActionMng,1, 6, 3, 5, "pommelstrike"),
			//new ActionEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0),
			//new BuffEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0,mod,"defensive"),
			new TurnEndingBuffEvent("E. Active Camo", "Activate your stealth camouflage. Enemies have an increased chance to miss until you move.", pActionMng, 0 ,0,new TurnEndingBuff(),"defensive"),
			new MoveActionEvent("R. Move", "Move to a specific position.",pActionMng)
		};
		playerAnimations = new UnitAnimations(pTexMng->textureTable["coolarmymove2"]);
	}


	pCharacters.push_back(std::make_shared<Player>(playerAnimations, UnitType::player, type, playerActions, 25, 25, x, y));

	pUnitMng->unitList.emplace(pUnitMng->unitList.begin(), pCharacters.back().get());

	if (x >= 0 && y >= 0 && x < 30 && y < 30)
		pLvlMng->setOccupied(x, y, true);

	pCharacters.back()->nickName = pUnitMng->getRandomName();

	playerVector.push_back(pCharacters.back().get());
}

std::vector<Unit*> Game::pathFindFuncPtr(Unit* unit) {
	return pLoS->getVisibleUnits(pLvlMng, pUnitMng, unit);
}

void Game::update() {
	int mousex, mousey = 0;

	std::vector<Unit*> visibleUnits;
	int oldX = pCharacters[activeP]->x;
	int oldY = pCharacters[activeP]->y;
	std::vector<Node> path;
	
	window.setMouseCursorVisible(false);

	auto isPlayer = [](auto vec, auto unit) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == unit)
				return true;
		}
		return false;
	};

	bool aiTurn = false;

	sf::Event event {};

	auto losFunc = [this](Unit* unit) {
		return this->pLoS->getVisibleUnits(this->pLvlMng, pUnitMng, unit);
	};


	// Main Game Loop
	while (window.isOpen())
	{
		if (!pUiMng->isAnimating())
		{
			aiTurn = false;

			// Selects whether player or AI makes next turn
			if (state == GameState::combat) {

				pGuiMng->updateCombatQueue(*currentCombat);
				currentCombat->updateListOfUnits();
				
				Unit* npc = currentCombat->getFirstUnit();
				
				//for (int i = 0; i < pCharacters.size(); i++) {
				//	currentCombat->addUnitsToCombat(pLoS->getVisibleUnits(pLvlMng, pUnitMng, pCharacters[i].get()));	//adds all visible enemies to the combat
				//}
				if (npc->currAP == 0)	//no one has any AP left, turn over
				{
					pGuiMng->handleCombatEvents(currentCombat->endTurn(),playerVector);
					continue;
				}
				if (playerVector.size() > 0) {	//checks and removes the players in playerVector since this is not done and the other functions cannot access it
					for (int i = 0; i < playerVector.size(); i++) {
						if (playerVector[i]->currHP <= 0)
							playerVector.erase(playerVector.begin() + i, playerVector.begin() + i + 1);
					}
				}

				if (npc->type != UnitType::player) { //!isPlayer(playerVector, npc)) {

					bool skip = false;
					auto events = npc->aiComponent->runAI(currentCombat, &pLvlMng->createGraph(), &pUnitMng->pathFinder, losFunc, npc);

					aiTurn = true;

					for (auto& ev : events) {
						if (ev.type == CombatEventType::Skip || ev.type == CombatEventType::NotValid) {
							currentCombat->skipTurn(npc);
							skip = true;
							break;
						}
					}

					if (skip)
						continue;

					/*if (events[0].type == CombatEventType::Skip) {
						currentCombat->skipTurn();
						continue;
					}*/

					pGuiMng->handleCombatEvents(events, playerVector);
					currentCombat->updateListOfUnits();
					//currentCombat->cycleUnitModifiers(); // A turn passed -> reduce duration of mods by 1
					//currentCombat->replenishUnitAP();
				}
			}

			while (window.pollEvent(event))
			{
				if (aiTurn) break;

				if (inputState == InputState::normal) {

					if (event.type == sf::Event::Closed)//|| sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
						window.close();

					if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
						if (currentCombat != nullptr) currentCombat->listUnits();


					if (state == GameState::combat) {

						if (event.type == sf::Event::KeyPressed) {
							int i = 0;

							if (event.key.code == sf::Keyboard::Q) i = 0;
							else if (event.key.code == sf::Keyboard::W) i = 1;
							else if (event.key.code == sf::Keyboard::E) i = 2;
							else if (event.key.code == sf::Keyboard::R) i = 3;
							else continue;

							getGUIevent(activeP, i);
						}
					}

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

							oldX = pCharacters[activeP]->x;
							oldY = pCharacters[activeP]->y;

							// Set lookForUnits true to discover units along the path 
							path = pUnitMng->moveUnit(xPos, yPos, pCharacters[activeP].get(), pLoS, true);

							if (path.size() != 0)
							{
								Unit* current = pCharacters[activeP].get();

								pAnimationMng->registerMovement(current->sprite, new DrawableUnit{ (float)current->x, (float)current->y, current->sprite->sprite }, path, 0.5);
								
								pGuiMng->updatePlayerPositionMap(oldX, oldY, pCharacters[activeP]->x, pCharacters[activeP]->y);
								
								visibleUnits = pLoS->getVisibleUnits(pLvlMng, pUnitMng, pCharacters[activeP].get());
							}
						}
					}
				}
				// This state is entered when the player has selected an ability. Lets the player choose a target location to use the ab. on
				else if (inputState == InputState::WaitingForActionInput) {

					if (event.type == sf::Event::Closed)
						window.close();

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
						delete userActionInput;
						userActionInput = nullptr;
						cursor.setTexture(cursorRegular);
						inputState = InputState::normal;
					}

					else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left)
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
							//auto gains = currentCombat->replenishUnitAP();
							//std::vector<CombatEvent> apEvents{};
							//for (auto gainEv : gains) {

							//	CombatEvent e{ std::get<1>(gainEv), CombatEventType::AP };

							//	// Since the ActionManager returns loses as positive values I implemented the GUI to asume updates as values
							//	// So for displaying gains we need to currently send the negative value (gui adds --2 for example)
							//	e.setAPChange(-std::get<0>(gainEv));

							//	apEvents.push_back(e);
							//}

							currentCombat->updateListOfUnits();
							//currentCombat->cycleUnitModifiers(); // A turn passed -> reduce duration of mods by 1

						//pGuiMng->handleCombatEvents(apEvents, playerVector, false);
						}
					}
				}

				pGuiMng->handleEvent(event);

				//One or more enemies are in range and no combat is happening right now -> enter new combat
				if (visibleUnits.size() > 1 && currentCombat == nullptr) {

					bool enemyAround = false;

					for (auto& u : visibleUnits) { // Have to check if only other player is around because the line of sight dies if i try to exclude players from it
						if (u->type != UnitType::player)
							enemyAround = true;
					}

					if (!enemyAround) continue;

					for (int i = 1; i < playerVector.size(); i++)
					{
						path.pop_back();
						bool p1nomove = false;
						playerVector[i]->x = oldX;//playerVector[0]->x;
						playerVector[i]->y = oldY;// playerVector[0]->y;
						sf::Vector2i newPos;
						if (path.size() > 0)
							newPos = { path.back().x,path.back().y };
						else
						{
							newPos = pLvlMng->findNextFreeTile(playerVector[0]->x, playerVector[0]->y);
							p1nomove = true;
						}
						std::vector<Node> path = pUnitMng->moveUnit(newPos.x, newPos.y, playerVector[i]);
						if (path.size() == 0)
						{
							pLvlMng->setOccupied(newPos.x, newPos.y, true);
						}
						if(p1nomove)
							pLvlMng->setOccupied(oldX, oldY, true);

						pAnimationMng->registerMovement(playerVector[i]->sprite, new DrawableUnit{ (float)playerVector[0]->x, (float)playerVector[0]->y, playerVector[i]->sprite->sprite }, path, 0.5);
						visibleUnits.push_back(playerVector[i]);
					}

					state = GameState::combat;
					currentCombat = new CombatState(visibleUnits);

					currentCombat->updateListOfUnits();

					pActionMng->setCombatState(currentCombat);
					pGuiMng->initCombatGUI(*currentCombat, playerVector, pLvlMng->map);

				}

				// Combat is over, go back to overworld exploration
				if (state == GameState::combat && currentCombat->isDone()) {
					for (int i = 1; i < playerVector.size(); i++)
					{
						//std::vector<Node> path = pUnitMng->moveUnit(playerVector[0]->x, playerVector[0]->y, playerVector[i]);
						//std::cout << path.size() << std::endl;
						//pAnimationMng->registerMovement(playerVector[i]->sprite, new DrawableUnit{ (float)playerVector[i]->x, (float)playerVector[i]->y, playerVector[i]->sprite->sprite }, path, 0.5);
						pGuiMng->handleCombatEvents(currentCombat->endTurn(), playerVector);
						pLvlMng->setOccupied(playerVector[i]->x, playerVector[i]->y, false);
						playerVector[i]->x = -100;
						playerVector[i]->y = -100;
					}

					pGuiMng->returnToExploration(playerVector.size());
					delete currentCombat;
					currentCombat = nullptr;
					visibleUnits.clear();
					state = GameState::exploration;

				}
			}
		}
		//this is so mouse camera controls still work while animations are going on
		else
		{
			while (window.pollEvent(event))
			{
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
			}
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
		
		pAnimationMng->update(frameTime);
		pUnitMng->update(frameTime);
		pUiMng->setUnits(pUnitMng->getUnits());
		pUiMng->setItems(pAnimationMng->getItems());
		pUiMng->setAnimations(pAnimationMng->getAnimations());
		pUiMng->setMovement(pAnimationMng->getMovement());
		pUiMng->drawEverything(&window);
		pGuiMng->draw();
		window.draw(cursor);
		window.display();
	}
}

void Game::end() {

}
