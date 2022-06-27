
#pragma once
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "mesh.h"
#include "Inventory.h"
#include "Tasks.h"

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
	Texture* Active;
	Texture* Disabled;
	Texture* Hover;
	Texture* Normal;
	BUTTONTYPE type;
	Vector4 xyhw;
	//si contiene texto
	const char* text;
	//si contiene un icono
	Texture* icon;
	
	void RenderButtonText();
	void RenderButtonIcon();

	Button(BUTTONTYPE t, Vector4 v, const char* te);
	Button(BUTTONTYPE t, Vector4 v, Texture* i);
};


class Menu : public UI {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;
	Inventory* inventory;

	Menu();
	void RenderMenu();
	void UpdateMenu();

	void ShowNote(int id);


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


class CodeScreen : public UI {
public:
	Texture* bg;//fondo de la ui
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	CodeScreen(Lab* lab);
	void RenderCodeScreen(Code* code);

};
