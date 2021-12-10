#include "shadowbuffer.h"

ShadowBuffer::ShadowBuffer() {
	createFramebuffer();
	glGenTextures(1, &depthMap);
	create2Dtexture();
	attachTextureToFramebuffer();
	texProg = Program::fromPaths("tex_vert.glsl", "tex_frag.glsl");
}

void  ShadowBuffer::createFramebuffer() {
	glGenFramebuffers(1, &depthMapFBO);
}

void  ShadowBuffer::create2Dtexture() {
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _SHADOW_WIDTH, _SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void ShadowBuffer::attachTextureToFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowBuffer::render(Light* light, Scene* scene) {
	if (_SHADOW_WIDTH != SHADOW_WIDTH || _SHADOW_HEIGHT != SHADOW_HEIGHT) {
		_SHADOW_WIDTH = SHADOW_WIDTH;
		_SHADOW_HEIGHT = SHADOW_HEIGHT;
		create2Dtexture();
	}

	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glCullFace(GL_FRONT);
	scene->draw(SHADOW_DRAW_MODE);
	//glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	texProg->use();
	texProg->setUniform1i("Texture", 0);
	Mesh::getScreenQuad()->draw();
}

