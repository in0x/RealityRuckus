#pragma once
#include <TGUI/TGUI.hpp> 
#include <map>
#include <array>
#include "Unit.h"
#include "tile.h"
#include "CombatState.h"
#include "Game.h"
#include "CombatEvent.h"

struct PlayerCell {
public:
	PlayerCell(int x, int y, int index) : x(x), y(y), playerIndex(index) {}
	int x;
	int y;
	int playerIndex;
};

enum class ActionMenuEnabled {player0,player1,player2,player3,none};

class Game;

class GUImanager {
public:
	GUImanager();
	GUImanager(int screenWidth, int screenHeight, sf::RenderWindow* window, Game* gameObjectPtr);
	void draw();
	void handleEvent(sf::Event event);
	void initExplorationGUI(std::vector<Unit*> players, std::array<std::array<Tile, 30>, 30>& map);
	void initCombatGUI(CombatState& combat, std::vector<Unit*>& player, std::array<std::array<Tile, 30>, 30>& map);
	void returnToExploration(int amountOfPlayers);
	void handleCombatEvents(std::vector<CombatEvent>& combatEvents, std::vector<Unit*>& playerVector);
	void updatePlayerPositionMap(int oldX, int oldY, int newX, int newY);
	void updateWindowOffset(int x, int y, int unitsInCombat);
	void displayMessage(std::string message, sf::Color color);

private:
	tgui::Panel::Ptr explorationGUI = std::make_shared<tgui::Panel>();
	tgui::Panel::Ptr combatGUI = std::make_shared<tgui::Panel>();
	tgui::Panel::Ptr mapContainer = std::make_shared<tgui::Panel>();

	std::shared_ptr<tgui::Gui> gui = nullptr;
	sf::RenderWindow* window;
	sf::Font font;
	sf::Vector2f guiOffset{};

	ActionMenuEnabled actionMenuEnabled = ActionMenuEnabled::none;
	Game* pGameObject = nullptr;

	struct EnemyPanel {
		Unit* unit;
		tgui::Widget::Ptr trigger;
		tgui::Widget::Ptr infoLabel;
		sf::Vector2i originalPos;
	};

	std::vector<tgui::Widget::Ptr> explorationWidgets = std::vector<tgui::Widget::Ptr>();
	std::vector<tgui::Widget::Ptr> combatWidgets = std::vector<tgui::Widget::Ptr>();
	std::vector<EnemyPanel> enemyInfos = std::vector<EnemyPanel>();

	void setActionMenuEnabled(ActionMenuEnabled);
	void createPlayerProfile(int playerIndex, Unit* player, tgui::Panel::Ptr parent, bool combat = false);
	void createMap(std::array<std::array<Tile, 30>, 30> map, std::vector<PlayerCell> players, tgui::Panel::Ptr parent);
	void createProfilePicture(int playerIndex, Unit* player, sf::Vector2f profilePos, tgui::Panel::Ptr parent);
	void createBar(int playerIndex, int maxVal, int currVal, sf::Vector2f pos, std::string type, tgui::Panel::Ptr parent, bool combat = false);
	void createActionMenu(Unit* player, int playerIndex, tgui::Panel::Ptr parent, int offset);
	void addToCleanupCollection(tgui::Panel::Ptr parent, tgui::Widget::Ptr child);
	void updateAPbar(int index, int value, bool combat = false);
	void updateHPbar(int index, int value, bool combat = false);
	void displayUnitChangeText(CombatEvent& event);
	void createEnemyInfo(Unit* unit, int unitIndex);
	void updateEnemyPanelsPos(int amount);
};
