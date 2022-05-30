#include "entity.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"


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
	
	for (int i = 0; i < this->numRooms; i++) {
		Room* room = new Room();
		this->rooms.push_back(room);
	}
	LoadRooms();
	LoadDoors("data/DoorObjects.scene");
	
}

void Lab::LoadRooms() {

	this->rooms[int(sRooms::HALL)]->LoadEntities("data/EstructuraPasillo.scene");
	this->rooms[int(sRooms::HALL)]->LoadEntities("data/ObjetosPasillo.scene");
	this->rooms[int(sRooms::CHEM)]->LoadEntities("data/EstructuraChemicalLab.scene");
	this->rooms[int(sRooms::CHEM)]->LoadEntities("data/ObjetosChemicalLab.scene");
	this->rooms[int(sRooms::MACH)]->LoadEntities("data/EstructuraMachineLab.scene");
	this->rooms[int(sRooms::MACH)]->LoadEntities("data/ObjetosMachineLab.scene");

}

void Door::Move(Shader* a_shader, Camera* cam)
{
	
	this->model.translate(-1.0f * speed, 0.0f, 0.0f);
}
