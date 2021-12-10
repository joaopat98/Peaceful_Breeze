#ifndef SCENE_H
#define SCENE_H

#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include "camera.h"

#define DEFAULT_DRAW_MODE "default"

struct IDrawable {
	vector<Program *> programs;
	virtual void draw(string drawMode) = 0;
};

struct Transform {
	vec3 position, scale;
	quaternion rotation;
};

struct SceneNode {
	bool enabled;
	Transform transform;

	IDrawable* drawable = nullptr;
	vector<SceneNode*> children;
	void draw(mat4 viewMatrix, mat4 projMatrix, mat4 parentMatrix, string drawMode = DEFAULT_DRAW_MODE);
	SceneNode* createNode(IDrawable* d);
	SceneNode* createNode();
	SceneNode* createNode(IDrawable* d, Transform initT);
};

struct Scene
{
	double _timeAcum = 0;

	Camera* cam = nullptr;
	vector<SceneNode*> children;

	SceneNode* createNode(IDrawable* d);
	SceneNode* createNode();
	SceneNode* createNode(IDrawable* d, Transform initT);

	void draw(string drawMode = DEFAULT_DRAW_MODE);
};

#endif // !SCENE_H
