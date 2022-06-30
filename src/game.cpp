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

bool test = false;

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
std::vector<Stage*> stages;

STAGE_ID currentStage = STAGE_ID::INTRO;



Stage* GetStage(STAGE_ID id) {
	return stages[(int)id];
}

Stage* GetCurrentStage() {
	return GetStage(currentStage);
}

void SetStage(STAGE_ID id) {
	currentStage = id;
}


void InitStages(GameState* state, Shader* shader, Camera* camera) {
	stages.reserve(5);
	//cargamos el Load stage que sera el primero ya que servira de loading
	stages.push_back(new LoadStage());

	std::cout << "cargando Stages" << std::endl;

	//Render Load Image
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

	//renderLoadStage
	stages[0]->Render(shader, camera);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(Game::instance->window);

	stages.push_back(new IntroStage());
	stages.push_back(new TutorialStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());


	std::cout << "All Loaded" << std::endl;
	Game::instance->gameState->isLoaded = true;
}






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


	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//init
	instance->gameState = new GameState();
	InitStages(instance->gameState, shader, camera);

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	// init Audio;
	audio = new Audio();
	audio->PlayGameSound("data/Sound/Fondo/Intro.mp3", true);
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

	GetCurrentStage()->Render(shader, camera);
	if (GetCurrentStage()->GetId() == PLAY) {
		//Draw the floor grid

		if (test == true) {
			drawGrid();
			//render the FPS, Draw Calls, etc
			drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
		}
		if (!Game::instance->gameState->isLoaded) {
			GetStage(LOAD)->Render(shader, camera);
		}

	}

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);

	
}


void Game::update(double seconds_elapsed)
{
	float speed = 0.02 * seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	if (GetCurrentStage()->GetId() == PLAY) {
		

		if (this->gameState->isFinished) {
			SetStage(END);
		}
		if (!Game::instance->gameState->isReLoaded) {
			PlayStage* newPlay = (PlayStage*)GetStage(PLAY);
			newPlay->RePlayStage();
			this->gameState->isFinished = false;
			SetStage(PLAY);


		}
	}
	GetCurrentStage()->Update(seconds_elapsed, cameralocked, speed, shader, camera, mouse_locked);
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	PlayStage* playStage = (PlayStage*)stages[STAGE_ID::PLAY];
	Lab* lab = playStage->lab;
	GameState* state = Game::instance->gameState;
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	case SDLK_8: if (GetCurrentStage()->GetId() == PLAY) playStage->RayPick(camera); break;
	case SDLK_KP_PLUS: if (GetCurrentStage()->GetId() == PLAY) playStage->RotateSelected(10.0f); break;
	case SDLK_KP_MINUS: if (GetCurrentStage()->GetId() == PLAY) playStage->RotateSelected(-10.0f); break;
	case SDLK_i: state->OpenInventory = state->PauseMenu ? false : !state->OpenInventory; break;
	case SDLK_m: state->PauseMenu = !state->PauseMenu; break;
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
			PlayStage* playStage = (PlayStage*)GetCurrentStage();
			InventoryMenu* playMenu = (InventoryMenu*)playStage->invMenu;
			/*Que Ocurre si se hace click a un boton que esta dentro del menu*/
			if ((state->OpenInventory == true)) {
				/*de la lista de botones, busco cual esta activo, menos el de salida*/

				if (playMenu->inventory->Notes.size() > 0) {
					for (int i = 0; i < playMenu->Buttons.size() - 1; i++) {
						Note* currentNote = playMenu->inventory->Notes[i];
						Button* current = playMenu->Buttons[i];
						if (current->type == H) {
							//std::cout << "UIACTIVE: " << current->text << std::endl;

							for (int j = 0; j < playMenu->inventory->Notes.size(); j++) {
								//los botones y la lista de notas deberia ser la misma size, a menos que haya botones disabled, 
								//por lo que hay menos notas que botones disponibles
								if (i == j) {
									currentNote->isShowing = true;
								}
								else {
									playMenu->inventory->Notes[j]->isShowing = false;

								}
								current->type = N;
							}
						}
					}
					/*busco cual nota se esta mostrando*/
					for (int i = 0; i < playMenu->inventory->Notes.size(); i++) {
						Note* current = playMenu->inventory->Notes[i];
						// si se muestra
						if (current->isShowing == true) {
							Button* exit = playMenu->Buttons[playMenu->Buttons.size() - 1];
							/* y esta en hover el boton de salida*/
							if (exit->type == H) {
								//std::cout << "UIACTIVE: Exit" << std::endl;
								/*se cierra la imagen*/
								playMenu->inventory->Notes[i]->isShowing = false;

								exit->type = N;
							}
						}
					}
				}

			}
			CodeScreen* code = playStage->codeUI;
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
				Lab* lab = playStage->lab;
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

			DevelopScreen* dev = playStage->devUI;
			/*Que ocurre si se da click a un boton de la tarea codigo*/
			if (state->DevUiActive) {
				Develop* currentCode;

				for (int i = 0; i < state->devs.size(); i++) {
					if (state->devs[i]->isActive == true) {
						currentCode = state->devs[i];
					}
				}

				//*hago click en los botones para crear un codigo de 4 digitos*/
				for (int i = 0; i < dev->Buttons.size() - 1; i++) {
					Button* current = dev->Buttons[i];
					if (current->type == H) {
						//std::cout << "UIACTIVE: " << current->text << std::endl;
						current->activated = !current->activated;
						//std::cout << "Test: " << currentCode->test << std::endl;
						current->type = N;
					}
				}

				if (dev->isCorrect(dev->Buttons)) {
					state->DevUiActive = false;
					currentCode->isActive = false;
					currentCode->isCompleted = true;

					for (int i = 0; i < dev->Buttons.size(); i++) {
						dev->Buttons[i]->activated = false;
					}
				}

				Button* exit = dev->Buttons[dev->Buttons.size() - 1];
				/* y esta en hover el boton de salida*/
				if (exit->type == H) {
					//std::cout << "UIACTIVE: Exit" << std::endl;
					/*se cierra la CodeUI*/
					state->DevUiActive = false;


					exit->type = N;
				}

			}

			PauseMenu* pauseMenu = playStage->pauseMenu;
			/*Que ocurre si se da click a un boton del men� de pausa*/
			if (state->PauseMenu == true) {
				/*de la lista de botones, busco cual esta activo, menos el de salida*/
				for (int i = 0; i < pauseMenu->Buttons.size() - 1; i++) {
					Button* current = pauseMenu->Buttons[i];

					if (current->type == H) {
						if (current->text == "Continue") {
							state->PauseMenu = false;
						}
						else if (current->text == "Tutorial") {
							state->OpenInventory = false;
							state->PauseMenu = false;
							SetStage(STAGE_ID::TUTORIAL);
						}
					}


				}
				Button* exit = pauseMenu->Buttons[pauseMenu->Buttons.size() - 1];
				/* y esta en hover el boton de salida*/
				if (exit->type == H) {
					state->OpenInventory = false;
					state->PauseMenu = false;
					exit->type = N;
					SetStage(STAGE_ID::INTRO);
				}
			}
		}

		else if (GetCurrentStage()->GetId() == INTRO) {
			IntroStage* introStage = (IntroStage*)GetCurrentStage();
			IntroMenu* introMenu = (IntroMenu*)introStage->menu;
			/*Que Ocurre si se hace click a un boton que esta dentro del menu*/
			/*de la lista de botones, busco cual esta activo, menos el de salida*/
			for (int i = 0; i < introMenu->Buttons.size() - 1; i++) {
				Button* current = introMenu->Buttons[i];
				if (current->type == H) {
					if (current->text == "Play") {
						SetStage(PLAY);
					}
					else if (current->text == "Tutorial") {
						SetStage(TUTORIAL);
					}
				}
			}

			Button* exit = introMenu->Buttons[introMenu->Buttons.size() - 1];
			/* y esta en hover el boton de salida*/
			if (exit->type == H) {

				Game::instance->must_exit = true;


				exit->type = N;
			}
		}

		else if (GetCurrentStage()->GetId() == TUTORIAL) {
			TutorialStage* tutStage = (TutorialStage*)GetCurrentStage();
			TutorialMenu* introMenu = (TutorialMenu*)tutStage->menu;
			/*Que Ocurre si se hace click a un boton que esta dentro del menu*/
			/*de la lista de botones, busco cual esta activo, menos el de salida*/
			if (Game::instance->gameState->isTutorialMenu) {
				for (int i = 0; i < introMenu->Buttons.size() - 3; i++) {
					Button* current = introMenu->Buttons[i];
					if (current->type == H) {
						if (current->text == "Read") {

							Game::instance->gameState->isTutorialMenu = false;
							std::cout << "is tutorial bool: " + Game::instance->gameState->isTutorialMenu << std::endl;
						}
						else if (current->text == "Play") {
							SetStage(PLAY);

						}
					}
				}
			}
			if (!Game::instance->gameState->isTutorialMenu) {
				Button* exit = introMenu->Buttons[introMenu->Buttons.size() - 1];
				Button* right = introMenu->Buttons[introMenu->Buttons.size() - 3];
				Button* left = introMenu->Buttons[introMenu->Buttons.size() - 2];
				/* y esta en hover el boton de salida*/
				if (exit->type == H) {
					Game::instance->gameState->isTutorialMenu = true;
					exit->type = N;
				}
				if (right->type == H) {
					Game::instance->gameState->currentInstrucction += 1;
					std::cout << "next" << std::endl;
					right->type = N;
				}
				if (left->type == H) {
					std::cout << "ant" << std::endl;
					Game::instance->gameState->currentInstrucction -= 1;
					left->type = N;
				}


				if (Game::instance->gameState->currentInstrucction == introMenu->instrucctions.size()) {
					Game::instance->gameState->currentInstrucction = 0;
				}if (Game::instance->gameState->currentInstrucction < 0) {
					Game::instance->gameState->currentInstrucction = introMenu->instrucctions.size() - 1;
				}
			}
		}
		else if (GetCurrentStage()->GetId() == LOAD) {
			LoadStage* loadStage = (LoadStage*)GetCurrentStage();
			Button* exit = loadStage->menu->Buttons[0];
			/* y esta en hover el boton de salida*/
			if (exit->type == H) {

				SetStage(TUTORIAL);


				exit->type = N;
			}
		}


		else if (GetCurrentStage()->GetId() == END) {
			EndStage* endStage = (EndStage*)GetCurrentStage();
			EndingMenu* endMenu = (EndingMenu*)endStage->menu;
			/*Que Ocurre si se hace click a un boton que esta dentro del menu*/
			/*de la lista de botones, busco cual esta activo, menos el de salida*/
			for (int i = 0; i < endMenu->Buttons.size() - 1; i++) {
				Button* current = endMenu->Buttons[i];
				if (current->type == H) {
					Game::instance->gameState->isReLoaded = false;

					if (current->text == "Tutorial") {
						SetStage(TUTORIAL);
					}
					else if (current->text == "Play") {
						SetStage(PLAY);
					}
				}
			}
			Button* exit = endMenu->Buttons[endMenu->Buttons.size() - 1];
			/* y esta en hover el boton de salida*/
			if (exit->type == H) {

				Game::instance->must_exit = true;


				exit->type = N;
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

