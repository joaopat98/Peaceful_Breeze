#version 430 core

layout (vertices = 3) out;

//in vec3 WorldPos_CS_in[];
in vec3 Normal_CS_in[];
out vec3 Normal_ES_in[];

uniform int tesselation;

//out vec3 WorldPos_ES_in[];
void main()                                                                                     
{
    gl_TessLevelInner[0] = tesselation;
    gl_TessLevelOuter[0] = tesselation;
    gl_TessLevelOuter[1] = tesselation;
    gl_TessLevelOuter[2] = tesselation;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
    //WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
}