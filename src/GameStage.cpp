#include "GameStage.h"
#include "game.h"
#include "input.h"
//para testear la posicion del mouse en las UI, y la posicion del player durante el juego
bool testMouse = false;

void PickButton(std::vector<Button*> buttons) {
	Vector2 mouse = Input::mouse_position;
	for (int i = 0; i < buttons.size(); i++) {
		Button* current = buttons[i];
		Vector2 size = Vector2(current->xyhw.z, current->xyhw.w);
		Vector2 position = Vector2(current->xyhw.x - (size.x / 2), current->xyhw.y - (size.y / 2));
		if ((current->type == N) || (current->type == H)) {
			if (((mouse.x > (position.x)) && (mouse.y > position.y)) && ((mouse.x < (position.x + size.x)) && (mouse.y < (position.y + size.y)))) {
				//std::cout << i << ". " << "mouse.x > position.x) && (mouse.y > position.y)" << std::endl;
				current->type = H;


			}
			else {
				current->type = N;

			}
		}

	}
}

Lab* PlayStage::GetLab()
{
	return this->lab;
}

void PlayStage::RayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->entities.size(); i++) {
			Entity* entity = this->lab->rooms[r]->entities[i];
			Vector3 pos;
			Vector3 normal;
			if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {

				this->selectedEntity = entity;
				break;
			}
		}
	}
}

void PlayStage::RotateSelected(float angleDegrees) {
	if (this->selectedEntity == NULL) {
		return;
	}

	this->selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
}


STAGE_ID PlayStage::GetId()
{
	return STAGE_ID::PLAY;
}

PlayStage::PlayStage()
{
	this->lab = new Lab();
	this->player = new Player();
	this->invMenu = new InventoryMenu();
	this->pauseMenu = new PauseMenu();
	this->invMenu->inventory = new Inventory();
	this->pum.reserve(3);
	this->pum.push_back(new PopUpMessage(0, "Push R to return", Texture::Get("data/UI/Cards/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 200, Game::instance->window_width - 200, 50)));
	this->pum.push_back(new PopUpMessage(1, "Push Q to save the object to Inventory", Texture::Get("data/UI/Cards/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 260, Game::instance->window_width - 200, 50)));
	this->pum.push_back(new PopUpMessage(2, "Push E to view the object", Texture::Get("data/UI/Cards/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 200, Game::instance->window_width - 200, 50)));
	this->codeUI = new CodeScreen(this->lab);
	this->devUI = new DevelopScreen(this->lab);

	Game::instance->gameState->codes.push_back(new Code(0, "4567", lab->doors[2], lab->doors[3]));
	Game::instance->gameState->codes.push_back(new Code(1, "1618", lab->doors[4]));

	Game::instance->gameState->read.push_back(new ReadNote(0, lab->doors[0], lab->doors[1]));
	Game::instance->gameState->read.push_back(new ReadNote(0));

	Game::instance->gameState->devs.push_back(new Develop(0));
}

void PlayStage::RePlayStage()
{
	this->lab = new Lab();
	this->player = new Player();
	this->invMenu = new InventoryMenu();
	this->invMenu->inventory = new Inventory();

	
	Game::instance->gameState = new GameState();
	std::vector<Code*> newCode;
	Game::instance->gameState->codes = newCode;
	Game::instance->gameState->codes.push_back(new Code(0, "4567", lab->doors[2], lab->doors[3]));
	Game::instance->gameState->codes.push_back(new Code(1, "1618", lab->doors[4]));

	std::vector<ReadNote*> newRead;
	Game::instance->gameState->read = newRead;
	Game::instance->gameState->read.push_back(new ReadNote(0, lab->doors[0], lab->doors[1]));
	Game::instance->gameState->read.push_back(new ReadNote(0));

	std::vector<Develop*> newDevelop;
	Game::instance->gameState->devs = newDevelop;
	Game::instance->gameState->devs.push_back(new Develop(0));

	Game::instance->gameState->isLoaded = true;


}



void PlayStage::Render(Shader* a_shader, Camera* cam)
{
	GameState* state = Game::instance->gameState;

	if (!isViewingTask) {
		Matrix44 playerModel;
		playerModel.translate(this->player->pos.x, this->player->pos.y, this->player->pos.z);
		playerModel.rotate(this->player->yaw * DEG2RAD, Vector3(0, 1, 0));

		Matrix44 camModel = playerModel;
		camModel.rotate(this->player->pitch * DEG2RAD, Vector3(1, 0, 0));

		Vector3 eye = playerModel * Vector3(0, 1.2, 0);
		Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
		Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));

		cam->lookAt(eye, center, up);
	}


	lab->ground->RenderMesh(a_shader, cam);

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->entities.size(); i++)
		{
			Entity* entity = this->lab->rooms[r]->entities[i];
			entity->RenderMesh(a_shader, cam);

		}
	}

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->taskEntities.size(); i++)
		{
			TaskEntity* entity = this->lab->rooms[r]->taskEntities[i];

			if (!entity->isSaved) {
				entity->RenderMesh(a_shader, cam);
			}
			else if (entity->isViewing) {
				entity->RenderMesh(a_shader, cam);
			}

		}
	}

	for (size_t i = 0; i < this->lab->doors.size(); i++)
	{
		Door* door = this->lab->doors[i];
		door->RenderMesh(a_shader, cam);

	}

	//UI

	for (size_t i = 0; i < this->pum.size(); i++) {
		PopUpMessage* m = this->pum[i];
		if (m->isActive == true)
			m->RenderPopUp();
	}


	if (state->OpenInventory == true) {
		this->invMenu->RenderMenu();

		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}

	if (state->CodeUiActive == true) {
		this->codeUI->RenderCodeScreen(state->codes[state->currentTaskId]);
		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}

	if (state->DevUiActive == true) {
		this->devUI->RenderDevelopScreen(state->devs[state->currentTaskId]);
		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}

	if (state->PauseMenu == true) {
		this->pauseMenu->RenderMenu();
		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}

	if (testMouse == true) {
		/*testeo para saber posicion del player*/

		std::string text = "Player Position2D: " + std::to_string(this->player->pos.x) + ", " + std::to_string(this->player->pos.y) + ", " + std::to_string(this->player->pos.z);
		drawText(0, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
	}

}

void PlayStage::Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	GameState* state = Game::instance->gameState;

	/* si al menos un tipo de UI esta activado -> que el bool UIActive es true*/
	if (state->OpenInventory == true || state->CodeUiActive == true || state->DevUiActive == true || state->PauseMenu == true) {
		state->UIActive = true;
	}
	else {
		state->UIActive = false;
	}

	if (state->UIActive == false) {

		isViewingTask = false;
		
		this->checkNearTaskEntity(seconds_elapsed);
		if (this->selectedTaskEntity != NULL) {
			if (this->selectedTaskEntity->type == NOTE) {

				if (this->selectedTaskEntity->isReturning) {
					this->selectedTaskEntity->returnView(camera, seconds_elapsed);
					isViewingTask = true;
					this->selectedTaskEntity->isViewed = true;
					// cuando el usuario vuelve (ha leido la nota), abre las puertas
					state->read[this->selectedTaskEntity->id]->OpenDoors();
					this->selectedTaskEntity = NULL;

				}
				else if (this->selectedTaskEntity->isViewing) {
					this->selectedTaskEntity->viewToTask(camera, seconds_elapsed);
					isViewingTask = true;
					this->pum[0]->isActive = true;
					//std::cout << "Push R to return" << std::endl;
					if (Input::isKeyPressed(SDL_SCANCODE_R)) {
						this->selectedTaskEntity->isReturning = true;
						this->pum[0]->isActive = false;
						this->pum[1]->isActive = false;

						if (state->read[this->selectedTaskEntity->id]->isCompleted == false) {
							state->read[this->selectedTaskEntity->id]->isCompleted = true;

						}
						state->read[this->selectedTaskEntity->id]->isActive = false;

					}

					if (this->selectedTaskEntity->canBeSaved) {
						this->pum[1]->isActive = true;
						//std::cout << "Push Q to save the object to Inventory" << std::endl;
						if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
							state->currentTaskId = this->selectedTaskEntity->id;
							this->selectedTaskEntity->isReturning = true;
							this->selectedTaskEntity->isViewed = true;
							this->selectedTaskEntity->isViewing = false;
							isViewingTask = false;
							this->invMenu->inventory->addNote(this->selectedTaskEntity, this->selectedTaskEntity->img, this->selectedTaskEntity->id);
							this->invMenu->UpdateMenu();
							this->selectedTaskEntity->isSaved = true;
							this->pum[0]->isActive = false;
							this->pum[1]->isActive = false;

							if (state->read[this->selectedTaskEntity->id]->isCompleted == false) {
								state->read[this->selectedTaskEntity->id]->isCompleted = true;
							}
							state->read[this->selectedTaskEntity->id]->isActive = false;
							state->read[this->selectedTaskEntity->id]->OpenDoors();
							this->selectedTaskEntity = NULL;


						}
					}

				}
				else if (Input::isKeyPressed(SDL_SCANCODE_E)) {
					this->selectedTaskEntity->isViewing = true;
					isViewingTask = true;
					this->pum[2]->isActive = false;
					//activamos la tarea de leer nota
					state->read[this->selectedTaskEntity->id]->isActive = true;
				}
				else {
					this->pum[0]->isActive = false;
					this->pum[1]->isActive = false;
					this->pum[2]->isActive = true;
					//std::cout << "Push E to view the object " << this->pum[2]->isActive << std::endl;


				}
			}
			else if (this->selectedTaskEntity->type == PC) {

				if (Input::isKeyPressed(SDL_SCANCODE_E)) {
					this->pum[2]->isActive = false;

					if (state->codes[this->selectedTaskEntity->id]->isCompleted == true) {
						state->currentTaskId = this->selectedTaskEntity->id + 1;
						state->codes[this->selectedTaskEntity->id + 1]->isActive = true;

					}
					else {
						if (this->selectedTaskEntity->id == 0 || (this->selectedTaskEntity->id == 1 && state->devs[0]->isCompleted)) {
							state->CodeUiActive = true;
							state->codes[this->selectedTaskEntity->id]->isActive = true;
							state->currentTaskId = this->selectedTaskEntity->id;
						}


					}
				}
				else if (state->codes[this->selectedTaskEntity->id]->isCompleted == false) {

					if (this->selectedTaskEntity->id == 0 || (this->selectedTaskEntity->id == 1 && state->devs[0]->isCompleted)) {
						this->pum[2]->isActive = true;
					}
					
					//std::cout << "Push E to view the object " << this->pum[2]->isActive << std::endl;
				}
			}

			else if (this->selectedTaskEntity->type == CONSOLE) {

				if (Input::isKeyPressed(SDL_SCANCODE_E)) {
					this->pum[2]->isActive = false;
					if (state->devs[this->selectedTaskEntity->id]->isCompleted == true) {
						//state->codes[1]->isActive = true;

					}
					else {
						this->selectedTaskEntity->isViewing = true;
						isViewingTask = true;
						this->selectedTaskEntity->viewToTask(camera, seconds_elapsed);
						state->DevUiActive = true;
						state->devs[this->selectedTaskEntity->id]->isActive = true;
						state->currentTaskId = this->selectedTaskEntity->id;

					}
				}
				else if (state->devs[this->selectedTaskEntity->id]->isCompleted == false) {
					this->pum[2]->isActive = true;
					//std::cout << "Push E to view the object " << this->pum[2]->isActive << std::endl;
				}
			}

		}
		else {
			this->pum[0]->isActive = false;
			this->pum[1]->isActive = false;
			this->pum[2]->isActive = false;
		}



		if (!isViewingTask) {
			//mouse input to rotate the cam
			if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
			{

				camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
				camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
			}

			if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
				cameralocked = !cameralocked;
			}
			float playerSpeed = 5.0f * seconds_elapsed;
			float rotSpeed = 200.0f * DEG2RAD * seconds_elapsed;

			this->player->yaw += -Input::mouse_delta.x * 10.0f * seconds_elapsed;
			this->player->pitch += -Input::mouse_delta.y * 10.0f * seconds_elapsed;


			if (this->player->pitch >= 90) {
				this->player->pitch = 90;
			}

			if (this->player->pitch <= -90) {
				this->player->pitch = -90;
			}
			Input::centerMouse();
			SDL_ShowCursor(false);


			Matrix44 playerRotation = this->player->model;
			playerRotation.rotate(this->player->yaw * DEG2RAD, Vector3(0, 1, 0));

			Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, 1));
			Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
			Vector3 playerVel;

			if (Input::isKeyPressed(SDL_SCANCODE_S)) playerVel = playerVel + (forward * playerSpeed);
			if (Input::isKeyPressed(SDL_SCANCODE_W)) playerVel = playerVel - (forward * playerSpeed);
			if (Input::isKeyPressed(SDL_SCANCODE_D)) playerVel = playerVel + (right * playerSpeed);
			if (Input::isKeyPressed(SDL_SCANCODE_A)) playerVel = playerVel - (right * playerSpeed);

			this->player->pos = nextPosNoCol(this->player->pos + playerVel, seconds_elapsed);

			lab->doors[0]->Open(a_shader, seconds_elapsed);
			lab->doors[1]->Open(a_shader, seconds_elapsed);
			lab->doors[2]->Open(a_shader, seconds_elapsed);
			lab->doors[3]->Open(a_shader, seconds_elapsed);
			lab->doors[4]->Open(a_shader, seconds_elapsed);

			if (this->player->pos.z <= -17.844) {
				//std::cout << "WIN" << std::endl;
				if (!state->isFinished)
					state->isFinished = true;
			}

		}

	}
	else {

		/*Por cada UI que contenga botones, compruebo si alguno esta en Hover*/
		if (state->OpenInventory == true) {
			SDL_ShowCursor(true);

			PickButton(this->invMenu->Buttons);

		}
		if (state->CodeUiActive == true) {
			SDL_ShowCursor(true);

			PickButton(this->codeUI->Buttons);
		}
		if (state->DevUiActive == true) {
			SDL_ShowCursor(true);

			PickButton(this->devUI->Buttons);
		}
		if (state->PauseMenu == true) {
			SDL_ShowCursor(true);

			PickButton(this->pauseMenu->Buttons);
		}
		//std::cout << "mouseX:"<<mouse.x <<" mouseY:" <<mouse.y << std::endl;

	}
}

Vector3 PlayStage::nextPosNoCol(Vector3 nextPos, float seconds_elapsed)
{
	Vector3 character_center = nextPos + Vector3(0, 0.3, 0);
	//para cada objecto de la escena...


	int numDoors = this->lab->doors.size();

	for (int r = 0; r < numDoors; r++) {
		Entity* entity = this->lab->doors[r];

		Vector3 coll;
		Vector3 collnorm;

		//comprobamos si colisiona el objeto con la esfera (radio 3)
		if (!entity->mesh->testSphereCollision(entity->model, character_center, 0.05f, coll, collnorm))
			continue; //si no colisiona, pasamos al siguiente objeto

					  //si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
		Vector3 push_away = normalize(coll - character_center) * seconds_elapsed;

		nextPos = this->player->pos - push_away; //move to previous pos but a little bit further
												 //reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
												 //velocity = reflect(velocity, collnorm) * 0.95;
	}

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->entities.size(); i++) {
			Entity* entity = this->lab->rooms[r]->entities[i];

			Vector3 coll;
			Vector3 collnorm;

			if (!entity->mesh->testSphereCollision(entity->model, character_center, 0.05f, coll, collnorm)) {
				continue;
			}
			//si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
			Vector3 push_away = normalize(coll - character_center) * seconds_elapsed;

			nextPos = this->player->pos - push_away; //move to previous pos but a little bit further
													 //reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
													 //velocity = reflect(velocity, collnorm) * 0.95;

		}
	}
	nextPos.y = 0.0;
	return nextPos;
}

void PlayStage::checkNearTaskEntity(double seconds_elapsed)
{
	selectedTaskEntity = NULL;

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->taskEntities.size(); i++) {
			TaskEntity* entity = this->lab->rooms[r]->taskEntities[i];


			if (!entity->isSaved) {
				Vector3 coll;
				Vector3 collnorm;

				if (!entity->mesh->testSphereCollision(entity->model, this->player->pos + Vector3(0, 0.3, 0), 12.f, coll, collnorm)) {
					continue;
				}
				//std::cout << "current: " << entity->type << std::endl;
				selectedTaskEntity = entity;
			}
		}
	}
}



STAGE_ID IntroStage::GetId()
{
	return STAGE_ID::INTRO;
}

IntroStage::IntroStage()
{
	this->menu = new IntroMenu();
}

void IntroStage::Render(Shader* a_shader, Camera* cam)
{
	this->menu->RenderMenu();

	if (testMouse == true) {
		/*testeo para saber posicion del mouse*/
		Vector2 mouse = Input::mouse_position;
		std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
		drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
	}
}

void IntroStage::Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	SDL_ShowCursor(true);

	PickButton(this->menu->Buttons);
}

TutorialStage::TutorialStage()
{

	this->menu = new TutorialMenu();

}

STAGE_ID TutorialStage::GetId()
{
	return STAGE_ID::TUTORIAL;
}

void TutorialStage::Render(Shader* a_shader, Camera* cam)
{
	

	/*aqui renderizamos*/
	this->menu->RenderMenu();


	if (testMouse == true) {
		/*testeo para saber posicion del mouse*/
		Vector2 mouse = Input::mouse_position;
		std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
		drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
	}



}

void TutorialStage::Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	SDL_ShowCursor(true);

	PickButton(this->menu->Buttons);
}

EndStage::EndStage()
{
	this->menu = new EndingMenu();
}

STAGE_ID EndStage::GetId()
{
	return STAGE_ID::END;
}

void EndStage::Render(Shader* a_shader, Camera* cam)
{
	this->menu->RenderMenu();



	if (testMouse == true) {
		/*testeo para saber posicion del mouse*/
		Vector2 mouse = Input::mouse_position;
		std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
		drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
	}
}

void EndStage::Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	SDL_ShowCursor(true);

	PickButton(this->menu->Buttons);
}

LoadStage::LoadStage()
{
	//std::cout << "loadedScreen" << std::endl;
	this->menu = new LoadScreen();
}

STAGE_ID LoadStage::GetId()
{
	return STAGE_ID();
}

void LoadStage::Render(Shader* a_shader, Camera* cam)
{
	this->menu->RenderMenu();
}

void LoadStage::Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	SDL_ShowCursor(true);

	PickButton(this->menu->Buttons);

}
