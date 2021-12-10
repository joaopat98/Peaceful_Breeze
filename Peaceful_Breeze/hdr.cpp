#include "hdr.h"

void HDRToneMappingPass::setupTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, texFB);

	glBindTexture(GL_TEXTURE_2D, luminanceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, luminanceTex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

struct LuminanceUpdate : IPostUpdateCallback {
	HDRToneMappingPass* pass;
	LuminanceUpdate(HDRToneMappingPass* pass) {
		this->pass = pass;
	}

	void call(double seconds)
	{
		pass->adaptLuminance(seconds);
	}
};

void HDRToneMappingPass::adaptLuminance(double seconds)
{
	luminance = lerp(luminance, newLuminance, adaptationSpeed * (float)seconds);
}

HDRToneMappingPass::HDRToneMappingPass(Engine* engine, unsigned int winX, unsigned int winY)
{
	toneMappingProg = Program::fromPaths("tex_vert.glsl", "hdr_tonemapping.glsl");
	luminanceProg = Program::fromPaths("tex_vert.glsl", "luminance.glsl");

	width = winX;
	height = winY;

	glGenFramebuffers(1, &texFB);
	glGenTextures(1, &luminanceTex);

	engine->registerPostUpdateCallback(new LuminanceUpdate(this));

	setupTextures();
}

HDRToneMappingPass::~HDRToneMappingPass()
{
	glDeleteFramebuffers(1, &texFB);
	glDeleteTextures(1, &luminanceTex);
}

void HDRToneMappingPass::exec(PostProcessingStack* stack)
{

	toneMappingProg->setUniform1f("exposure", exposure);
	toneMappingProg->setUniform1f("gamma", gamma);
	if (autoExpose) {
		glBindFramebuffer(GL_FRAMEBUFFER, texFB);
		luminanceProg->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
		luminanceProg->setUniform1i("Texture", 0);
		Mesh::getScreenQuad()->draw();
		glBindTexture(GL_TEXTURE_2D, luminanceTex);

		int maxLevel = (int)floor(log2((float)max(width, height)));

		glGenerateMipmap(GL_TEXTURE_2D);
		glGetTexImage(GL_TEXTURE_2D, maxLevel, GL_RED, GL_FLOAT, &newLuminance);
		toneMappingProg->setUniform1i("autoExpose", 1);
		toneMappingProg->setUniform1f("autoExposureMultiplier", autoExposureMultiplier);
		toneMappingProg->setUniform1f("avgLuminance", luminance);
	}
	else
	{
		toneMappingProg->setUniform1i("autoExpose", 0);
	}
	toneMappingProg->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
	toneMappingProg->setUniform1i("Texture", 0);
	stack->drawToNextFrameBuffer();
}

void HDRToneMappingPass::resize(unsigned int winX, unsigned int winY)
{
	width = winX;
	height = winY;

	setupTextures();
}
