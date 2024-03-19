#include "DrawTest.h"
#include "ShaderManager.h"
#include <glm/glm.hpp>
#include "MatrixManager.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>


 void DrawTest::draw(long t)  {
	
	GLint ModelLoc = glGetUniformLocation(this->meshes[0].shaderProgramId, "Model");
	GLint ViewLoc = glGetUniformLocation(this->meshes[0].shaderProgramId, "View");
	GLint projectionLoc = glGetUniformLocation(this->meshes[0].shaderProgramId, "projection");
	GLint viewPos = glGetUniformLocation(this->meshes[0].shaderProgramId, "viewPos");
	ShaderManager::activateProgram(this->meshes[0].shaderProgramId);
	glMatrixMode(GL_MODELVIEW);

	loadTextures(&this->meshes[0]);

	glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(MatrixManager::getCTM()));
	glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(Camera::viewMatrix));
	
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(Camera::projectionMatrix));
	float v[] = { 0, 2, 3, 1};
	glUniform4fv(viewPos, 1, v);

	

	/*glUniform4fv(glGetUniformLocation(this->meshes[0].shaderProgramId, "material.ambient"), 1,robotLowerBody1_mat_ambient);
	glUniform4fv(glGetUniformLocation(this->meshes[0].shaderProgramId, "material.diffuse"), 1,robotLowerBody1_mat_diffuse);
	glUniform4fv(glGetUniformLocation(this->meshes[0].shaderProgramId, "material.specular"), 1,robotLowerBody1_mat_specular);
	glUniform1f(glGetUniformLocation(this->meshes[0].shaderProgramId, "material.shininess"),robotLowerBody1_mat_shininess[0]);*/

	glUniform1f(glGetUniformLocation(this->meshes[0].shaderProgramId, "tuneDisplacement"), this->meshes[0].displacementTune);


	glBindVertexArray(this->meshes[0].vaoID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboID);
	int i = 0;
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDrawElements(GL_TRIANGLES, 
		this->meshes->objdata->numIndices,
		GL_UNSIGNED_INT, (void*)this->meshes[i].startPoint);// (void*)this->allIndices[this->meshes[0].startPoint]);
	//glMatrixMode(GL_MODELVIEW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

DrawTest::DrawTest() : Drawable(1) {
	//this->meshes = (Mesh*)malloc(sizeof(Mesh));
	//meshes[0].objdata = (ObjData*)malloc(sizeof(ObjData));
	


	this->meshes[0].vertFileName = (char *)"a3.vert";
	this->meshes[0].fragFileName = (char*)"a3.frag";
	
	this->meshes[0].mapType = UV_ORTHO;
	this->meshes[0].displacementTune  = 4;

	this->numMeshes = 1;
	this->meshes[0].shaderProgramId = ShaderManager::createProgram(this->meshes[0].vertFileName, this->meshes[0].fragFileName);
	loadMeshes();

}
