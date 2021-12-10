#include "SSAO.h"

void SSAORenderer::initShaders() {
	ssaoProg = Program::fromPaths("ssao_vert.glsl", "ssao_frag.glsl");
	ssaoBlurProg = Program::fromPaths("ssao_blur_vert.glsl", "ssao_blur_frag.glsl");
}

void SSAORenderer::setupTextures() {
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);

	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);

	glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTexture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAORenderer::initKernel() {
	uniform_real_distribution<float> random(0.0, 1.0);
	default_random_engine generator;

	ssaoKernel.clear();

	//kernel samples
	for (int i = 0; i < _numSamples; ++i)
	{
		vec3 sample(random(generator) * 2.0f - 1.0f, random(generator) * 2.0f - 1.0f, random(generator));
		sample = sample.normalize();
		sample *= random(generator);
		float scale = (float)i / numSamples;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	ssaoProg->setUniformv3("samples", numSamples, &ssaoKernel[0]);
	ssaoProg->setUniform1i("numSamples", numSamples);
}

void SSAORenderer::initNoiseTex() {
	uniform_real_distribution<float> random(0.0, 1.0);
	default_random_engine generator;
	for (unsigned int i = 0; i < 16; i++)
	{
		vec3 noise(random(generator) * 2.0f - 1.0f, random(generator) * 2.0f - 1.0f, 0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

SSAORenderer::SSAORenderer(int winX, int winY, DeferredRenderer* defRenderer) {
	this->defRenderer = defRenderer;

	numSamples = _numSamples;
	initShaders();
	initKernel();
	initNoiseTex();

	width = winX;
	height = winY;

	glGenFramebuffers(1, &ssaoBuffer);
	glGenFramebuffers(1, &ssaoBlurBuffer);

	glGenTextures(1, &ssaoTexture);
	glGenTextures(1, &ssaoBlurTexture);

	setupTextures();
}

SSAORenderer::~SSAORenderer()
{
	glDeleteFramebuffers(1, &ssaoBuffer);
	glDeleteFramebuffers(1, &ssaoBlurBuffer);
	glDeleteTextures(1, &ssaoTexture);
	glDeleteTextures(1, &ssaoBlurTexture);
}

void SSAORenderer::resize(int winX, int winY) {
	width = winX;
	height = winY;

	setupTextures();
}

void SSAORenderer::draw(Scene* scene) {
	if (_numSamples != numSamples) {
		_numSamples = numSamples;
		initKernel();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, defRenderer->getPosTex());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, defRenderer->getNormalTex());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	ssaoProg->use();
	ssaoProg->setUniformMat4("ProjectionMatrix", scene->cam->getProjectionMatrix());
	ssaoProg->setUniform1i("width", width);
	ssaoProg->setUniform1i("height", height);
	ssaoProg->setUniform1f("sampleRadius", sampleRadius);
	ssaoProg->setUniform1f("bias", ssaoBias);
	ssaoProg->setUniform1i("gPosition", 0);
	ssaoProg->setUniform1i("gNormal", 1);
	ssaoProg->setUniform1i("noiseTex", 2);
	Mesh::getScreenQuad()->draw();

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurBuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	ssaoBlurProg->use();
	Mesh::getScreenQuad()->draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint SSAORenderer::getSSAOTex() {
	return ssaoBlurTexture;
}