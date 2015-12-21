#include "Node.h"

//Disgutingly dirty fix
struct CompareQueueEl
{
	typedef std::pair<int, Node> queueEl;
	bool operator()(queueEl lhs, queueEl rhs) {
		return lhs.first > rhs.first;
	}
};