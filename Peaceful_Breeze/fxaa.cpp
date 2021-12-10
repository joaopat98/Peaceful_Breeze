#include "fxaa.h"

FXAAPass::FXAAPass()
{
	fxaaProg = Program::fromPaths("tex_vert.glsl", "fxaa.glsl");
}

void FXAAPass::exec(PostProcessingStack* stack)
{
    fxaaProg->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
    fxaaProg->setUniform1i("Texture", 0);
    stack->drawToNextFrameBuffer();
}
