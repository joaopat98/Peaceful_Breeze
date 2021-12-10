#pragma once
#include "engine.h"
#include "light.h"

#define SHADOW_DRAW_MODE "shadow"

struct ShadowBuffer {
	GLuint depthMapFBO;
	GLuint depthMap;
	GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	GLuint _SHADOW_WIDTH = 2048, _SHADOW_HEIGHT = 2048;
	Program* texProg;
	
	ShadowBuffer();
	void createFramebuffer();
	void create2Dtexture();
	void attachTextureToFramebuffer();
	void render(Light* light, Scene* scene);
};
