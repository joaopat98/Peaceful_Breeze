#ifndef FLY_CAMERA_H
#define FLY_CAMERA_H

#include "camera.h"
#include "input.h"

struct FlyCamera : Camera {
	float moveSpeed;
	float lookSpeed;
	vec3 initPos;
	vec2 initRot;
	vec2 rot;
	vec3 movement;
	FlyCamera(vec3 initPos, vec2 initRot, Projection* p, float moveSpeed, float lookSpeed);

	void update(double time_elapsed);
};

#endif // !FLY_CAMERA_H