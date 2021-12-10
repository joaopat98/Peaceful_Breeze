#include "shaders.h"
#include <fstream>

static unordered_map<GLint, Program*> programs({ {0, new Program()} });
static unordered_map<string, Program*> cached;

UniformNotFoundException::UniformNotFoundException(string name)
{
	uniformName = name;
}

#pragma region File I/O

unsigned long getFileLength(ifstream& file)
{
	if (!file.good()) return 0;

	unsigned long pos = (unsigned long)file.tellg();
	file.seekg(0, ios::end);
	unsigned long len = (unsigned long)file.tellg();
	file.seekg(ios::beg);

	return len;
}

vector<string> loadShaderLines(const char* filename) {
	vector<string> lines;
	string str;
	ifstream file;
	file.open(filename, ios::in);
	while (getline(file, str)) {
		/**/
		if (str.rfind("#include", 0) == 0) {
			int from = (int)str.find_first_of("\"") + 1;
			int to = (int)str.find_last_of("\"");
			string pathstr = str.substr(from, (size_t)to - (size_t)from);
			auto newLines = loadShaderLines(pathstr.c_str());
			if (newLines.empty()) {
				return {};
			}
			else
			{
				lines.insert(lines.end(), newLines.begin(), newLines.end());
			}
		}
		/**/
		else
			lines.push_back(str + "\n");
	}
	return lines;
}

string loadShader(const char* filename)
{
	/*/
	ifstream file;
	file.open(filename, ios::in); // opens as ASCII!
	if (file.fail()) {
		return NULL;
	}

	int len = getFileLength(file);

	auto* ShaderSource = (char*) new char[len + static_cast<__int64>(1)];
	ShaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good())
	{
		ShaderSource[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	ShaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();
	/**/
	string source;
	auto lines = loadShaderLines(filename);
	for (auto line : lines) {
		source += line;
	}

	return source;
}

#pragma endregion

#pragma region Shaders

Shader::Shader()
{
}

Shader::Shader(string path, GLuint type) {
	this->type = type;
	this->path = path;
}

bool Shader::compile() {
	auto sourceStr = loadShader(path.c_str());
	auto source = sourceStr.c_str();
	if (source == NULL) {
		errorMsg = "File \"" + path + "\" corrupted or not found\n";
	}
	id = glCreateShader(type);
	glShaderSource(id, 1, &source, 0);
	glCompileShader(id);

	GLint isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		errorMsg = "Error compiling \"" + path + "\":\n" + string(errorLog.begin(), errorLog.end());

		glDeleteShader(id);
		return false;
	}
	return true;
}

void Shader::destroy()
{
	for (GLint prog : programs) {
		glDetachShader(prog, id);
	}
	glDeleteShader(id);
}

#pragma endregion

#pragma region Programs

#pragma region Initialization

Program::Program()
{

}

Program::~Program()
{
	destroy();
}

void Program::create() {
	id = glCreateProgram();
	programs[id] = this;
}

bool Program::link() {
	glLinkProgram(id);
	GLint linkStatus;
	glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		errorMsg = string(errorLog.begin(), errorLog.end());

		return false;
	}
	GLint numActive = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &numActive);
	GLint maxLength;
	glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	for (int unif = 0; unif < numActive; ++unif)
	{
		GLint maxLengthEnd = maxLength + 1;
		GLsizei actualLength = 0;
		GLint uniformSize;
		GLenum uniformType;
		char* nameArr = new char[maxLengthEnd]();
		glGetActiveUniform(id, unif, maxLength, &actualLength, &uniformSize, &uniformType, nameArr);
		string name(nameArr, actualLength);
		GLint location = glGetUniformLocation(id, nameArr);
		delete[] nameArr;
		uniforms[name] = location;
	}
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &numActive);
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	for (int attrib = 0; attrib < numActive; ++attrib)
	{
		GLint maxLengthEnd = maxLength + 1;
		GLsizei actualLength = 0;
		GLint attribSize;
		GLenum attribType;
		char* nameArr = new char[maxLengthEnd]();
		glGetActiveAttrib(id, attrib, maxLength, &actualLength, &attribSize, &attribType, nameArr);
		string name(nameArr, actualLength);
		GLint location = glGetAttribLocation(id, nameArr);
		delete[] nameArr;
		attribs[name] = location;
	}
	return true;
}

Program* Program::fromPaths(string vertex, string fragment) {
	Program* program = new Program();
	program->create();

	auto vertexShader = Shader(vertex, GL_VERTEX_SHADER);
	auto fragmentShader = Shader(fragment, GL_FRAGMENT_SHADER);
	if (!vertexShader.compile()) {
		cout << vertexShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!fragmentShader.compile()) {
		cout << fragmentShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	program->addShader(vertexShader);
	program->addShader(fragmentShader);

	if (!program->link()) {
		cout << "Error linking program:\n" + program->errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	fragmentShader.destroy();
	vertexShader.destroy();
	return program;
}

Program* Program::fromPaths(string vertex, string fragment, string geometry) {
	Program* program = new Program();
	program->create();

	auto vertexShader = Shader(vertex, GL_VERTEX_SHADER);
	auto fragmentShader = Shader(fragment, GL_FRAGMENT_SHADER);
	auto geometryShader = Shader(geometry, GL_GEOMETRY_SHADER);
	if (!vertexShader.compile()) {
		cout << vertexShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!fragmentShader.compile()) {
		cout << fragmentShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!geometryShader.compile()) {
		cout << geometryShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}


	program->addShader(vertexShader);
	program->addShader(geometryShader);
	program->addShader(fragmentShader);

	if (!program->link()) {
		cout << "Error linking program:\n" + program->errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	fragmentShader.destroy();
	vertexShader.destroy();
	geometryShader.destroy();

	return program;
}

Program* Program::fromPaths(string vertex, string fragment, string tcs, string tes) {
	Program* program = new Program();
	program->create();

	auto vertexShader = Shader(vertex, GL_VERTEX_SHADER);
	auto tcsShader = Shader(tcs, GL_TESS_CONTROL_SHADER);
	auto tesShader = Shader(tes, GL_TESS_EVALUATION_SHADER);
	auto fragmentShader = Shader(fragment, GL_FRAGMENT_SHADER);
	if (!vertexShader.compile()) {
		cout << vertexShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!tcsShader.compile()) {
		cout << tcsShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!tesShader.compile()) {
		cout << tesShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!fragmentShader.compile()) {
		cout << fragmentShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	program->addShader(vertexShader);
	program->addShader(tcsShader);
	program->addShader(tesShader);
	program->addShader(fragmentShader);

	if (!program->link()) {
		cout << "Error linking program:\n" + program->errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	fragmentShader.destroy();
	tesShader.destroy();
	tcsShader.destroy();
	vertexShader.destroy();
	return program;
}

Program* Program::fromPaths(string vertex, string fragment, string tcs, string tes, string geometry) {
	Program* program = new Program();
	program->create();

	auto vertexShader = Shader(vertex, GL_VERTEX_SHADER);
	auto tcsShader = Shader(tcs, GL_TESS_CONTROL_SHADER);
	auto tesShader = Shader(tes, GL_TESS_EVALUATION_SHADER);
	auto geometryShader = Shader(geometry, GL_GEOMETRY_SHADER);
	auto fragmentShader = Shader(fragment, GL_FRAGMENT_SHADER);
	if (!vertexShader.compile()) {
		cout << vertexShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!tcsShader.compile()) {
		cout << tcsShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!tesShader.compile()) {
		cout << tesShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!geometryShader.compile()) {
		cout << geometryShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}
	if (!fragmentShader.compile()) {
		cout << fragmentShader.errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	program->addShader(vertexShader);
	program->addShader(tcsShader);
	program->addShader(tesShader);
	program->addShader(geometryShader);
	program->addShader(fragmentShader);

	if (!program->link()) {
		cout << "Error linking program:\n" + program->errorMsg << "press any key to exit...";
		int a = getchar();
		exit(EXIT_FAILURE);
	}

	fragmentShader.destroy();
	geometryShader.destroy();
	tesShader.destroy();
	tcsShader.destroy();
	vertexShader.destroy();
	return program;
}

string strjoin(vector<string> strs) {
	stringstream str;
	if (strs.size() > 0) {
		str << strs[0];
		for (int i = 1; i < strs.size(); i++) {
			str << " " << strs[i];
		}
	}
	return str.str();
}

Program* Program::getOrCompile(string vertex, string fragment)
{
	Program* prog;
	string key = strjoin({ vertex, fragment });
	auto it = cached.find(key);
	if (it == cached.end()) {
		prog = fromPaths(vertex, fragment);
		cached[key] = prog;
	}
	else
	{
		prog = it->second;
	}
	return prog;
}

Program* Program::getOrCompile(string vertex, string fragment, string geometry)
{
	Program* prog;
	string key = strjoin({ vertex, fragment, geometry });
	auto it = cached.find(key);
	if (it == cached.end()) {
		prog = fromPaths(vertex, fragment, geometry);
		cached[key] = prog;
	}
	else
	{
		prog = it->second;
	}
	return prog;
}

Program* Program::getOrCompile(string vertex, string fragment, string tcs, string tes)
{
	Program* prog;
	string key = strjoin({ vertex, fragment, tcs, tes });
	auto it = cached.find(key);
	if (it == cached.end()) {
		prog = fromPaths(vertex, fragment, tcs, tes);
		cached[key] = prog;
	}
	else
	{
		prog = it->second;
	}
	return prog;
}

Program* Program::getOrCompile(string vertex, string fragment, string tcs, string tes, string geometry)
{
	Program* prog;
	string key = strjoin({ vertex, fragment, tcs, tes, geometry });
	auto it = cached.find(key);
	if (it == cached.end()) {
		prog = fromPaths(vertex, fragment, tcs, tes, geometry);
		cached[key] = prog;
	}
	else
	{
		prog = it->second;
	}
	return prog;
}

#pragma endregion

#pragma region Methods

void Program::use() {
	glUseProgram(id);
}

void Program::destroy()
{
	glUseProgram(0);
	glDeleteProgram(id);
}

void Program::addShader(Shader s)
{
	glAttachShader(id, s.id);
	s.programs.push_back(id);
}

GLint Program::getUniformLocation(const char* name) {
	try {
		return uniforms.at(name);
	}
	catch (exception e) {
		throw UniformNotFoundException(name);
	}
}

bool Program::hasUniform(string name)
{
	return uniforms.find(name) != uniforms.end();
}

bool Program::hasAttrib(string name)
{
	return attribs.find(name) != attribs.end();
}

void Program::removeAttrib(string name)
{
	if (attribs.find(name) != attribs.end()) {
		glDisableVertexAttribArray(attribs[name]);
		attribs.erase(name);
	}
}

Program* Program::getByID(GLint id)
{
	return programs[id];
}

Program* Program::getCurrent()
{
	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
	return programs[id];
}

#pragma endregion

#pragma region Uniforms

void Program::setUniform1i(const char* name, int val) {
	Program* old = getCurrent();
	use();
	glUniform1i(getUniformLocation(name), val);
	old->use();
}

void Program::setUniform1f(const char* name, float val) {
	Program* old = getCurrent();
	use();
	glUniform1f(getUniformLocation(name), val);
	old->use();
}

void Program::setUniform4fv(const char* name, float* val) {
	Program* old = getCurrent();
	use();
	glUniform4fv(getUniformLocation(name), 1, val);
	old->use();
}

void Program::setUniform4f(const char* name, float v0, float v1, float v2, float v3) {
	Program* old = getCurrent();
	use();
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
	old->use();
}

void Program::setUniform3fv(const char* name, float* val) {
	Program* old = getCurrent();
	use();
	glUniform3fv(getUniformLocation(name), 1, val);
	old->use();
}

void Program::setUniform3f(const char* name, float v0, float v1, float v2) {
	Program* old = getCurrent();
	use();
	glUniform3f(getUniformLocation(name), v0, v1, v2);
	old->use();
}

void Program::setUniformv2(const char* name, vec2 v) {
	Program* old = getCurrent();
	use();
	float data[2];
	v.toArray(data);
	glUniform2fv(getUniformLocation(name), 1, data);
	old->use();
}

void Program::setUniformv2(const char* name, int count, vec2* v)
{
	Program* old = getCurrent();
	use();
	float* data = new float[(size_t)count * 2];
	for (int i = 0; i < count; i++) {
		v[i].toArray(data + ((size_t)i * 2));
	}
	ostringstream sb;
	sb << name << "[0]";
	glUniform2fv(getUniformLocation(sb.str().c_str()), count, data);
	delete[] data;
	old->use();
}

void Program::setUniformv3(const char* name, vec3 v) {
	Program* old = getCurrent();
	use();
	float data[3];
	v.toArray(data);
	glUniform3fv(getUniformLocation(name), 1, data);
	old->use();
}

void Program::setUniformv3(const char* name, int count, vec3* v)
{
	Program* old = getCurrent();
	use();
	float* data = new float[(size_t)count * 3];
	for (int i = 0; i < count; i++) {
		v[i].toArray(data + ((size_t)i * 3));
	}
	ostringstream sb;
	sb << name << "[0]";
	glUniform3fv(getUniformLocation(sb.str().c_str()), count, data);
	delete[] data;
	old->use();
}

void Program::setUniformv4(const char* name, vec4 v) {
	Program* old = getCurrent();
	use();
	float data[4];
	v.toArray(data);
	glUniform4fv(getUniformLocation(name), 1, data);
	old->use();
}

void Program::setUniformv4(const char* name, int count, vec4* v)
{
	Program* old = getCurrent();
	use();
	float* data = new float[(size_t)count * 4];
	for (int i = 0; i < count; i++) {
		v[i].toArray(data + ((size_t)i * 4));
	}
	ostringstream sb;
	sb << name << "[0]";
	glUniform4fv(getUniformLocation(sb.str().c_str()), count, data);
	delete[] data;
	old->use();
}

void Program::setUniformMat2(const char* name, mat2 m) {
	Program* old = getCurrent();
	use();
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_TRUE, m.data);
	old->use();
}

void Program::setUniformMat3(const char* name, mat3 m) {
	Program* old = getCurrent();
	use();
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_TRUE, m.data);
	old->use();
}

void Program::setUniformMat4(const char* name, mat4 m) {
	Program* old = getCurrent();
	use();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, m.data);
	old->use();
}

#pragma endregion

#pragma endregion