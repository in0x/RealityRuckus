#include "Node.h"
#include <queue>

template<typename T>
struct PriorityQueue {

  typedef std::pair<int, T> queueEl;

  std::priority_queue<queueEl, std::vector<queueEl>, std::greater<queueEl>> elements;

  bool empty() {
   return elements.empty(); 
 }

  void put(T item, int priority) {
    elements.emplace(priority, item);
  }

  T get() {
      T topNode = elements.top().second;
      elements.pop();
      return topNode;
  }
};