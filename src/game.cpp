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

bool test = true;

//some globals
Mesh* mesh = NULL;
//Mesh* mesh2 = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
//
Mesh* planeMesh = NULL;
Texture* planeTexture = NULL;
Matrix44 planeModel;

bool cameralocked = false;
bool isUp = true;
//Mesh* bombMesh = NULL;
//Texture* bombTexture = NULL;
//Matrix44 bombModel;
//Matrix44 bombOffset;
//bool bombAttached = true;


Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;

const int planes_width = 200;
const int planes_heigth = 200;
float padding = 20.0f;

float lod_distance = 200.0f;
float no_render_distance = 1000.0f;

//SDL_GetTicks();

class Base {
public:
	//int a;
	virtual void Foo(){};
};

class Child :public Base {
	void Foo() override{};
};


//class Prop {
//	int id;
//	Mesh* mesh;
//	Texture* texture;
//};
//Prop props[20];
//
//class Entity {
//public:
//	Matrix44 model;
//	Mesh* mesh;
//	Texture* texture;
//};

std::vector<Door*> doors;



Lab* lab;

/*
Vector3 Lerp(Vector3 a, Vector3 b, float t) {
	Vector3 ab = b-a;
	retunr a + (ab * t);

}
*/




//void LoadPropEditorScene(const char* path,  std::vector<Entity*>& entities) {
//	std::string content = "";
//	readFile(path, content);
//	std::stringstream ss(content);
//	int nEnts;
//
//	ss >> nEnts;
//
//	int idEnt = -1;
//
//	while (!ss.eof()) {
//		//read stuff for that entity
//		std::string meshPath = ReadMeshPath(ss);
//		const char* c = &meshPath[0];
//		std::string textPath = ReadTextPath(ss);
//		const char* t = &textPath[0];
//		Vector3 pos = ReadVector3(ss);
//		Vector3 rot = ReadVector3(ss);
//		ss >> idEnt;
//
//		//create and add the entity;
//		Entity* entity = new Entity();
//		entity->mesh = Mesh::Get(c);
//		entity->texture = Texture::Get(t);
//		entity->model.translate(pos.x, pos.y, pos.z);
//		entity->model.rotate(rot.x * DEG2RAD, Vector3(1, 0, 0));
//		entity->model.rotate(rot.y * DEG2RAD, Vector3(0, 1, 0));
//		entity->model.rotate(rot.z * DEG2RAD, Vector3(0, 0, 1));
//
//		entities.push_back(entity);
//
//	}
//}


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

	/*std::cout << sizeof(Base) << std::endl;
	std::cout << sizeof(Child) << std::endl;*/

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

//	bombOffset.setTranslation(0.0f, -2.0f, 0.0f);

	//create our camera
	camera = new Camera();
	
	camera->lookAt(Vector3(-6.5f, 0.6f, -8.8f), Vector3(-6.5f, 0.6f, -6.0f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(35.f, window_width / (float)window_height, 0.1f, 100000.f); //set the projection, we want to be perspective

	//load one texture without using the Texture Manager (Texture::Get would use the manager)
	//texture = new Texture();
	//texture->load("data/texture.tga");
	lab = new Lab();
	// example of loading Mesh from Mesh Manager
	//mesh = Mesh::Get("data/lab.obj");
	//texture = Texture::Get("data/lab.png");
	//mesh2 = Mesh::Get("data/sphere.obj");
	Matrix44 model;
	//lab = new Lab(model, mesh, texture);

	//planeMesh = Mesh::Get("data/spitfire/spitfire.ASE");
	//planeTexture = Texture::Get("data/spitfire/spitfire_color_spec.tga");

	//bombMesh = Mesh::Get("data/torpedo_bullet/torpedo.ASE");
	//bombTexture = Texture::Get("data/torpedo_bullet/torpedo.tga");

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

void RenderMesh(Matrix44 model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam) {

	assert(a_mesh != NULL, "mesh in renderMesh was null");
	if (!a_shader) return;

	//enable shader
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	a_shader->setUniform("u_texture", tex, 0);
	a_shader->setUniform("u_model", model);
	a_shader->setUniform("u_time", time);

	a_mesh->render(GL_TRIANGLES);

	//disable shader
	a_shader->disable();

}



void RenderIslands() {

	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		//shader->setUniform("u_model", m);
		shader->setUniform("u_time", time);

		//float padding = 10000.0f;
		Matrix44 m;
		for (size_t i = 0; i < 10; i++) {
			for (size_t j = 0; j < 10; j++) {
				Vector3 size = mesh->box.halfsize * 2;
				m.setTranslation(size.x * i, 0.0f, size.z * j);
				shader->setUniform("u_model", m);
				//do the draw call
				mesh->render(GL_TRIANGLES);
			}
		}

		//shader->setUniform("u_model", m2);
		//mesh2->render(GL_TRIANGLES);


		//disable shader
		shader->disable();
	}
}

void RenderPlanes() {

	Camera* cam = Game::instance->camera;

	shader->enable();

	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	shader->setUniform("u_texture", planeTexture, 0);
	//enable shader
	shader->setUniform("u_time", time);
	for (size_t i = 0; i < planes_width; i++) {
		for (size_t j = 0; j < planes_heigth; j++)
		{
			Matrix44 model;
			model.translate(i * padding, 0.0f, j * padding);

			Vector3 planePos = model.getTranslation();


			Vector3 camPos = cam->eye;

			float dist = planePos.distance(camPos);

			if (dist > no_render_distance) {
				continue;
			}

			Mesh* mesh = Mesh::Get("data/spitfire/spitfire_low2.ASE");
			if (dist < lod_distance) {
				mesh = Mesh::Get("data/spitfire/spitfire.ASE");
			}
			BoundingBox worldAABB = transformBoundingBox(model, mesh->box);
			if (!cam->testBoxInFrustum(worldAABB.center, worldAABB.halfsize)) {
				continue;
			}
			shader->setUniform("u_model", model);
			mesh->render(GL_TRIANGLES);


		}
	}
	//disable shader
	shader->disable();
}

/*
void AddEntityInFront(Camera* cam) {
	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;


	Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);
	model.scale(3.0f, 3.0f, 3.0f);

	Entity* entity = new Entity();
	entity->model = model;
	entity->mesh = Mesh::Get("data/MachineLab/Objetos/chairDesk.obj");
	entity->texture = Texture::Get("data/MachineLab/Texturas/chairDesk.png");
	entities.push_back(entity);

*/

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

				hasCol = true;
			}
		}
	}
	
	return hasCol;
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


	//RenderMesh(lab->getModel(), lab->getMesh(), lab->getTexture(), shader, camera);

	//lab->Render(shader, camera);

	//if (cameralocked) {
	//	Vector3 eye = planeModel * Vector3(0.0f, 9.0f, 16.0f);
	//	Vector3 center = planeModel * Vector3(0.0f, 0.0f, -20.0f);
	//	Vector3 up = planeModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
	//	camera->lookAt(eye, center, up);
	//}

	//Matrix44 islandmodel;
	//RenderMesh(islandmodel, mesh, texture, shader, camera);
	////RenderMesh(planeModel, planeMesh, planeTexture, shader, camera);
	////RenderPlanes();
	//RenderMesh(bombModel, bombMesh, bombTexture, shader, camera);

	//create model matrix for cube
	//Matrix44 m;
	//m.rotate(angle * DEG2RAD, Vector3(0, 1, 0));

	/*Matrix44 m2;
	m2.translate(150, 0, 0);
	m2.rotateGlobal(angle * DEG2RAD, Vector3(0, 1, 0));
	m2.scale(40, 40, 40);*/


	//RenderIslands();

	for (int r = 0; r < lab->numRooms; r++) {
		for (size_t i = 0; i < lab->rooms[r]->entities.size(); i++)
			{
				Entity* entity = lab->rooms[r]->entities[i];
				RenderMesh(entity->model, entity->mesh, entity->texture, shader, camera);

			}
	}

	for (size_t i = 0; i < lab->doors.size(); i++)
	{
		Door* door = lab->doors[i];
		RenderMesh(door->model, door->mesh, door->texture, shader, camera);

	}
	

	//mesh->renderBounding(islandmodel);

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
	if (cameralocked) {
		float planeSpeed = 50.0f * elapsed_time;
		float rotSpeed = 90.0f * DEG2RAD * elapsed_time;
		if (Input::isKeyPressed(SDL_SCANCODE_W)) planeModel.translate(0.0f, 0.0f, -planeSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) planeModel.translate(0.0f, 0.0f, planeSpeed);

		if (Input::isKeyPressed(SDL_SCANCODE_A)) {
			planeModel.rotate(-rotSpeed, Vector3(0.0, 1.0f, 0.0f));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_D)) {
			planeModel.rotate(rotSpeed, Vector3(0.0, 1.0f, 0.0f));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_E)) {
			planeModel.rotate(rotSpeed, Vector3(0.0, 0.0f, 1.0f));
		}
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) {
			planeModel.rotate(-rotSpeed, Vector3(0.0, 0.0f, 1.0f));
		}
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
		aux->center= camera->center;
		aux->fov = camera->fov;
		aux->move(Vector3(movement.x, movement.y, movement.z) * speed);

		if (!RayPickCheck(camera, movement)) {
			camera->move(Vector3(movement.x, movement.y, movement.z) * speed);
		}
		
	}

	camera->eye.y = isUp ? 0.6 : 0.3;

	/*if (Input::wasKeyPressed(SDL_SCANCODE_F)) {
		bombAttached = false;
	}
	if (bombAttached) {
		bombModel = bombOffset * planeModel;
	}
	else {
		bombModel.translateGlobal(0.0f,-9.8f * elapsed_time, 0.0f);
	}*/

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	//case SDLK_2: AddEntityInFront(camera); break;
	case SDLK_3: lab->doors[0]->Move(shader,camera); break;
	case SDLK_4: lab->doors[1]->Move(shader, camera); break;
	case SDLK_5: lab->doors[2]->Move(shader, camera); break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_LCTRL: isUp = !isUp; break;
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

