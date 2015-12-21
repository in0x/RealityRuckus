#pragma once
#include <unordered_set>
#include "Node.h"
#include "Graph.h"

class WeightedGraph : public Graph { 
public:
	int cost(Node goTo);
	WeightedGraph(int w, int h, std::array<std::array<Tile, 30>, 30>& m): Graph(w, h, m) {}

private:
 	std::unordered_set<Node> nodeCosts;
};
