#pragma once
#include "Graph.h"

std::vector<Node> Graph::neighbours(Node node) {
	std::vector<Node> neighbours;

	std::vector<Node> directions = {Node{0, 1}, Node{1, 0}, Node{0, -1}, Node{-1, 0}};

	for (auto& d : directions) {

		if (isInLimits(Node {node.x + d.x, node.y + d.y}) && isAccesible(Node{node.x + d.x, node.y + d.y}))
			neighbours.push_back(Node{node.x + d.x, node.y + d.y});
	}

	return neighbours;
}

bool Graph::isAccesible(Node n) {
	return map[n.x][n.y].accessible;
}

bool Graph::isInLimits(Node n) {
	return 0 <= n.x && n.x < width && 0 <= n.y && n.y < height;
}


//  std::array<Node, 4> directions = {Node{1, 0}, Node{0, -1}, Node{-1, 0}, Node{0, 1}};
//std::array<std::array<int, 3>, 3> arr = {{{5, 8, 2}, {8, 3, 1}, {5, 3, 9}}};