#pragma once
#include "utils.h"
#include <map>


class ShaderManager
{
public:
	static void activateProgram(GLuint id);
	static GLuint createProgram(const char* vert, const char *frag);
	static void deactivateProgram();

	static std::string readShaderFromFile(const char*);

private:
	static GLuint activeProgram;
	static std::map<std::string, GLuint> programIds;
};

