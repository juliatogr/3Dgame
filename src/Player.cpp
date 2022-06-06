#include "Player.h"

void Player::RenderPlayer(Camera* camera) {
	Matrix44 playerModel = this->model;
	playerModel.translate(this->pos.x, this->pos.y, this->pos.z);
	playerModel.rotate(this->yaw * DEG2RAD, Vector3(0, 1, 0));
	
	Matrix44 camModel = playerModel;
	camModel.rotate(this->pitch * DEG2RAD, Vector3(1, 0, 0));

	Vector3 eye = playerModel * Vector3(0, 0.6, 0);
	Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
	Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));

	camera->lookAt(eye, center, up);
}