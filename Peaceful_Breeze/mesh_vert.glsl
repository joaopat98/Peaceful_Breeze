#version 330 core

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 exTexCoord;
out vec3 exNormal;
out vec4 FragPos;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1);
	exNormal = vec3(transpose(inverse(ModelMatrix)) * vec4(inNormal, 1));
	exTexCoord = inTexCoord;
	FragPos = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(inPosition, 1);
}