#pragma once

#include "lvlManager.h"
#include <iostream>
#include <thread>         
#include <chrono>
#include "player.h"

lvlManager::lvlManager()
{
	srand(time(NULL));
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
	for (int j = x; j <  x+tiles.x; j++){
		for (int i = y; i < y+tiles.y; i++){
			map[j][i] = tiles.tiles[count];
			map[j][i].accessible = true;
			count++;
			//unitMng->spawnUnit(doge, x, y);
			if (rand() % 100 + 1 <= 5) {
				if(rand()%2)
					unitMng->spawnUnit(robotdude, j, i, map[j][i].accessible);
				else	
					unitMng->spawnUnit(robotfly, j, i, map[j][i].accessible);
				setOccupied(j, i, true);
			}
		}
	}
}

void lvlManager::genMap(TextureManager* texMng, UnitManager* unitMng) {
	Block b = generateTiles(buildKey(texMng), texMng);
	genTiles(b, 0, 15, unitMng);
	for (int i = 0; i < 50; i++) {
		Block b = generateTiles(buildKey(texMng), texMng);
		sf::Vector2<int> v = findNextSpot(b.x, b.y);
		if (v.x != -1)
			genTiles(b, v.x, v.y, unitMng);
	}
}

void lvlManager::genDrawable(sf::Sprite* output){
	sf::RenderTexture texture;
	texture.create(3840, 3840);
	for (int i = 0; i < 30; i++){
		for (int j = 0; j < 30; j++){
			sf::Sprite renderSprite = (map[i][j].sprite);
			renderSprite.setPosition(i * 128, j * 128);
			texture.draw(renderSprite);
			//CharTexture prüfen/renderen
			/*if (map[i][j].pawn != NULL){
				renderSprite.setTexture(*(map[i][j].pawn->sprite.getTexture()));
				texture.draw(renderSprite);
			}*/
			//Leere Tiles auf inaccessible setzten und blocked textur verwenden
			if (map[i][j].sprite.getTexture() == NULL){
				sf::Texture tex = sf::Texture();
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
				tex.loadFromFile(buffer);
				map[i][j].sprite.setTexture(tex);
				renderSprite.setTexture(tex);
				texture.draw(renderSprite);
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
    	std::this_thread::sleep_for (std::chrono::seconds(1));
    }
}

bool lvlManager::isAccessible(int x, int y){
	if (x < 0 || y < 0 || x > 3840 || y > 3840){
		return false;
	}
	return map[x / 128][y / 128].accessible;
}

sf::Vector2<int> lvlManager::findNextSpot(int w, int h){
	bool valid;
	int x, y;
	int tries = 0;
	do{
		if (tries > 50)
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
			}
		}
		if (outAccessible == 0 || outAccessible >= 2)
			valid = false;
		tries++;
	} while (!valid);
	sf::Vector2<int> ret = sf::Vector2<int>(x, y);
	return ret;
}
