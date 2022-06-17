#include "UI.h"
#include "game.h"
UI::UI()
{
	this->a_shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	
}

void UI::Render(Texture* texture, float x, float y, float w, float h, bool u) {

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
	this->Card = Texture::Get("data/UI/HologramInterface/Card X1/Card X2.png");
	int x = 100;
	this->Buttons.reserve(3);
	this->Buttons.push_back(new Button(N, Vector4(x, 100, 150, 50), "1"));
	this->Buttons.push_back(new Button(D, Vector4(x+200, 100, 150, 50), "2"));
	this->Buttons.push_back(new Button(N, Vector4(x+400, 100, 150, 50), "3"));
	this->Buttons.push_back(new Button(D, Vector4(x+600, 100, 150, 50), "4"));



	
}

void Menu::RenderMenu()
{
	this->Render(this->Card, Game::instance->window_width/2, Game::instance->window_height/2, Game::instance->window_width+50, Game::instance->window_height+50, false);
	for (int i=0; i < this->Buttons.size(); i++) {
		this->Buttons[i]->RenderButton();
	}
}
