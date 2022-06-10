#include "GameStage.h"
#include "game.h"
#include "input.h"

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

void GameStage::RotateSelected(float angleDegrees) {
	if (this->selectedEntity == NULL) {
		return;
	}

	this->selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
}

boolean GameStage::RayPickCheck(Camera* cam, Vector3 movement) {

	boolean hasCol = false;

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	movement.x = movement.x / movement.z;
	movement.y = movement.y / movement.z;
	Vector3 dir = cam->getRayDirection(movement.x, movement.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (int r = 0; r < lab->numRooms; r++) {
		for (size_t i = 0; i < lab->rooms[r]->entities.size(); i++) {
			Entity* entity = lab->rooms[r]->entities[i];
			Vector3 pos;
			Vector3 normal;
			if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal, 0.3)) {

				hasCol = true;
			}
		}
	}

	return hasCol;
}


STAGE_ID PlayStage::GetId()
{
	return STAGE_ID::PLAY;
}

PlayStage::PlayStage()
{
	this->lab = new Lab();
	this->player = new Player();

}


void PlayStage::Render(Shader* a_shader, Camera* cam)
{
	Matrix44 playerModel;
	playerModel.translate(this->player->pos.x, this->player->pos.y, this->player->pos.z);
	playerModel.rotate(this->player->yaw * DEG2RAD, Vector3(0, 1, 0));

	Matrix44 camModel = playerModel;
	camModel.rotate(this->player->pitch * DEG2RAD, Vector3(1, 0, 0));

	Vector3 eye = playerModel * Vector3(0, 0.6, 0);
	Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
	Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));

	cam->lookAt(eye, center, up);
	

	for (int r = 0; r < this->lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->entities.size(); i++)
		{
			Entity* entity = this->lab->rooms[r]->entities[i];
			entity->RenderMesh(a_shader, cam);

		}
	}

	for (size_t i = 0; i < this->lab->doors.size(); i++)
	{
		Door* door = this->lab->doors[i];
		door->RenderMesh(a_shader, cam);

	}
}

void PlayStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera, bool mouse_locked)
{
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

	Vector3 nextPos = this->player->pos + playerVel;
	//calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
	Vector3 character_center = nextPos + Vector3(0, 0.4, 0);
	//para cada objecto de la escena...


	int numDoors = lab->doors.size();

	for (int r = 0; r < numDoors; r++) {
		Entity* entity = this->lab->doors[r];

		Vector3 coll;
		Vector3 collnorm;

		//comprobamos si colisiona el objeto con la esfera (radio 3)
		if (!entity->mesh->testSphereCollision(entity->model, character_center, 0.32f, coll, collnorm))
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

			if (!entity->mesh->testSphereCollision(entity->model, character_center, 0.32f, coll, collnorm)) {
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

	this->player->pos = nextPos;

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

void IntroStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera, bool mouse_locked)
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

void TutorialStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera,  bool mouse_locked)
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

void EndStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera,  bool mouse_locked)
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

void WinStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera,  bool mouse_locked)
{
}
