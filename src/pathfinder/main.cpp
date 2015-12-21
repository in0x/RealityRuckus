#include <iostream>
#include <array>
#include <vector>
#include "Graph.h"
#include "WeightedGraph.h"
#include "Tile.h"
#include "aStarSearch.h"
//#include "Node.h"

int main() {

	std::array<std::array<Tile, 30>, 30> m;

	for (int y = 0; y < 30; y++)
		for (int x = 0 ; x < 30; x++) {
			m[y][x].accessible = true;
			if (y == 15) {
				if (x != 13) {
					m[y][x].accessible = false;
				}
			} 
		}

	

	for (int y = 0; y < 30; y++) {
		for (int x = 0; x < 30; x++) {
			m[y][x].isPathed = false;
			m[y][x].isTarget = false;
		 	m[y][x].isStart = false;
		 }
	}	

	m[9][9].accessible = false;
	m[10][9].accessible = false;
	m[11][9].accessible = false;
	m[12][9].accessible = false;
	m[13][9].accessible = false;
	m[14][9].accessible = false;
	m[15][9].accessible = false;

	for (int i = 9; i < 15; i++) {
		m[13][i].accessible = false;
	}

	m[2][2].accessible = false;
	m[2][1].accessible = false;
	m[1][2].accessible = false;
	m[0][2].accessible = false;
	m[2][0].accessible = false;

	WeightedGraph wG {30, 30, m};

	aStarSearch a {};

	std::vector<Node> path = a.findPath(Node{0,0}, Node{18, 20}, wG);

	for (auto node : path) {
		m[node.x][node.y].isPathed = true;
		std::cout << "{" << node.x << " , " << node.y << "}" << std::endl;
	}

	m[0][0].isStart = true;
	m[18][20].isTarget = true;

	

	for (auto& arr : m) {
		for (auto& t : arr) {
			if (t.isStart) {
				std::cout << " S ";
			}	
			else if (t.isTarget) {
				std::cout << " T ";
			}
			else if (!t.accessible) {
				std::cout <<  " @ ";
			}
			else if (t.isPathed) {
				std::cout << " P ";
			}
			else if (t.accessible) {
				std::cout << " + ";
			}
		}
		std::cout << "\n";
	}
	return 0;

}