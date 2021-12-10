#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include "engine.h"

class PostProcessingPass;

class PostProcessingStack
{
public:
	PostProcessingStack(int winX, int winY);
	~PostProcessingStack();
	void beginPostProcessing();
	void endPostProcessing();
	void drawToFrameBuffer(GLuint target);
	GLuint getCurrentTexture();
	void drawToNextFrameBuffer();
	void resize(int winX, int winY);
	vector <PostProcessingPass*> passes;
	unsigned int width, height;
private:
	void setupTextures();

	GLuint rboDepth, rboColor;
	GLuint initFB, currFB, nextFB;
	GLuint currTex, nextTex;
};

class PostProcessingPass {
public:
	bool enabled = true;
	virtual void exec(PostProcessingStack* stack) = 0;
	virtual void resize(unsigned int winX, unsigned int winY);
};

#endif // !POST_PROCESSING_H
