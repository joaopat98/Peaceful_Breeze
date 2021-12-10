#pragma once
#include "engine.h"

struct Light {
	float near_plane = 1, far_plane = 18.0f;

	float box_width = 15.0f, box_height = 15.0f;


	vec3 position;
	vec3 directionPoint;
	vec3 color = vec3(1.0f);
	Camera camera;

	Light(vec3 position, vec3 direction);
	void startCamera();
	void updateCamera();
};
