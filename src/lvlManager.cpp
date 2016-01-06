#pragma once

#include "lvlManager.h"
#include <iostream>
#include <thread>         
#include <chrono>
#include "player.h"

lvlManager::lvlManager()
{
	srand(time(NULL));
	//srand(251561);
	unitRenderTex.create(3840, 3840);
}

lvlManager::~lvlManager()
{
}

WeightedGraph lvlManager::createGraph() {
	WeightedGraph graph {30, 30, map};
	return graph;
}

void lvlManager::moveUnitInMap(int newX, int newY, Unit* unit) {
	setOccupied(unit->x, unit->y, false);
	setOccupied(newX, newY, true);
}

void lvlManager::setAccessTile(int x, int y, bool accessability) {
	bool prev = map[x][y].accessible;
	map[x][y].accessible = accessability;
}

void lvlManager::setOccupied(int x, int y, bool occupied) {
	map[x][y].occupied = occupied;
}

bool lvlManager::isOccupied(int x, int y) {
	/*if (x < 0 || y < 0 || x >= 30 || y >= 30) {
		return false;
	}*/
	return map[x][y].occupied;
}

std::string lvlManager::buildKey(TextureManager* texMng){
	std::string key;
	int length = texMng->floors.size();
	int pick = rand() % length;
	key = texMng->floors[pick];
	int dotPos = key.find_last_of('.');
	key.erase(dotPos, key.length() - dotPos);
	return key;
}

void lvlManager::genTiles(Block tiles, int x, int y, UnitManager* unitMng){
	int count = 0;
	bool room = !(tiles.x == 1 || tiles.y == 1);

	if(!room)
	for (int j = -1; j < tiles.x+1; j++) {
		for (int i = -1; i < tiles.y+1; i++) {
			if (!((x + j < 0 || x + j >= 30) || (y + i < 0 || y + i >= 30)))
					if ((tiles.x == 1 && j == 0 && (i == -1 || i == tiles.y)) || (tiles.y == 1 && i == 0 && (j == -1 || j == tiles.x)))
						placementmap[x + j][y + i] += 1;
					else
						if (!((j <= -1 || j >= tiles.x) && (i <= -1 || i >= tiles.y)))
							placementmap[x + j][y + i] += 2;
						else
							placementmap[x + j][y + i] += 3;
		}
	}
	else
	for (int j = -2; j < tiles.x + 2; j++) {
		for (int i = -2; i < tiles.y + 2; i++) {
			if (!((x + j < 0 || x + j >= 30) || (y + i < 0 || y + i >= 30)))
				if (!((j <= -1 || j >= tiles.x) && (i <= -1 || i >= tiles.y)))
					placementmap[x + j][y + i] += 2;
				else
					placementmap[x + j][y + i] += 3;
		}
	}
	for (int j = x; j <  x+tiles.x; j++){
		for (int i = y; i < y+tiles.y; i++){
			map[j][i] = tiles.tiles[count];
			map[j][i].accessible = true;
			count++;

			if (j > 3) {
				if (rand() % 100 + 1 <= 7) {

			/*if (j > 3) {
				if (rand() % 100 + 1 <= 5) { */
					if (rand() % 2)
						unitMng->spawnUnit(robotdude, j, i);
					else
						unitMng->spawnUnit(robotfly, j, i);
					setOccupied(j, i, true);
				}
			}
		}
	}
	//spawnUnits(unitMng);
}

void lvlManager::spawnUnits(UnitManager* unitMng) {
	/*srand(time(nullptr));


	struct SpawnInfo {
		int howMany;
		int width;
		int height;
	};

/*	for (auto& row : map) {
		
		for (auto& cell : row){
			
			
		}
	}*/

	for (int j = 0; j < 30; j++) {
		for (int i = 0; i < 30; i++) {
			if (j > 3 && map[j][i].accessible) {
				if (rand() % 100 + 1 <= 5) {
					if (rand() % 2)
						unitMng->spawnUnit(robotdude, j, i);
					else
						unitMng->spawnUnit(robotfly, j, i);
					setOccupied(j, i, true);
				}
			}
		}
	}
			
}

void lvlManager::genMap(TextureManager* texMng, UnitManager* unitMng) {
	std::cout << "started Mapgen" << std::endl;
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			placementmap[i][j] = -1;
		}
	}
	

	Block b = generateTiles(buildKey(texMng), texMng);
	genTiles(b, 0, 15, unitMng);
	for (int i = 0; i < 150; i++) {
		Block b = generateTiles(buildKey(texMng), texMng);
		sf::Vector2<int> v = findNextSpot(b.x, b.y);
		if (v.x != -1)
			genTiles(b, v.x, v.y, unitMng);
	}

	int tiles = 100;
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			if (map[i][j].accessible)
				tiles--;
		}
	}
	if (tiles > 0)	//less than [tiles] amount of tiles present
	{
		std::cout << "hey, ho, lets regenerate" << tiles << std::endl;
		map = std::array<std::array<Tile, 30>, 30>();
		genMap(texMng, unitMng);
	}
	else
	{
		std::cout << "spawning units" << std::endl;
		spawnUnits(unitMng);
	}
}


void lvlManager::genDrawable(TextureManager* texMng, sf::Sprite* output){
	std::cout << "started DrawGen" << std::endl;
	sf::RenderTexture texture;
	texture.create(3840, 3840);
	for (int i = 0; i < 30; i++){
		for (int j = 0; j < 30; j++){
			sf::Sprite renderSprite = (map[i][j].sprite);
			renderSprite.setPosition(i * 128, j * 128);
			
			//CharTexture prüfen/renderen
			/*if (map[i][j].pawn != NULL){
				renderSprite.setTexture(*(map[i][j].pawn->sprite.getTexture()));
				texture.draw(renderSprite);
			}*/
			//Leere Tiles auf inaccessible setzten und blocked textur verwenden
			if(map[i][j].sprite.getTexture() != nullptr)
				texture.draw(renderSprite);
			else{
				//sf::RenderTexture tex;
				//texture.create(128, 128);
				sf::Sprite temp = sf::Sprite();
				temp.setPosition(i * 128, j * 128);
				temp.setTexture(texMng->textureTable["SciFiWall_4"]);
				texture.draw(temp);

				{
					if (isAccessible(i - 1,j) && isAccessible(i, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_0_0"]);
					else if (isAccessible(i - 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_0_2"]);
					else if (isAccessible(i, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_0_3"]);
					else if (isAccessible(i - 1, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_0_1"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_1"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i + 1, j) && isAccessible(i, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_2_0"]);
					else if (isAccessible(i + 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_2_2"]);
					else if (isAccessible(i, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_2_3"]);
					else if (isAccessible(i + 1, j - 1))
						temp.setTexture(texMng->textureTable["SciFiWall_2_1"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i - 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_3"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i + 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_5"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i - 1, j) && isAccessible(i, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_6_0"]);
					else if (isAccessible(i - 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_6_2"]);
					else if (isAccessible(i, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_6_3"]);
					else if (isAccessible(i - 1, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_6_1"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_7"]);
					texture.draw(temp);
				}
				{
					if (isAccessible(i + 1, j) && isAccessible(i, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_8_0"]);
					else if (isAccessible(i + 1, j))
						temp.setTexture(texMng->textureTable["SciFiWall_8_2"]);
					else if (isAccessible(i, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_8_3"]);
					else if (isAccessible(i + 1, j + 1))
						temp.setTexture(texMng->textureTable["SciFiWall_8_1"]);
					texture.draw(temp);
				}
				//tex.display();
				/*
				int connections = 0;
				if (j == 0 || map[i][j - 1].accessible == false)//up
					connections += 8;
				if (i == 29 || map[i + 1][j].accessible == false)//right
					connections += 4;
				if (j == 29 || map[i][j + 1].accessible == false)//down
					connections += 2;
				if (i == 0 || map[i - 1][j].accessible == false)//left
					connections += 1;
				char buffer[30];
				strcpy(buffer, "Sprites/Walls/SciFi");
				char buffer2[5];
				if(connections != 15)
					sprintf(buffer2, "%d", connections);
				else
				{
					
					if(i != 29 && j != 29 && map[i + 1][j+1].accessible == true)
						sprintf(buffer2, "15_1");
					else
						if (i != 0 && j != 29 && map[i - 1][j+1].accessible == true)
							sprintf(buffer2, "15_2");
						else
							if (i != 29 && j != 0 && map[i + 1][j-1].accessible == true)
								sprintf(buffer2, "15_3");
							else
								if (i != 0 && j != 0 && map[i - 1][j-1].accessible == true)
									sprintf(buffer2, "15_4");
								else
									sprintf(buffer2, "15");
				}
				strcat(buffer, buffer2);
				strcat(buffer, ".png");
				tex.loadFromFile(buffer);*/
				//map[i][j].sprite.setTexture(tex.getTexture());
				//renderSprite.setTexture(tex.getTexture());
				//texture.draw(renderSprite);
			}
		}
	}
	/*for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			if (!(j == 29 || map[i][j + 1].accessible == false))//up
				map[i][j].accessible = true;
		}
	}*/
	tex = texture.getTexture();
	(*output).setTexture(tex);

	std::cout << "done" << std::endl;
}


//Only for rectangular blocks now
Block lvlManager::generateTiles(std::string name, TextureManager* texMng) {
	std::vector<Tile> tiles;
	sf::Texture block = texMng->textureTable.at(name);

	//Split string, get size, create amount of tiles with textures, set their subrects
	int x = 0;
	int y = 0;
	
	int first_ = name.find('_');
	x = std::stoi(name.substr(first_ + 1, 1)); // This is going to crash and burn if blocks are ever 
	y = std::stoi(name.substr(first_ + 3, 1)); // larger than 9
	
	sf::Vector2u blockSize = block.getSize();
	
	for (int x = blockSize.x; x > 0; x -= 128) {
		for (int y = blockSize.y; y > 0; y -= 128) {
			Tile t;
			sf::Sprite s;
			s.setTexture(texMng->textureTable.at(name));
			sf::Rect<int> rekt(x -128, y - 128, 128, 128);
			s.setTextureRect(rekt);
			t.sprite = s;

			std::vector<Tile>::iterator it = tiles.begin();
			tiles.insert(it,t);
		
		}
	}


	Block b = {tiles, x, y};
	return  b;
}

void lvlManager::testTileGen(TextureManager* texMng, sf::RenderWindow* window_ptr) {
	if (!texMng->hasLoaded) {
		std::cout << "textures not loaded" << std::endl;
		return;
	}

	Block b = generateTiles("test_1_3", texMng);
	std::vector<Tile> tiles = b.tiles;
	for(std::vector<Tile>::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    	it->sprite.getTextureRect();
    	window_ptr->clear();
    	window_ptr->draw(it->sprite);
    	window_ptr->display();
    }
}

bool lvlManager::isAccessible(int x, int y){
	if (x < 0 || y < 0 || x >= 30 || y >= 30){
		return false;
	}
	return map[x][y].accessible;
}

sf::Vector2<int> lvlManager::findNextSpot(int w, int h){
	bool valid;
	int x, y;
	bool room = !(w == 1 || h == 1);
	int tries = 0;
	do{
		if (tries > 10000)
			return sf::Vector2<int>(-1, -1);
		int outAccessible = 0;
		x = rand() % (28-w) +1;
		y = rand() % (28-h) +1;
		valid = true;
		for (int i = x - 1; i < x + w + 1; i++){
			for (int j = y - 1; j < y + h + 1; j++){
				if (!((i == x - 1 || i == x + w) && (j == y - 1 || j == y + h)))
				if ((i < x || i >= x + w || j < y || j >= y + h)){
					if (map[i][j].accessible == true){
						outAccessible++;
					}		
				}
				else{
					if (map[i][j].accessible == true)
						valid = false;
				}
				else
				{
					if (room && (placementmap[i][j] >= 1))
						valid = false;
					if (!room && placementmap[i][j] > 1)
						valid = false;
				}
			}
		}
		/*for (int j = 0; j < tiles.x + 1; j++) {
			for (int i = 0; i < tiles.y + 1; i++) {
			}
		}*/
		if (outAccessible == 0 || outAccessible >= 2)
			valid = false;
		tries++;
	} while (!valid);
	sf::Vector2<int> ret = sf::Vector2<int>(x, y);
	return ret;
}

sf::Vector2i lvlManager::findNextFreeTile(int x, int y)
{
	int leeway = 4;
	sf::Vector2i ret = { -1,-1 };
	float dist = 64;
	for (int fx = x-leeway; fx < x+leeway; fx++) {
		for (int fy = y-leeway; fy < y+leeway; fy++) {

			if (fx == x && fy == y)
				continue;

			if (fx > 0 && fy > 0 && fx < 30 && fy < 30) {
				if (map[fx][fy].accessible == true && map[fx][fy].occupied == false) {
					float newdist = sqrt(pow(x - fx, 2) + pow(y - fy, 2));
					if (newdist < dist)
					{
						dist = newdist;
						ret = { fx, fy };
						if (dist == 1)
							return ret;
					}
				}
			}
		}
	}
	return ret;
}