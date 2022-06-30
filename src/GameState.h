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
	bool isTutorialMenu;


	std::vector<Code*> codes;
	std::vector<ReadNote*> read;
	std::vector<Develop*> devs;
	int currentTaskId;
	int currentInstrucction;


	GameState();
};

