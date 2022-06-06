#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"

#include <cmath>

#include "entity.h"
#include "GameStage.h"
#include "Player.h"

bool test = true;

//some globals
Shader* shader = NULL;

bool cameralocked = false;

Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;
Entity* selectedEntity = NULL;

const int planes_width = 200;
const int planes_heigth = 200;
float padding = 20.0f;

float lod_distance = 200.0f;
float no_render_distance = 1000.0f;

/*Stages*/
std::vector<GameStage*> stages;
STAGE_ID currentStage = STAGE_ID::PLAY;


GameStage* GetStage(STAGE_ID id) {
	return stages[(int)id];
}

GameStage* GetCurrentStage() {
	return GetStage(currentStage);
}

void SetStage(STAGE_ID id) {
	currentStage = id;
}

void InitStages() {
	stages.reserve(5);
	//stages.size();//0
	//stages.capacity();//4
	stages.push_back(new IntroStage());
	stages.push_back(new TutorialStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());
	stages.push_back(new WinStage());

	//stages.size();//4

}

//SDL_GetTicks();

//Lab* lab;

Player player;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	

	camera->lookAt(Vector3(-1.f, 0.6f, -8.8f), Vector3(-6.5f, 0.6f, -6.0f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0

	camera->setPerspective(35.f, window_width / (float)window_height, 0.1f, 100000.f); //set the projection, we want to be perspective

	player.pos = Vector3(-1.f, 0.0f, -8.8f);
	//load one texture without using the Texture Manager (Texture::Get would use the manager)

	//lab = new Lab();
	//init
	InitStages();
	
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}
/*
boolean RayPickCheck(Camera* cam, Vector3 movement) {

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
				std::cout << "Col" << std::endl;
				hasCol = true;
			}
		}
	}
	
	return hasCol;
}*/
/*
//RayPickCheck v2
void RayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (int r = 0; r < lab->numRooms; r++) {
		for (size_t i = 0; i < lab->rooms[r]->entities.size(); i++) {
			Entity* entity = lab->rooms[r]->entities[i];
			Vector3 pos;
			Vector3 normal;
			if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {

				selectedEntity = entity;
				break;
			}
		}
	}
}


void RotateSelected(float angleDegrees) {
	if (selectedEntity == NULL) {
		return;
	}
	
	selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0,1,0));
}*/



//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.7, 0.7, 0.8, 1.0);			// white background to simulate the ceiling

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	/*Matrix44 playerModel;
	playerModel.translate(player.pos.x, player.pos.y, player.pos.z);
	playerModel.rotate(player.yaw * DEG2RAD, Vector3(0, 1, 0));

	Matrix44 camModel = playerModel;
	camModel.rotate(player.pitch * DEG2RAD, Vector3(1, 0, 0));

	Vector3 eye = playerModel * Vector3(0, 0.6, 0);
	Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
	Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));

	camera->lookAt(eye, center, up);*/



	GetCurrentStage()->Render(shader, camera);

	//Draw the floor grid
	drawGrid();

	if (test == true) {
		//render the FPS, Draw Calls, etc
		drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	}
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}



void Game::update(double seconds_elapsed)
{
	float speed = 0.02*seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		cameralocked = !cameralocked;
	}


	GetCurrentStage()->Update(seconds_elapsed, cameralocked, elapsed_time, speed, camera, player);

}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	Lab* lab = GetCurrentStage()->lab;
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	//case SDLK_2: AddEntityInFront(camera); break;
	case SDLK_3: lab->doors[0]->Move(shader,camera); break;
	case SDLK_4: lab->doors[1]->Move(shader, camera); break;
	case SDLK_5: lab->doors[2]->Move(shader, camera); break;

	case SDLK_6: lab->doors[3]->Move(shader, camera); break;

	case SDLK_7: GetCurrentStage()->RayPick(camera); break; 
	case SDLK_KP_PLUS: GetCurrentStage()->RotateSelected(10.0f); break;
	case SDLK_KP_MINUS: GetCurrentStage()->RotateSelected(10.0f); break;

	}

}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	} 
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
	std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	camera->aspect = width / (float)height;
	window_width = width;
	window_height = height;
}

