#include "GameStage.h"
#include "game.h"
#include "input.h"

bool testMouse = true;

Lab* GameStage::GetLab()
{
	return this->lab;
}

void GameStage::RayPick(Camera* cam) {

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
void GameStage::PickButton(std::vector<Button*> buttons) {

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

void GameStage::RotateSelected(float angleDegrees) {
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
	this->menu = new Menu();
	this->menu->inventory = new Inventory();
	this->pum.reserve(3);
	this->pum.push_back(new PopUpMessage(0, "Push R to return", Texture::Get("data/UI/Elements/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 200, Game::instance->window_width - 200, 50)));
	this->pum.push_back(new PopUpMessage(1, "Push Q to save the object to Inventory", Texture::Get("data/UI/Elements/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 260, Game::instance->window_width - 200, 50)));
	this->pum.push_back(new PopUpMessage(2, "Push E to view the object", Texture::Get("data/UI/Elements/BlockInformation.png"), Vector4(Game::instance->window_width / 2, (Game::instance->window_height / 2) + 200, Game::instance->window_width - 200, 50)));
	this->codeUI = new CodeScreen(this->lab);
	this->IsActiveUIs = false;
}



void PlayStage::Render(Shader* a_shader, Camera* cam)
{
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


	if (this->menu->isActive == true) {
		this->menu->RenderMenu();

		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}

	if (this->codeUI->isActive == true) {
		this->codeUI->RenderCodeScreen();
		if (testMouse == true) {
			/*testeo para saber posicion del mouse*/
			Vector2 mouse = Input::mouse_position;
			std::string text = "Mouse Position2D: " + std::to_string((int)mouse.x) + ", " + std::to_string((int)mouse.y);
			drawText((Game::instance->window_width) - 290, (Game::instance->window_height) - 25, text, Vector3(1, 1, 1), 2);
		}
	}







}

void PlayStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
	/* si al menos un tipo de UI esta activado -> que el bool IsActiveUis es true*/
	if (this->menu->isActive == true || this->codeUI->isActive == true) {
		this->IsActiveUIs = true;
	}
	else {
		this->IsActiveUIs = false;
	}

	if (this->IsActiveUIs == false) {

		isViewingTask = false;
		this->selectedTaskEntity = NULL;
		this->checkNearTaskEntity(elapsed_time);
		if (this->selectedTaskEntity != NULL) {
			if (this->selectedTaskEntity->type == NOTE) {
				if (this->selectedTaskEntity->isReturning) {
					this->selectedTaskEntity->returnView(camera, seconds_elapsed);
					isViewingTask = true;
					this->selectedTaskEntity->isViewed = true;
					// cuando el usuario vuelve (ha leido la nota), abre las puertas
					this->lab->doors[0]->isOpening = true;
					this->lab->doors[1]->isOpening = true;
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
					}

					if (this->selectedTaskEntity->canBeSaved) {
						this->pum[1]->isActive = true;
						//std::cout << "Push Q to save the object to Inventory" << std::endl;
						if (Input::isKeyPressed(SDL_SCANCODE_Q)) {

							this->selectedTaskEntity->isReturning = true;
							this->selectedTaskEntity->isViewed = true;
							this->selectedTaskEntity->isViewing = false;
							isViewingTask = false;
							this->menu->inventory->addNote(this->selectedTaskEntity);
							this->menu->UpdateMenu();
							this->selectedTaskEntity->isSaved = true;
							this->selectedTaskEntity = NULL;
							this->pum[0]->isActive = false;
							this->pum[1]->isActive = false;

						}
					}



				}
				else if (Input::isKeyPressed(SDL_SCANCODE_E)) {
					this->selectedTaskEntity->isViewing = true;
					isViewingTask = true;
					this->pum[2]->isActive = false;
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
					this->codeUI->isActive = true;
					if (this->codeUI->codes[this->selectedTaskEntity->id]->isCompleted == true) {
						this->codeUI->codes[this->selectedTaskEntity->id + 1]->isActive = true;

					}
					else {
						this->codeUI->codes[this->selectedTaskEntity->id]->isActive = true;
					}
				}
				else {
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
			float playerSpeed = 5.0f * elapsed_time;
			float rotSpeed = 200.0f * DEG2RAD * elapsed_time;

			this->player->yaw += -Input::mouse_delta.x * 10.0f * elapsed_time;
			this->player->pitch += -Input::mouse_delta.y * 10.0f * elapsed_time;


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

			this->player->pos = nextPosNoCol(this->player->pos + playerVel, elapsed_time);

			lab->doors[0]->Open(a_shader, seconds_elapsed);
			lab->doors[1]->Open(a_shader, seconds_elapsed);
			lab->doors[2]->Open(a_shader, seconds_elapsed);
			lab->doors[3]->Open(a_shader, seconds_elapsed);
			lab->doors[4]->Open(a_shader, seconds_elapsed);

		}

	}
	else {

		/*Por cada UI que contenga botones, compruebo si alguno esta en Hover*/
		if (this->menu->isActive == true) {
			SDL_ShowCursor(true);

			this->PickButton(this->menu->Buttons);

		}
		if (this->codeUI->isActive == true) {
			SDL_ShowCursor(true);

			this->PickButton(this->codeUI->Buttons);
		}
		//std::cout << "mouseX:"<<mouse.x <<" mouseY:" <<mouse.y << std::endl;

	}
}

Vector3 PlayStage::nextPosNoCol(Vector3 nextPos, float elapsed_time)
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
		Vector3 push_away = normalize(coll - character_center) * elapsed_time;

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
			Vector3 push_away = normalize(coll - character_center) * elapsed_time;

			nextPos = this->player->pos - push_away; //move to previous pos but a little bit further
													 //reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
													 //velocity = reflect(velocity, collnorm) * 0.95;

		}
	}
	nextPos.y = 0.0;
	return nextPos;
}

void PlayStage::checkNearTaskEntity(float elapsed_time)
{

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
}

void IntroStage::Render(Shader* a_shader, Camera* cam)
{
}

void IntroStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
}

TutorialStage::TutorialStage()
{
}

STAGE_ID TutorialStage::GetId()
{
	return STAGE_ID::TUTORIAL;
}

void TutorialStage::Render(Shader* a_shader, Camera* cam)
{
}

void TutorialStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
}

EndStage::EndStage()
{
}

STAGE_ID EndStage::GetId()
{
	return STAGE_ID::END;
}

void EndStage::Render(Shader* a_shader, Camera* cam)
{
}

void EndStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
}

WinStage::WinStage()
{
}

STAGE_ID WinStage::GetId()
{
	return STAGE_ID::WIN;
}

void WinStage::Render(Shader* a_shader, Camera* cam)
{
}

void WinStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked)
{
}
