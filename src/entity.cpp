#include "entity.h"

Lab::Lab(Matrix44 model, Mesh* mesh, Texture* texture)
{
	this->model = model;
	this->mesh = mesh;
	this->texture = texture;
}

Matrix44 Lab::getModel()
{
	return this->model;
}

Mesh* Lab::getMesh()
{
	return this->mesh;
}

Texture* Lab::getTexture()
{
	return this->texture;
}

void Lab::Render( Shader* a_shader, Camera* cam)
{
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
