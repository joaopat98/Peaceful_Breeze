#version 330 core

in vec4 FragPos;
in vec2 exTexCoord;
in vec3 exNormal;

uniform vec3 Color;
uniform sampler2D occlusion;
uniform bool useOcclusion = true;

out vec4 outColor;

void main(void)
{
	outColor = vec4(Color * (useOcclusion ? texture(occlusion, FragPos.xy / FragPos.w * 0.5 + 0.5).r : 1), 1);
	//outColor = vec4(vec3(FragPos.xy, 0) , 1);
}