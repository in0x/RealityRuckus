#pragma once
#include <vector>
#include "Node.h"
#include "tile.h"
#include "UnitManager.h"

class lvlManager;
class UnitManager;


class OrthogonalLineOfSight {
public:
	std::vector<Unit*> getVisibleUnits(lvlManager*, UnitManager*, Unit* unit);

	void subsampleMap(std::array<std::array<Tile, 30>, 30>* map);

private:
	std::vector<bool> findLoSHFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map, std::array<std::array<bool, 60>, 60>* subMap);
	std::vector<bool> findLoSVFirst(int startX, int startY, int endX, int endY, std::array<std::array<Tile, 30>, 30>* map, std::array<std::array<bool, 60>, 60>* subMap);
	
	bool checkVisibility(std::vector<bool>);
	
	std::array<std::array<bool, 60>, 60> subMap;
};
