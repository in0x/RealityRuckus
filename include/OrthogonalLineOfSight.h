#pragma once
#include <vector>
#include "Node.h"
#include "tile.h"
#include "UnitManager.h"

class OrthogonalLineOfSight {
public:
	std::vector<Unit*> getVisibleUnits(lvlManager*, UnitManager*, Unit* unit);

private:
	std::vector<Tile> findLoSHFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map);
	std::vector<Tile> findLoSVFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map);
	bool checkVisibility(std::vector<Tile>);
};
