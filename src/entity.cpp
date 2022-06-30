#include "entity.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "game.h"


Ground::Ground() {
	this->mesh = new Mesh();
	this->mesh->createPlane(500);
	this->texture = Texture::Get("data/Estructuras/Floors/Floor1.tga");
}

void Entity::RenderMesh(Shader* a_shader, Camera* cam) {

	assert(this->mesh != NULL, "mesh in renderMesh was null");
	if (!a_shader) return;

	//enable shader
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	a_shader->setUniform("u_texture", this->texture, 0);
	a_shader->setUniform("u_model", this->model);
	a_shader->setUniform("u_time", time);

	this->mesh->render(GL_TRIANGLES);

	//disable shader
	a_shader->disable();
}

std::string ReadMeshPath(std::stringstream& ss) {

	std::string meshPath;
	std::string type;
	ss >> type;
	if (ss.peek() == ' ') ss.ignore();
	ss >> meshPath;
	if (ss.peek() == ' ') ss.ignore();
	return meshPath;
}

std::string ReadEntityType(std::stringstream& ss) {

	std::string entityType;
	std::string type;
	ss >> type;
	if (ss.peek() == ' ') ss.ignore();
	ss >> entityType;
	if (ss.peek() == ' ') ss.ignore();
	return entityType;
}

int ReadEntityID(std::stringstream& ss) {

	int entityType;
	std::string type;
	ss >> type;
	if (ss.peek() == ' ') ss.ignore();
	ss >> entityType;
	if (ss.peek() == ' ') ss.ignore();
	return entityType;
}


std::string ReadTextPath(std::stringstream& ss) {

	std::string textPath;

	std::string type;
	ss >> type;
	if (ss.peek() == ' ') ss.ignore();
	ss >> textPath;
	if (ss.peek() == ' ') ss.ignore();
	return textPath;
}


std::string ReadNEntities(std::stringstream& ss) {

	std::string type;

	std::string count;
	ss >> type;
	if (ss.peek() == ' ') ss.ignore();
	ss >> count;
	if (ss.peek() == ' ') ss.ignore();
	return count;
}

Vector3 ReadVector3(std::stringstream& ss) {
	Vector3 vec;
	ss >> vec.x;
	if (ss.peek() == ',') ss.ignore();
	ss >> vec.y;
	if (ss.peek() == ',') ss.ignore();
	ss >> vec.z;
	return vec;
}

void Room::LoadEntities(const char* path) {
	std::string content = "";
	readFile(path, content);
	std::stringstream ss(content);
	int nEntities = std::stoi(ReadNEntities(ss));
	int count = 0;
	while (count != nEntities) {

		std::string meshPath = ReadMeshPath(ss);
		const char* c = &meshPath[0];
		std::string textPath = ReadTextPath(ss);
		const char* t = &textPath[0];
		Vector3 pos = ReadVector3(ss);
		Vector3 rot = ReadVector3(ss);
		Vector3 scl = ReadVector3(ss);

		std::cout << c << std::endl;
		std::cout << t << std::endl;

		//create and add the entity;
		Entity* entity = new Entity();
		entity->mesh = Mesh::Get(c);
		entity->texture = Texture::Get(t);
		Matrix44 model;
		model.translate(pos.x, pos.y, pos.z);
		model.rotate(rot.x * DEG2RAD, Vector3(1, 0, 0));
		model.rotate(rot.y * DEG2RAD, Vector3(0, 1, 0));
		model.rotate(rot.z * DEG2RAD, Vector3(0, 0, 1));
		model.scale(scl.x, scl.y, scl.z);

		entity->model = model;

		entities.push_back(entity);
		count++;
	}
}


void Room::LoadTaskEntities(const char* path) {
	std::string content = "";
	readFile(path, content);
	std::stringstream ss(content);
	int nEntities = std::stoi(ReadNEntities(ss));
	int count = 0;
	while (count != nEntities) {

		std::string entType = ReadEntityType(ss);
		int entId = ReadEntityID(ss);
		std::string entImg;
		const char* im;
		if (entType == "Note") {
			 entImg = ReadTextPath(ss);
			 im = &entImg[0];
			 std::cout << im << std::endl;
		}
		std::string meshPath = ReadMeshPath(ss);
		const char* c = &meshPath[0];
		std::string textPath = ReadTextPath(ss);
		const char* t = &textPath[0];
		Vector3 pos = ReadVector3(ss);
		Vector3 rot = ReadVector3(ss);
		Vector3 scl = ReadVector3(ss);

		std::cout << c << std::endl;
		std::cout << t << std::endl;

		//create and add the entity;

		TaskEntity* entity = new TaskEntity();

		if (entType == "Note") {
			Note* noteEnt = new Note(entity, entity->img, entId);
			entity = noteEnt;
			entity->img = Texture::Get(im);
		}
		if (entType == "PC") {
			Computer* pcEnt = new Computer(entity, entId);
			entity = pcEnt;
		}
		if (entType == "Console") {
			Console* consoleEnt = new Console(entity, entId);
			entity = consoleEnt;
		}
		entity->id = entId;
		entity->mesh = Mesh::Get(c);
		entity->texture = Texture::Get(t);
		entity->rot = rot;
		Matrix44 model;
		model.translate(pos.x, pos.y, pos.z);
		model.rotate(rot.x * DEG2RAD, Vector3(1, 0, 0));
		model.rotate(rot.y * DEG2RAD, Vector3(0, 1, 0));
		model.rotate(rot.z * DEG2RAD, Vector3(0, 0, 1));
		model.scale(scl.x, scl.y, scl.z);

		entity->model = model;
		entity->pos = pos;

		taskEntities.push_back(entity);
		count++;
	}
}

void Lab::LoadDoors(const char* path) {
	std::string content = "";
	readFile(path, content);
	std::stringstream ss(content);
	int nEntities = std::stoi(ReadNEntities(ss));
	int count = 0;
	while (count != nEntities) {

		std::string meshPath = ReadMeshPath(ss);
		const char* c = &meshPath[0];
		std::string textPath = ReadTextPath(ss);
		const char* t = &textPath[0];
		Vector3 pos = ReadVector3(ss);
		Vector3 rot = ReadVector3(ss);
		Vector3 scl = ReadVector3(ss);

		std::cout << c << std::endl;
		std::cout << t << std::endl;

		//create and add the entity;
		Door* door = new Door();
		door->mesh = Mesh::Get(c);
		door->texture = Texture::Get(t);
		Matrix44 model;
		model.translate(pos.x, pos.y, pos.z);
		model.rotate(rot.x * DEG2RAD, Vector3(1, 0, 0));
		model.rotate(rot.y * DEG2RAD, Vector3(0, 1, 0));
		model.rotate(rot.z * DEG2RAD, Vector3(0, 0, 1));
		model.scale(scl.x, scl.y, scl.z);

		door->model = model;

		doors.push_back(door);
		count++;
	}
}

Lab::Lab() {

	this->ground = new Ground();
	for (int i = 0; i < this->numRooms; i++) {
		Room* room = new Room();
		this->rooms.push_back(room);
	}
	LoadRooms();
	LoadDoors("data/DoorObjects.scene");
	LoadRoomsTaskEntities();
}

void Lab::LoadRooms() {

	this->rooms[int(ROOMS_ID::HALL)]->LoadEntities("data/EstructuraPasillo.scene");
	this->rooms[int(ROOMS_ID::HALL)]->LoadEntities("data/ObjetosPasillo.scene");
	this->rooms[int(ROOMS_ID::CHEM)]->LoadEntities("data/EstructuraChemicalLab.scene");
	this->rooms[int(ROOMS_ID::CHEM)]->LoadEntities("data/ObjetosChemicalLab.scene");
	this->rooms[int(ROOMS_ID::MACH)]->LoadEntities("data/EstructuraMachineLab.scene");
	this->rooms[int(ROOMS_ID::MACH)]->LoadEntities("data/ObjetosMachineLab.scene");

}

void Lab::LoadRoomsTaskEntities() {

	this->rooms[int(ROOMS_ID::HALL)]->LoadTaskEntities("data/ObjetosTareasPasillo.scene");
	this->rooms[int(ROOMS_ID::CHEM)]->LoadTaskEntities("data/ObjetosTareasChemicalLab.scene");
	this->rooms[int(ROOMS_ID::MACH)]->LoadTaskEntities("data/ObjetosTareasMachineLab.scene");

}

void Door::Open(Shader* a_shader, float seconds_elapsed)
{
	if (this->isOpening == true) {
		currDist += -0.2f * speed;
		if (abs(currDist) < maxDist) {
			this->model.translate(-1.0f * speed, 0.0f, 0.0f);
		}
		else {
			this->isOpening = false;
		}
	}

}

void Note::Show(Shader* a_shader, Camera* cam) {

}

void TaskEntity::viewToTask(Camera* cam, float seconds_elapsed) {

	Vector3 viewCenter = this->pos;
	Vector3 viewEye = viewCenter + Vector3(0, 0.5, 0);
	Vector3 viewUp;

	viewUp.z = this->rot.y >= 180? 1 : -1 ;
	if (this->type == CONSOLE) {
		viewUp.z = 0;
		viewUp.y = 1;
		viewCenter = viewCenter + Vector3(0, 0.15, 0.3);
		viewEye = viewCenter + Vector3(-1, 0, 0);
	}
	if (cam->eye.x != viewEye.x || cam->eye.y != viewEye.y || cam->eye.z != viewEye.z) {
		cam->lookAt(viewEye, viewCenter, viewUp);

	}
}
void TaskEntity::returnView(Camera* cam, float seconds_elapsed) {
	this->isViewed = true;
	this->isViewing = false;

	cam->lookAt(this->pos + Vector3(0, 0.3, 0), this->pos, Vector3(0, 1, 0));
	this->isReturning = false;
}

Note::Note(Entity* e, Texture* i, int nid) {
	this->id = nid;
	this->canBeSaved = true;
	this->isShowing = false;
	this->texture = e->texture;
	this->img = i;
	this->type = NOTE;
}

Computer::Computer(Entity* e, int i)
{
	this->isShowing = false;
	this->type = PC;
	this->id = i;
}

Console::Console(Entity* e, int i)
{
	this->isShowing = false;
	this->type = CONSOLE;
	this->id = i;
}
