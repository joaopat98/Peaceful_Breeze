#include "scene.h"

void SceneNode::draw(mat4 viewMatrix, mat4 projMatrix, mat4 parentMatrix, string drawMode)
{
	if (enabled) {
		mat4 current = parentMatrix *
			MatrixFactory::translateMat3(transform.position) *
			MatrixFactory::mat3to4(transform.rotation.toRotationMatrix()) *
			MatrixFactory::scaleMat4(transform.scale);
		for (SceneNode* node : children)
		{
			node->draw(viewMatrix, projMatrix, current, drawMode);
		}
		if (drawable != nullptr) {
			for (Program* program : drawable->programs) {
				if (program->hasUniform("ProjectionMatrix"))
					program->setUniformMat4("ProjectionMatrix", projMatrix);
				if (program->hasUniform("ViewMatrix"))
					program->setUniformMat4("ViewMatrix", viewMatrix);
				if (program->hasUniform("ModelMatrix"))
					program->setUniformMat4("ModelMatrix", current);
			}
			drawable->draw(drawMode);
		}
		glUseProgram(0);
	}
}

SceneNode* SceneNode::createNode(IDrawable* d)
{
	SceneNode* node = createNode();
	node->drawable = d;
	return node;
}

SceneNode* SceneNode::createNode()
{
	SceneNode* node = new SceneNode();
	node->enabled = true;
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

SceneNode* Scene::createNode(IDrawable* d)
{
	SceneNode* node = createNode();
	node->drawable = d;
	return node;
}

SceneNode* Scene::createNode()
{
	SceneNode* node = new SceneNode();
	node->enabled = true;
	node->transform.position = { 0,0,0 };
	node->transform.rotation = { 1,{0,0,0} };
	node->transform.scale = { 1,1,1 };
	children.push_back(node);
	return node;
}

SceneNode* Scene::createNode(IDrawable* d, Transform initT)
{
	SceneNode* node = createNode(d);
	node->transform = initT;
	return node;
}

SceneNode* SceneNode::createNode(IDrawable* d, Transform initT)
{
	SceneNode* node = createNode(d);
	node->transform = initT;
	return node;
}

void Scene::draw(string drawMode)
{
	mat4 projMatrix = cam->getProjectionMatrix();
	mat4 viewMatrix = cam->getViewMatrix();

	for (SceneNode* node : children) {
		node->draw(viewMatrix, projMatrix, MatrixFactory::identity4(), drawMode);
	}
}
