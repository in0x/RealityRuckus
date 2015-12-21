#pragma once
#include <unordered_map>
#include <vector>
#include <SFML/audio.hpp>
#include <iostream>

class SoundManager {
public:
	void getFiles();
	SoundManager();
	~SoundManager();
	void createSounds();
	bool hasLoaded;
	std::vector<std::string> soundNames;
	std::unordered_map<std::string, sf::SoundBuffer> soundTable;
};