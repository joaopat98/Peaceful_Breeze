#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;

const int maxDivs = 10;
const int arraySize = int(pow(pow(2,maxDivs) + 1,2));
vec3 tris[arraySize];

int width;

int divs = 3;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

in vec2 geomTexCoord[];
in vec3 geomNormal[];

out vec2 exTexCoord;
out vec3 exNormal;

uniform float time;

vec3 getArr(int x, int y){
	return tris[y * width + x];
}

void setArr(int x, int y, vec3 v){
	tris[y * (width + 1) + x] = v;
}

void emit(vec3 v1, vec3 v2, vec3 v3) {
		vec3 norm = normalize(cross(v2 - v1, v3 - v1));
		gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(v1,1);
		exTexCoord = vec2(0,0);
		exNormal = norm;
		EmitVertex();
		gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(v2,1);
		exTexCoord = vec2(0,0);
		exNormal = norm;
		EmitVertex();
		gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(v3,1);
		exTexCoord = vec2(0,0);
		exNormal = norm;
		EmitVertex();
		EndPrimitive();
}

void subdivide(vec3 v1, vec3 v2, vec3 v3, int divs) {
	float len = length(v1);
	setArr(0, 0, v1);
	setArr(width, 0, v2);
	setArr(0, width, v3);
	for(int i = 0; i < divs; i++) {
		int stride = width / int(pow(2, i + 1));
		int steps = int(pow(2, i));
		int y = 0;
		for(int outer = 0; outer < steps; outer++) {
			int x = stride;
			for (int inner = 0; inner < steps - outer; inner++) {
				vec3 newPos = normalize((getArr(x - stride, y) + getArr(x + stride, y))/2) * len;
				setArr(x, y, newPos);
				newPos = normalize((getArr(y, x - stride) + getArr(y, x + stride))/2) * len;
				setArr(y, x, newPos);
				newPos = normalize((getArr(width - x - y + stride, x - stride) + getArr(width - x - y - stride, x + stride))/2) * len;
				setArr(width - x - y, x, newPos);
				x += stride * 2;
			}
			y += stride * 2;
		}
	}
	for (int i = 0; i < width; i++) {
		vec3 v1, v2, v3;
		for (int j = 0; j < width - i; j++) 
			emit(getArr(j, i), getArr(j + 1, i), getArr(j, i + 1));
		for (int j = 0; j < width - i - 1; j++) 
			emit(getArr(j + 1, i + 1), getArr(j, i + 1), getArr(j + 1, i));
	}
}

void main() {
	/*
	vec3 norm = normalize(cross(vec3(gl_in[1].gl_Position - gl_in[0].gl_Position), vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)));
    gl_Position = gl_in[0].gl_Position;
	exTexCoord = geomTexCoord[0];
	exNormal = normalize((norm + geomNormal[0]) / 2);
    EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	exTexCoord = geomTexCoord[1];
	exNormal = normalize((norm + geomNormal[1]) / 2);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
	exTexCoord = geomTexCoord[2];
	exNormal = normalize((norm + geomNormal[2]) / 2);
    EmitVertex();
    EndPrimitive();
	*/
	width = int(pow(2,divs));
	float lmao = time;
	subdivide(vec3(gl_in[0].gl_Position), vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position), divs);
}  