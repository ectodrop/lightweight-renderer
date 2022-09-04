#pragma once
#include "pch.h"
#include "./material.h"

class Mesh3D {
public:
    glm::vec3 _Position, _Euler_Angles, _Scale;
    glm::mat4 _Model_Matrix = glm::mat4(1.0f);
    std::string _Mesh_Name;
    bool _Visible;
    Material _Mesh_Material;

    Mesh3D(glm::vec3 position, glm::vec3 euler_angles, glm::vec3 scale) {
        Init(position, euler_angles, scale);
    }

    Mesh3D() {
        Init(glm::vec3(0), glm::vec3(0), glm::vec3(1));
    }

    void Init(glm::vec3 position, glm::vec3 euler_angles, glm::vec3 scale) {
        _Position = position;
        _Euler_Angles = euler_angles;
        _Scale = scale;
    }

    glm::mat4 GetLocalTransform() {
        glm::mat4 rotateX = glm::rotate(glm::mat4(1), glm::radians(_Euler_Angles.x), glm::vec3(1, 0, 0));
        glm::mat4 rotateY = glm::rotate(glm::mat4(1), glm::radians(_Euler_Angles.y), glm::vec3(0, 1, 0));
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1), glm::radians(_Euler_Angles.z), glm::vec3(0, 0, 1));

        glm::mat4 rotation = rotateY * rotateX * rotateZ;

        return glm::translate(glm::mat4(1), _Position) * rotation * glm::scale(glm::mat4(1), _Scale);
    }

    void ResetModelMatrix() {
        _Model_Matrix = GetLocalTransform();
    }

    void UpdateModelMatrix(glm::mat4 parent_transform) {
        _Model_Matrix = parent_transform * GetLocalTransform();
    }
};
