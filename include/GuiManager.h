#pragma once
#include <TGUI/TGUI.hpp> 
#include <map>
#include <array>
#include "Unit.h"
#include "tile.h"
#include "CombatState.h"
#include "Game.h"

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
	void updateAPbar(int index, int value);
	void updateHPbar(int index, int value);
	void updatePlayerPositionMap(int oldX, int oldY, int newX, int newY);
	void initCombatGUI(CombatState& combat, std::vector<Unit*>& player, std::array<std::array<Tile, 30>, 30>& map);
	void returnToExploration(int amountOfPlayers);
private:
	tgui::Panel::Ptr explorationGUI = std::make_shared<tgui::Panel>();
	tgui::Panel::Ptr combatGUI = std::make_shared<tgui::Panel>();
	std::shared_ptr<tgui::Gui> gui = nullptr;
	sf::RenderWindow* window;
	tgui::Panel::Ptr mapContainer = std::make_shared<tgui::Panel>();
	sf::Font font;
	ActionMenuEnabled actionMenuEnabled = ActionMenuEnabled::none;
	Game* pGameObject = nullptr;
	std::vector<tgui::Widget::Ptr> explorationWidgets = std::vector<tgui::Widget::Ptr>();
	std::vector<tgui::Widget::Ptr> combatWidgets = std::vector<tgui::Widget::Ptr>();

	void setActionMenuEnabled(ActionMenuEnabled);
	void createPlayerProfile(int playerIndex, Unit* player, tgui::Panel::Ptr parent, bool combat = false);
	void createMap(std::array<std::array<Tile, 30>, 30> map, std::vector<PlayerCell> players, tgui::Panel::Ptr parent);
	void createProfilePicture(int playerIndex, Unit* player, sf::Vector2f profilePos, tgui::Panel::Ptr parent);
	void createBar(int playerIndex, int maxVal, int currVal, sf::Vector2f pos, std::string type, tgui::Panel::Ptr parent, bool combat = false);
	void createActionMenu(Unit* player, int playerIndex, tgui::Panel::Ptr parent);
	void addToCleanupCollection(tgui::Panel::Ptr parent, tgui::Widget::Ptr child);
};
