#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;
uniform sampler2D PositionTex;
uniform sampler2D cocTex;

uniform float aperture;

uniform int numRings = 5;
uniform int numSegments = 8;

uniform float maxDepthDelta = 20;

uniform bool showCoc;
uniform bool dynamicRingSegments;

uniform float ringRatio;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(Texture, 0));
	float depth = texture(PositionTex, TexCoord).b;
	vec3 selfColor = texture(Texture, TexCoord).rgb;
	float coc = texture(cocTex, TexCoord).r * aperture;
	float ringSize = coc / numRings;
	int samples = 0;
	vec3 color = vec3(0);
	int ns = numSegments;
	for(int ring; ring < numRings; ring++) {
		float radius = ringSize * (ring + 1);
		if (dynamicRingSegments){
			ns = max(1,int(floor(radius * ringRatio)));
		}
		for (int segment = 0; segment < ns; segment++) {
			float angle = 3.1415 * 2 / ns * segment;
			vec2 sampleCoords = TexCoord + (vec2(cos(angle), sin(angle)) * radius * texelSize);
			float sampleCoc = texture(cocTex, sampleCoords).r; 
			if (sampleCoc < coc && depth - texture(PositionTex, sampleCoords).b > maxDepthDelta)
				color += selfColor;
			else
				color += texture(Texture, sampleCoords).rgb;
			samples++;
		}
	}
	color /= samples;
	FragColor = vec4(vec3(color),1);
	if(showCoc)
		FragColor = vec4(vec3(coc),1);
}