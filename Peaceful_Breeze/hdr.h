#ifndef HDR_H
#define HDR_H

#include "post_processing.h"
#include <algorithm>

using namespace std;

class HDRToneMappingPass : public PostProcessingPass {
public:
	HDRToneMappingPass(Engine* engine, unsigned int winX, unsigned int winY);
	~HDRToneMappingPass();
	virtual void exec(PostProcessingStack* stack);
	void resize(unsigned int winX, unsigned int winY);
	bool autoExpose = true;
	float adaptationSpeed = 1;
	float autoExposureMultiplier = 5;
	float exposure = 1;
	float gamma = 1.0f;

private:
	unsigned int width, height;
	Program* toneMappingProg, * luminanceProg;
	GLuint luminanceTex;
	GLuint texFB;
	float luminance = 0;
	float newLuminance;

	void adaptLuminance(double seconds);
	void setupTextures();
	friend struct LuminanceUpdate;
};

#endif // !HDR_H
