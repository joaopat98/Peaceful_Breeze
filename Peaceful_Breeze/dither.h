#ifndef DITHER_H
#define DITHER_H

#include "post_processing.h"

class DitherPass : public PostProcessingPass {
public:
	DitherPass();
	void exec(PostProcessingStack* stack);
private:
	Program* ditherProg;
	GLuint ditherTex;
};

#endif // !DITHER_H
