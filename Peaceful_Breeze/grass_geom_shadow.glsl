#version 430
#define MAX_GRASS_VERTICES 32
#include "noise.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = MAX_GRASS_VERTICES) out;

in vec3 Normal_GS_in[];

uniform vec3 grassVertices[MAX_GRASS_VERTICES];
uniform vec2 grassTexCoords[MAX_GRASS_VERTICES];
uniform int grassVerticesSize;
uniform vec3 bladeSize;

uniform mat4 ModelMatrix;
uniform mat4 LightSpaceMatrix;

uniform vec3 noisePos;
uniform float noiseScale;
uniform vec3 windDir;
uniform float windStrength;

out vec2 TexCoord;

mat4 rotate (float angle, vec3 axis) {
    float c = cos(angle);
    float s = sin(angle);
    float c_diff = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    return mat4(
        c + x * x * c_diff, y * x * c_diff + z * s, z * x * c_diff - y * s, 0,
        x * y * c_diff - z * s, c + y * y * c_diff, z * y * c_diff + x * s, 0,
        x * z * c_diff + y * s, y * z * c_diff - x * s, c + z * z * c_diff, 0,
        0, 0, 0, 1
    );
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 apply(mat4 m, vec3 v) {
    return vec3(m * vec4(v,1));
}

void main()
{
    vec3 rootpos = vec3(ModelMatrix * gl_in[0].gl_Position);

    mat3 normalMatrix = transpose(inverse(mat3(ModelMatrix)));
    vec3 surfaceNormal = normalize(normalMatrix * Normal_GS_in[0]);
    
    float noiseVal = snoise(noisePos + rootpos * noiseScale);
    
    vec3 rotAxis = vec3(1,0,0);
    if (windDir != surfaceNormal)
        vec3 rotAxis = normalize(cross(windDir, surfaceNormal));

    vec3 up = vec3(0,1,0);
    vec3 tangent, bitangent;
    if (surfaceNormal == up) {
        tangent = vec3(1,0,0);
        bitangent = vec3(0,0,1);
    }
    else {
        tangent = normalize(cross(surfaceNormal, up));
        bitangent = normalize(cross(tangent, surfaceNormal));
    }
    mat4 tangentMatrix = rotate(noiseVal * windStrength * (1 - abs(dot(surfaceNormal, windDir))), rotAxis) * rotate(rand(rootpos.xz) * 1000, surfaceNormal) * mat4(mat3(tangent, surfaceNormal, bitangent));

    vec3 pos;
    for (int i = 0; i < grassVerticesSize; i += 3) {
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i]);
        TexCoord = grassTexCoords[i];
        gl_Position = LightSpaceMatrix * vec4(pos, 1);
        EmitVertex();
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i+1]);
        TexCoord = grassTexCoords[i + 1];
        gl_Position = LightSpaceMatrix * vec4(pos, 1);
        EmitVertex();
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i+2]);
        TexCoord = grassTexCoords[i + 2];
        gl_Position = LightSpaceMatrix * vec4(pos, 1);
        EmitVertex();
        EndPrimitive();
    }
}