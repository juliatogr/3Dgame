#include "GameState.h"


GameState::GameState(){
	this->UIActive = false;
	this->OpenInventory = false;
	this->CodeUiActive = false;
	this->DevUiActive = false;
	this->PauseMenu = false;
	this->isFinished = false;
	this->isLoaded = false;
	this->isReLoaded = true;
	this->isTutorialMenu=true;
	this->currentInstrucction = 0;

}
