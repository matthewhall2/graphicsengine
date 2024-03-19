#include "World.h"
#include "MatrixManager.h"
#include "BufferManager.h"
using mm = MatrixManager;


World::World() : Drawable(3) {
	//this->meshes[0].meshFilename = (char*)"meshes/zep_prop.obj";
	this->meshes[0].texture.textureFolder = (char*)"textures/ground";
	this->meshes[0].mapType = UV_ORTHO;
	this->meshes[0].vertFileName = (char*)"a3_2.vert";
	this->meshes[0].fragFileName = (char*)"a3_2.frag";



	this->meshes[0].meshFilename = (char*)"meshes/zep_car.obj";
	this->meshes[1].meshFilename = (char*)"meshes/zep_car.obj";
	this->meshes[2].meshFilename = (char*)"meshes/zep_car.obj";

	this->meshes[1].texture.textureFolder = (char*)"textures/bulding_window";
	this->meshes[2].texture.textureFolder = (char*)"textures/brick_1";

	this->meshes[1].mapType = UV_MAP_CYLINDRICAL;
	this->meshes[2].mapType = UV_MAP_CYLINDRICAL;

	this->meshes[1].vertFileName = (char*)"a3.vert";
	this->meshes[1].fragFileName = (char*)"a3.frag";
		this->meshes[2].vertFileName = (char*)"a3.vert";
	this->meshes[2].fragFileName = (char*)"a3.frag";
	this->hasHealth = false;
	loadMeshes();
	//this->sameShaderForAll();
	this->assignShaders();
	//this->loadMeshes();
	ground = &this->meshes[0];
	this->meshes[0].objdata->positions = (Vector3D*)malloc(sizeof(Vertex) * 4);
	this->meshes[0].objdata->normals = (Vector3D*)malloc(sizeof(Vertex) * 4);
	this->meshes[0].objdata->indices = (GLuint*)malloc(sizeof(GLuint) * 6);
	this->meshes[0].width = 20;
	this->meshes[0].height = 20;
	this->meshes[0].objdata->numIndices = 6;
	this->meshes[0].objdata->numVertices = 4;


	
	this->meshes[0].objdata->positions[0].x = -10;
	this->meshes[0].objdata->positions[0].y = -10;
	this->meshes[0].objdata->positions[0].z = 0;

	this->meshes[0].objdata->positions[1].x = -10;
	this->meshes[0].objdata->positions[1].y = 10;
	this->meshes[0].objdata->positions[1].z = 0;

	this->meshes[0].objdata->positions[2].x = 10;
	this->meshes[0].objdata->positions[2].y = 10;
	this->meshes[0].objdata->positions[2].z = 0;

	this->meshes[0].objdata->positions[3].x = 10;
	this->meshes[0].objdata->positions[3].y = -10;
	this->meshes[0].objdata->positions[3].z = 0;

	this->meshes[0].objdata->normals[0].x = 0;
	this->meshes[0].objdata->normals[0].y = 0;
	this->meshes[0].objdata->normals[0].z = 1;

	this->meshes[0].objdata->normals[1].x = 0;
	this->meshes[0].objdata->normals[1].y = 0;
	this->meshes[0].objdata->normals[1].z = 1;

	this->meshes[0].objdata->normals[2].x = 0;
	this->meshes[0].objdata->normals[2].y = 0;
	this->meshes[0].objdata->normals[2].z = 1;

	this->meshes[0].objdata->normals[3].x = 0;
	this->meshes[0].objdata->normals[3].y = 0;
	this->meshes[0].objdata->normals[3].z = 1;

	this->meshes[0].objdata->indices[0] = 0;
	this->meshes[0].objdata->indices[1] = 3;
	this->meshes[0].objdata->indices[2] = 2;
	this->meshes[0].objdata->indices[3] = 0;
	this->meshes[0].objdata->indices[4] = 2;
	this->meshes[0].objdata->indices[5] = 1;

	this->meshes[0].startPoint = 0;

	glGenBuffers(1, &this->vboID);
	glGenBuffers(1, &this->eboID);


	Utils::genVertexList(&this->meshes[0]);
	BufferManager::requestVAO(&this->meshes[0].vaoID);
	this->totalVertices += this->meshes[0].objdata->numVertices;
	this->totalIndices += this->meshes[0].objdata->numIndices;
	Utils::textureMap(this->meshes[0].mapType, &this->meshes[0]);
	Utils::loadTextureImages(&this->meshes[0]);


	buildBigArray();
	for (int i = 0; i < numMeshes; i++) {
		Utils::textureMap(this->meshes[i].mapType, &this->meshes[i]);
	}

	setupVAOS();

}

void World::draw() {
	mm::push();
	//mm::rotate(90, 1, 0, 0);
//	mm::scale(12, 12, 1);
	mm::push();
	float y = meshes[2].height;
	mm::translate(-20, y / 2, -5);
	mm::scale(1, 1.5, 1);
	drawMesh(&meshes[2], this);
	mm::pop();

	mm::push();
	y = meshes[1].height;
	//mm::rotate(90, 1, 0, 0);
	//mm::scale(12, 12, 1);
	mm::translate(15, y / 2, 5);
	mm::scale(1, 1.2, 1);
	drawMesh(&meshes[1], this);
	mm::pop();

	mm::push();
	y = meshes[2].height;
	mm::translate(11, y / 2, -10);
	mm::scale(1, 1.5, 1);
	drawMesh(&meshes[2], this);
	mm::pop();

	mm::push();
	y = meshes[1].height;
	//mm::rotate(90, 1, 0, 0);
	//mm::scale(12, 12, 1);
	mm::translate(-10, y / 2, 0);
	mm::scale(1, 1.2, 1);
	drawMesh(&meshes[1], this);
	mm::pop();

	mm::push();
	mm::rotate(90, 1, 0, 0);
	mm::scale(12, 12, 1);
	drawMesh(ground, this);
	mm::pop();
}

void World::genGround() {
	int nx = 100; int nz = 100; float minX = -10; float maxX = 10; float minZ = -10; float maxZ = 10;
	this->meshes[0].objdata->positions = (Vector3D*)malloc(sizeof(Vertex) * nx * nz);
	this->meshes[0].objdata->normals = (Vector3D*)malloc(sizeof(Vertex) * nx * nz);
	this->meshes[0].objdata->numVertices = nx * nz;
	float dx = (maxX - minX) / nx;
	float dz = (maxZ - minZ) / nz;
	//this->meshes[0].width = 

	for (int i = 0; i < 10000; i++) {
		this->meshes[0].objdata->positions[i].x = minX + static_cast<float>(i % nx) * dx;
		this->meshes[0].objdata->positions[i].z = minZ + static_cast<float>(i / nz) * dz;
		this->meshes[0].objdata->positions[i].y = 0;

		this->meshes[0].objdata->positions[i].x = 0;
		this->meshes[0].objdata->positions[i].y = 1;
		this->meshes[0].objdata->positions[i].z = 0;
	}

	this->meshes[0].objdata->indices = (GLuint*)malloc(sizeof(Vertex) * nx * nz * 3);
	for (int i = 0; i < 10000; i++) {
		
	}


}