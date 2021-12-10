#version 330 core

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main(void)
{
	vec4 viewPos = ViewMatrix * ModelMatrix * vec4(inPosition, 1.0);
    FragPos = vec3(viewPos);
    
    mat3 normalMatrix = transpose(inverse(mat3(ViewMatrix * ModelMatrix)));
    Normal = normalMatrix * (inNormal);
    
    gl_Position = ProjectionMatrix * viewPos;

    TexCoord = inTexCoord;
}