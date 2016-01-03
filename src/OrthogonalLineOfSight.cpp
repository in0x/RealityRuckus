#include "OrthogonalLineOfSight.h"
#include <cmath>
#include <array>

//Returns Vector of Unit* that are visible from current player location
std::vector<Unit*> OrthogonalLineOfSight::getVisibleUnits(lvlManager* lvlMng, UnitManager* unitMng, Unit* unit) {
	std::vector<Unit*> visibleUnits;
	std::array<std::array<Tile, 30>, 30>* map = &(lvlMng->map);
	/*int xPlayer = unitMng->player->x;
	int yPlayer = unitMng->player->y;
	*/
	int x = unit->x;
	int y = unit->y;

	for (int i = 0; i < unitMng->unitList.size(); i++) {

		Unit* currentUnit = unitMng->unitList[i];
		if(currentUnit->x >= 0 && currentUnit->y >= 0)
			if (checkVisibility(findLoSHFirst(x, y, currentUnit->x, currentUnit->y, map)) || checkVisibility(findLoSVFirst(x, y, currentUnit->x, currentUnit->y, map)))
			{
				visibleUnits.push_back(currentUnit);
			}
	}

	return visibleUnits;
}


//Returns LineOfSightVector Horizontal step checked first
std::vector<Tile> OrthogonalLineOfSight::findLoSHFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map) {
	std::vector<Tile> lineOfSight;

	int dx = endX - startX;
	int dy = endY - startY;
	int nx = abs(dx);
	int ny = abs(dy);

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

	Tile crossedNode = (*map)[startX][startY];
	int currX = startX;
	int currY = startY;

	int ix = 0;
	int iy = 0;
	for (ix = 0, iy = 0; ix < nx || iy < ny;) {
		if ((0.5 + ix) / nx < (0.5 + iy) / ny) {	//Horizontal step
			currX += sign_x;
			ix++;
		}
		else {
			currY += sign_y;			//Vertical step
			iy++;
		}
		lineOfSight.push_back((*map)[currX][currY]);
	}
	return lineOfSight;
}

//Returns LineOfSightVector Vertical step checked first
std::vector<Tile> OrthogonalLineOfSight::findLoSVFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map) {
	std::vector<Tile> lineOfSight;

	int dx = endX - startX;
	int dy = endY - startY;
	int nx = abs(dx);
	int ny = abs(dy);

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

	Tile crossedNode = (*map)[startX][startY];
	int currX = startX;
	int currY = startY;

	int ix = 0;
	int iy = 0;
	for (ix = 0, iy = 0; ix < nx || iy < ny;) {
		if ((0.5 + ix) / nx > (0.5 + iy) / ny) {	
			currY += sign_y;			//Vertical step
			iy++;
		}
		else {
			currX += sign_x;			//Horizontal step
			ix++;
			
		}
		lineOfSight.push_back((*map)[currX][currY]);
	}
	return lineOfSight;
}

//checks LineOfSight for inaccessible Tiles
bool OrthogonalLineOfSight::checkVisibility(std::vector<Tile> lineOfSight) {
	if (lineOfSight.size() > 9) //If line to enemy is longer than 8 tiles
		return false;
	if (lineOfSight.size() == 0)
		return true;
	for (int i = 0; i < lineOfSight.size(); i++) {
		if (lineOfSight[i].accessible == false)
			return false;
	}
	return true;
}