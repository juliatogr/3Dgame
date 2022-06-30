
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
	bool activated;
	BUTTONTYPE type;
	Vector4 xyhw;
	//si contiene texto
	std::string text;
	//si contiene un icono
	Texture* icon;
	
	void RenderButtonText(int scale);
	void RenderButtonIcon();

	Button(BUTTONTYPE t, Vector4 v, std::string te);
	Button(BUTTONTYPE t, Vector4 v, Texture* i);
};

class Menu : public UI {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	virtual void RenderMenu() = 0;
	virtual void UpdateMenu() = 0;
};

class InventoryMenu : public Menu {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;
	Inventory* inventory;

	InventoryMenu();
	void RenderMenu();
	void UpdateMenu();

	void ShowNote(int id);
};
class LoadScreen : public Menu {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;
	bool isTutorial;

	LoadScreen();
	void RenderMenu();
	void UpdateMenu();

};

class PauseMenu : public Menu {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	PauseMenu();
	void RenderMenu();
	void UpdateMenu();

}; 

class IntroMenu : public Menu {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	IntroMenu();
	void RenderMenu();
	void UpdateMenu();

};

class TutorialMenu : public Menu {
public:
	Texture* Card;
	std::vector < Texture*> instrucctions;
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	TutorialMenu();
	void RenderMenu();
	void UpdateMenu();

};

class EndingMenu : public Menu {
public:
	Texture* Card;
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	EndingMenu();
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


class CodeScreen : public UI {
public:
	Texture* bg;//fondo de la ui
	std::vector<Button*> Buttons;
	Vector4 xyhw;

	CodeScreen(Lab* lab);
	void RenderCodeScreen(Code* code);

};

class DevelopScreen : public UI {
public:
	std::vector<Button*> Buttons;
	Vector4 xyhw;
	std::vector<bool> correctActives;

	DevelopScreen(Lab* lab);
	void RenderDevelopScreen(Develop* code);
	bool isCorrect(std::vector<Button*> buttons);

};
