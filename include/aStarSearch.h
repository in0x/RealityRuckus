#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#include "Node.h"
#include "WeightedGraph.h"

class aStarSearch {
public:
	std::vector<Node> findPath(Node start, Node target, WeightedGraph& graph); 

private:

	std::vector<Node> reconstruct_path( Node start, Node goal, std::unordered_map<Node, Node>& cameFrom); 

	int heuristic(Node a, Node b);

	void aStar(WeightedGraph graph, Node start, Node goal, std::unordered_map<Node, Node>& cameFrom, std::unordered_map<Node, int>& costSoFar);

};