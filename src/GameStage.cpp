#include "GameStage.h"
#include "game.h"
#include "input.h"

Lab* GameStage::GetLab()
{
	return this->lab;
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

void GameStage::Colision(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
{
	if (cameralocked) {
		float planeSpeed = 50.0f * elapsed_time;
		float rotSpeed = 90.0f * DEG2RAD * elapsed_time;

	}
	else {
		//async input to move the camera around
		Vector3 movement;

		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 5; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) movement.z = 1.0f;
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) movement.z = -1.0f;
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) movement.x = 1.0f;
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) movement.x = -1.0f;
		if (Input::isKeyPressed(SDL_SCANCODE_E)) movement.y = -1.0f;
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) movement.y = 1.0f;

		Camera* aux = new Camera();
		aux->eye = camera->eye;
		aux->far_plane = camera->far_plane;
		aux->center = camera->center;
		aux->fov = camera->fov;
		aux->move(Vector3(movement.x, movement.y, movement.z) * speed);
		if (!RayPickCheck(camera, movement)) {
			camera->move(Vector3(movement.x, movement.y, movement.z) * speed);
		}

	}
}

STAGE_ID PlayStage::GetId()
{
	return STAGE_ID::PLAY;
}

PlayStage::PlayStage()
{
	this->lab = new Lab();
}


void PlayStage::Render(Shader* a_shader, Camera* cam)
{
	for (int r = 0; r < lab->numRooms; r++) {
		for (size_t i = 0; i < this->lab->rooms[r]->entities.size(); i++)
		{
			Entity* entity = this->lab->rooms[r]->entities[i];
			entity->RenderMesh(a_shader, cam);
			//RenderMesh(entity->model, entity->mesh, entity->texture, shader, camera);

		}
	}

	for (size_t i = 0; i < this->lab->doors.size(); i++)
	{
		Door* door = this->lab->doors[i];
		door->RenderMesh(a_shader, cam);
		//RenderMesh(door->model, door->mesh, door->texture, shader, camera);

	}
}

void PlayStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
{
	this->Colision(seconds_elapsed, cameralocked, elapsed_time, speed, camera);
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

void IntroStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
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

void TutorialStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
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

void EndStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
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

void WinStage::Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Camera* camera)
{
}
