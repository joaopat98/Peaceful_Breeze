#version 430 core
in vec2 TexCoord;

uniform sampler2D Texture;
uniform sampler2D bloomTex;

uniform bool first;
uniform float intensity = 0.5;
uniform float weight;
uniform bool last;

out vec4 Color;

void main()
{
	vec3 frag;
	if (first) {
		frag = intensity * weight * texture(bloomTex, TexCoord).rgb;
	}
	else if (last) {
		frag = texture(Texture, TexCoord).rgb + texture(bloomTex, TexCoord).rgb;
	}
	else {
	frag = texture(Texture, TexCoord).rgb + intensity * weight * texture(bloomTex, TexCoord).rgb;
	}

	Color = vec4(frag, 1);
}  