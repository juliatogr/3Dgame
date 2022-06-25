#include "UI.h"

class PopUpMessage : public UI {
public:
	int Id;
	const char* text;
	Texture* bg;
	Vector4 xyhw;

	PopUpMessage(int i, const char* t, Texture* b, Vector4 xyhw);
	void RenderPopUp();


};


