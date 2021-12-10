#version 330 core

in float exBrightness;
out vec4 outColor;

uniform vec4 Color;

void main(void)
{
	outColor = Color * exBrightness;
}