#version 330 core

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 Normal;
out vec3 FragPos;

out vec2 TexCoords;
	
void main(void)
{
	vec4 worldPosition = ModelMatrix * vec4(inPosition, 1);
	vec4 posRelativeToCam = ViewMatrix * worldPosition;
	gl_Position = ProjectionMatrix * posRelativeToCam;
	
	FragPos = vec3(ModelMatrix * vec4(inPosition, 1));
	Normal = mat3(transpose(inverse(ModelMatrix))) * inNormal;
	
	TexCoords = inTexCoord;
}