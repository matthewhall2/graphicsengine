#pragma warning (disable : 4996)
#include "Drawable.h"
#include "utils.h"
#include "BufferManager.h"
#include "ShaderManager.h"
#include <cstring>
#include <cassert>
#include  <cstddef>
#include "Lighting.h"
#include "MatrixManager.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>

void Drawable::draw(long t)
{

}

Drawable::Drawable(int numMeshes) {
	this->numMeshes = numMeshes;
	this->meshes = (Mesh*)malloc(sizeof(Mesh) * numMeshes);
	this->totalVertices = 0;
	this->totalIndices = 0;
	this->hasHealth = false;

	for (int i = 0; i < numMeshes; i++) {
		meshes[i].objdata = (ObjData*)malloc(sizeof(ObjData));
		meshes[i].meshFilename = new char[30];
		meshes[i].texture.colourId = 0;
		meshes[i].texture.displacementId = 0;
		meshes[i].texture.occlusionId = 0;
		meshes[i].texture.normalId = 0;
		meshes[i].texture.roughnessId = 0;

		meshes[i].displacementTune = 1;
		meshes[i].texture.textureFolder = new char[30];
		meshes[i].texture.textureFolder[0] = '\0';

	}
}

void Drawable::loadMeshes() {
	printf("\nloading meshes\n");
	glGenBuffers(1, &this->vboID);
	glGenBuffers(1, &this->eboID);
	
	printf("gened buffers\n");
	for (int i = 0; i < numMeshes; i++) {
		//assert(meshes[i].meshFilename[0] != '\0');
		//assert(meshes[i].textureFilename[0] != '\0');
	}

	for (int i = 0; i < numMeshes; i++) {
		Utils::loadObj(this->meshes[i].meshFilename, this->meshes[i].objdata);
		Utils::genVertexList(&this->meshes[i]);
		BufferManager::requestVAO(&this->meshes[i].vaoID);
		this->totalVertices += this->meshes[i].objdata->numVertices;
		this->totalIndices += this->meshes[i].objdata->numIndices;
		Utils::textureMap(this->meshes[i].mapType, &this->meshes[i]);
		Utils::loadTextureImages(&this->meshes[i]);
	}

	buildBigArray();
	for (int i = 0; i < numMeshes; i++) {
		Utils::textureMap(this->meshes[i].mapType, &this->meshes[i]);
	}

	setupVAOS();
}

void Drawable::setNames(Mesh *mesh, const char* objName, const char* textName, MAP_TYPE mapType) {
	mesh->meshFilename = new char[strlen(objName) + 1];
	strcpy(mesh->meshFilename, objName);

	//mesh->texture.colorFilename = new char[strlen(objName) + 1];
	//strcpy(mesh->texture.colorFilename, textName);

	mesh->mapType = mapType;
}

void Drawable::buildBigArray() {
	allVertices = (Vertex*)malloc(sizeof(Vertex) * totalVertices);
	allIndices = (GLuint*)malloc(sizeof(GLuint) * totalIndices);
	int count = 0;
	for (int i = 0; i < numMeshes; i++) {
		GLfloat minHeight = std::numeric_limits<double>::max();
		GLfloat maxHeight = std::numeric_limits<double>::min();

		GLfloat minWidth = std::numeric_limits<double>::max();
		GLfloat maxWidth = std::numeric_limits<double>::min();

		GLfloat minDepth = std::numeric_limits<double>::max();
		GLfloat maxDepth = std::numeric_limits<double>::min();

		meshes[i].startPoint = count;
		for (int j = 0; j < meshes[i].objdata->numVertices; j++) {
			allVertices[count] = meshes[i].vertices[j];
			minHeight = std::min(minHeight, allVertices[count].position.y);
			maxHeight = std::max(maxHeight, allVertices[count].position.y);

			minWidth = std::min(minWidth, allVertices[count].position.x);
			maxWidth = std::max(maxWidth, allVertices[count].position.x);

			minDepth = std::min(minDepth, allVertices[count].position.z);
			maxDepth = std::max(maxDepth, allVertices[count].position.z);
			count += 1;
		}
		meshes[i].height = maxHeight - minHeight;
		meshes[i].width = maxWidth - minWidth;
		meshes[i].depth = maxDepth - minDepth;

		meshes[i].minHeight = minHeight;
		meshes[i].minWidth = minWidth;
	}
	count = 0;
	int offset;
	for (int i = 0; i < numMeshes; i++) {
		if (i == 0) offset = 0; else offset += meshes[i - 1].objdata->numVertices;
		printf("offset = %d", offset);
		meshes[i].startPoint = count;
		for (int j = 0; j < meshes[i].objdata->numIndices; j++) {
			allIndices[count] = meshes[i].objdata->indices[j] + offset;
			//printf("at %d = %d\n", count, allIndices[count]);
			count += 1;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * totalIndices, allIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * totalVertices, allVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		
}

void Drawable::setupVAOS() {
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	for (int i = 0; i < numMeshes; i++) {
		GLuint id = meshes[i].vaoID;
		glBindVertexArray(id);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);

		// Specify the format and location of the vertex attributes
		glEnableVertexAttribArray(0);  // Vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

		glEnableVertexAttribArray(1);  // Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		
		glEnableVertexAttribArray(2);  // Texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));
		
	}
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Drawable::loadTextures(Mesh *mesh) {
	if (mesh->texture.colourId != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->texture.colourId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	if (mesh->texture.occlusionId != 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->texture.occlusionId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	}

	if (mesh->texture.roughnessId != 0) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mesh->texture.roughnessId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	}

	if (mesh->texture.displacementId != 0) {
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mesh->texture.displacementId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	}

	if (mesh->texture.normalId != 0) {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mesh->texture.normalId);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}




void Drawable::setDisplacementTuners(GLfloat* adjustments, int len) {
	for (int i = 0; i < this->numMeshes && i < len; i++) {
		this->meshes[i].displacementTune = adjustments[i];
	}
}



void Drawable::drawMesh(Mesh* mesh, Drawable *d) {
	GLint ModelLoc = glGetUniformLocation(mesh->shaderProgramId, "Model");
	GLint ViewLoc = glGetUniformLocation(mesh->shaderProgramId, "View");
	GLint projectionLoc = glGetUniformLocation(mesh->shaderProgramId, "projection");
	GLint viewPos = glGetUniformLocation(mesh->shaderProgramId, "viewPos");
	
	ShaderManager::activateProgram(mesh->shaderProgramId);

	Drawable::loadTextures(mesh);

	Lighting::addLights(mesh);

	glUniform1f(glGetUniformLocation(mesh->shaderProgramId, "tuneDisplacement"), mesh->displacementTune);

	if (d->hasHealth) {
		float col[] = {1 - ((float)d->health / 85.0f), 0, 0, 1};
		glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "material.ambient"), 1, col);
	}
	else {
		float col2[] = { 0, 0, 0, 1 };
		glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "material.ambient"), 1, col2);
	}
	

	glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(MatrixManager::getCTM()));
	glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(Camera::viewMatrix));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(Camera::projectionMatrix));

	glUniform4fv(viewPos, 1, glm::value_ptr(Camera::camPos));

	glBindVertexArray(mesh->vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, d->vboID);
//	int size = 0;
	//glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->eboID);
	//printf("size of array buffer = %d, \t", size / sizeof(Vertex));
	//size = 0;
//	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	//printf("size of element buffer = %d\t", size / sizeof(GLuint));
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->objdata->numVertices, mesh->vertices, GL_STATIC_DRAW);
	
	int i = 0;
	/*glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);*/
	//printf("drawing mesh %s, with %d indices and starting offset %d \t", mesh->meshFilename, mesh->objdata->numIndices, mesh->startPoint);
	glDrawElements(GL_TRIANGLES,
		mesh->objdata->numIndices,
		GL_UNSIGNED_INT, (void*)(mesh->startPoint * sizeof(GLuint)));// (void*)this->allIndices[this->meshes[0].startPoint]);
	ShaderManager::deactivateProgram();
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Drawable::sameShaderForAll() {
	for (int i = 1; i < this->numMeshes; i++) {
		this->meshes[i].vertFileName = this->meshes[0].vertFileName;
		this->meshes[i].fragFileName = this->meshes[0].fragFileName;
	}
}

void Drawable::assignShaders() {
	for (int i =0; i < this->numMeshes; i++) {
		meshes[i].shaderProgramId = ShaderManager::createProgram(meshes[i].vertFileName, meshes[i].fragFileName);
	}
}