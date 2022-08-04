#ifndef SCENE_H
#define SCENE_H
#include <mesh3d.h>
#include <bvh.h>
#include "pch.h"

class Scene {
private:
    int traverseScene(aiNode *root_node, const aiScene *scene) {
        int parent = MakeMesh();
        for (int i = 0; i < root_node->mNumChildren; i++) {
            traverseScene(root_node->mChildren[i], scene);
        }
        for (int i = 0; i < root_node->mNumMeshes; i++) {
            int child = MakeMesh();
            AddMeshChild(parent, child);
            addGeometry(scene->mMeshes[i], scene, child);
        }

        return parent;
    }

    void addGeometry(aiMesh *mesh, const aiScene* scene, int mesh_id) {
        int num_vertices = _Vertices.size();
        for (int v = 0; v < mesh->mNumVertices; v++) {
            glm::vec3 pos(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            _Vertices.push_back(pos);

            if (mesh->HasNormals()) {
                glm::vec3 normal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
                _Normals.push_back(normal);
            }

            if (mesh->HasTextureCoords(0)) {
                auto tex = mesh->mTextureCoords[0][v];
                glm::vec3 texcoord(tex.x, tex.y, tex.z);
                _Texcoords.push_back(texcoord);
            }
        }

        for (int f = 0; f < mesh->mNumFaces; f++) {
            Triangle t;
            auto indicies = mesh->mFaces[f].mIndices;
            // offset the indicies by how many vertices are already in the list
            // this is to be able to fit the indicies and vertices into 2 lists
            t = { indicies[0] + num_vertices, indicies[1] + num_vertices, indicies[2] + num_vertices };
            // printf("%d/%d/%d\n", t.v0, t.v1, t.v2);
            _Triangles.push_back(t);
            _Triangle_Mesh_Map.push_back(mesh_id);
        }
    }
public:
	std::vector<glm::vec3> _Vertices;
    std::vector<glm::vec3> _Normals;
    std::vector<glm::vec3> _Texcoords;
	std::vector<Triangle> _Triangles;
    std::vector<std::unique_ptr<Mesh3D>> _Meshes; 
    std::map<int, std::vector<int>> _Scene_Tree;// mesh id -> children in scene heirarchy
    std::vector<int> _Triangle_Mesh_Map;
    BVH *tree;

	void LoadScene(const char * path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenUVCoords);
        std::cout << scene->mNumMaterials << " " << scene->mNumTextures << std::endl;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        if (scene->mRootNode != NULL) {
            int mesh_id = traverseScene(scene->mRootNode, scene);
            _Meshes[mesh_id]->ResetModelMatrix();
            UpdateMeshMatrices(mesh_id);
        }

        tree = new BVH(_Triangles, _Vertices);
        tree->BuildBVH();

    }

    void AddMeshChild(int mesh_id, int child) {
        _Scene_Tree[mesh_id].push_back(child);
    }

    int MakeMesh() {
        auto mesh = std::make_unique<Mesh3D>();
        int mesh_id = _Meshes.size();
        mesh->_ID = mesh_id;
        _Scene_Tree[mesh_id] = std::vector<int>();
        _Meshes.push_back(std::move(mesh));
        return mesh_id;
    }

    void UpdateMeshMatrices(int parent_id) {
        for (int i = 0; i < _Scene_Tree[parent_id].size(); i++) {
            // update the child model matrix with the parent model matrix
            _Meshes[_Scene_Tree[parent_id][i]]->UpdateModelMatrix(_Meshes[parent_id]->_Model_Matrix);
            UpdateMeshMatrices(_Scene_Tree[parent_id][i]);
        }
    }

    void LoadModel(std::string path) {
        
    }
};

#endif