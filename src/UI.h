#pragma once
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "mesh.h"
#include "Inventory.h"

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
	const char* text;

	void RenderButton();

	Button(BUTTONTYPE t, Vector4 v, const char* te);
};


class Menu : public UI {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	bool isActive;
	Vector4 xyhw;
	Inventory* inventory;

	Menu();
	void RenderMenu();
	void UpdateMenu();


};

class PopUpMessage : public UI {
public:
	int Id;
	const char* text;
	Texture* bg;
	Vector4 xyhw;

	bool isActive;

	PopUpMessage(int i, const char* t, Texture* b, Vector4 xyhw);
	void RenderPopUp();


};
