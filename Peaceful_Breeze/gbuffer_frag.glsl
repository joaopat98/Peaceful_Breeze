#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform bool useTexture;
uniform sampler2D Texture;

void main()
{
    if (useTexture && texture(Texture, TexCoord).a < 0.5)
        discard;
    gPosition += FragPos;
    gNormal = normalize(Normal);
}  