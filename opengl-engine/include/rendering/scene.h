#pragma once
#include "pch.h"
#include "./material.h"
#include "./mesh3d.h"
#include "./triangle.h"
#include "./camera.h"
#include <accel/bvh.h>

class Scene {
private:
    Camera* _camera;
    
public:
    std::vector<Mesh3D*> _Meshes;
    std::map<int, std::vector<int>> _Scene_Tree;// mesh id -> children in scene heirarchy
    std::vector<int> _Triangle_Mesh_Map;

    Scene() {
        _camera = new Camera(glm::vec3(1.0f, 1.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            INITIAL_WIDTH,
            INITIAL_HEIGHT,
            45.0f);
    }
    
    Camera* GetCamera() {
        return _camera;
    }

    void AddMeshChild(int mesh_id, int child) {
        _Scene_Tree[mesh_id].push_back(child);
    }

    int MakeMesh(const char * name) {
        auto mesh = new Mesh3D();
        int mesh_id = _Meshes.size();
        mesh->_Mesh_Name = std::string(name);
        _Scene_Tree[mesh_id] = std::vector<int>();
        _Meshes.push_back(mesh);
        return mesh_id;
    }

    void AddMaterial(int mesh_id, Material mat) {
        _Meshes[mesh_id]->_Mesh_Material = mat;
    }

    void UpdateMeshMatrices(int parent_id) {
        for (int i = 0; i < _Scene_Tree[parent_id].size(); i++) {
            // update the child model matrix with the parent model matrix
            _Meshes[_Scene_Tree[parent_id][i]]->UpdateModelMatrix(_Meshes[parent_id]->_Model_Matrix);
            UpdateMeshMatrices(_Scene_Tree[parent_id][i]);
        }
    }
};
