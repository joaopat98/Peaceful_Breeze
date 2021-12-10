#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include "engine.h"

#define DEFERRED_DRAW_MODE "deferred"

class DeferredRenderer {
private:
	GLuint gBuffer;
	GLuint gPosition, gNormal;
	GLuint rboDepth;
	int width, height;

	void initShaders();

	void setupTextures();

public:
	DeferredRenderer(int winX, int winY);
	~DeferredRenderer();
	void draw(Scene* scene);
	void resize(int winX, int winY);
	GLuint getPosTex();
	GLuint getNormalTex();
};

#endif // !DEFERRED_RENDERING_H