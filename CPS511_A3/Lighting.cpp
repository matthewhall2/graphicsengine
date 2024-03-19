#include "Lighting.h"


GLfloat Lighting:: light1_position0[] = { 100, 100, 100, 1.0 };
GLfloat Lighting:: light1_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat Lighting:: light1_diffuse[] = { 0.1, 0.1, 0.1, 0.1 };
GLfloat Lighting:: light1_specular[] = { 0.1, 0.1, 0.1, 0.1 };
GLfloat Lighting:: light1_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

void Lighting ::addLights(Mesh * mesh) {
	glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "light.position"), 1, light1_position0);
	glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "light.ambient"), 1, light1_ambient);
	glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "light.diffuse"), 1, light1_diffuse);
	glUniform4fv(glGetUniformLocation(mesh->shaderProgramId, "light.specular"), 1, light1_specular);
}

