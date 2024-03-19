#include "utils.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <cmath>
#include <filesystem>
#pragma warning (disable : 4996)

void Utils::rotateParallelAxis(float x, float y, float z, Axis axis, float angle) {
	glTranslatef(x, y, z);
	glRotatef(angle, 1.0 * axis == 0, 1.0 * axis == 1, 1.0 * axis == 2);
	glTranslatef(-x, -y, -z);
}


void Utils::rotateCamera(float angle) {

}

void Utils::loadObj(char* fileName, ObjData* objData) {
	printf("loading obj %s\n", fileName);
	char buf[1024];
	char key[1024];
	int n;
	FILE* fin;

	int fc = 0; // face count
	int vc = 0; // vertex count
	int nc = 0; // normal count

	/* Process each line of the OBJ file, count the number of vertices, indices */
	if ((fin = fopen(fileName, "r")))
	{
		while (fgets(buf, 1024, fin))
			if (sscanf(buf, "%s%n", key, &n) >= 1)
			{
				if (!strcmp(key, "f"))
				{
					fc += 3;
				}
				else if (!strcmp(key, "v"))
				{
					vc++;
				}
				else if (!strcmp(key, "vn"))
				{
					nc++;
				}
			}
		fclose(fin);

		objData->numIndices = fc;
		objData->numVertices = vc;
	}
	else
	{
		printf("error: %d\n", errno);
		perror("failed");
	}
	printf("done counting\n");
	/* Allocate appropriate amount of memory */
	objData->positions = (Vector3D*)malloc(sizeof(Vector3D) * (objData->numVertices));
	objData->normals = (Vector3D*)malloc(sizeof(Vector3D) * (objData->numVertices));
	objData->indices = (unsigned int*)malloc(sizeof(unsigned int) * (objData->numIndices));
	printf("done mallocing\n");
	/* Process each line of the OBJ file again but save data this time */
	fc = 0; // face count
	vc = 0; // vertex count
	nc = 0; // normal count
	if ((fin = fopen(fileName, "r")))
	{
		while (fgets(buf, 1024, fin))
			if (sscanf(buf, "%s%n", key, &n) >= 1)
			{

				if (!strcmp(key, "f"))
				{
					sscanf(buf + 1, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &(objData->indices[fc]), &(objData->indices[fc + 1]), &(objData->indices[fc + 2]));
					fc += 3;
				}
				else if (!strcmp(key, "v"))
				{
					sscanf(buf + 1, "%f %f %f", &(objData->positions[vc].x), &(objData->positions[vc].y), &(objData->positions[vc].z));
					vc++;
				}
				else if (!strcmp(key, "vn"))
				{
					sscanf(buf + 2, "%f %f %f", &(objData->normals[nc].x), &(objData->normals[nc].y), &(objData->normals[nc].z));
					nc++;
				}
			}
		fclose(fin);
	}
	printf("%d indices\n", objData->numIndices);
	printf("%d vertices\n", objData->numVertices);
	printf("done\n");
}

void Utils::genVertexList(Mesh *mesh) {
	GLuint numVertices = mesh->objdata->numVertices;
	mesh->vertices = (Vertex *)malloc(numVertices * sizeof(Vertex));
	for (int i = 0; i < numVertices; i++) {
		mesh->vertices[i].normal = mesh->objdata->normals[i];
		mesh->vertices[i].position = mesh->objdata->positions[i];
		Vector2D x;
		x.u = 0;
		x.v = 0;
		mesh->vertices[i].textureCoordinates = x;
	}
}


void Utils::textureMap(MAP_TYPE t, Mesh* mesh) {
	if (t == UV_MAP_CYLINDRICAL) {
		Utils::cylindrialMap(mesh);
	}
	else if (t == UV_ORTHO) {
		Utils::orthoMap(mesh);
	}
}

void Utils::orthoMap(Mesh* mesh) {
	for (int i = 0; i < mesh->objdata->numVertices; i++) {
		//mesh->vertices[i].textureCoordinates.u = ((mesh->vertices[i].position.x / (mesh->width / 2)) + 1) / 2;
		//mesh->vertices[i].textureCoordinates.v = ((mesh->vertices[i].position.y / (mesh->height / 2)) + 1) / 2;

		mesh->vertices[i].textureCoordinates.u = mesh->vertices[i].position.x / mesh->width + 0.5;
		mesh->vertices[i].textureCoordinates.v = mesh->vertices[i].position.y / mesh->height + 0.5;
	}
}

void Utils::cylindrialMap(Mesh* mesh) {
	GLuint count = mesh->objdata->numVertices;
	for (int i = 0; i < count; i++) {
		Vertex* v = &(mesh->vertices[i]);
		GLdouble angle = atan2(v->normal.z, v->normal.x);
		v->textureCoordinates.u = (angle / (2 * 3.141592)) + 0.5f;
		v->textureCoordinates.v = (v->normal.y + 1.0f) / 2;
	}
}


GLuint Utils::loadTextureImages(Mesh *mesh) {
	
	GLuint id;
	if (mesh->texture.textureFolder[0] == '\0') {
		return 1;
	}

	for (const auto& entry : std::filesystem::directory_iterator(mesh->texture.textureFolder)) {
		std::string filePath = entry.path().string();

		if (filePath.find("Displacement") != std::string::npos) {
			mesh->texture.displacementId = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
		else if (filePath.find("NormalGL") != std::string::npos) {
			mesh->texture.normalId = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
		else if (filePath.find("Ambient") != std::string::npos || filePath.find("Metalness") != std::string::npos) {
			mesh->texture.occlusionId = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
		else if (filePath.find("Roughness") != std::string::npos) {
			mesh->texture.roughnessId = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
		else if (filePath.find("Color") != std::string::npos) {
			mesh->texture.colourId = SOIL_load_OGL_texture(filePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
	}
	return 1;
}




	

