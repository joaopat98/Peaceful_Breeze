#include "bloom.h"

void BloomPass::setupTextures() {
	glBindTexture(GL_TEXTURE_2D, thresholdTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < numDivisions; i++) {
		int div = (int)pow(2, i);

		glBindTexture(GL_TEXTURE_2D, subTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / div, height / div, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, subTexturesBlur[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / div, height / div, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, subTexturesBlurTemp[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / div, height / div, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}

BloomPass::BloomPass(int winX, int winY)
{
	thresholdProg = Program::fromPaths("tex_vert.glsl", "bloom_threshold.glsl");
	blurProg = Program::fromPaths("tex_vert.glsl", "bloom_blur.glsl");
	texProg = Program::fromPaths("tex_vert.glsl", "tex_frag.glsl");
	compositeProg = Program::fromPaths("tex_vert.glsl", "bloom_composite.glsl");

	width = winX;
	height = winY;

	glGenFramebuffers(1, &texFB);
	glGenTextures(1, &thresholdTex);
	for (int i = 0; i < numDivisions; i++) {
		GLuint tex;
		glGenTextures(1, &tex);
		subTextures.push_back(tex);

		glGenTextures(1, &tex);
		subTexturesBlur.push_back(tex);

		glGenTextures(1, &tex);
		subTexturesBlurTemp.push_back(tex);
	}

	setupTextures();
}

BloomPass::~BloomPass()
{
	glDeleteFramebuffers(1, &texFB);
	glDeleteTextures(1, &thresholdTex);
	for (int i = 0; i < numDivisions; i++) {
		glDeleteTextures(1, &subTextures[i]);
		glDeleteTextures(1, &subTexturesBlur[i]);
		glDeleteTextures(1, &subTexturesBlurTemp[i]);
	}
}

void BloomPass::exec(PostProcessingStack* stack)
{
	glBindFramebuffer(GL_FRAMEBUFFER, texFB);
	glActiveTexture(GL_TEXTURE0);

	thresholdProg->use();
	glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
	thresholdProg->setUniform1i("Texture", 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thresholdTex, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	Mesh::getScreenQuad()->draw();

	texProg->setUniform1i("Texture", 0);
	blurProg->setUniform1i("Texture", 0);
	for (int i = 0; i < numDivisions; i++)
	{
		int div = (int)pow(2, i);
		glViewport(0, 0, width / div, height / div);

		texProg->use();
		glBindTexture(GL_TEXTURE_2D, i == 0 ? thresholdTex : subTexturesBlur[i - 1]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, subTextures[i], 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Mesh::getScreenQuad()->draw();

		blurProg->use();

		for (int j = 0; j < numBlurPasses; j++)
		{
			glBindTexture(GL_TEXTURE_2D, j == 0 ? subTextures[i] : subTexturesBlur[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, subTexturesBlurTemp[i], 0);
			glClear(GL_COLOR_BUFFER_BIT);
			Mesh::getScreenQuad()->draw();

			GLuint temp = subTexturesBlur[i];
			subTexturesBlur[i] = subTexturesBlurTemp[i];
			subTexturesBlurTemp[i] = temp;
		}
	}
	compositeProg->use();
	compositeProg->setUniform1i("Texture", 0);
	compositeProg->setUniform1i("bloomTex", 1);
	compositeProg->setUniform1f("intensity", intensity);
	if (progressiveUpsampling) {
		compositeProg->setUniform1i("last", false);
		for (int i = 0; i < numDivisions - 1; i++)
		{
			int div = (int)pow(2, numDivisions - i - 2);
			glViewport(0, 0, width / div, height / div);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, subTexturesBlurTemp[numDivisions - i - 2], 0);
			glClear(GL_COLOR_BUFFER_BIT);

			compositeProg->setUniform1i("first", i == 0);

			if (i > 0) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, subTexturesBlurTemp[numDivisions - i - 1]);
			}
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, subTexturesBlur[numDivisions - i - 1]);
			compositeProg->setUniform1f("weight", 1 / ((i - 1) / 8.0f + 1));
			Mesh::getScreenQuad()->draw();
			//compositeProg->setUniform1f("weight", 1);
			//stack->drawToNextFrameBuffer();
		}

		/**/
		compositeProg->setUniform1i("last", true);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, subTexturesBlurTemp[0]);
		compositeProg->setUniform1f("weight", 1 / ((numDivisions - 1) / 8.0f + 1));
		stack->drawToNextFrameBuffer();

		compositeProg->setUniform1i("last", false);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, subTexturesBlur[0]);
		compositeProg->setUniform1f("weight", 1 / ((numDivisions - 1) / 8.0f + 1));
		stack->drawToNextFrameBuffer();
	}
	else {
		compositeProg->setUniform1i("first", false);
		compositeProg->setUniform1i("last", false);
		for (int i = 0; i < numDivisions; i++)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, subTexturesBlur[i]);
			compositeProg->setUniform1f("weight", 1 / (i / 8.0f + 1));
			//compositeProg->setUniform1f("weight", 1);
			stack->drawToNextFrameBuffer();
		}
	}
}

void BloomPass::resize(unsigned int winX, unsigned int winY)
{
	width = winX;
	height = winY;

	setupTextures();
}
