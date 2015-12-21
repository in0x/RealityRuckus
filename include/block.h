#pragma once
#include <vector>
#include "tile.h"

struct Block {
	std::vector<Tile> tiles;
	int x;
	int y;
};