#include "PopUpMessage.h"
#include "utils.cpp"

PopUpMessage::PopUpMessage(int i, const char* t, Texture* b,Vector4 xyhw)
{
	this->Id = i;
	this->text = t;
	this->bg = b;
	this->xyhw = xyhw;
}

void PopUpMessage::RenderPopUp()
{
	this->Render(this->bg, this->xyhw.x, this->xyhw.y, this->xyhw.z, this->xyhw.w, false);
	drawText(this->xyhw.x - 5, this->xyhw.y - 5, this->text, Vector3(1, 1, 1), 2);

}
