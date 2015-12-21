#pragma once
#include "GuiManager.h"

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
	font.loadFromFile("Cabin-Medium.otf");
	this->window = window;
	pGameObject = gameObjectPtr;
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

	for (int i = 0; i < amountOfPlayers; i++) {
		std::shared_ptr<tgui::Widget> widget = explorationGUI->get("playerProfile" + std::to_string(i));
		tgui::Panel& profile = dynamic_cast<tgui::Panel&>((*widget));
		profile.setOpacity(0.3f);
	}

	explorationGUI->show();
}
 
void GUImanager::updateAPbar(int index, int value) {
	std::shared_ptr<tgui::Widget> widget = explorationGUI->get("apBar" + index);
	tgui::ProgressBar& apBar = dynamic_cast<tgui::ProgressBar&>((*widget));
	apBar.setValue(value);
}

void GUImanager::updateHPbar(int index, int value) {
	std::shared_ptr<tgui::Widget> widget = explorationGUI->get("hpBar" + index);
	tgui::ProgressBar& hpBar = dynamic_cast<tgui::ProgressBar&>((*widget));
	hpBar.setValue(value);
}

void GUImanager::updatePlayerPositionMap(int oldX, int oldY, int newX, int newY) {
		
	//Set old cell back to white
	std::shared_ptr<tgui::Widget> widget = gui->get("mapCell_" + std::to_string(oldX) + "_" + std::to_string(oldY));
	tgui::Panel& playerCell = dynamic_cast<tgui::Panel&>((*widget));
	playerCell.setBackgroundColor(sf::Color::White);
	
	// Set new cell to red
	auto newWidget = gui->get("mapCell_" + std::to_string(newX) + "_" + std::to_string(newY));
	auto newCell = dynamic_cast<tgui::Panel&>((*newWidget));
	newCell.setBackgroundColor(sf::Color::Red);
}

//On click of Player Info Tab this hides the current one shows the new one
void GUImanager::setActionMenuEnabled(ActionMenuEnabled menuToEnable) {
	if (menuToEnable == actionMenuEnabled)
		return;

	std::shared_ptr<tgui::Widget> widget = combatGUI->get("actionMenu" + std::to_string((int)actionMenuEnabled));
	tgui::Panel& actionMenu = dynamic_cast<tgui::Panel&>((*widget));
	actionMenu.hide();
	
	//key = "actionMenuButton" + playerIndex;
	std::shared_ptr<tgui::Widget> buttonWidget = combatGUI->get("actionMenuButton" + std::to_string((int)actionMenuEnabled));
	tgui::Button& buttonPressed = dynamic_cast<tgui::Button&>((*buttonWidget));
	buttonPressed.enable();
	buttonPressed.getRenderer()->setBackgroundColor(sf::Color::Transparent);

	std::shared_ptr<tgui::Widget> newWidget = combatGUI->get("actionMenu" + std::to_string((int)menuToEnable));
	tgui::Panel& newActionMenu = dynamic_cast<tgui::Panel&>((*newWidget));
	newActionMenu.show();
	
	std::shared_ptr<tgui::Widget> buttonWidgetToDisable = combatGUI->get("actionMenuButton" + std::to_string((int)menuToEnable));
	tgui::Button& buttonPressedToDisable = dynamic_cast<tgui::Button&>((*buttonWidgetToDisable));
	buttonPressedToDisable.disable();
	buttonPressedToDisable.getRenderer()->setBackgroundColor(sf::Color::White);

	for (int i = 0; i < 4; i++) {
		std::shared_ptr<tgui::Widget> actionButtonWidget = combatGUI->get("abilityButton" + std::to_string(i) + std::to_string((int)actionMenuEnabled));
		actionButtonWidget->hide();
		std::shared_ptr<tgui::Widget> actionButtonWidgetNew = combatGUI->get("abilityButton" + std::to_string(i) + std::to_string((int)menuToEnable));
		actionButtonWidgetNew->show();
	}

	actionMenuEnabled = menuToEnable;
}

//Creates the sprites used in the info tabs
void GUImanager::createProfilePicture(int playerIndex, Unit* player, sf::Vector2f profilePos, tgui::Panel::Ptr parent) {
	const sf::Texture* img = player->sprite.getTexture();
	
	tgui::Picture::Ptr playerProfilePicture = std::make_shared<tgui::Picture>(*img);
	playerProfilePicture->setPosition((profilePos.x - 5), profilePos.y + 20);
	playerProfilePicture->setSize(70, 70);
	playerProfilePicture->setOpacity(0.7f);

	std::string key = "playerProfilePicture" + playerIndex;
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
		bar->getRenderer()->setForegroundColor(sf::Color::Blue);
	else
		bar->getRenderer()->setForegroundColor(sf::Color::Green);

	std::string key = "";
	for (char c : type)
		key += std::tolower(c, std::locale());
	key += "Bar";
	parent->add(bar, key);

	tgui::TextBox::Ptr barText = std::make_shared<tgui::TextBox>();

	if (combat) {
		std::string boxString = std::to_string(currVal) + " / " + std::to_string(maxVal) + " " + type;

		barText->setText(boxString);
		barText->setTextSize(28);
		barText->getRenderer()->setTextColor(sf::Color::White);
		barText->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		barText->getRenderer()->setBorderColor(sf::Color::Transparent);
		barText->setSize(200, 50);
		barText->setPosition(bar->getPosition().x + bar->getSize().x + 15, pos.y - 5);
		barText->setFont(font);

		key = "";
		for (char c : type)
			key += std::tolower(c, std::locale());
		key += "barText" + std::to_string(playerIndex);
		parent->add(barText, key);
	}
	else {
		barText->setText(type);
		barText->setTextSize(21);
		barText->getRenderer()->setTextColor(sf::Color::White);
		barText->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		barText->getRenderer()->setBorderColor(sf::Color::Transparent);
		barText->setSize(200, 50);
		barText->setPosition(bar->getPosition().x + bar->getSize().x + 15, pos.y);
		barText->setFont(font);

		key = "";
		for (char c : type)
			key += std::tolower(c, std::locale());
		key += "barText" + std::to_string(playerIndex);
		parent->add(barText, key);
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
	//playerProfile->setPosition((windowSize.x / 8) * playerIndex, (windowSize.y - windowSize.y / 9));

	if (playerIndex % 2 == 0)
		playerProfile->setPosition((windowWidth)* (playerIndex / 2), windowSize.y - (windowHeight));
	else
		playerProfile->setPosition((windowWidth)* (playerIndex / 2), (windowSize.y - (2 * windowHeight)));

	playerProfile->setBackgroundColor(sf::Color::Black);
	playerProfile->setOpacity(0.3f);

	key = "playerProfile" + std::to_string(playerIndex); //wtf
	parent->add(playerProfile, key);
	std::cout << key << std::endl;
	addToCleanupCollection(parent, playerProfile);
	auto profilePos = playerProfile->getPosition();

	createProfilePicture(playerIndex, player, playerProfile->getPosition(), parent);
	createBar(playerIndex, player->maxAP, player->currAP, sf::Vector2f(profilePos.x + 60, profilePos.y + 30), "AP", parent, combat);
	createBar(playerIndex, player->maxHP, player->currHP, sf::Vector2f(profilePos.x + 60, profilePos.y + 70), "HP", parent, combat);

	if (combat) {
		tgui::Button::Ptr actionMenuButton = std::make_shared<tgui::Button>();
		actionMenuButton->setSize(playerProfile->getSize());
		actionMenuButton->setPosition(playerProfile->getPosition());
		actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::Transparent);
		actionMenuButton->getRenderer()->setBorderColor(sf::Color::Transparent);
		actionMenuButton->setOpacity(0.2f);

		if (playerIndex == 0)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player0);
		else if (playerIndex == 1)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player1);
		else if (playerIndex == 2)
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player2);
		else
			actionMenuButton->connect("clicked", &GUImanager::setActionMenuEnabled, this, ActionMenuEnabled::player3);

		actionMenuButton->connect("MouseEntered ",
			[](tgui::Button::Ptr button) -> void {button->getRenderer()->setBackgroundColor(sf::Color::White);}
		, actionMenuButton);

		actionMenuButton->connect("MouseLeft ",
			[](tgui::Button::Ptr button) -> void {button->getRenderer()->setBackgroundColor(sf::Color::Transparent);}
		, actionMenuButton);

		key = "actionMenuButton" + std::to_string(playerIndex);
		parent->add(actionMenuButton, key);
		addToCleanupCollection(parent, actionMenuButton);
		if (playerIndex == 0) {
			actionMenuButton->getRenderer()->setBackgroundColor(sf::Color::White);
			actionMenuButton->disable();
		}
	}//explorationGUI->hide(); //this works to hide all elements that are in the label);
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

void GUImanager::initCombatGUI(CombatState& combat, std::vector<Unit*>& players, std::array<std::array<Tile, 30>, 30>& map) {
	//explorationGUI->hideWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(800));
	explorationGUI->hide();
	//while (explorationGUI->isVisible());

	int index = 0;
	std::vector<PlayerCell> playerCells;
	for (auto player : players) {
		createPlayerProfile(index, player, combatGUI, true);
		playerCells.push_back(PlayerCell(player->x, player->y, index));
		createActionMenu(player, index, combatGUI);
		index++;
	}

	actionMenuEnabled = ActionMenuEnabled::player0;
	combatGUI->show();
}

//Creates the info window containing a players skills text size 28
void GUImanager::createActionMenu(Unit* player, int playerIndex, tgui::Panel::Ptr parent) {
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

		//abilityDescription->setPosition(buttonPos.x + buttonSize.x, buttonPos.y);
		abilityDescription->setPosition(buttonPos.x + buttonSize.x, menuPos.y);
		abilityDescription->setSize(buttonSize.x * 1.5, buttonSize.y * 2.5);
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