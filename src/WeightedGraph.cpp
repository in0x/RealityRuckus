#pragma once
#include "WeightedGraph.h"

int WeightedGraph::cost(Node goTo) {
	// ie: return nodeCosts.count(goTo) ? 5 : 1;, can be used to make terrain harder to traverse
	return 1; // currently not in use
}

// WeightedGraph::WeightedGraph(int w, int h, std::array<std::array<Tile, 30>, 30>& m): Graph(w, h, m) {
// 	nodeCosts = std::unordered_set<Node>();
// }

