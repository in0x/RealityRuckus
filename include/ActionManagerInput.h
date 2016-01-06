#pragma once

class ActionManagerInput {
public:
	ActionManagerInput(int p, int a) : playerIndex(p), actionIndex(a) {}
	ActionManagerInput(int p, int a, int x, int y) : playerIndex(p), actionIndex(a), mouseX(x), mouseY(y) {}
	int playerIndex = 0;
	int actionIndex = 0;
	int mouseX = 0;
	int mouseY = 0;
};