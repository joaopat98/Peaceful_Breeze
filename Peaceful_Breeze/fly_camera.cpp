#include "fly_camera.h"

FlyCamera::FlyCamera(vec3 initPos, vec2 initRot, Projection* p, float moveSpeed, float lookSpeed) : Camera(initPos, vec3(), AXIS3_Y, p)
{
	this->moveSpeed = moveSpeed;
	this->lookSpeed = lookSpeed;
	this->initRot = initRot;
	rot = initRot;
	this->initPos = initPos;
	eye = initPos;
}

void FlyCamera::update(double seconds)
{
	vec3 camMovement;
	if (Input::GetKey(GLFW_KEY_W)) {
		camMovement += getForward();
	}
	if (Input::GetKey(GLFW_KEY_S)) {
		camMovement -= getForward();
	}
	if (Input::GetKey(GLFW_KEY_A)) {
		camMovement -= getRight();
	}
	if (Input::GetKey(GLFW_KEY_D)) {
		camMovement += getRight();
	}

	if (Input::GetKey(GLFW_KEY_SPACE)) {
		camMovement += getUp();
	}
	if (Input::GetKey(GLFW_KEY_LEFT_CONTROL)) {
		camMovement -= getUp();
	}

	movement = vec3::lerp(movement, camMovement, 15 * (float)seconds);
	translate(movement * moveSpeed * (float)seconds);
	vec2 mouseDelta = Input::GetMouseMovement();
	rot.x += mouseDelta.x * lookSpeed * (float)seconds;
	rot.y -= mouseDelta.y * lookSpeed * (float)seconds;
	rot.y = (float)fmin(deg2rad(85), fmax(deg2rad(-85), rot.y));

	if (Input::GetKeyDown(GLFW_KEY_R)) {
		movement = vec3();
		eye = initPos;
		rot = initRot;
	}

	vec3 centerVec = vec3(cos(rot.y) * cos(rot.x), sin(rot.y), cos(rot.y) * sin(rot.x));
	center = eye + centerVec;
}
