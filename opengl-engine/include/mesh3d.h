#ifndef MESH3D_H

#define MESH3D_H
#include <shader.h>
#include <camera.h>
#include <texture.h>

#include "pch.h"
typedef struct _triangle {
    unsigned int v0, v1, v2;
} Triangle;

class Mesh3D {
protected:
    glm::vec3 _position, _euler_angles, _scale;
public:
    glm::mat4 _Model_Matrix = glm::mat4(1.0f);
    int _ID;
    Mesh3D(glm::vec3 position, glm::vec3 euler_angles, glm::vec3 scale) {
        Init(position, euler_angles, scale);
    }

    Mesh3D() {
        Init(glm::vec3(0), glm::vec3(0), glm::vec3(1));
    }

    void Init(glm::vec3 position, glm::vec3 euler_angles, glm::vec3 scale) {
        this->_position = position;
        this->_euler_angles = euler_angles;
        this->_scale = scale;
    }

    glm::mat4 GetLocalTransform() {
        glm::mat4 rotateX = glm::rotate(glm::mat4(1), glm::radians(_euler_angles.x), glm::vec3(1, 0, 0));
        glm::mat4 rotateY = glm::rotate(glm::mat4(1), glm::radians(_euler_angles.y), glm::vec3(0, 1, 0));
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1), glm::radians(_euler_angles.z), glm::vec3(0, 0, 1));

        glm::mat4 rotation = rotateY * rotateX * rotateZ;

        return glm::translate(glm::mat4(1), _position) * rotation * glm::scale(glm::mat4(1), _scale);
    }

    void ResetModelMatrix() {
        _Model_Matrix = GetLocalTransform();
    }

    void UpdateModelMatrix(glm::mat4 parent_transform) {
        _Model_Matrix = parent_transform * GetLocalTransform();
    }
};

#endif