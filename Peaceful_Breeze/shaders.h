#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glew.h>
#include <string>
#include <exception>
#include <vector>
#include <unordered_map>
#include "vectors.h"
#include "matrix.h"

using namespace std;

class UniformNotFoundException : public exception {
public:
	string uniformName;
	UniformNotFoundException(string name);
};

struct Shader {
	string errorMsg;
	bool good = true;
	string path;
	GLuint id = 0;
	GLuint type = 0;
	vector<GLint> programs;
	Shader();
	Shader(string path, GLuint type);
	bool compile();
	void destroy();
};

struct Program {
	GLuint id = 0;
	vector<Shader> shaders;
	string errorMsg;
	unordered_map<string, GLint> attribs;
	unordered_map<string, GLint> uniforms;
	Program();
	~Program();
	void create();
	bool link();
	static Program* fromPaths(string vertex, string fragment);
	static Program* fromPaths(string vertex, string fragment, string geometry);
	static Program* fromPaths(string vertex, string fragment, string tcs, string tes);
	static Program* fromPaths(string vertex, string fragment, string tcs, string tes, string geometry);
	static Program* getOrCompile(string vertex, string fragment);
	static Program* getOrCompile(string vertex, string fragment, string geometry);
	static Program* getOrCompile(string vertex, string fragment, string tcs, string tes);
	static Program* getOrCompile(string vertex, string fragment, string tcs, string tes, string geometry);
	static Program* getByID(GLint id);
	static Program* getCurrent();
	void use();
	void destroy();
	void addShader(Shader s);
	GLint getUniformLocation(const char* name);
	bool hasUniform(string name);
	bool hasAttrib(string name);
	void removeAttrib(string name);
	void setUniform1i(const char* name, int val);
	void setUniform1f(const char* name, float val);
	void setUniform4fv(const char* name, float* val);
	void setUniform4f(const char* name, float v0, float v1, float v2, float v3);
	void setUniform3fv(const char* name, float* val);
	void setUniform3f(const char* name, float v0, float v1, float v2);
	void setUniformv2(const char* name, vec2 v);
	void setUniformv2(const char* name, int count, vec2 *v);
	void setUniformv3(const char* name, vec3 v);
	void setUniformv3(const char* name, int count, vec3 *v);
	void setUniformv4(const char* name, vec4 v);
	void setUniformv4(const char* name, int count, vec4 *v);
	void setUniformMat2(const char* name, mat2 m);
	void setUniformMat3(const char* name, mat3 m);
	void setUniformMat4(const char* name, mat4 m);
};

#endif // !SHADERS_H
