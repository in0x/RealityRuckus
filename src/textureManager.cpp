#include "TextureManager.h"
#include <sys/types.h>
#include "dirent.h""
#include <vector>
#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

TextureManager::TextureManager() {
	walls = std::vector<std::string>();
	floors = walls = std::vector<std::string>();
	units = walls = std::vector<std::string>();
	textureTable = std::unordered_map<std::string, sf::Texture>();
	hasLoaded = false;
}

TextureManager::~TextureManager() {

}

void TextureManager::createTextures() {
	sf::Texture texture;

	for(std::vector<std::string>::iterator it = units.begin(); it != units.end(); ++it) {
    	std::string name = *it;
		texture.loadFromFile("Sprites/Units/" + name);
		//std::cout << "Sprites/Units/" << name << std::endl;

    	int dotPos = name.find_last_of('.');
    	name.erase(dotPos, name.length() - dotPos);

    	textureTable.emplace(name, texture);
	}

	for(std::vector<std::string>::iterator it = walls.begin(); it != walls.end(); ++it) {
    	std::string name = *it;
		texture.loadFromFile("Sprites/Walls/" + name);
		//std::cout << "Sprites/Walls/" << name << std::endl;

    	int dotPos = name.find_last_of('.');
    	name.erase(dotPos, name.length() - dotPos);

    	textureTable.emplace(name, texture);
	}
	hasLoaded = true;

	for(std::vector<std::string>::iterator it = floors.begin(); it != floors.end(); ++it) {
    	std::string name = *it;
			texture.loadFromFile("Sprites/Floors/" + name);
			//std::cout << "Sprites/Floors/" << name << std::endl;

			int dotPos = name.find_last_of('.');
			name.erase(dotPos, name.length() - dotPos);


			textureTable.emplace(name, texture);
	}
}

void TextureManager::getFiles() {
	DIR *curDir;
	struct dirent * dirEntry;
	curDir = opendir("Sprites/Units"); 
	  if (curDir) {
	    while ((dirEntry = readdir(curDir))) {
	    	std::string name(dirEntry->d_name);
	        if (name[0] != '.')
	        	units.push_back(name);
	    }
		closedir(curDir);
	} else 
		std::cout << "WARNING: Could not get Directory \"Sprites/Units\" " << std::endl;

	curDir = opendir("Sprites/Walls"); 
	  if (curDir) {
	    while ((dirEntry = readdir(curDir))) {
	    	std::string name(dirEntry->d_name);
	        if (name[0] != '.')
	        	walls.push_back(name);
	    }
		closedir(curDir);
	} else 
		std::cout << "WARNING: Could not get Directory \"Sprites/Walls\" " << std::endl;

	curDir = opendir("Sprites/Floors"); 
	  if (curDir) {
	    while ((dirEntry = readdir(curDir))) {
	    	std::string name(dirEntry->d_name);
	        if (name[0] != '.' && name.find("SciFi")!=-1)
	        	floors.push_back(name);
	    }
		closedir(curDir);
	} else 
		std::cout << "WARNING: Could not get Directory \"Sprite/Floors\" " << std::endl;
}
