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

	this->cam2D.setOrthographic(0,windowWidth, windowHeight, 0, -1, 1);

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
	//this->quadModel.translate(sin(Game::instance->time)*0.20,0,0);
	this->a_shader->setUniform("u_model", this->quadModel);
	this->a_shader->setUniform("u_time", time);
	
	this->quad.render(GL_TRIANGLES);
	
	//disable shader
	this->a_shader->disable();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void Button::RenderButton()
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
	if(this->type == N) {
		this->Render(this->Normal, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	}
	int nC = count_chars(this->text);
	drawText(this->xyhw.x - nC * 6, this->xyhw.y - (this->xyhw.w / 10), this->text, Vector3(1, 1, 1), 2);
}

Button::Button(BUTTONTYPE t, Vector4 v, const char* te)
{
	this->Active = Texture::Get("data/UI/HologramInterface/Button 1/Button Active.png");
	this->Disabled = Texture::Get("data/UI/HologramInterface/Button 1/Button Disable.png");
	this->Hover = Texture::Get("data/UI/HologramInterface/Button 1/Button Hover.png");
	this->Normal = Texture::Get("data/UI/HologramInterface/Button 1/Button Normal.png");
	this->type = t;
	this->xyhw = v;
	this->text = te;
	
}

Menu::Menu()
{
	this->Card = Texture::Get("data/UI/HologramInterface/Card X3/Card X6.png");
	this->xyhw = Vector4(Game::instance->window_width / 2, Game::instance->window_height / 2, Game::instance->window_width, Game::instance->window_height);
	/*Inventory buttons, as they initialy doesn't have any object they are visible but disabled*/
	Vector4 xywh = Vector4(100, 250, 100, 100);
	int r = 3;
	int c = 3;
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			std::cout << "cell button added" << std::endl;
		
				this->Buttons.push_back(new Button(D, Vector4(xywh.x * (i + 1), xywh.y + (j * xywh.w), xywh.z, xywh.w), ""));
				std::cout << xywh.y * (j + 1) << std::endl;
			
		}
	}


	this->isActive = false;



	
}

void Menu::RenderMenu()
{
	this->Render(this->Card, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars("Inventory");
	drawText(this->xyhw.x - nC * 6, 135, "Inventory", Vector3(1, 1, 1), 2);
	for (int i=0; i < this->Buttons.size(); i++) {
		this->Buttons[i]->RenderButton();
	}

	

}

void Menu::UpdateMenu()
{
	for (int i = 0; i < this->inventory->numSavedEntities; i++) {
		this->Buttons[i]->type = N;
		this->Buttons[i]->text = "Nota";
	}
}

PopUpMessage::PopUpMessage(int i, const char* t, Texture* b, Vector4 xyhw)
{
	this->Id = i;
	this->text = t;
	this->bg = b;
	this->xyhw = xyhw;
	this->isActive = false;

}



void PopUpMessage::RenderPopUp()
{
	this->Render(this->bg, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	int nC = count_chars(this->text);
	drawText(this->xyhw.x - nC*6, this->xyhw.y - (this->xyhw.w/10), this->text, Vector3(1, 1, 1), 2);

}
