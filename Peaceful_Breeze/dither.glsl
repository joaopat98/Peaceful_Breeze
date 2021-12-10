#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;
uniform sampler2D ditherMatrix;

void main()
{
    vec4 color=texture2D(Texture, TexCoord);
    float bayer=texture2D(ditherMatrix,gl_FragCoord.xy/8.).r*(255./64.);
    const float rgbByteMax=255.;
    vec4 rgba=rgbByteMax*color;
    vec4 head=floor(rgba);
    vec4 tail=rgba-head;
    color=head+step(bayer,tail);
    FragColor=color/rgbByteMax;
}