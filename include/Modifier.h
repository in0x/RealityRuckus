#pragma once

#include <functional>

enum class ModifierType {
	HPLoss,
	HPGain,
	APLoss,
	APGain,
	Movement
};

struct Modifier {
public:
	ModifierType type;
	int rounds;
	std::function<void(float&)> func;
};