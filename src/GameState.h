#pragma once
#include <vector>
#include "Tasks.h"

class GameState
{
public:
	static GameState* instance;

	bool UIActive;
	bool OpenInventory;
	bool CodeUiActive;
	bool PauseMenu;


	std::vector<Code*> codes;
	std::vector<ReadNote*> read;
	int currentTaskId;


	GameState();
};

