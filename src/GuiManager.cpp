#pragma once
#include "GuiManager.h"
#include <typeinfo>
#include <algorithm>


GUImanager::GUImanager(int screenWidth, int screenHeight, sf::RenderWindow* window, Game* gameObjectPtr) {
	explorationGUI->setSize(screenWidth, screenHeight);
	explorationGUI->setOpacity(1.f);
	explorationGUI->setBackgroundColor(sf::Color::Transparent);
	explorationGUI->setWidgetName(explorationGUI, "explorationGUI");

	combatGUI->setSize(screenWidth, screenHeight);
	combatGUI->setOpacity(1.f);
	combatGUI->setBackgroundColor(sf::Color::Transparent);
	combatGUI->hide();
	combatGUI->setWidgetName(combatGUI, "combatGUI");

	gui = std::make_shared<tgui::Gui>(*window);
	gui->add(explorationGUI);
	gui->add(combatGUI);
	gui->setFont(font);
	font.loadFromFile("Cabin-Medium.otf");
	this->window = window;
	pGameObject = gameObjectPtr;
}

bool GUImanager::enterGameOverScreen() {

	bool restartFlag = false;

	gui = std::make_shared<tgui::Gui>(*window);
	gui->setFont(font);

	sf::Texture bg;
	bg.loadFromImage(window->capture());

	sf::Sprite spr{ bg };

	auto screenWide = std::make_shared<tgui::Panel>();
	screenWide->setSize(pGameObject->WINDOW_WIDTH, pGameObject->WINDOW_HEIGHT);
	screenWide->setBackgroundColor(sf::Color{ 0,0,0,120 });
	screenWide->hide();


	auto title = std::make_shared<tgui::Label>();
	title->setText("You died.");
	title->setTextSize(40);
	title->getRenderer()->setTextColor(sf::Color::White);
	title->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	title->getRenderer()->setBorderColor(sf::Color::Transparent);
	auto dim = title->getSize();
	title->setPosition(pGameObject->WINDOW_WIDTH / 2 - (dim.x /2), pGameObject->WINDOW_HEIGHT / 2);
	title->hide();



	auto restart = std::make_shared<tgui::Button>();
	restart->setText("Restart");
	restart->setTextSize(50);

	restart->connect("clicked", [](Game* game, bool* flag) {*flag = true; game->end();}, pGameObject, &restartFlag);
	restart->connect("MouseEntered", [restart]() {restart->getRenderer()->setTextColor(sf::Color{ 255,255,255,255 });});
	restart->connect("MouseLeft", [restart]() {restart->getRenderer()->setTextColor(sf::Color{ 255,255,255,100 });});

	restart->getRenderer()->setTextColor(sf::Color{255,255,255,100});
	restart->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	restart->getRenderer()->setBorderColor(sf::Color::Transparent);

	dim = restart->getSize();
	restart->setPosition(pGameObject->WINDOW_WIDTH / 2 - dim.x - 30, pGameObject->WINDOW_HEIGHT / 2);
	restart->hide();

	auto end = std::make_shared<tgui::Button>();
	end->setText("End");
	end->setTextSize(50);
	end->getRenderer()->setTextColor(sf::Color{ 255,255,255,100 });
	end->getRenderer()->setBackgroundColor(sf::Color::Transparent);

	end->connect("clicked", [](Game* game, bool* flag) {*flag = false, game->end();}, pGameObject, &restartFlag);
	end->connect("MouseEntered", [end]() {end->getRenderer()->setTextColor(sf::Color{ 255,255,255,255 });});
	end->connect("MouseLeft", [end]() {end->getRenderer()->setTextColor(sf::Color{ 255,255,255,100 });});

	end->getRenderer()->setBorderColor(sf::Color::Transparent);

	dim = end->getSize();
	end->setPosition(pGameObject->WINDOW_WIDTH / 2 + dim.x + 30, pGameObject->WINDOW_HEIGHT / 2);
	end->hide();

	window->clear();

	gui->add(screenWide);
	gui->add(restart);
	gui->add(end);
	gui->add(title);

	screenWide->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(1000));
	restart->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(1000));
	end->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(1000));
	title->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(1000));

	while (true) {

		sf::Event ev;
		pGameObject->cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window)));

		while (window->pollEvent(ev)) {
			gui->handleEvent(ev);
			
			if (ev.type == sf::Event::MouseButtonReleased)
				return restartFlag;
		}

		window->clear();
		window->draw(spr);
		gui->draw();
		window->draw(pGameObject->cursor);
		window->display();

	}

	return restartFlag;
}

void GUImanager::addToCleanupCollection(tgui::Panel::Ptr parent, tgui::Widget::Ptr child) {
	std::string name;
	parent->getWidgetName(parent, name);
	if (name == "explorationGUI")
		explorationWidgets.push_back(child);
	else
		combatWidgets.push_back(child);
}

//let combat gui elements fall out of scope so they get deleted
void GUImanager::returnToExploration(int amountOfPlayers) {
	combatGUI->hide();

	combatGUI->removeAllWidgets();

	for (auto& widgetPtr : combatWidgets) {
		gui->remove(widgetPtr);
	}

	combatWidgets.clear();

	for (auto& enemyPanel : enemyInfos) {
		gui->remove(enemyPanel.trigger);
		gui->remove(enemyPanel.infoLabel);
	}

	enemyInfos.clear();

	for (int i = 0; i < amountOfPlayers; i++) {
		auto widget = explorationGUI->get("playerProfile" + std::to_string(i));
		auto profile = dynamic_cast<tgui::Panel&>((*widget));
		profile.setOpacity(0.3f);
	}

	actionMenuEnabled = ActionMenuEnabled::player0;
	pGameObject->setActivePlayer(0);

	explorationGUI->show();
}

void GUImanager::displayMessage(std::string message, sf::Color color = sf::Color{255,255,255,255}) {
	tgui::Label::Ptr mes = std::make_shared<tgui::Label>();
	
	mes->setFont(font);
	mes->setTextSize(40);
	mes->setText(message);
	mes->setPosition(0, 125);

	mes->getRenderer()->setBackgroundColor(sf::Color(0, 0, 0, 80));
	mes->getRenderer()->setTextColor(color);
	mes->getRenderer()->setBorderColor(sf::Color::Transparent);
	mes->getRenderer()->setPadding(tgui::Borders(30.f, 30.f));

	gui->add(mes);
	mes->show();
	mes->hideWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(4000));
}

void GUImanager::displayUnitChangeText(CombatEvent& event) {
	sf::Vector2f pos{ (float)event.affected->x * 128, (float)event.affected->y * 128 };
	std::string text = "";

	if (event.type == CombatEventType::AP) {
		text = std::to_string(-event.APChange) + " AP";
	}

	else if (event.type == CombatEventType::HP) {
		text = std::to_string(-event.HPChange) + " HP";
		pos.x += 64;
	}

	tgui::Label::Ptr notif = std::make_shared<tgui::Label>();
	if (text != "") {
		notif->setPosition(pos.x + guiOffset.x, pos.y + guiOffset.y);
		notif->setFont(font);
		notif->getRenderer()->setBorderColor(sf::Color::Transparent);
		notif->setTextSize(30);
		if(text[0]=='-')
			notif->getRenderer()->setTextColor(sf::Color::Red);
		else
			notif->getRenderer()->setTextColor(sf::Color::Green);
		notif->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		notif->setText(text);
		combatGUI->add(notif);
		notif->show();
		notif->hideWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(1000));
	}
}

void GUImanager::handleCombatEvents(std::vector<CombatEvent>& events, std::vector<Unit*>& playerVector, bool displayPopUps) {
	if (events.size() == 0) return;

	auto findPlayer = [](auto vec, auto unit) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == unit)
				return i;
		}
		return -1;
	};

	int index = 0;

	for (auto& e : events) {
		if (e.type == CombatEventType::None)
			continue;

		index = findPlayer(playerVector, e.affected);

		if (e.type == CombatEventType::UnitDied) {

			std::cout << "Unit died" << std::endl;

			if (e.affected->type == player) {
				removePlayerPanel(index);
				playerVector.erase(playerVector.begin() + index, playerVector.begin() + index + 1);
			}
			else {
				removeInfoPanel(e);
			}
		}

		if (index != -1) {
			if (e.type == CombatEventType::Move) {
				updatePlayerPositionMap(e.oldX, e.oldY, e.newX, e.newY);
			}

			else if (e.type == CombatEventType::AP) {
				updateAPbar(index, e.affected->currAP);
				updateAPbar(index, e.affected->currAP, true);
				lockActionMenu(playerVector);
			}

			else if (e.type == CombatEventType::HP) {
				updateHPbar(index, e.affected->currHP);
				updateHPbar(index, e.affected->currHP, true);
			}
		}
		else {
			if (e.type == CombatEventType::HP) {
				updateEnemyPanelInfo(e.affected);
			}
		}

		if (displayPopUps) displayUnitChangeText(e);
	}
}

void GUImanager::removePlayerPanel(int index) {


	//TODO: remove player profile here
	std::string i = std::to_string(index);

	std::vector<std::string> keys = { "playerProfile" , "actionMenuButton", "apBar", "hpBar", "playerProfilePicture", "apbarText", "hpbarText" };
	
	for (auto& key : keys) {
		auto panel = combatGUI->get(key + i);
		combatGUI->remove(panel);
	}

}

void GUImanager::removeInfoPanel(CombatEvent& e) {

	EnemyPanel* toDestroy = nullptr;

	for (auto& panel : enemyInfos) {
		
		if (panel.unit == e.affected) {
			toDestroy = &panel;
			break;
		}
	}

	if (toDestroy != nullptr) {
		gui->remove(toDestroy->infoLabel);
		gui->remove(toDestroy->trigger);

		auto it = enemyInfos.begin();

		for (it; it != enemyInfos.end(); ++it) {
			if (it->unit == toDestroy->unit)
				break;
		}

		enemyInfos.erase(it);
	}
}


void GUImanager::lockActionMenu(std::vector<Unit*>& playerVector)
{
	std::vector<int> indices;
	float maxRatio=0;
	for (int i = 0; i < playerVector.size(); i++)
	{
		float currRatio = playerVector[i]->currAP / playerVector[i]->maxAP;
		if (currRatio > maxRatio)
		{
			maxRatio = currRatio;
			indices = std::vector<int>();
			indices.push_back(i);
		}
		else if (currRatio == maxRatio)
			indices.push_back(i);
	}
	
	setActionMenuEnabled((ActionMenuEnabled)indices[0], combatGUI);
	for (int i = 0; i < playerVector.size(); i++)
	{
		std::string key = "actionMenuButton" + std::to_string(i);
		tgui::Button::Ptr actionMenuButton = combatGUI->get<tgui::Button>(key);

		if (actionMenuButton != nullptr) {
			if (std::find(indices.begin(), indices.end(), i) != indices.end()) {
				// Find the item
				actionMenuButton->enable();
				actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::Transparent);
			}
			else
			{
				actionMenuButton->disable();
				actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::White);
			}
		}
	}

	setActionMenuEnabled((ActionMenuEnabled)indices[0], combatGUI);

}


void GUImanager::updateEnemyPanelInfo(Unit* unit) {
	for (auto& panel : enemyInfos) {
		if (panel.unit == unit) {
			std::string t = unit->nickName + "\n" + unit->name + "\n";

			float health = unit->currHP / unit->maxHP;

			if (health >= 0.99)
				t += "In perfect shape.";
			else if (health >= 0.75)
				t += "Looks bruised.";
			else if (health >= 0.5)
				t += "Is visibly hurt";
			else if (health >= 0.15)
				t += "Heavy wounds are showing";
			else
				t += "Probably can't hold out much longer";

			dynamic_cast<tgui::Label*>(panel.infoLabel.get())->setText(t);
		}

	}
}

void GUImanager::updateAPbar(int index, int change, bool combat) {
	tgui::Widget::Ptr widget;
	tgui::Widget::Ptr text;

	if (!combat) {
		widget = explorationGUI->get("apBar" + std::to_string(index));
		text = explorationGUI->get("apbarText" + std::to_string(index));
	}

	else {
		widget = combatGUI->get("apBar" + std::to_string(index));
		text = combatGUI->get("apbarText" + std::to_string(index));
	}

	if (widget == nullptr)
		return;

	auto apBar = dynamic_cast<tgui::ProgressBar*>(widget.get());
	std::cout << "change" << change << std::endl;
	apBar->setValue(change);

	auto barText = dynamic_cast<tgui::Label*>(text.get());
	barText->setText(std::to_string(apBar->getValue()) + " / " + std::to_string(apBar->getMaximum()) + "AP");
}

void GUImanager::updateHPbar(int index, int change, bool combat) { 
	tgui::Widget::Ptr widget;
	tgui::Widget::Ptr text;

	if (!combat) {
		widget = explorationGUI->get("hpBar" + std::to_string(index));
		text = explorationGUI->get("hpbarText" + std::to_string(index));
	}

	else {
		widget = combatGUI->get("hpBar" + std::to_string(index));
		text = combatGUI->get("hpbarText" + std::to_string(index));
	}

	if (widget == nullptr)
		return;

	auto hpBar = dynamic_cast<tgui::ProgressBar*>(widget.get());
	hpBar->setValue(change);

	auto barText = dynamic_cast<tgui::Label*>(text.get());
	barText->setText(std::to_string(hpBar->getValue()) + " / " + std::to_string(hpBar->getMaximum()) + "HP");

}

void GUImanager::updatePlayerPositionMap(int oldX, int oldY, int newX, int newY) {

	//Set old cell back to white
	auto widget = gui->get("mapCell_" + std::to_string(oldX) + "_" + std::to_string(oldY));
	auto playerCell = dynamic_cast<tgui::Panel*>(widget.get());
	if (playerCell != nullptr) playerCell->setBackgroundColor(sf::Color::White);

	// Set new cell to red
	auto newWidget = gui->get("mapCell_" + std::to_string(newX) + "_" + std::to_string(newY));
	auto newCell = dynamic_cast<tgui::Panel*>(newWidget.get());
	if (newCell != nullptr) newCell->setBackgroundColor(sf::Color::Red);
}

//On click of Player Info Tab this hides the current one shows the new one
void GUImanager::setActionMenuEnabled(ActionMenuEnabled menuToEnable, tgui::Panel::Ptr parent) {
	if (menuToEnable == actionMenuEnabled) {}
		//	return;
		//std::cout << "does this happen often??" << std::endl;


	auto buttonWidget = parent->get("actionMenuButton" + std::to_string((int)actionMenuEnabled));
	auto buttonPressed = dynamic_cast<tgui::Button*>(buttonWidget.get());
	
	if (buttonPressed != nullptr) {
		buttonPressed->enable();
		buttonPressed->getRenderer()->setBackgroundColor(sf::Color::Transparent);
	}
	
	auto buttonWidgetToDisable = parent->get("actionMenuButton" + std::to_string((int)menuToEnable));
	auto buttonPressedToDisable = dynamic_cast<tgui::Button*>(buttonWidgetToDisable.get());
	
	if (buttonPressedToDisable != nullptr) {
		buttonPressedToDisable->disable();
		buttonPressedToDisable->getRenderer()->setBackgroundColor(sf::Color::Green);
	}
	
	if (parent == combatGUI) {
		auto widget = parent->get("actionMenu" + std::to_string((int)actionMenuEnabled));
		auto actionMenu = dynamic_cast<tgui::Panel*>(widget.get());
		actionMenu->hide();

		auto newWidget = parent->get("actionMenu" + std::to_string((int)menuToEnable));
		auto newActionMenu = dynamic_cast<tgui::Panel*>(newWidget.get());
		newActionMenu->show();

		for (int i = 0; i < 4; i++) {
			std::shared_ptr<tgui::Widget> actionButtonWidget = parent->get("abilityButton" + std::to_string(i) + std::to_string((int)actionMenuEnabled));
			actionButtonWidget->hide();
			std::shared_ptr<tgui::Widget> actionButtonWidgetNew = parent->get("abilityButton" + std::to_string(i) + std::to_string((int)menuToEnable));
			actionButtonWidgetNew->show();
		}
	}

	actionMenuEnabled = menuToEnable;
	pGameObject->setActivePlayer((int)menuToEnable);
}

void GUImanager::enableActionMenu(ActionMenuEnabled toEnable) {
	if (pGameObject->state == GameState::exploration)
		setActionMenuEnabled(toEnable, explorationGUI);
	else if (pGameObject->state == GameState::combat)
		setActionMenuEnabled(toEnable, combatGUI);
}

//Creates the sprites used in the info tabs
void GUImanager::createProfilePicture(int playerIndex, Unit* player, sf::Vector2f profilePos, tgui::Panel::Ptr parent) {
	const sf::Texture& img = player->sprite->getTexture();

	tgui::Picture::Ptr playerProfilePicture = std::make_shared<tgui::Picture>(img);
	playerProfilePicture->setPosition((profilePos.x - 5), profilePos.y + 20);
	playerProfilePicture->setSize(70, 70);
	playerProfilePicture->setOpacity(0.7f);

	std::string key = "playerProfilePicture" + std::to_string(playerIndex);
	parent->add(playerProfilePicture, key);
	addToCleanupCollection(parent, playerProfilePicture);
}

//creates HP & AP bars
void GUImanager::createBar(int playerIndex, int maxVal, int currVal, sf::Vector2f pos, std::string type, tgui::Panel::Ptr parent, bool combat) {

	tgui::ProgressBar::Ptr bar = std::make_shared<tgui::ProgressBar>();
	bar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);

	bar->setMaximum(maxVal);
	bar->setPosition(pos);
	while (bar->incrementValue() != currVal);
	bar->getRenderer()->setBorderColor(sf::Color::Transparent);
	bar->setOpacity(0.7f);
	bar->setSize(bar->getSize().x / 1.5, bar->getSize().y);

	if (type == "AP")
		bar->getRenderer()->setForegroundColor(sf::Color{ 75,188,246,255 });
	else
		bar->getRenderer()->setForegroundColor(sf::Color{ 152,228,102,255 });

	std::string key = "";
	for (char c : type)
		key += std::tolower(c, std::locale());
	key += "Bar" + std::to_string(playerIndex);
	parent->add(bar, key);

	tgui::Label::Ptr barText = std::make_shared<tgui::Label>();

	auto genBarText = [&](std::string boxString, int textSize, int xOffset, int yOffset) {
		barText->setText(boxString);
		barText->setTextSize(textSize);
		
		barText->getRenderer()->setTextColor(sf::Color::White);
		barText->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		barText->getRenderer()->setBorderColor(sf::Color::Transparent);
		
		barText->setSize(200, 50);
		barText->setPosition(bar->getPosition().x + bar->getSize().x + xOffset, pos.y + yOffset);
		barText->setFont(font);

		if (type == "AP")
			barText->setTextColor(sf::Color{ 75,188,246,255 });
		else
			barText->setTextColor(sf::Color{ 152,228,102,255 });

		key = "";
		for (char c : type)
			key += std::tolower(c, std::locale());
		key += "barText" + std::to_string(playerIndex);
		
		parent->add(barText, key);
	};

	if (combat) {
		std::string boxString = std::to_string(currVal) + " / " + std::to_string(maxVal) + " " + type;
		genBarText(boxString, 28, 15, -5);
	}
	else {
		genBarText(type, 21, 15, 0);
	}

	addToCleanupCollection(parent, bar);
	addToCleanupCollection(parent, barText);
}

//creates the info tab
void GUImanager::createPlayerProfile(int playerIndex, Unit* player, tgui::Panel::Ptr parent, bool combat) {
	auto windowSize = explorationGUI->getSize();
	std::string key = "";

	int windowWidth = combat == false ? (windowSize.x / 8) : (windowSize.x / 5.5);
	int windowHeight = combat == false ? (windowSize.y / 9) : (windowSize.y / 8.5);

	tgui::Panel::Ptr playerProfile = std::make_shared<tgui::Panel>();
	playerProfile->setSize(windowWidth, windowHeight);

	if (playerIndex % 2 == 0)
		playerProfile->setPosition((windowWidth)* (playerIndex / 2), windowSize.y - (windowHeight));
	else
		playerProfile->setPosition((windowWidth)* (playerIndex / 2), (windowSize.y - (2 * windowHeight)));

	playerProfile->setBackgroundColor(sf::Color::Black);
	playerProfile->setOpacity(0.6f);
	key = "playerProfile" + std::to_string(playerIndex);
	parent->add(playerProfile, key);
	addToCleanupCollection(parent, playerProfile);
	auto profilePos = playerProfile->getPosition();

	createProfilePicture(playerIndex, player, playerProfile->getPosition(), parent);
	createBar(playerIndex, player->maxAP, player->currAP, sf::Vector2f(profilePos.x + 60, profilePos.y + 30), "AP", parent, combat);
	createBar(playerIndex, player->maxHP, player->currHP, sf::Vector2f(profilePos.x + 60, profilePos.y + 70), "HP", parent, combat);

	//if (combat) {
		tgui::Button::Ptr actionMenuButton = std::make_shared<tgui::Button>();
		actionMenuButton->setSize(playerProfile->getSize());
		actionMenuButton->setPosition(playerProfile->getPosition());
		actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		actionMenuButton->getRenderer()->setBorderColor(sf::Color::Transparent);
		actionMenuButton->setOpacity(0.2f);

		if (playerIndex == 0)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player0, parent);
		else if (playerIndex == 1)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player1, parent);
		else if (playerIndex == 2)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player2, parent);
		else
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player3, parent);

		actionMenuButton->connect("MouseEntered ",
			[](tgui::Button::Ptr button) -> void {button->getRenderer()->setBackgroundColor(sf::Color::White);}
		, actionMenuButton);

		actionMenuButton->connect("MouseLeft ",
			[](tgui::Button::Ptr button) -> void {button->getRenderer()->setBackgroundColor(sf::Color::Transparent);}
		, actionMenuButton);

		key = "actionMenuButton" + std::to_string(playerIndex);
		parent->add(actionMenuButton, key);
		addToCleanupCollection(parent, actionMenuButton);
		if (!combat || playerIndex == 0) {
			actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::White);
			actionMenuButton->disable();
			actionMenuEnabled = ActionMenuEnabled::player0;
		}

	//}
}

void GUImanager::createMap(std::array<std::array<Tile, 30>, 30> map, std::vector<PlayerCell> players, tgui::Panel::Ptr parent) {
	tgui::Panel::Ptr mapPanel = std::make_shared<tgui::Panel>();
	mapPanel->setSize(310, 310);
	mapPanel->setPosition(explorationGUI->getSize().x - 310, explorationGUI->getSize().y - 310);
	mapPanel->setBackgroundColor(sf::Color::Green);
	mapPanel->setOpacity(0.4f);
	mapContainer->setSize(310, 310);
	mapContainer->setPosition(explorationGUI->getSize().x - 310, explorationGUI->getSize().y - 310);

	auto containerPos = mapContainer->getPosition();
	std::string key = "";

	for (int y = 0; y < 30; y++) {
		for (int x = 0; x < map[y].size(); x++) {
			if (map[x][y].accessible) {
				tgui::Panel::Ptr mapCell = std::make_shared<tgui::Panel>();

				mapCell->setPosition(containerPos.x + 10 + 10 * x, containerPos.y + 10 + 10 * y);
				mapCell->setSize(10, 10);
				mapCell->setBackgroundColor(sf::Color::White);

				if (map[x][y].occupied)
					mapCell->setBackgroundColor(sf::Color::Blue);

				mapCell->setOpacity(0.4f);

				key = "mapCell_" + std::to_string(x) + "_" + std::to_string(y);

				for (auto player : players) {
					if (x == player.x && y == player.y) {
						mapCell->setBackgroundColor(sf::Color::Red);
					}
				}
				gui->add(mapCell, key);
			}
		}
	}
}

void GUImanager::initExplorationGUI(std::vector<Unit*> players, std::array<std::array<Tile, 30>, 30>& map) {
	
	int index = 0;
	
	std::vector<PlayerCell> playerCells;

	for (auto player : players) {
		createPlayerProfile(index, player, explorationGUI);
		playerCells.push_back(PlayerCell(player->x, player->y, index));
		index++;
	}

	createMap(map, playerCells, explorationGUI);
}

//Creates the info window containing a players skills text size 28
void GUImanager::createActionMenu(Unit* player, int playerIndex, tgui::Panel::Ptr parent, int offset) {
	auto windowSize = explorationGUI->getSize();
	std::string key = "";

	tgui::Panel::Ptr playerProfile = std::make_shared<tgui::Panel>();
	int panelWidth = (windowSize.x / 5.5);
	int panelHeight = 2 * (windowSize.y / 8.5);

	playerProfile->setSize(panelWidth, panelHeight);
	playerProfile->setPosition(2 * panelWidth, parent->getSize().y - panelHeight);

	playerProfile->setBackgroundColor(sf::Color::Transparent);
	playerProfile->setOpacity(1.0f);

	key = "actionMenu" + std::to_string(playerIndex);
	parent->add(playerProfile, key);
	addToCleanupCollection(parent, playerProfile);
	playerProfile->hide();

	auto menuPos = playerProfile->getPosition();
	int sizeX = playerProfile->getSize().x;
	int sizeY = playerProfile->getSize().y / player->learnedActions.size();

	for (int i = 0; i < player->learnedActions.size(); i++) {

		std::string name = player->learnedActions[i]->name;
		std::string description = player->learnedActions[i]->description;

		tgui::Button::Ptr abilityButton = std::make_shared<tgui::Button>();

		abilityButton->getRenderer()->setBackgroundColor(sf::Color(0, 0, 0, 80));

		auto buttonFunc = [&](int pIndex, int abilityIndex) {
			pGameObject->getGUIevent(pIndex, abilityIndex);
		};

		abilityButton->setText(name);
		abilityButton->setPosition(menuPos.x, menuPos.y + (sizeY*i));
		abilityButton->setSize(sizeX, sizeY);
		abilityButton->setFont(font);
		abilityButton->setTextSize(28);
		abilityButton->getRenderer()->setTextColor(sf::Color(255, 255, 255, 255));
		abilityButton->getRenderer()->setBorderColor(sf::Color::Transparent);

		abilityButton->connect("clicked", buttonFunc, playerIndex, i);

		key = "abilityButton" + std::to_string(i) + std::to_string(playerIndex);
		parent->add(abilityButton, key);
		addToCleanupCollection(parent, abilityButton);

		auto buttonPos = abilityButton->getPosition();
		auto buttonSize = abilityButton->getSize();

		tgui::Label::Ptr abilityDescription = std::make_shared<tgui::Label>();

		abilityDescription->setText(description);
		abilityDescription->setFont(font);
		abilityDescription->setTextSize(28);

		abilityDescription->setPosition(buttonPos.x + buttonSize.x, menuPos.y);
		//abilityDescription->setSize(buttonSize.x * 1.5, buttonSize.y * 2.5);
		abilityDescription->setSize(buttonSize.x * 1.5, buttonSize.y*4);
		abilityDescription->getRenderer()->setBackgroundColor(sf::Color(0, 0, 0, 80));
		abilityDescription->getRenderer()->setTextColor(sf::Color::White);
		abilityDescription->getRenderer()->setBorderColor(sf::Color::Transparent);
		abilityDescription->getRenderer()->setPadding(tgui::Borders(30.f, 30.f));

		abilityButton->connect("MouseEntered", [](tgui::Button::Ptr button, tgui::Label::Ptr description) {
			button->getRenderer()->setBackgroundColor(sf::Color(255, 255, 255, 60));
			description->show();
		}, abilityButton, abilityDescription);

		abilityButton->connect("MouseLeft", [](tgui::Button::Ptr button, tgui::Label::Ptr descritption) {
			button->getRenderer()->setBackgroundColor(sf::Color(0, 0, 0, 80));
			descritption->hide();
		}, abilityButton, abilityDescription);


		parent->add(abilityDescription);
		addToCleanupCollection(parent, abilityDescription);
		abilityDescription->hide();

		if (playerIndex != 0)
			abilityButton->hide();
	}

	if (playerIndex != 0)
		playerProfile->hide();
}

void GUImanager::draw() {
	gui->draw();

}

void GUImanager::handleEvent(sf::Event event) {
	gui->handleEvent(event);
}

void GUImanager::initCombatGUI(CombatState& combat, std::vector<Unit*>& players, std::array<std::array<Tile, 30>, 30>& map) {
	explorationGUI->hide();
	
	int index = 0;
	std::vector<PlayerCell> playerCells;
	for (auto& player : players) {
		createPlayerProfile(index, player, combatGUI, true);
		playerCells.push_back(PlayerCell(player->x, player->y, index));
		createActionMenu(player, index, combatGUI, (int)((float)players.size() / 2 + 1));
		index++;
	}

	index = 0;
	for (auto& unit : combat.unitsInCombat) {
		if (unit->type != UnitType::player) createEnemyInfo(unit, index);
		index++;
	}

	createCombatQueue(combat);


	actionMenuEnabled = ActionMenuEnabled::player0;
	pGameObject->setActivePlayer(0);
	combatGUI->show();
}

void GUImanager::createCombatQueue(CombatState& state) {

	tgui::Panel::Ptr queueBar = std::make_shared<tgui::Panel>();

	queueBar->setPosition(0, 0);
	queueBar->setSize(pGameObject->WINDOW_WIDTH, 125);

	queueBar->setBackgroundColor(sf::Color(0, 0, 0, 80));

	combatGUI->add(queueBar, "queueBar");

	tgui::Label::Ptr title = std::make_shared<tgui::Label>();

	title->setFont(font);
	title->setTextSize(28);
	title->getRenderer()->setBorders(1);
	title->setTextColor(sf::Color::White);

	title->getRenderer()->setBorderColor(sf::Color::Transparent);
	title->setText("Combat\nOrder");
	title->setPosition(5, 30);

	int offset = title->getSize().x + 30;
	qOffset = offset;

	combatGUI->add(title);

	float panelLength = ((float)pGameObject->WINDOW_WIDTH - offset) / state.unitsInCombat.size();
	int i = 0;
	
	for (auto& unit : state.unitsInCombat) {

		tgui::Panel::Ptr profile = std::make_shared<tgui::Panel>();
		
		profile->setSize(panelLength, 125);
		profile->setPosition(i*panelLength + offset, 0);
		profile->setBackgroundColor(sf::Color{0,0,0,0});

		combatGUI->add(profile, "queueProfile" + std::to_string(i));

		const sf::Texture& img = unit->sprite->getTexture();

		tgui::Picture::Ptr picture = std::make_shared<tgui::Picture>(img);

		picture->setPosition((i*panelLength + offset + 5), 15);
		picture->setSize(70, 70);
		picture->setOpacity(0.7f);

		std::string key = "queuePicture" + std::to_string(i);
		combatGUI->add(picture, key);
		addToCleanupCollection(combatGUI, picture);	

		tgui::Label::Ptr info = std::make_shared<tgui::Label>();

		info->setFont(font);
		info->setTextSize(28);
		info->getRenderer()->setBorders(1);
		info->getRenderer()->setPadding(tgui::Borders(3.f, 3.f)); //infoLabel->getRenderer()->setPadding(tgui::Borders(15.f, 15.f));
		info->setTextColor(sf::Color::White);
		info->getRenderer()->setBorderColor(sf::Color::Transparent);
		
		auto pos = picture->getPosition();
		info->setPosition(pos.x + 80, pos.y+15);

		std::string text = std::to_string(i+1) + ". " + unit->nickName + "\n" + unit->name;
		info->setText(text);
		combatGUI->add(info, "queueInfo" + std::to_string(i));

		qPanels.push_back({ unit, profile, picture, info });

		for (auto& ip : enemyInfos) {
			if (ip.unit == unit) {

				info->connect("MouseEntered", [](tgui::Widget::Ptr infoLabel) {
					std::cout << "show" << std::endl;
					infoLabel->show();
				}, ip.infoLabel);

				info->connect("MouseLeft", [](tgui::Widget::Ptr infoLabel) {
					std::cout << "hide" << std::endl;
					infoLabel->hide();
				}, ip.infoLabel);
			}
		}

		i++;
	}
	
}

void GUImanager::updateCombatQueue(CombatState& state) {

	int i = 0;
	float panelLength = ((float)pGameObject->WINDOW_WIDTH - qOffset) / state.unitsInCombat.size();

	for (auto& widget : qPanels) {

		bool found = false;
		int j = 0;
		for (auto& unit : state.unitsInCombat) {

			if (unit == widget.unit) {
				widget.profile->setPosition(j*panelLength + qOffset, 0);

				widget.picture->setPosition((j*panelLength + qOffset + 5), 15);

				auto pos = widget.picture->getPosition();
				widget.info->setPosition(pos.x + 80, pos.y + 15);
				tgui::Label* info = dynamic_cast<tgui::Label*>(widget.info.get());
				info->setText(std::to_string(j+1)+info->getText().substring(info->getText().find(".")));
				found = true;
				continue;
			}
			j++;
		}

		if (!found) {
			combatGUI->remove(widget.picture);
			combatGUI->remove(widget.info);
			combatGUI->remove(widget.profile);
			widget.picture = nullptr;
			widget.profile = nullptr;
			widget.info = nullptr;
		}

		i++;
	}
}


void GUImanager::createEnemyInfo(Unit* unit, int unitIndex) {
	
	tgui::Label::Ptr trigger = std::make_shared<tgui::Label>();
	tgui::Label::Ptr infoLabel = std::make_shared<tgui::Label>();

	//trigger->setOpacity(0.f);
	trigger->setPosition(unit->x * 128 + guiOffset.x, unit->y * 128 + guiOffset.y);
	trigger->setSize(128, 128);

	infoLabel->setFont(font);
	infoLabel->setTextSize(24);
	infoLabel->getRenderer()->setBorders(1);
	
	std::string t = unit->nickName + "\n" + unit->name + "\n";

	float health = unit->currHP / unit->maxHP;

	if (health >= 0.99)
		t += "In perfect shape.";
	else if (health >= 0.75)
		t += "Looks bruised.";
	else if (health >= 0.5)
		t += "Is visibly hurt";
	else if (health >= 0.15)
		t += "Heavy wounds are showing";
	else
		t += "Probably can't hold out much longer";

	infoLabel->setText(t);

	infoLabel->setPosition(unit->x * 128 + guiOffset.x + 128, unit->y * 128 + guiOffset.y);
	infoLabel->hide();

	infoLabel->getRenderer()->setBackgroundColor(sf::Color(0, 0, 0, 80));
	infoLabel->getRenderer()->setTextColor(sf::Color::White);
	infoLabel->getRenderer()->setBorderColor(sf::Color::Transparent);
	infoLabel->getRenderer()->setPadding(tgui::Borders(15.f, 15.f));


	trigger->connect("MouseEntered", [infoLabel]() {
		//infoLabel->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(300));
		infoLabel->show();
	});

	trigger->connect("MouseLeft", [infoLabel]() {
		//infoLabel->hideWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(300));
		infoLabel->hide();
	});

	combatGUI->add(trigger);
	combatGUI->add(infoLabel);

	enemyInfos.push_back(EnemyPanel{unit, trigger, infoLabel, sf::Vector2i{ unit->x * 128, unit->y * 128 } });
}

void GUImanager::updateEnemyPanelsPos(int amount) {

	for (auto& em : enemyInfos) {
		/*
		em.trigger->setPosition(em.originalPos.x + guiOffset.x , em.originalPos.y + guiOffset.y);
		em.infoLabel->setPosition(em.originalPos.x + guiOffset.x + 128, em.originalPos.y + guiOffset.y);
		*/
		em.trigger->setPosition(em.unit->x * 128 + guiOffset.x, em.unit->y * 128 + guiOffset.y);
		em.infoLabel->setPosition(em.unit->x * 128 + guiOffset.x + 128, em.unit->y * 128 + guiOffset.y);

	}

}

void GUImanager::updateWindowOffset(int x, int y, int unitsInCombat) {
	guiOffset = sf::Vector2f{ (float)x, (float)y };
	updateEnemyPanelsPos(unitsInCombat);
}

