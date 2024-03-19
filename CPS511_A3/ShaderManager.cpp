#include "ShaderManager.h"
#include <iostream>
#include <fstream>
GLuint ShaderManager::activeProgram = 0;
std::map<std::string, GLuint> ShaderManager::programIds = std::map<std::string, GLuint>();

void ShaderManager::activateProgram(GLuint id) {
	if (activeProgram != id) {
		//printf("activating program\n");
		glUseProgram(id);
		activeProgram = id;
	}
}

void ShaderManager::deactivateProgram() {
	glUseProgram(0);
	activeProgram = 0;
}

GLuint ShaderManager::createProgram(const char* vert, const char* frag) {
	using namespace std;

	string v(vert);
	string f(frag);
	string progId = v.append(f);
	printf("%d\n", programIds.size());
	if (programIds.find(progId) != programIds.end()) {
		printf("found\n");
		return programIds[progId];
	}

	string vertShader = ShaderManager::readShaderFromFile(vert);
	const char* vertShaderSrc = vertShader.c_str();
	

	string fragShader = ShaderManager::readShaderFromFile(frag);
	const char* fragShaderSrc = fragShader.c_str();


	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLint compileStatus;
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vShader);
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(vShader, logLength, NULL, infoLog);

#ifdef _WIN32
		//OutputDebugString(infoLog);
#else
		std::cerr << infoLog << std::endl;
#endif
		delete infoLog;
		printf("vertex shader faield\n");
		exit(-1);
	}
	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint logLength;
		glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* infoLog = new GLchar[logLength];
		glGetShaderInfoLog(fShader, logLength, NULL, infoLog);

#ifdef _WIN32
		//OutputDebugString(infoLog);
#else
		std::cerr << infoLog << std::endl;
#endif
		delete infoLog;
		printf("frag shader faield\n");

		exit(-1);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	programIds[progId] = vfProgram;

	// Check for errors

	return vfProgram;
}

std::string ShaderManager::readShaderFromFile(const char* filePath) {
	using namespace std;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + '\n');
	}

	fileStream.close();
	return content;
}
