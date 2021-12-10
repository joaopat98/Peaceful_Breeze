#include "dither.h"

static const unsigned char bayerPattern[] = {
        0,  32,  8, 40,  2, 34, 10, 42,
        48, 16, 56, 24, 50, 18, 58, 26,
        12, 44,  4, 36, 14, 46,  6, 38,
        60, 28, 52, 20, 62, 30, 54, 22,
        3,  35, 11, 43,  1, 33,  9, 41,
        51, 19, 59, 27, 49, 17, 57, 25,
        15, 47,  7, 39, 13, 45,  5, 37,
        63, 31, 55, 23, 61, 29, 53, 21, };

DitherPass::DitherPass()
{
    ditherProg = Program::fromPaths("tex_vert.glsl", "dither.glsl");
    
    glGenTextures(1, &ditherTex);
    glBindTexture(GL_TEXTURE_2D, ditherTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, bayerPattern);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void DitherPass::exec(PostProcessingStack* stack)
{
    ditherProg->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stack->getCurrentTexture());
    ditherProg->setUniform1i("Texture", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ditherTex);
    ditherProg->setUniform1i("ditherMatrix", 1);
    stack->drawToNextFrameBuffer();
}
