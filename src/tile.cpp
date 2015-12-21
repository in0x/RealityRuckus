#include "tile.h"

Tile::Tile(){}

Tile::Tile(sf::Sprite s, bool b) {
	accessible = b;
	sprite = s;
}

Tile::~Tile() {

}

bool Tile::notePosition(int x_arr, int y_arr) {
	if (x < 0 || x > 29 || y < 0 || y > 29) 
		return false;
	x = x_arr;
	y = y_arr;
	return true;
}