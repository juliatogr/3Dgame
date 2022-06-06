#pragma once
#include "framework.h"
#include "entity.h"
class Player : public Entity{
public:
	Vector3 pos;
	float yaw;
	float pitch;

	Player();

};
