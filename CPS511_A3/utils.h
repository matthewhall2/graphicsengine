#pragma once
#pragma warning (disable : 4996)
#include <GL/glew.h>
#include <GL/glut.h>
#include <limits>
#include <string>
#include "SOIL2/SOIL2.h"
// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)

typedef enum {
	AXIS_X=0,
	AXIS_Y=1,
	AXIS_Z=2
} Axis;

typedef struct
{
	GLfloat x, y, z;
} Vector3D;

typedef struct 
{
	GLfloat u, v;
} Vector2D;

typedef struct  {
	Vector3D position;
	Vector3D normal;
	Vector2D textureCoordinates;
} Vertex;

typedef struct 
{
	Vector3D* positions;
	Vector3D* normals;
	GLuint* indices;
	
	unsigned int numVertices;
	unsigned int numIndices;
} ObjData;

typedef enum {
	UV_MAP_CYLINDRICAL = 0,
	UV_ORTHO
} MAP_TYPE;

typedef struct {
	GLuint colourId;
	GLuint displacementId;
	GLuint normalId;
	GLuint occlusionId;
	GLuint roughnessId;

	char* textureFolder;

} Texture;

typedef struct  {
	ObjData *objdata;
	Vector2D* textureCoordinates;
	char *meshFilename;

	char *vertFileName;
	char *fragFileName;
	GLuint shaderProgramId;
	GLuint vaoID;

	Texture texture;

	Vertex* vertices;
	GLuint startPoint;
	MAP_TYPE mapType;

	GLdouble height;
	GLdouble width;
	GLdouble depth;

	GLdouble minHeight;
	GLdouble minWidth;

	GLfloat displacementTune;
} Mesh;



class Utils {
public:
	static void rotateParallelAxis(float x, float y, float z, Axis axis, float angle);

	static void rotateCamera(float angle);

	static void loadObj(char* fileName, ObjData* objData);

	static GLuint loadTextureImages(Mesh *mesh);

	static void textureMap(MAP_TYPE, Mesh *);

	static void genVertexList(Mesh *);

	static void orthoMap(Mesh*);

private:
	static void cylindrialMap(Mesh*);
};




