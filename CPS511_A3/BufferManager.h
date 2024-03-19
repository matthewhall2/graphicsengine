#pragma once
#include "utils.h"
#include <unordered_map>
#define MAX_VBOS 10

class BufferManager
{
public:
	static void requestVAO(GLuint *);
	static void RequestVBO(GLuint *, char *name);
	static void buildVBO(char* name);

private:
	static int numVBO[MAX_VBOS];
	static std::unordered_map<char *, int> names;
};

