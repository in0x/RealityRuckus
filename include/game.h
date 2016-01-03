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
#include "AnimationManager.h"
#include "AIComponent.h"
#include <memory>

class GUImanager; //forward declartion so game and gui can use each other

class Game {
public:
	Game();
	~Game();

	void init();
	void update();
	void end();
	void getGUIevent(int playerIndex, int abilityIndex);
	void spawnPlayer(PlayerType type, int x, int y);
	void setActivePlayer(int index);

	sf::Font* gameFont = nullptr;

	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;
	
private: 
	enum class InputState {normal, WaitingForActionInput};

	int activeP = 0;

	sf::RenderWindow window;
	sf::Clock frameClock;
	sf::Sprite terrain;
	sf::Texture cursorRegular;
	sf::Texture cursorAttack;
	sf::Sprite cursor;

	lvlManager* pLvlMng = nullptr;
	UnitManager* pUnitMng = nullptr;
	SoundManager* pSoundMng = nullptr;
	TextureManager* pTexMng = nullptr;
	UIManager* pUiMng = nullptr;
	ActionManager* pActionMng = nullptr;
	OrthogonalLineOfSight* pLoS = nullptr;
	GUImanager* pGuiMng = nullptr;
	CombatState* currentCombat = nullptr;
	ActionManagerInput* userActionInput = nullptr;
	AnimationManager* pAnimationMng = nullptr;
	AIFactory aiFactory = {};

	GameState state;
	InputState inputState = InputState::normal;

	std::vector<Unit*>& playerVector = std::vector<Unit*>();
	
	std::tuple<bool, std::string> updateActionManager(ActionManagerInput inputEvent);
	
	std::vector<std::shared_ptr<Player>> pCharacters{};

};