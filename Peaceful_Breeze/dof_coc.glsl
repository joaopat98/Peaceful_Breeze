#version 430 core

out float coc;

in vec2 TexCoord;

uniform sampler2D PositionTex;

uniform float aperture;
uniform float focusDistance;
uniform float focalLength;

uniform bool focusOnCenter = false;

void main() {
	float depth = texture(PositionTex, TexCoord).b;
	if (focusOnCenter) {
		float centerDepth = texture(PositionTex, vec2(0.5)).b;
		coc = abs(aperture * (focalLength * (centerDepth - depth)) / (depth * (centerDepth - focalLength)));
	}
	else {
		coc = abs(aperture * (focalLength * (focusDistance - depth)) / (depth * (focusDistance - focalLength)));
	}
	coc = min(aperture, coc);
}