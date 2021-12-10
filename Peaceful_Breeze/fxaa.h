#ifndef FXAA_H
#define FXAA_H

#include "post_processing.h"

class FXAAPass : public PostProcessingPass {
public:
	FXAAPass();
	void exec(PostProcessingStack* stack);
private:
	Program* fxaaProg;
};

#endif // !FXAA_H
