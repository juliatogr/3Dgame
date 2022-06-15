#pragma once
class GameState
{
public:
	static GameState* instance;
	bool UIActive;
	bool OpenInventory;
	bool PauseMenu;

	GameState();
};

