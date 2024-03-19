#include "MatrixManager.h"

std::stack<glm::mat4> MatrixManager::matrixStack = std::stack<glm::mat4>();


    MatrixManager::MatrixManager() {
        // Initialize the stack with the identity matrix
       /// matrixStack.push(glm::mat4(1.0f));
    }

    void MatrixManager::loadIdentity() {
        if (matrixStack.size() == 0) {
            matrixStack.push(glm::mat4(1.0f));
            return;
        }
        if (matrixStack.size() > 1) {
            matrixStack.pop();
            matrixStack.push(glm::mat4(1.0f));
        }
    }

    void  MatrixManager::push() {
        // Push a copy of the current top matrix onto the stack
        matrixStack.push(matrixStack.top());
    }

    void  MatrixManager::pop() {
        // Pop the top matrix from the stack
        if (matrixStack.size() > 1) {
            matrixStack.pop();
        }
        else {
            std::cerr << "Error: Attempted to pop the last matrix from the stack." << std::endl;
        }
    }

    void  MatrixManager::translate(GLdouble tx, GLdouble ty, GLdouble tz) {
        // Translate the top matrix
        matrixStack.top() = glm::translate(matrixStack.top(), glm::vec3(tx, ty, tz));
    }

    void  MatrixManager::rotate(float angle, GLdouble axisX, GLdouble axisY, GLdouble axisZ) {
        // Rotate the top matrix
        matrixStack.top() = glm::rotate(matrixStack.top(), glm::radians(angle), glm::vec3(axisX, axisY, axisZ));
        
    }

    void  MatrixManager::scale(GLdouble sX, GLdouble sY, GLdouble sZ) {
        // Scale the top matrix
        
        matrixStack.top() = glm::scale(matrixStack.top(), glm::vec3(sX, sY, sZ));
    }

    glm::mat4 MatrixManager::getCTM() {
        //matrixStack.push(glm::mat4(1.0f));
        return matrixStack.top();
    }

