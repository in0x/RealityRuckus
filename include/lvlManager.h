#pragma once

#include "tile.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "textureManager.h"
#include "Tile.h"
#include <vector>
#include <string>
#include "block.h"
#include "Unit.h"
#include "drawableUnit.h"
#include <array>
#include "WeightedGraph.h"
#include "UnitManager.h"
class UnitManager;
class lvlManager
{
public:
	std::array<std::array<Tile, 30>, 30> map; 
	std::string buildKey(TextureManager*);

	sf::Sprite output;
	sf::Texture tex;
	sf::Texture unitTex;
	sf::Sprite unitRenderSprite;
	sf::RenderTexture unitRenderTex;
	sf::Vector2<int> findNextSpot(int, int);

	sf::Vector2i findNextFreeTile(int x, int y);

	lvlManager();
	~lvlManager();

	void genMap(TextureManager*, UnitManager* unitMng);
	void genTiles(Block, int, int, UnitManager* unitMng);
	void genDrawable(sf::Sprite*);
	void testTileGen(TextureManager*, sf::RenderWindow*);
	void setAccessTile(int x, int y, bool accessability);
	void setOccupied(int x, int y, bool occupied);
	void moveUnitInMap(int newX, int newY, Unit* unit);
	void spawnUnits(UnitManager* unitMng);

	bool isAccessible(int, int);
	bool isOccupied(int x, int y);

	Block generateTiles(std::string, TextureManager*);

	WeightedGraph createGraph();
};

