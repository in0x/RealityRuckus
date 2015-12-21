#include "EncryptedInputStream.h"

using namespace sf;
using namespace std;


	bool EncryptedInputStream::open(std::string filename)
	{
		readFile = ifstream(filename, ios::in | ios::binary| ios::ate);
		size = readFile.tellg();
		readFile.seekg(0);
		position = 0;
		return readFile.is_open();
		
	}
	Int64 EncryptedInputStream::read(void* data, Int64 size)
	{
		char* input = new char[size];
		readFile.read(input, size);
		for (int i = 0; i < size; i++)
		{
			//if (i<size-1 || readFile.good())
				input[i] ^= 'L';
			((char*)data)[i] = input[i];
		}
		//*(char*)data = *input;
		//readFile.read((char*)data, size);
		return size;
	}
	Int64 EncryptedInputStream::seek(Int64 position)
	{
		readFile.seekg(position);
		if (readFile.good())
			return position;
		else
			return -1;
	}
	Int64 EncryptedInputStream::tell()
	{
		return readFile.tellg();
	}
	Int64 EncryptedInputStream::getSize()
	{
		return size;
	}

	EncryptedInputStream::EncryptedInputStream()
	{
	}


	EncryptedInputStream::~EncryptedInputStream()
	{
	}
