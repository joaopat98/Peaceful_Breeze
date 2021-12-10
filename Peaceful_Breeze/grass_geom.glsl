#version 430
#define MAX_GRASS_VERTICES 64
#include "noise.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = MAX_GRASS_VERTICES) out;

in vec3 Normal_GS_in[];

uniform vec3 grassVertices[MAX_GRASS_VERTICES];
uniform vec2 grassTexCoords[MAX_GRASS_VERTICES];
uniform vec3 grassNormals[MAX_GRASS_VERTICES];
uniform int grassVerticesSize;
uniform vec3 bladeSize;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 LightSpaceMatrix;

uniform vec3 noisePos;
uniform float noiseScale;
uniform vec3 windDir;
uniform float windStrength;
/*/
uniform float sinFactor;
uniform float offsetValue;
uniform float kAmpValue;
uniform bool kAmp;
/**/

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;


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
    vec4 nv = m * vec4(v,1);
    return (nv/nv.w).xyz;
}

void lightCalcs(vec3 pos, vec3 normal, int index) {
	vec4 posRelativeToCam = ViewMatrix * vec4(pos,1);
	gl_Position = ProjectionMatrix * posRelativeToCam;
	
	FragPos = pos;
	Normal = normal;
	
	TexCoords = grassTexCoords[index];
}

mat4 scalemat(vec3 v) {
    return mat4(
        v.x, 0, 0, 0,
        0, v.y, 0, 0,
        0, 0, v.z, 0,
        0, 0, 0, 1
    );
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

    vec3 pos, normal;

    /*/
    vec3 vWindDirection = vec3(1.0f, 0.0f, 1.0f);
	vWindDirection = normalize(vWindDirection);

    float windPower = 0.0f;
    if(kAmp)
        windPower = offsetValue+sin(rootpos.x*kAmpValue+rootpos.y*kAmpValue+rootpos.z*kAmpValue+fTimePassed*sinFactor);
    else
        windPower = offsetValue+sin(rootpos.x*rand(rootpos.xz)+rootpos.y*rand(rootpos.xz)+rootpos.z*rand(rootpos.xz)+fTimePassed*sinFactor);
    
    if(windPower < 0.0f)
	    windPower = windPower*0.2f;
    else windPower = windPower*0.3f;
    windPower *= windStrength;
    /**/

    for (int i = 0; i < grassVerticesSize; i += 3) {
        mat4 modelNormalMatrix = transpose(inverse(scalemat(bladeSize)));
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i]);
        normal = vec3(modelNormalMatrix * vec4(grassNormals[i],1));
        normal = apply(tangentMatrix, normal);
        lightCalcs(pos, normal, i);
        EmitVertex();
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i+1]);
        normal = vec3(modelNormalMatrix * vec4(grassNormals[i + 1],1));
        normal = apply(tangentMatrix, normal);
        lightCalcs(pos, normal, i+1);
        EmitVertex();
        pos = rootpos + apply(tangentMatrix, bladeSize * grassVertices[i+2]);
        normal = vec3(modelNormalMatrix * vec4(grassNormals[i + 2],1));
        normal = apply(tangentMatrix, normal);
        lightCalcs(pos, normal, i+2);
        EmitVertex();
        EndPrimitive();
        /**/
    }
}