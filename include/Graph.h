#pragma once
#include <array>
#include <vector>
#include "Tile.h"
#include "Node.h"

class Graph {
private:

  bool isInLimits(Node node);
  bool isAccesible(Node node);

public:  
  int width;
  int height;
  Graph(int w, int h, std::array<std::array<Tile, 30>, 30>& m) : width(w), height(h), map(m) {}
  std::array<std::array<Tile, 30>, 30>& map;
  std::vector<Node> neighbours(Node node);
};


