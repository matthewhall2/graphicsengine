#pragma once
#include <iostream>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"

class MatrixManager {
public:

    MatrixManager();

    static void push();

    static void pop();

    static void translate(GLdouble tx, GLdouble ty, GLdouble tz);

    static void rotate(float angle, GLdouble axisX, GLdouble axisY, GLdouble axisZ);

    static void scale(GLdouble sX, GLdouble SY, GLdouble sZ);

    static glm::mat4 getCTM();

    static void loadIdentity();
        

private:
    static std::stack<glm::mat4> matrixStack;
};



