#version 430 core
in vec2 TexCoord;

uniform sampler2D Texture;

out float luminance;

void main()
{
	vec3 frag = texture(Texture, TexCoord).rgb;
	luminance = dot(frag, vec3(0.2126, 0.7152, 0.0722));
}  