#include "defered_rendering.h"

void DeferredRenderer::initShaders() {
}

void DeferredRenderer::setupTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DeferredRenderer::DeferredRenderer(int winX, int winY) {
	initShaders();

	width = winX;
	height = winY;

	glGenFramebuffers(1, &gBuffer);
	glGenRenderbuffers(1, &rboDepth);

	glGenTextures(1, &gPosition);
	glGenTextures(1, &gNormal);

	setupTextures();
}

DeferredRenderer::~DeferredRenderer()
{
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
}

void DeferredRenderer::resize(int winX, int winY) {
	width = winX;
	height = winY;

	setupTextures();
}

void DeferredRenderer::draw(Scene* scene) {
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	static const float clear[] = { 0, 0, FLT_MAX, 1 };
	static const float empty[] = { 0, 0, 0, 1 };
	glClearBufferfv(gBuffer, 0, clear);
	glClearBufferfv(gBuffer, 1, empty);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->draw(DEFERRED_DRAW_MODE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint DeferredRenderer::getPosTex() {
	return gPosition;
}

GLuint DeferredRenderer::getNormalTex() {
	return gNormal;
}