#include <sys/types.h>
#include "dirent.h"
#include "SoundManager.h"
#include <SFML/Audio.hpp>

SoundManager::SoundManager() {
	soundNames = std::vector<std::string>();
	soundTable = std::unordered_map<std::string, sf::SoundBuffer>();
	hasLoaded = false;
}


SoundManager::~SoundManager() {

}

void SoundManager::getFiles() {
	DIR *curDir;
	struct dirent * dirEntry;
	curDir = opendir("Sounds"); 
	  if (curDir) {
	    while ((dirEntry = readdir(curDir))) {
	    	std::string name(dirEntry->d_name);
	        if (name[0] != '.')
	        	soundNames.push_back(name);
	    }
		closedir(curDir);
	} else 
		std::cout << "WARNING: Could not get Directory \"Sounds\" " << std::endl;

	hasLoaded = true;
}

void SoundManager::createSounds() {
	sf::SoundBuffer buffer;
	
	for(std::vector<std::string>::iterator it = soundNames.begin(); it != soundNames.end(); ++it) {
    	std::string name = *it;
		buffer.loadFromFile("Sounds/" + name);

    	int dotPos = name.find_last_of('.');
    	name.erase(dotPos, name.length() - dotPos);

    	std::cout << "LOADING: " << name << std::endl;

    	soundTable.emplace(name, buffer);
	}
}