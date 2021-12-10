#version 330 core

in vec4 inPosition;
in float inBrightness;
out float exBrightness;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * inPosition;
	exBrightness = inBrightness;
}