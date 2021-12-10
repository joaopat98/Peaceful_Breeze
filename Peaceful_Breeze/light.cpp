#include "light.h"

Light::Light(vec3 in_position, vec3 in_direction_point) {
	position = in_position;
	directionPoint = in_direction_point;
}

void Light::startCamera() {
	camera.projection = new OrthoProjection(-box_width / 2, box_width / 2, -box_height / 2, box_height / 2, near_plane, far_plane);
}

void Light::updateCamera() {
	camera.eye = position;
	camera.center = directionPoint;
	camera.up = vec3(0.0f, 1.0f, 0.0f);
	((OrthoProjection*)(camera.projection))->near = near_plane;
	((OrthoProjection*)(camera.projection))->far = far_plane;
	((OrthoProjection*)(camera.projection))->left = -box_width / 2;
	((OrthoProjection*)(camera.projection))->right = box_width / 2;
	((OrthoProjection*)(camera.projection))->bottom = -box_height / 2;
	((OrthoProjection*)(camera.projection))->top = box_height / 2;
}