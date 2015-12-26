#include "OrthogonalLineOfSight.h"
#include <cmath>
#include <array>

//Returns Vector of Unit* that are visible from current player location
std::vector<Unit*> OrthogonalLineOfSight::getVisibleUnits(lvlManager* lvlMng, UnitManager* unitMng) {
	std::vector<Unit*> visibleUnits;
	std::array<std::array<Tile, 30>, 30> map = lvlMng->map;
	int xPlayer = unitMng->player->x;
	int yPlayer = unitMng->player->y;

	for (int i = 1; i < unitMng->unitList.size(); i++) {
		
		if (checkVisibility(findLoS(unitMng->player->x, unitMng->player->y, unitMng->unitList[i]->x, unitMng->unitList[i]->y, map)))
			visibleUnits.push_back(unitMng->unitList[i]);
	}

	return visibleUnits;
}

//Returns LineOfSightVector
std::vector<Tile> OrthogonalLineOfSight::findLoS(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30> map) {
	std::vector<Tile> lineOfSight;

	int dx = endX - startX;
	int dy = endY - startY;
	int nx = abs(dx);
	int ny = abs(dy);

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

	Tile crossedNode = map[startX][startY];
	int currX = startX;
	int currY = startY;
	
	int ix = 0;
	int iy = 0;
	for (ix = 0, iy = 0; ix < nx || iy < ny;) {
		if ((0.5+ix) / nx < (0.5+iy) / ny) {	//Horizontal step
			currX += sign_x;
			ix++;
		}
		else {
			currY += sign_y;			//Vertical step
			iy++;
		}
		lineOfSight.push_back(map[currX][currY]);
	}
	return lineOfSight;
}

//checks LineOfSight for inaccessible Tiles
bool OrthogonalLineOfSight::checkVisibility(std::vector<Tile> lineOfSight) {
	if (lineOfSight.size() > 9) //If line to enemy is longer than 8 tiles
		return false;
	if (lineOfSight.size() == 0)
		return true;
	for (int i = 0; i < lineOfSight.size(); i++) {	//Do not check playerTile and enemyTile for accessibility: check Tile 1 until size-1
		if (lineOfSight[i].accessible == false)
			return false;
	}
	return true;
}

