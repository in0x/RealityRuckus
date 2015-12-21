#pragma once
#include <functional>

struct Node {
	int x, y;

	bool operator== (const Node& other) {
	    return (x == other.x && y == other.y);
	}

	bool operator!= (const Node& other) {
		return !(*this == other); 
	}

	friend bool operator== (const Node& lhs, const Node& rhs)
    {
        return rhs.x == lhs.x && rhs.y == lhs.y;
    }

    friend bool operator!= (const Node& lhs, const Node& rhs) {
		return !(lhs == rhs); 
	}

	friend bool operator< (const Node& lhs, const Node& rhs) {
		return rhs.x < lhs.x && rhs.y < lhs.y;
	}

	friend bool operator> (const Node& lhs, const Node& rhs) {
		return rhs.x > lhs.x && rhs.y > lhs.y;
	}
};


// struct HashNode {
//   int operator() (const Node& n) const{
// 		// int max = 30*30;
// 		// return (((n.x + n.y) * (max)) % max);
//   		return std::hash<int>()(n.x) ^ std::hash<int>()(n.y);
//    }
// };

namespace std
{
template<>
	struct hash<Node> {
	    size_t operator()(const Node &n) const {
	        return std::hash<int>()(n.x) ^ std::hash<int>()(n.y);
	    }
	};
}