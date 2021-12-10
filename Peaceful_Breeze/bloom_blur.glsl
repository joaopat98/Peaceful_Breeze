#version 430 core
in vec2 TexCoord;

uniform float seed;

uniform sampler2D Texture;

float kernel[25] = {
	1,	4,	7,	4,	1,
	4,	16,	26,	16,	4,
	7,	26,	41,	26,	7,
	4,	16,	26,	16,	4,
	1,	4,	7,	4,	1
};

out vec4 Color;

void main()
{
	vec3 result = vec3(0);
	vec2 texelSize = 1.0 / vec2(textureSize(Texture, 0));
	for(int x = -2; x < 3; x++) {
		for(int y = -2; y < 3; y++) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(Texture, TexCoord + offset).rgb * kernel[(y + 2) * 5 + x + 2];
		}
	}
	Color = vec4(result / float(273), 1);
}  