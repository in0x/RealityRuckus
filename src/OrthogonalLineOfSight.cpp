#include "OrthogonalLineOfSight.h"
#include <cmath>
#include <array>

void OrthogonalLineOfSight::subsampleMap(std::array<std::array<Tile, 30>, 30>* map) {
	int xMap = 0; int yMap = 0;
	for (int ySub = 0; ySub < 60; ySub++) {
		yMap = ySub / 2;
		for (int xSub = 0; xSub < 60; xSub++) {
			xMap = xSub / 2;
			subMap.at(xSub).at(ySub) = map->at(xMap).at(yMap).accessible;
		}
	}
}

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

		//Vertical first pass disabled for now to save 4 line of sights -> ~50% faster
		if(currentUnit->x >= 0 && currentUnit->y >= 0)
			if (checkVisibility(findLoSHFirst(x*2, y*2, currentUnit->x*2, currentUnit->y*2, map, &subMap)))// || checkVisibility(findLoSVFirst(x * 2, y * 2, currentUnit->x * 2, currentUnit->y * 2, map, &subMap)))
			{
				visibleUnits.push_back(currentUnit);
			}
			else if (checkVisibility(findLoSHFirst(x * 2+1, y * 2, currentUnit->x * 2+1, currentUnit->y * 2, map, &subMap)))// || checkVisibility(findLoSVFirst(x * 2, y * 2, currentUnit->x * 2, currentUnit->y * 2, map, &subMap)))
			{
				visibleUnits.push_back(currentUnit);
			}
			else if (checkVisibility(findLoSHFirst(x * 2, y * 2+1, currentUnit->x * 2, currentUnit->y * 2+1, map, &subMap)))// || checkVisibility(findLoSVFirst(x * 2, y * 2, currentUnit->x * 2, currentUnit->y * 2, map, &subMap)))
			{
				visibleUnits.push_back(currentUnit);
			}
			else if (checkVisibility(findLoSHFirst(x * 2+1, y * 2+1, currentUnit->x * 2+1, currentUnit->y * 2+1, map, &subMap)))// || checkVisibility(findLoSVFirst(x * 2, y * 2, currentUnit->x * 2, currentUnit->y * 2, map, &subMap)))
			{
				visibleUnits.push_back(currentUnit);
			}

	}

	return visibleUnits;
}


//Returns LineOfSightVector Horizontal step checked first
std::vector<bool> OrthogonalLineOfSight::findLoSHFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map, std::array<std::array<bool, 60>, 60>* subMap) {
	std::vector<bool> lineOfSight;

	int dx = endX - startX;
	int dy = endY - startY;
	int nx = abs(dx);
	int ny = abs(dy);

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

	int currX = startX;
	int currY = startY;

	int ix = 0;
	int iy = 0;
	for (ix = 0, iy = 0; ix < nx || iy < ny;) {
		if ((0.5 + ix) / nx <= (0.5 + iy) / ny) {	//Horizontal step
			currX += sign_x;
			ix++;
		}
		else {
			currY += sign_y;			//Vertical step
			iy++;
		}
		lineOfSight.push_back(subMap->at(currX).at(currY));
	}
	return lineOfSight;
}

//Returns LineOfSightVector Vertical step checked first
std::vector<bool> OrthogonalLineOfSight::findLoSVFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map, std::array<std::array<bool, 60>, 60>* subMap) {
	std::vector<bool> lineOfSight;

	int dx = endX - startX;
	int dy = endY - startY;
	int nx = abs(dx);
	int ny = abs(dy);

	int sign_x = dx > 0 ? 1 : -1;
	int sign_y = dy > 0 ? 1 : -1;

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
		lineOfSight.push_back(subMap->at(currX).at(currY));
	}
	return lineOfSight;
}

//checks LineOfSight for inaccessible Tiles
bool OrthogonalLineOfSight::checkVisibility(std::vector<bool> lineOfSight) {
	if (lineOfSight.size() > 18) //If line to enemy is longer than 8 tiles, *2 because of subsampling
		return false;
	if (lineOfSight.size() == 0)
		return true;
	for (int i = 0; i < lineOfSight.size(); i++) {
		if (lineOfSight[i] == false)
			return false;
	}
	return true;
}