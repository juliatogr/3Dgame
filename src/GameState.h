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
	bool DevUiActive;
	bool PauseMenu;
	bool isFinished;
	bool isLoaded;
	bool isReLoaded;


	std::vector<Code*> codes;
	std::vector<ReadNote*> read;
	std::vector<Code*> devs;
	int currentTaskId;


	GameState();
};

