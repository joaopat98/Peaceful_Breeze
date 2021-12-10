#version 430 core

in vec3 inPosition;
in vec3 inNormal;

out vec3 Normal_CS_in;

void main(void)
{
	Normal_CS_in = inNormal;
	gl_Position = vec4(inPosition, 1);
}