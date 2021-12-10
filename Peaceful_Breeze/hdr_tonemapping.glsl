#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;
uniform float exposure;
uniform float autoExposureMultiplier;
uniform bool autoExpose = false;
uniform float avgLuminance;
uniform float gamma;

void main()
{             
    vec3 hdrColor = texture(Texture, TexCoord).rgb;

	float e = exposure;
	if (autoExpose) {
		float lmax = 9.6 * avgLuminance;
		e = autoExposureMultiplier/lmax;
	}
    vec3 result = vec3(1.0) - exp(-hdrColor * e);    
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}