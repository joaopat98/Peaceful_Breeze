#include "dof.h"

void DOFPass::setupTextures()
{
	glBindTexture(GL_TEXTURE_2D, cocTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, cocBlurTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

DOFPass::DOFPass(DeferredRenderer* defRenderer)
{
	cocProg = Program::fromPaths("tex_vert.glsl", "dof_coc.glsl");
	cocBlurProg = Program::fromPaths("tex_vert.glsl", "dof_coc_blur.glsl");
	dofProg = Program::fromPaths("tex_vert.glsl", "dof_apply.glsl");
	this->defRenderer = defRenderer;

	glGenFramebuffers(1, &cocFB);
	glGenTextures(1, &cocTex);
	glGenTextures(1, &cocBlurTex);

	GLint screen[4];
	glGetIntegerv(GL_VIEWPORT, screen);
	width = screen[2];
	height = screen[3];

	setupTextures();
}

DOFPass::~DOFPass()
{
	glDeleteTextures(1, &cocTex);
	glDeleteTextures(1, &cocBlurTex);
	glDeleteFramebuffers(1, &cocFB);
}



void DOFPass::exec(PostProcessingStack* stack)
{
	GLint screen[4];
	glGetIntegerv(GL_VIEWPORT, screen);
	if (width != screen[2] || height != screen[3]) {
		width = screen[2];
		height = screen[3];

		setupTextures();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, cocFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cocTex, 0);
	cocProg->use();
	cocProg->setUniform1f("aperture", aperture);
	cocProg->setUniform1f("focusDistance", focusDistance);
	cocProg->setUniform1f("focalLength", 1 / (1 / focusDistance + 1 / imageDistance));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defRenderer->getPosTex());
	cocProg->setUniform1i("PositionTex", 0);
	Mesh::getScreenQuad()->draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cocBlurTex, 0);
	cocBlurProg->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cocTex);
	cocBlurProg->setUniform1i("Texture", 0);
	Mesh::getScreenQuad()->draw();

	dofProg->use();
	dofProg->setUniform1f("aperture", aperture);
	dofProg->setUniform1f("maxDepthDelta", maxDepthDelta);
	dofProg->setUniform1i("numRings", numRings);
	dofProg->setUniform1i("numSegments", numSegments);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
	dofProg->setUniform1i("Texture", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, defRenderer->getPosTex());
	dofProg->setUniform1i("PositionTex", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cocBlurTex);
	dofProg->setUniform1i("cocTex", 2);
	dofProg->setUniform1i("dynamicRingSegments", dynamicRingSegments);
	dofProg->setUniform1f("ringRatio", ringRatio);
	dofProg->setUniform1i("showCoc", showCoc);
	stack->drawToNextFrameBuffer();
}

