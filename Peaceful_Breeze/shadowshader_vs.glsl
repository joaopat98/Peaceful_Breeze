#version 430 core

in vec3 inPosition;
in vec2 inTexCoord;

uniform mat4 LightSpaceMatrix;
uniform mat4 ModelMatrix;

out vec2 TexCoord;

void main()
{
    gl_Position = LightSpaceMatrix * ModelMatrix * vec4(inPosition, 1);
    TexCoord = inTexCoord;
}  