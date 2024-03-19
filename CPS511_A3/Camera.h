#pragma once
#include <glm/glm.hpp>
#include "utils.h"
class Camera
{
public:
	static void lookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble lookX, GLdouble lookY, GLdouble lookZ, GLdouble upX, GLdouble upY, GLdouble upZ);
	static glm::mat4 viewMatrix;
	static glm::mat4 projectionMatrix;
	static void setProj(GLdouble fov, GLdouble apsect, GLdouble nearPlane, GLdouble farPlane);

	static glm::vec3 camPos;
	static glm::vec3 lookingAt;

	static void rotate(GLfloat angle, GLfloat axisX, GLfloat axisY, GLfloat axisZ);
	static void zoom(GLfloat);

private:
	GLfloat viewRadius;
	GLfloat viewPhi;
	GLfloat viewTheta;
};



