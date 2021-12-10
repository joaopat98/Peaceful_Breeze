#ifndef MESH_H
#define MESH_H

#include "matrix.h"
#include "quaternions.h"
#include "shaders.h"
#include "vectors.h"
#include <iostream>
#include <fstream>
#include <map>

struct VertexProps {
	unsigned int v;
	unsigned int vt;
	unsigned int vn;
};

struct VertexPropsHash
{
	size_t operator() (const VertexProps& vp) const
	{
		size_t h1 = hash<unsigned int>()(vp.v);
		size_t h2 = hash<unsigned int>()(vp.vt);
		size_t h3 = hash<unsigned int>()(vp.vn);

		return h1 ^ h2 ^ h3;
	}
};

struct VertexPropsCompare
{
	bool operator() (const VertexProps& lhs, const VertexProps& rhs) const
	{
		return (lhs.v == rhs.v) && (lhs.vt == rhs.vt) && (lhs.vn == rhs.vn);
	}
};

using namespace std;

struct Face {
	vector<unsigned int> indices;
};

struct FaceParams {
	vector<unsigned int> vertices;
	vector<unsigned int> texCoords;
	vector<unsigned int> normals;
};

struct Mesh {
	vector<vec3> vertices;
	vector<vec2> texCoords;
	vector<vec3> normals;
	GLuint vaoId = 0;
	vector<Face> faces;
	bool hasVertices = false;
	bool hasTexCoords = false;
	bool hasNormals = false;
	GLuint _vboVertices = 0;
	GLuint _vboTexcoords = 0;
	GLuint _vboNormals = 0;
	GLuint _vboElements = 0;
	unordered_map<VertexProps, unsigned int, VertexPropsHash, VertexPropsCompare> _indices;
	unsigned int _indexCounter = 0;
	unsigned int _numTris = 0;
	unsigned int _allocVertex(VertexProps vp);
	void _parseVertex(stringstream& sin);
	void _parseTexcoord(stringstream& sin);
	void _parseNormal(stringstream& sin);
	void _parseFace(stringstream& sin);
	void _parseLine(stringstream& sin);
	void _loadMeshData(string& filename);
	const void _createMesh(string& filename);
	void _createBufferObjects();
	Mesh();
	static Mesh *fromObj(string path);
	static Mesh *fromFaces(vector<vec3> v, vector<vec2> t, vector<vec3> n, vector<FaceParams> f);
	static Mesh* getScreenQuad();
	void draw(bool useTesselation = false);
	void destroy();
};

#endif // !MESH_H
