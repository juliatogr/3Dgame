#include "UI.h"
#include "game.h"
#include "input.h"

int count_chars(std::string s) {
	int count = 0;
	for (int i = 0; i < s.size(); i++)
		count++;

	return count;
}

UI::UI()
{
	this->a_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

}

void UI::Render(Texture* texture, float x, float y, float w, float h, bool u) {
	// code to render the images
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int windowWidth = Game::instance->window_width;
	int windowHeight = Game::instance->window_height;
	this->quad.createQuad(x, y, w, h, u);

	//Camera cam2D = this->cam2D;

	this->cam2D.setOrthographic(0, windowWidth, windowHeight, 0, -1, 1);

	if (!this->a_shader) return;

	//enable shader
	this->a_shader->enable();

	//upload uniforms
	this->a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	this->a_shader->setUniform("u_viewprojection", this->cam2D.viewprojection_matrix);
	if (texture != NULL) {
		this->a_shader->setUniform("u_texture", texture, 0);
	}
	this->a_shader->setUniform("u_tex_tilling", 1.0f);
	//this->quadModel.translate(sin(Game::instance->time)*0.20,0,0);this->type
	this->a_shader->setUniform("u_model", this->quadModel);
	this->a_shader->setUniform("u_time", time);

	this->quad.render(GL_TRIANGLES);

	//disable shader
	this->a_shader->disable();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}


void Button::RenderButtonText()
{
	if (this->type == A) {
		this->Render(this->Active, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == D) {
		this->Render(this->Disabled, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == H) {
		this->Render(this->Hover, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == N) {
		this->Render(this->Normal, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	int nC = count_chars(this->text);
	drawText(this->xyhw.x - nC * 6, this->xyhw.y - (this->xyhw.w / 10), this->text, Vector3(1, 1, 1), 2);
}
void Button::RenderButtonIcon()
{
	if (this->type == A) {
		this->Render(this->Active, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == D) {
		this->Render(this->Disabled, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == H) {
		this->Render(this->Hover, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	if (this->type == N) {
		this->Render(this->Normal, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	this->Render(this->icon, this->xyhw.x, this->xyhw.y, this->xyhw.z / 2, this->xyhw.w / 2, false);
}



Button::Button(BUTTONTYPE t, Vector4 v, std::string te)
{
	this->Active = Texture::Get("data/UI/Buttons/Button Active.png");
	this->Disabled = Texture::Get("data/UI/Buttons/Button Disable.png");
	this->Hover = Texture::Get("data/UI/Buttons/Button Hover.png");
	this->Normal = Texture::Get("data/UI/Buttons/Button Normal.png");
	this->type = t;
	this->xyhw = v;
	this->text = te;

}

Button::Button(BUTTONTYPE t, Vector4 v, Texture* i)
{
	this->Active = Texture::Get("data/UI/Buttons/Button Active.png");
	this->Disabled = Texture::Get("data/UI/Buttons/Button Disable.png");
	this->Hover = Texture::Get("data/UI/Buttons/Button Hover.png");
	this->Normal = Texture::Get("data/UI/Buttons/Button Normal.png");
	this->type = t;
	this->xyhw = v;
	this->icon = i;
}

InventoryMenu::InventoryMenu()
{
	this->Card = Texture::Get("data/UI/Cards/Card X1.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width + 50, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(100, 250, 100, 100);
	int r = 3;
	int c = 3;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			//std::cout << "cell button added" << std::endl;

			this->Buttons.push_back(new Button(D, Vector4(xywh.x * (i + 1), xywh.y + (j * xywh.w), xywh.z, xywh.w), ""));

		}
	}
	Button* exit = new Button(N, Vector4(75, 60, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);

}

void InventoryMenu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("Inventory");
	drawText(this->xyhw.x - nC * 6, 70, "Inventory", Vector3(1, 1, 1), 2);
	/*Renderizamos todos los botones menos el de exit*/
	for (int i = 0; i < this->Buttons.size() - 1; i++) {
		this->Buttons[i]->RenderButtonText();
	}


	for (int i = 0; i < this->inventory->Notes.size(); i++) {
		Note* current = this->inventory->Notes[i];
		//me muestra la nota actual
		if (current->isShowing == true) {
			this->ShowNote(i);

		}

	}


}

void InventoryMenu::UpdateMenu()
{
	/*Update the data that would be shown in the menu*/
	for (int i = 0; i < this->inventory->Notes.size(); i++) {
		this->Buttons[i]->type = N;
		this->Buttons[i]->text = "Nota" + std::to_string(i + 1);
	}
}


IntroMenu::IntroMenu()
{
	this->Card = Texture::Get("data/UI/Cards/Card X6Negro.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(Game::instance->window_width / 2, 220, 200, 150);
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y, xywh.z, xywh.w), "Play"));
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y * 2, xywh.z, xywh.w), "Tutorial"));
	Button* exit = new Button(N, Vector4(75, 60, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);
}

void IntroMenu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("MJ Lab");
	drawText(this->xyhw.x - nC * 6, 70, "MJ Lab", Vector3(1, 1, 1), 2);
	/*Renderizamos todos los botones menos el de exit*/
	for (int i = 0; i < this->Buttons.size() - 1; i++) {
		this->Buttons[i]->RenderButtonText();
	}
	this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();

}

void IntroMenu::UpdateMenu()
{
}

PauseMenu::PauseMenu()
{
	this->Card = Texture::Get("data/UI/Cards/Card X1.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width + 50, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(Game::instance->window_width / 2, 220, 200, 150);
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y, xywh.z, xywh.w), "Continue"));
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y * 2, xywh.z, xywh.w), "Tutorial"));
	Button* exit = new Button(N, Vector4(75, 60, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);
}

void PauseMenu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("MJ Lab");
	drawText(this->xyhw.x - nC * 6, 70, "MJ Lab", Vector3(1, 1, 1), 2);
	/*Renderizamos todos los botones menos el de exit*/
	for (int i = 0; i < this->Buttons.size() - 1; i++) {
		this->Buttons[i]->RenderButtonText();
	}
	this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();
}

void PauseMenu::UpdateMenu()
{
}

TutorialMenu::TutorialMenu()
{
	this->Card = Texture::Get("data/UI/Cards/Card X6Negro.png");//fondo
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(Game::instance->window_width / 2, 220, 200, 150);
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y, xywh.z, xywh.w), "Read"));
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y * 2, xywh.z, xywh.w), "Play"));

	this->instrucctions.push_back(Texture::Get("data/UI/Cards/Card X5.png"));
	this->instrucctions.push_back(Texture::Get("data/UI/Cards/Card X6.png"));
	this->instrucctions.push_back(Texture::Get("data/UI/Cards/LoadingScreen.png"));

	int offset = 35;
	Button* right = new Button(N, Vector4((Game::instance->window_width / 2)-offset+70, 550, 50, 50), Texture::Get("data/UI/Icons/ArrowRight.png"));
	this->Buttons.push_back(right);
	Button* left = new Button(N, Vector4((Game::instance->window_width / 2)- offset, 550, 50, 50), Texture::Get("data/UI/Icons/ArrowLeft.png"));
	this->Buttons.push_back(left);
	Button* exit = new Button(N, Vector4(75, 550, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);
}

void TutorialMenu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("Tutorial");
	drawText(this->xyhw.x - nC * 6, 70, "Tutorial", Vector3(1, 1, 1), 2);
	//si es el menu de Tutorial con los botones play or Continue
	if (!Game::instance->gameState->isTutorialMenu) {
		//std::cout << "instrucctions menu" << std::endl;

		Vector4 xywh = Vector4((Game::instance->window_width / 2), (Game::instance->window_height / 2) + 40, 500, 400);
	
		this->Render(this->instrucctions[Game::instance->gameState->currentInstrucction], xywh.x, xywh.y, xywh.z, xywh.w, false);
		
		this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();
		this->Buttons[this->Buttons.size() - 2]->RenderButtonIcon();
		this->Buttons[this->Buttons.size() - 3]->RenderButtonIcon();
	}
	else {
		//std::cout << "tutorial menu" << std::endl;
		/*Renderizamos todos los botones menos el de exit*/
		for (int i = 0; i < this->Buttons.size() - 3; i++) {
			this->Buttons[i]->RenderButtonText();
		}
		
	}
}

void TutorialMenu::UpdateMenu()
{
}

EndingMenu::EndingMenu()
{
	this->Card = Texture::Get("data/UI/Cards/Card X6Negro.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(Game::instance->window_width / 2, 220, 200, 150);
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y, xywh.z, xywh.w), "Tutorial"));
	this->Buttons.push_back(new Button(N, Vector4(xywh.x, xywh.y * 2, xywh.z, xywh.w), "Play"));
	Button* exit = new Button(N, Vector4(75, 60, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);
}

void EndingMenu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("End");
	drawText(this->xyhw.x - nC * 6, 60, "End", Vector3(1, 1, 1), 2);
	nC = count_chars("Congratulations");
	drawText(this->xyhw.x - nC * 6, 80, "Congratulations", Vector3(1, 1, 1), 2);
	nC = count_chars("You have finished the scape room");
	drawText(this->xyhw.x - nC * 6, 100, "You have finished the scape room", Vector3(1, 1, 1), 2);

	/*Renderizamos todos los botones menos el de exit*/
	for (int i = 0; i < this->Buttons.size() - 1; i++) {
		this->Buttons[i]->RenderButtonText();
	}
	this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();

}

void EndingMenu::UpdateMenu()
{
}


void InventoryMenu::ShowNote(int id) {
	UI* note = new UI();
	note->a_shader = this->a_shader;
	note->cam2D = this->cam2D;
	note->Render(this->inventory->Notes[id]->img, (Game::instance->window_width / 2) + 175, (Game::instance->window_height / 2) + 50, 350, 450, false);
	this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();

}

PopUpMessage::PopUpMessage(int i, const char* t, Texture* b, Vector4 xyhw)
{
	this->Id = i;
	this->text = t;
	this->bg = b;
	this->xyhw = xyhw;

}


void PopUpMessage::RenderPopUp()
{
	this->Render(this->bg, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars(this->text);
	drawText(this->xyhw.x - nC * 6, this->xyhw.y - (this->xyhw.w / 10), this->text, Vector3(1, 1, 1), 2);

}
char* codeButton(int i) {
	if (i == 1) {
		return "1";
	}if (i == 2) {
		return "2";
	}if (i == 3) {
		return "3";
	}if (i == 4) {
		return "4";
	}if (i == 5) {
		return "5";
	}if (i == 6) {
		return "6";
	}if (i == 7) {
		return "7";
	}if (i == 8) {
		return "8";
	}if (i == 9) {
		return "9";
	}
}

CodeScreen::CodeScreen(Lab* lab)
{
	this->bg = Texture::Get("data/UI/Cards/Card X1.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width + 50, Game::instance->window_height);


	Vector4 xywh = Vector4(300, 250, 100, 100);
	int r = 3;
	int c = 3;
	int co = 1;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {

			this->Buttons.push_back(new Button(N, Vector4(xywh.x + (i * xywh.z), xywh.y + (j * xywh.w), xywh.z, xywh.w), codeButton(co)));
			co++;
		}
	}

	Button* exit = new Button(N, Vector4(75, 60, 50, 50), Texture::Get("data/UI/Icons/21.png"));
	this->Buttons.push_back(exit);
	//std::cout << "exit pos: " << this->Buttons.size() << std::endl;
	Button* enter = new Button(N, Vector4(Game::instance->window_width / 2, 550, 50, 50), Texture::Get("data/UI/Icons/30.png"));
	this->Buttons.push_back(enter);
	//std::cout << "code butons size: " << this->Buttons.size() << std::endl;

}

int getIdcode(std::vector<Code*> codes) {
	for (int i = 0; i < codes.size(); i++) {
		if (codes[i]->isActive)
			return codes[i]->ID;
	}

}


void CodeScreen::RenderCodeScreen(Code* code)
{
	this->Render(this->bg, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("Introduce un numero de 4 Digitos");
	drawText(this->xyhw.x - nC * 6, 135, "Introduce un numero de 4 Digitos", Vector3(1, 1, 1), 2);
	nC = count_chars(code->test.c_str());
	drawText(this->xyhw.x - nC * 6, 70, code->test, Vector3(1, 1, 1), 2);


	for (int i = 0; i < this->Buttons.size() - 2; i++) {
		this->Buttons[i]->RenderButtonText();
	}

	this->Buttons[this->Buttons.size() - 2]->RenderButtonIcon();
	this->Buttons[this->Buttons.size() - 1]->RenderButtonIcon();


}

LoadScreen::LoadScreen()
{
	this->Card = Texture::Get("data/UI/LoadingScreen.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height);


}

void LoadScreen::RenderMenu()
{
	//std::cout << "loaded rendered" << std::endl;
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);



}

void LoadScreen::UpdateMenu()
{
}
