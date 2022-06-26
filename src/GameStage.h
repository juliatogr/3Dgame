#include "shader.h"
#include "camera.h"
#include "entity.h"
#include "Player.h"
#include "UI.h"



#ifndef STAGE_H
#define STAGE_H

enum STAGE_ID {
	INTRO = 0,
	TUTORIAL = 1,
	PLAY = 2,
	END = 3,
	WIN = 4
};


class GameStage {
public:

	Lab* lab;

	virtual Lab* GetLab();
	Entity* selectedEntity = NULL;
	TaskEntity* selectedTaskEntity = NULL;

	Player* player;
	Menu* menu;
	std::vector<PopUpMessage*> pum;
	CodeScreen* codeUI;


	virtual STAGE_ID GetId() = 0;
	virtual void RayPick(Camera* cam);
	virtual void PickButton(std::vector<Button*> buttons);
	virtual void RotateSelected(float angleDegrees);
	virtual void Render(Shader* a_shader, Camera* cam) = 0;
	virtual void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera,  bool mouse_locked) = 0;
};


class IntroStage : public GameStage {
public:
	STAGE_ID GetId();
	
	IntroStage();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
};

class PlayStage : public GameStage {
public:
	bool isViewingTask = false;
	STAGE_ID GetId();
	bool IsActiveUIs;

	PlayStage();
	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera, bool mouse_locked);
	Vector3 nextPosNoCol(Vector3 nextPos, float elapsed_time);
	void checkNearTaskEntity(float elapsed_time);
};

class TutorialStage : public GameStage {
public:

	TutorialStage();
	STAGE_ID GetId();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera,  bool mouse_locked);
};

class EndStage : public GameStage {
public:


	EndStage();
	STAGE_ID GetId();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera,  bool mouse_locked);
};

class WinStage : public GameStage {
public:

	WinStage();
	STAGE_ID GetId();

	void Render(Shader* a_shader, Camera* cam);
	void Update(double seconds_elapsed, boolean cameralocked, float elapsed_time, float speed, Shader* a_shader, Camera* camera,  bool mouse_locked);
};



#endif
