#ifndef DOF_H
#define DOF_H

#include "post_processing.h"
#include "defered_rendering.h"

class DOFPass : public PostProcessingPass {
public:
	DOFPass(DeferredRenderer* defRenderer);
	~DOFPass();
	virtual void exec(PostProcessingStack* stack);
	float aperture = 13;
	float focusDistance = 3;
	float imageDistance = 0.05f;
	float maxDepthDelta = 20;
	int numRings = 10;
	int numSegments = 8;
	bool showCoc = false;
	bool dynamicRingSegments = true;
	float ringRatio = 3;
private:
	GLuint cocTex, cocBlurTex;
	GLuint cocFB;
	int width, height;
	Program* cocProg, * cocBlurProg, * dofProg;
	DeferredRenderer* defRenderer;

	void setupTextures();
};

#endif // !DOF_H