#ifndef SSAO_H
#define SSAO_H

#include <random>

#include "engine.h"
#include "defered_rendering.h"

class SSAORenderer {
private:
	Program* ssaoProg, * ssaoBlurProg;

	DeferredRenderer* defRenderer;

	int _numSamples = 128;

	GLuint ssaoBuffer, ssaoBlurBuffer;
	int width, height;
	vector<vec3> ssaoKernel;
	vector<vec3> ssaoNoise;
	GLuint noiseTex;
	GLuint ssaoTexture, ssaoBlurTexture;

	void initShaders();
	void setupTextures();
	void initKernel();
	void initNoiseTex();

public:
	int numSamples;
	float sampleRadius = 0.4f;
	float ssaoBias = 0.001f;
	SSAORenderer(int winX, int winY, DeferredRenderer* defRenderer);
	~SSAORenderer();
	void draw(Scene* scene);
	void resize(int winX, int winY);
	GLuint getSSAOTex();
};

#endif

