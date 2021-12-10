#version 330 core
in vec3 ex_Color;
in vec3 ex_Normal;
in vec3 ex_Pos;
in vec2 ex_Texcoord;

out vec4 FragColor;
uniform float luminosity;
uniform vec3 lightColor;
uniform samplerCube skybox;
uniform bool useLightColorAbsolute;

//Color space conversions: https://gist.github.com/yiwenl/745bfea7f04c456e0101
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 rgb) {
 	float Cmax = max(rgb.r, max(rgb.g, rgb.b));
 	float Cmin = min(rgb.r, min(rgb.g, rgb.b));
 	float delta = Cmax - Cmin;

 	vec3 hsv = vec3(0., 0., Cmax);

 	if (Cmax > Cmin) {
 		hsv.y = delta / Cmax;

 		if (rgb.r == Cmax)
 			hsv.x = (rgb.g - rgb.b) / delta;
 		else {
 			if (rgb.g == Cmax)
 				hsv.x = 2. + (rgb.b - rgb.r) / delta;
 			else
 				hsv.x = 4. + (rgb.r - rgb.g) / delta;
 		}
 		hsv.x = fract(hsv.x / 6.);
 	}
 	return hsv;
 }

void main(void)
{
	vec4 color = texture(skybox, ex_Pos);
    vec3 colorHSV = rgb2hsv(color.rgb);
    vec3 lightHSV = rgb2hsv(lightColor);
    colorHSV.r += 0.4 + lightHSV.r;
    vec3 mixed = mix(color.rgb, hsv2rgb(colorHSV), lightHSV.g);
    vec3 mixedHSV = rgb2hsv(mixed);
    mixedHSV.b *= lightHSV.b;
    if (useLightColorAbsolute)
        FragColor = vec4(lightColor * luminosity, color.a);
    else
	    FragColor = vec4(hsv2rgb(mixedHSV) * luminosity, color.a);
}