#pragma once
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


};

class Lab : Entity {
public:

	Lab(Matrix44 model, Mesh* mesh, Texture* texture);
	Matrix44 getModel();
	Mesh* getMesh();
	Texture* getTexture();

	void Render(Shader* a_shader, Camera* cam);

};

