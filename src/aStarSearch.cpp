#include "aStarSearch.h"
#include "MyPriorityQueue.h"
#include <cmath>

std::vector<Node> aStarSearch::findPath(Node start, Node target, WeightedGraph& graph) {

  std::unordered_map<Node, Node> cameFrom;
  std::unordered_map<Node, int> costSoFar;

  aStar(graph, start, target, cameFrom, costSoFar);
  
  return reconstruct_path(start, target, cameFrom);
}

std::vector<Node> aStarSearch::reconstruct_path( Node start, Node target, std::unordered_map<Node, Node>& cameFrom) {

  std::vector<Node> path;

  if (cameFrom.size() == 1) {
	  path.push_back(start);
	  return path;
  }

  Node current = target;
  path.push_back(current);

  while (current != start) {
    current = cameFrom[current];
    path.push_back(current);
	if (path.size() > 100)
		return path;
  }

  std::reverse(path.begin(), path.end());
  return path;
}

int aStarSearch::heuristic(Node a, Node b) { //calculate distance between nodes, A' uses this to find shortest path
  return fabs(a.x - b.x) + fabs(a.y - b.y); //Might fail horribly on Windows, could be clang weirdness
}

void aStarSearch::aStar(WeightedGraph graph, Node start, Node goal, std::unordered_map<Node, Node>& cameFrom, std::unordered_map<Node, int>& costSoFar) {

  PriorityQueue<Node> frontier;

  frontier.put(start, 0);

  cameFrom[start] = start;
  costSoFar[start] = 0;
  
  while (!frontier.empty()) {
    auto current = frontier.get();

    if (current == goal) {
      break;
    }

    for (auto next : graph.neighbours(current)) {
      int newCost = costSoFar[current] + graph.cost(next);
      if (!costSoFar.count(next) || newCost < costSoFar[next]) {
        costSoFar[next] = newCost;
        int priority = newCost + heuristic(next, goal);
        frontier.put(next, priority);
        cameFrom[next] = current;
      }
    }
  }
}


