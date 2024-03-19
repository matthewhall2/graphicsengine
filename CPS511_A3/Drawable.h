#include "utils.h"
#define MAX_MESHES 20

class Drawable {
public: 
	
	virtual void draw(long t);
	GLuint* allIndices;

	Drawable(int numMeshes);

	Mesh* meshes;
	void loadMeshes();
	int numMeshes;
	GLuint vboID;
	GLuint eboID;
	static void setNames(Mesh* mesh, const char* objName, const char* textName, MAP_TYPE mapType);
	static void loadTextures(Mesh *);

	void setDisplacementTuners(GLfloat*, int);

	static void drawMesh(Mesh*, Drawable *d);

	void sameShaderForAll();

	void assignShaders();
	void buildBigArray();
	void setupVAOS();
	GLuint totalVertices;
	GLuint totalIndices;
	bool hasHealth;
	int health;


private:
	
	Vertex* allVertices;
	
};

