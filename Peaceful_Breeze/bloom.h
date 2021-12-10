#ifndef BLOOM_H
#define BLOOM_H

#include "post_processing.h"

class BloomPass : public PostProcessingPass {
public:
	BloomPass(int winX, int winY);
	~BloomPass();
	virtual void exec(PostProcessingStack* stack);
	virtual void resize(unsigned int winX, unsigned int winY);
	int numDivisions = 9;
	int numBlurPasses = 2;
	bool progressiveUpsampling = true;
	float intensity = 0.5f;
private:
	unsigned int width, height;
	Program* thresholdProg, * blurProg, * compositeProg, * texProg;
	GLuint texFB;
	GLuint thresholdTex;
	vector<GLuint> subTextures;
	vector<GLuint> subTexturesBlur;
	vector<GLuint> subTexturesBlurTemp;

	void setupTextures();
};

#endif // !BLOOM_H
