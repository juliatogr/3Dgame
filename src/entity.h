#include "framework.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
class Entity
{
public:
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;

	void RenderMesh(Shader* a_shader, Camera* cam);

};


class Door : public Entity {
public:
	bool blocked = true;
	bool isOpen = false;
	float speed = 0.02f;

	void Move(Shader* a_shader, Camera* cam);

};

class Room {
public:
	std::vector<Entity*> entities;
	void LoadEntities(const char* path);
};

class Lab {
public:
	enum class sRooms {
		HALL = 0,
		MACH = 1,
		CHEM = 2
	};
	int numRooms = 3;

	std::vector<Room*> rooms;
	std::vector<Door*> doors;

	Lab();
	void LoadDoors(const char* path);
	void LoadRooms();
};





