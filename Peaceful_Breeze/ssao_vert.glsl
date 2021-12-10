#version 330 core

in vec3 inPosition;
in vec2 inTexCoord;

out vec2 TexCoord;

void main(void)
{
	gl_Position = vec4(inPosition, 1);
	TexCoord = inTexCoord;
}