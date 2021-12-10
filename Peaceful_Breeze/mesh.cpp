#include "mesh.h"
#include <algorithm>

Mesh* screenQuad = nullptr;

unsigned int Mesh::_allocVertex(VertexProps vp)
{
	auto it = _indices.find(vp);
	if (it == _indices.end()) {
		_indices[vp] = _indexCounter;
		return _indexCounter++;
	}
	else {
		return (*it).second;
	}
}

void Mesh::_parseVertex(stringstream& sin)
{
	vec3 v;
	sin >> v.x >> v.y >> v.z;
	vertices.push_back(v);
}

void Mesh::_parseTexcoord(stringstream& sin)
{
	vec2 t;
	sin >> t.x >> t.y;
	texCoords.push_back(t);
}

void Mesh::_parseNormal(stringstream& sin)
{
	vec3 n;
	sin >> n.x >> n.y >> n.z;
	normals.push_back(n);
}

void Mesh::_parseFace(stringstream& sin)
{
	Face face;
	string token;
	vector<unsigned int> indices;
	while (getline(sin, token, '/'))
	{
		VertexProps vp = {};
		if (token.size() > 0)
			vp.v = stoi(token) - 1;
		getline(sin, token, '/');
		if (token.size() > 0)
			vp.vt = stoi(token) - 1;
		getline(sin, token, ' ');
		if (token.size() > 0)
			vp.vn = stoi(token) - 1;
		indices.push_back(_allocVertex(vp));
	}
	for (int i = 0; i < indices.size() - 2; i++)
	{
		face.indices.push_back(indices[0]);
		face.indices.push_back(indices[(size_t)i + 1]);
		face.indices.push_back(indices[(size_t)i + 2]);
		_numTris++;
	}
	faces.push_back(face);
}

void Mesh::_parseLine(stringstream& sin)
{
	string s;
	sin >> s;
	if (s.compare("v") == 0) _parseVertex(sin);
	else if (s.compare("vt") == 0) _parseTexcoord(sin);
	else if (s.compare("vn") == 0) _parseNormal(sin);
	else if (s.compare("f") == 0) _parseFace(sin);
}

void Mesh::_loadMeshData(string& filename)
{
	ifstream ifile(filename);
	string line;
	bool good = ifile.bad();
	while (getline(ifile, line)) {
		stringstream sline(line);
		_parseLine(sline);
	}
}

const void Mesh::_createMesh(string& filename)
{
	_loadMeshData(filename);
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void Mesh::_createBufferObjects()
{
	vec3* vr = nullptr, * nr = nullptr;
	vec2* tr = nullptr;
	vector<unsigned int> indices;

	if (vertices.size() > 0)
		vr = new vec3[_indexCounter];
	if (texCoords.size() > 0)
		tr = new vec2[_indexCounter];
	if (normals.size() > 0)
		nr = new vec3[_indexCounter];

	for (auto& vp : _indices) {
		if (vertices.size() > 0)
			vr[vp.second] = vertices[vp.first.v];
		if (texCoords.size() > 0)
			tr[vp.second] = texCoords[vp.first.vt];
		if (normals.size() > 0)
			nr[vp.second] = normals[vp.first.vn];
	}

	for (auto face : faces)
	{
		indices.insert(indices.end(), face.indices.begin(), face.indices.end());
	}

	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);
	{
		if (vertices.size() > 0) {
			hasVertices = true;
			glGenBuffers(1, &_vboVertices);
			glBindBuffer(GL_ARRAY_BUFFER, _vboVertices);
			glBufferData(GL_ARRAY_BUFFER, _indexCounter * sizeof(vec3), vr, GL_STATIC_DRAW);
		}
		if (texCoords.size() > 0) {
			hasTexCoords = true;
			glGenBuffers(1, &_vboTexcoords);
			glBindBuffer(GL_ARRAY_BUFFER, _vboTexcoords);
			glBufferData(GL_ARRAY_BUFFER, _indexCounter * sizeof(vec2), tr, GL_STATIC_DRAW);
		}
		if (normals.size() > 0) {
			hasNormals = true;
			glGenBuffers(1, &_vboNormals);
			glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
			glBufferData(GL_ARRAY_BUFFER, _indexCounter * sizeof(vec3), nr, GL_STATIC_DRAW);
		}
		glGenBuffers(1, &_vboElements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboElements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (vertices.size() > 0)
		delete[] vr;
	if (texCoords.size() > 0)
		delete[] tr;
	if (normals.size() > 0)
		delete[] nr;

	vertices.clear();
	texCoords.clear();
	normals.clear();
	faces.clear();
	_indices.clear();
}

void destroyBufferObjects(Mesh* mesh)
{
	if (mesh->hasVertices)
		glDeleteBuffers(1, &mesh->_vboVertices);
	if (mesh->hasTexCoords)
		glDeleteBuffers(1, &mesh->_vboTexcoords);
	if (mesh->hasNormals)
		glDeleteBuffers(1, &mesh->_vboVertices);
	glDeleteVertexArrays(1, &mesh->vaoId);
}

Mesh::Mesh()
{
}

Mesh* Mesh::fromObj(string path) {
	Mesh* m = new Mesh();
	m->_createMesh(path);
	m->_createBufferObjects();
	return m;
}

Mesh* Mesh::fromFaces(vector<vec3> v, vector<vec2> t, vector<vec3> n, vector<FaceParams> f) {
	Mesh* m = new Mesh();
	for (auto& faceParams : f)
	{
		Face face;
		vector<unsigned int> indices;
		int maxIndex = max(max((int)faceParams.vertices.size(), (int)faceParams.texCoords.size()), (int)faceParams.normals.size());
		for (int i = 0; i < maxIndex; i++)
		{
			VertexProps vp = {};
			if (faceParams.vertices.size() > 0)
				vp.v = faceParams.vertices[i];
			if (faceParams.texCoords.size() > 0)
				vp.vt = faceParams.texCoords[i];
			if (faceParams.normals.size() > 0)
				vp.vn = faceParams.normals[i];
			indices.push_back(m->_allocVertex(vp));
		}
		for (int i = 0; i < indices.size() - 2; i++)
		{
			face.indices.push_back(indices[0]);
			face.indices.push_back(indices[(size_t)i + 1]);
			face.indices.push_back(indices[(size_t)i + 2]);
			m->_numTris++;
		}
		m->faces.push_back(face);
	}
	m->vertices = v;
	m->texCoords = t;
	m->normals = n;
	m->_createBufferObjects();
	return m;
}

Mesh* Mesh::getScreenQuad()
{
	if (screenQuad == nullptr) {
		screenQuad = new Mesh();
		vector<vec3> vertices = {
			{-1, -1, 0},
			{1, -1, 0},
			{1, 1, 0},
			{-1, 1, 0}
		};
		vector<vec2> texCoords = {
			{0, 0},
			{1, 0},
			{1, 1},
			{0, 1}
		};
		vector<FaceParams> faces = { {
			{0,1,2,3},
			{0,1,2,3},
			{}
		} };
		screenQuad = fromFaces(vertices, texCoords, {}, faces);
	}
	return screenQuad;
}

void Mesh::draw(bool useTesselation)
{
	Program* program = Program::getCurrent();
	glBindVertexArray(vaoId);
	if (program->hasAttrib("inPosition") && hasVertices) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboVertices);
		glEnableVertexAttribArray(program->attribs["inPosition"]);
		glVertexAttribPointer(program->attribs["inPosition"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	}

	if (program->hasAttrib("inTexCoord") && hasTexCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboTexcoords);
		glEnableVertexAttribArray(program->attribs["inTexCoord"]);
		glVertexAttribPointer(program->attribs["inTexCoord"], 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
	}

	if (program->hasAttrib("inNormal") && hasNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
		glEnableVertexAttribArray(program->attribs["inNormal"]);
		glVertexAttribPointer(program->attribs["inNormal"], 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboElements);
	if (useTesselation) {
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, _numTris * 3, GL_UNSIGNED_INT, (void*)0);
	}
	else
		glDrawElements(GL_TRIANGLES, _numTris * 3, GL_UNSIGNED_INT, (void*)0);

	/*
	int faceStart = 0;
	for (Face face : faces) {
		glDrawArrays(GL_TRIANGLE_FAN, faceStart, (GLsizei)face.vertices.size());
		faceStart += (int)face.vertices.size();
	}

	*/
	if (program->hasAttrib("inPosition") && hasVertices) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboVertices);
		glDisableVertexAttribArray(program->attribs["inPosition"]);
	}

	if (program->hasAttrib("inTexCoord") && hasTexCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboTexcoords);
		glDisableVertexAttribArray(program->attribs["inTexCoord"]);
	}

	if (program->hasAttrib("inNormal") && hasNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
		glDisableVertexAttribArray(program->attribs["inNormal"]);
	}
}

void Mesh::destroy()
{
	destroyBufferObjects(this);
}
