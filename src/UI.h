#pragma once
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "mesh.h"

class UI
{
public:
	Shader* a_shader;
	Camera cam2D;
	Mesh quad;
	Matrix44 quadModel;

	UI();

	void Render(Texture* texture, float x, float y, float w, float h, bool u);
};

enum BUTTONTYPE {
	A,
	D,
	H,
	N
};

class Button : public UI{
public:
	bool onClick;
	Texture* Active;
	Texture* Disabled;
	Texture* Hover;
	Texture* Normal;
	BUTTONTYPE type;
	Vector4 xyhw;
	std::string text;

	void RenderButton();

	Button(BUTTONTYPE t, Vector4 v);
};


class Menu : public UI {
public:
	Texture* Card;
	std::vector<Button*> Buttons;

	Menu();
	void RenderMenu();

};
