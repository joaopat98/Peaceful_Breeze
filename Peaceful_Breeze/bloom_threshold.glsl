#version 430 core
in vec2 TexCoord;

uniform sampler2D Texture;

uniform float threshold = 1;

out vec4 Color;

void main()
{
	vec3 frag = texture(Texture, TexCoord).rgb;
	float brightness = dot(frag, vec3(0.2126, 0.7152, 0.0722));
	Color = brightness > threshold ? vec4((brightness - threshold) * frag,1) : vec4(0,0,0,1);
}  