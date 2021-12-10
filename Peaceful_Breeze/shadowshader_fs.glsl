#version 430 core

in vec2 TexCoord;

uniform bool useTexture;
uniform sampler2D Texture;

void main()
{             
    if (useTexture && texture(Texture, TexCoord).a < 0.5)
        discard;
    //gl_FragDepth = gl_FragCoord.z;
}  