#version 430 core

layout(triangles, equal_spacing, ccw, point_mode) in;

in vec3 Normal_ES_in[];
out vec3 Normal_GS_in;
//in vec3 WorldPos_ES_in[];
//
//uniform mat4 ModelMatrix;
//
//out vec3 WorldPos_FS_in;
//
vec4 interpolate3D(vec4 v0, vec4 v1, vec4 v2)
{
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

void main()
{
    Normal_GS_in = vec3(interpolate3D(vec4(Normal_ES_in[0], 1), vec4(Normal_ES_in[1], 1), vec4(Normal_ES_in[2], 1)));
    Normal_GS_in = normalize(Normal_GS_in);
    //gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) + (gl_TessCoord.z * gl_in[2].gl_Position);
    gl_Position = interpolate3D(gl_in[0].gl_Position, gl_in[1].gl_Position,gl_in[2].gl_Position);
    //    WorldPos_FS_in = interpolate3D(WorldPos_ES_in[0], WorldPos_ES_in[1], WorldPos_ES_in[2]);
    //    gl_Position = ModelMatrix * vec4(WorldPos_FS_in, 1.0);
}
                                 