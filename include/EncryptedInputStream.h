#pragma once
#include <SFML/System.hpp>
#include <fstream>
#include <iostream>
#include <string>

using namespace sf;
using namespace std;

class EncryptedInputStream :
	public sf::InputStream
{
public:
	EncryptedInputStream();
	~EncryptedInputStream();
	bool open(std::string filename);
	Int64 read(void* data, Int64 size);
	Int64 seek(Int64 position);

	Int64 tell();
	Int64 getSize();
protected:
	ifstream readFile;
	Int64 position = 0;
	Int64 size = 0;
};

