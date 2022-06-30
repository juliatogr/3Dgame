
#pragma once

#include "framework.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"

enum class ROOMS_ID {
	HALL = 0,
	MACH = 1,
	CHEM = 2
};

class Entity
{
public:
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;

	void RenderMesh(Shader* a_shader, Camera* cam);

};
enum TASKTYPE {
	NOTE,
	PC,
	CONSOLE

};

class TaskEntity : public Entity {
public:
	bool isViewing = false;
	bool isViewed = false;
	bool isReturning = false;
	bool canBeSaved = false;
	bool isSaved = false;
	bool updatedId = false;

	Vector3 pos;
	Vector3 rot;
	TASKTYPE type;
	Texture* img;
	int id;//id of the task, in this case as is a PC, the task would be Code

	void viewToTask(Camera* cam, float seconds_elapsed);
	void returnView(Camera* cam, float seconds_elapsed);
};

class Note : public TaskEntity {
public:
	bool isShowing;
	Note(Entity* e, Texture* i, int nid);
	void Show(Shader* a_shader, Camera* cam);
};
class Computer : public TaskEntity {
public:
	bool isShowing;
	Computer(Entity* e, int i);
};

class Console : public TaskEntity {
public:
	bool isShowing;
	Console(Entity* e, int i);
};
class Ground : public Entity {
public:
	bool canBeSaved = false;
	Ground();
};

class Door : public Entity {
public:

	bool canBeSaved = false;
	bool isOpening = false;
	float maxDist = 0.5;
	float currDist = 0;
	float speed = 0.001f;

	void Open(Shader* a_shader, float seconds_elapsed);

};

class Room {
public:
	std::vector<Entity*> entities;
	std::vector<TaskEntity*> taskEntities;
	void LoadEntities(const char* path);
	void LoadTaskEntities(const char* path);
};

class Lab {
public:

	int numRooms = 3;
	int numDoors = 4;

	Ground* ground;
	std::vector<Room*> rooms;
	std::vector<Door*> doors;

	Lab();
	void LoadDoors(const char* path);
	void LoadRooms();
	void LoadRoomsTaskEntities();
};



