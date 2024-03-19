#include "BufferManager.h"
#include <cstring>



	void BufferManager::requestVAO(GLuint* id) {
		glGenVertexArrays(1, id);
	}

	void BufferManager::RequestVBO(GLuint* id, char* name) {

	}

