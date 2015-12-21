#pragma once
#include <SFML/Graphics.hpp>
#include "lvlManager.h"
#include "UIManager.h"
#include "UnitManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "player.h"
#include "GameState.h"
#include "OrthogonalLineOfSight.h"
#include "Event.h"
#include "GuiManager.h"
#include "ActionManagerInput.h"

class GUImanager; //forward declartion so game and gui can use each other

class Game {
public:
	Game();
	~Game();
	void init();
	void update();
	void end();
	void getGUIevent(int playerIndex, int abilityIndex);
	sf::Font* gameFont = nullptr;
private: 
	enum class InputState {normal, WaitingForActionInput};
	sf::RenderWindow window;
	lvlManager* pLvlMng = nullptr;
	UnitManager* pUnitMng = nullptr;
	SoundManager* pSoundMng = nullptr;
	TextureManager* pTexMng = nullptr;
	UIManager* pUiMng = nullptr;
	ActionManager* pActionMng = nullptr;
	sf::Clock frameClock;
	Player* p;
	sf::Sprite terrain;
	GameState state;
	OrthogonalLineOfSight* pLoS = nullptr;
	GUImanager* pGuiMng = nullptr;
	std::vector<Unit*>& playerVector = std::vector<Unit*>();
	InputState inputState = InputState::normal;
	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;
	ActionManagerInput* userActionInput = nullptr;
	sf::Texture cursorRegular;
	sf::Texture cursorAttack;
	sf::Sprite cursor;
	void updateActionManager(ActionManagerInput inputEvent);
};