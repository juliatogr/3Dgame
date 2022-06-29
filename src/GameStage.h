#ifndef STAGE_H
#define STAGE_H

#include "shader.h"
#include "camera.h"
#include "entity.h"
#include "Player.h"
#include "UI.h"


enum STAGE_ID {
	LOAD = 0,
	INTRO = 1,
	TUTORIAL = 2,
	PLAY = 3,
	END = 4
};

class Stage {
public:
	Menu* menu;
	virtual STAGE_ID GetId() = 0;
	virtual void Render(Shader* a_shader, Camera* cam) = 0;
	virtual void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked) = 0;
};


class PlayStage : public Stage {
public:
	Lab* lab;

	virtual Lab* GetLab();
	Entity* selectedEntity = NULL;
	TaskEntity* selectedTaskEntity = NULL;

	Player* player;
	InventoryMenu* invMenu;
	PauseMenu* pauseMenu;
	std::vector<PopUpMessage*> pum;
	CodeScreen* codeUI;
	DevelopScreen* devUI;

	bool isViewingTask = false;
	STAGE_ID GetId();

	PlayStage();
	void RePlayStage();
	void RayPick(Camera* cam);
	void RotateSelected(float angleDegrees);
	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
	Vector3 nextPosNoCol(Vector3 nextPos, float seconds_elapsed);
	void checkNearTaskEntity(double seconds_elapsed);
};

class LoadStage : public Stage {
public:

	LoadScreen* menu;

	LoadStage();
	STAGE_ID GetId();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
};

class IntroStage : public Stage {
public:
	IntroMenu* menu;
	std::vector<PopUpMessage*> pum;
	CodeScreen* codeUI;
	STAGE_ID GetId();

	IntroStage();
	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
};



class TutorialStage : public Stage {
public:

	TutorialMenu* menu;

	TutorialStage();
	STAGE_ID GetId();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
};

class EndStage : public Stage {
public:
	EndingMenu* menu;
	EndStage();
	STAGE_ID GetId();
	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
};


void PickButton(std::vector<Button*> buttons);
#endif
