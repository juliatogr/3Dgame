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
	camera->lookAt(Vector3(-2.f, 1.2f, -17.6f), Vector3(-13.f, 1.2f, -12.0f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(35.f, window_width / (float)window_height, 0.1f, 100000.f); //set the projection, we want to be perspective

	//init
	instance->gameState = new GameState();

	InitStages();

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}




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

	if (GetCurrentStage()->GetId() == PLAY) {

		GetCurrentStage()->Render(shader, camera);
		//Draw the floor grid
		drawGrid();

		if (test == true) {
			//render the FPS, Draw Calls, etc
			drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
		}
	}

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}


void Game::update(double seconds_elapsed)
{
	float speed = 0.02 * seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	if (GetCurrentStage()->GetId() == PLAY) {
		if (Game::instance->gameState->UIActive == true) {
			//pickButton();
		}
		GetCurrentStage()->Update(seconds_elapsed, cameralocked, speed, shader, camera, mouse_locked);
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	Lab* lab = GetCurrentStage()->lab;
	PlayMenu* menu = GetCurrentStage()->menu;
	GameState* state = Game::instance->gameState;
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
		//case SDLK_2: AddEntityInFront(camera); break;
	case SDLK_3: lab->doors[0]->isOpening = true; break;
	case SDLK_4: lab->doors[1]->isOpening = true; break;
	case SDLK_5: lab->doors[2]->isOpening = true; break;
	case SDLK_6: lab->doors[3]->isOpening = true; break;
	case SDLK_7: lab->doors[4]->isOpening = true; break;
	case SDLK_8: GetCurrentStage()->RayPick(camera); break;
	case SDLK_KP_PLUS: GetCurrentStage()->RotateSelected(10.0f); break;
	case SDLK_KP_MINUS: GetCurrentStage()->RotateSelected(-10.0f); break;
	case SDLK_m: state->OpenInventory = !state->OpenInventory; break;
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
	GameState* state = Game::instance->gameState;

	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
	if (event.button == SDL_BUTTON_LEFT) //left mouse
	{
		if (GetCurrentStage()->GetId() == PLAY) {

			/*Que Ocurre si se hace click a un boton que esta dentro del menu*/
			PlayMenu* menu = GetCurrentStage()->menu;
			if ((state->OpenInventory == true)) {
				/*de la lista de botones, busco cual esta activo, menos el de salida*/
				for (int i = 0; i < menu->Buttons.size() - 1; i++) {
					Button* current = menu->Buttons[i];
					if (current->type == H) {
						//std::cout << "UIACTIVE: " << current->text << std::endl;
						menu->inventory->Notes[i]->isShowing = true;
						current->type = N;
					}

				}
				/*busco cual nota se esta mostrando*/
				for (int i = 0; i < menu->inventory->Notes.size(); i++) {
					Note* current = menu->inventory->Notes[i];
					// si se muestra
					if (current->isShowing == true) {
						Button* exit = menu->Buttons[menu->Buttons.size() - 1];
						/* y esta en hover el boton de salida*/
						if (exit->type == H) {
							//std::cout << "UIACTIVE: Exit" << std::endl;
							/*se cierra la imagen*/
							menu->inventory->Notes[i]->isShowing = false;

							exit->type = N;
						}
					}
				}
			}

			CodeScreen* code = GetCurrentStage()->codeUI;
			/*Que ocurre si se da click a un boton de la tarea codigo*/
			if (state->CodeUiActive) {
				Code* currentCode;

				for (int i = 0; i < state->codes.size(); i++) {
					if (state->codes[i]->isActive == true) {
						currentCode = state->codes[i];
					}
				}

				//*hago click en los botones para crear un codigo de 4 digitos*/
				for (int i = 0; i < code->Buttons.size() - 2; i++) {
					Button* current = code->Buttons[i];
					if (current->type == H) {
						//std::cout << "UIACTIVE: " << current->text << std::endl;
						currentCode->test = (currentCode->test + current->text);

						//std::cout << "Test: " << currentCode->test << std::endl;


						current->type = N;
					}
				}


				Button* enter = code->Buttons[code->Buttons.size() - 1];
				Lab* lab = GetCurrentStage()->lab;
				//* y esta en hover el boton de salida*/
				if (enter->type == H) {
					if (currentCode->obj == currentCode->test) {
						state->CodeUiActive = false;
						currentCode->isActive = false;
						currentCode->isCompleted = true;
						//std::cout << "UIACTIVE: Enter Correct" << std::endl;
						//si esta la clave correcta se abre la puerta correspondiente
						currentCode->OpenDoors();

					}
					else {
						currentCode->test = "";
						//std::cout << "UIACTIVE: Enter Wrong" << std::endl;

					}

					enter->type = N;
				}

				Button* exit = code->Buttons[code->Buttons.size() - 2];
				/* y esta en hover el boton de salida*/
				if (exit->type == H) {
					//std::cout << "UIACTIVE: Exit" << std::endl;
					/*se cierra la CodeUI*/
					currentCode->test = "";
					state->CodeUiActive = false;


					exit->type = N;
				}

			}

		}

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

