#include "post_processing.h"

void PostProcessingStack::setupTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, initFB);

	glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);

	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, currFB);

	glBindTexture(GL_TEXTURE_2D, currTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, nextFB);

	glBindTexture(GL_TEXTURE_2D, nextTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessingStack::PostProcessingStack(int winX, int winY)
{
	width = winX;
	height = winY;

	glGenRenderbuffers(1, &rboDepth);
	glGenRenderbuffers(1, &rboColor);
	glGenFramebuffers(1, &initFB);
	glGenFramebuffers(1, &currFB);
	glGenFramebuffers(1, &nextFB);
	glGenTextures(1, &currTex);
	glGenTextures(1, &nextTex);

	setupTextures();
}

PostProcessingStack::~PostProcessingStack()
{
	glDeleteFramebuffers(1, &currFB);
	glDeleteFramebuffers(1, &nextFB);
	glDeleteFramebuffers(1, &initFB);

	glDeleteRenderbuffers(1, &rboColor);
	glDeleteRenderbuffers(1, &rboDepth);

	glDeleteTextures(1, &currTex);
	glDeleteTextures(1, &nextTex);
}

void PostProcessingStack::beginPostProcessing()
{
	glBindFramebuffer(GL_FRAMEBUFFER, initFB);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessingStack::endPostProcessing()
{
	glBlitNamedFramebuffer(initFB, currFB, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	for (PostProcessingPass* pass : passes)
	{
		if (pass->enabled)
			pass->exec(this);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingStack::drawToFrameBuffer(GLuint target)
{
	/**/
	glBindFramebuffer(GL_FRAMEBUFFER, target);
	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	Mesh::getScreenQuad()->draw();
	/**/

	//glBlitNamedFramebuffer(currFB, target, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void PostProcessingStack::drawToNextFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, nextFB);
	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	Mesh::getScreenQuad()->draw();
	GLuint tempFB = currFB;
	currFB = nextFB;
	nextFB = tempFB;
	GLuint tempTex = currTex;
	currTex = nextTex;
	nextTex = tempTex;
}

void PostProcessingStack::resize(int winX, int winY)
{
	width = winX;
	height = winY;

	setupTextures();

	for (PostProcessingPass* pass : passes)
	{
		pass->resize(winX, winY);
	}
}

GLuint PostProcessingStack::getCurrentTexture()
{
	return currTex;
}

void PostProcessingPass::resize(unsigned int winX, unsigned int winY)
{
}
