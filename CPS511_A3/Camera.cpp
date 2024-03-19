#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
glm::vec3 Camera::camPos = glm::vec3(0, 0, 0);
glm::mat4 Camera::viewMatrix = glm::mat4();
glm::mat4 Camera::projectionMatrix = glm::mat4();
glm::vec3 Camera::lookingAt = glm::vec3(0, 0, 0);

void Camera::lookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble lookX, GLdouble lookY, GLdouble lookZ, GLdouble upX, GLdouble upY, GLdouble upZ) {
	camPos = glm::vec3(eyeX, eyeY, eyeZ);
	lookingAt = glm::vec3(lookX, lookY, lookZ);
	viewMatrix = glm::lookAt(glm::vec3(eyeX, eyeY, eyeZ), glm::vec3(lookX, lookY, lookZ), glm::vec3(upX, upY, upZ));
}

void Camera::setProj(GLdouble fov, GLdouble aspect, GLdouble nearPlane, GLdouble farPlane) {
	projectionMatrix = glm::perspective<GLdouble>(fov, aspect, nearPlane, farPlane);
}

void Camera::rotate(GLfloat angle, GLfloat axisX, GLfloat axisY, GLfloat axisZ) {
	if (angle == 0.0f) {
		return;
	}
	viewMatrix = glm::rotate(viewMatrix, glm::radians(angle), glm::vec3(axisX, axisY, axisZ));
	camPos = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(axisX, axisY, axisZ)) * glm::vec4(camPos, 1);
}

void Camera::zoom(GLfloat z) {
	glm::vec3 norm = glm::normalize(lookingAt - camPos);
	viewMatrix = glm::translate(viewMatrix, norm * z);
	camPos = glm::translate(glm::mat4(1.0f), norm * z) * glm::vec4(camPos, 1);
}


