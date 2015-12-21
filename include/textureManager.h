#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <sys/types.h>
#include "dirent.h"

class TextureManager {
public:
	void getFiles();
	TextureManager();
	~TextureManager();
	void createTextures();
	bool hasLoaded;
	std::vector<std::string> units;
	std::vector<std::string> floors;
	std::vector<std::string> walls;
	std::unordered_map<std::string, sf::Texture> textureTable;
};