#pragma once
#include "pch.h"
#include "rendering/scene.h"
#include "rendering/renderer.h"
#include "stb_image.h"


class Loader {
private:
    std::vector<glm::vec3> _vertices;
    std::vector<glm::vec3> _normals;
    std::vector<glm::vec3> _texcoords;
    std::vector<Triangle> _triangles;
    std::vector<GLuint> _textures;

    std::map<unsigned int, Material> _cached_materials;
    std::string _file_path;
    Scene* _scene;
    int TraverseScene(aiNode* root_node, const aiScene* scene) {
        int parent = _scene->MakeMesh(root_node->mName.C_Str());
        for (int i = 0; i < root_node->mNumChildren; i++) {
            auto child_node = root_node->mChildren[i];
            if (child_node->mNumChildren != 0 || child_node->mNumMeshes != 0) {
                int child = 0;
                // if the child node is a leaf node with only 1 mesh, then just reduce it to the mesh
                if (child_node->mNumChildren == 0 && child_node->mNumMeshes == 1) {
                    auto ai_mesh = scene->mMeshes[child_node->mMeshes[0]];
                    child = _scene->MakeMesh(ai_mesh->mName.C_Str());
                    //_scene->AddMaterial(child, GetMaterial(scene, ai_mesh->mMaterialIndex));
                    AddGeometry(ai_mesh, scene, child);
                }
                else {
                    child = TraverseScene(root_node->mChildren[i], scene);
                }
                _scene->AddMeshChild(parent, child);
            }
            
        }
        for (int i = 0; i < root_node->mNumMeshes; i++) {
            auto ai_mesh = scene->mMeshes[root_node->mMeshes[i]];
            int child = _scene->MakeMesh(ai_mesh->mName.C_Str());
            //_scene->AddMaterial(child, GetMaterial(scene, ai_mesh->mMaterialIndex));
            _scene->AddMeshChild(parent, child);
            AddGeometry(ai_mesh, scene, child);
        }

        return parent;
    }

    void AddGeometry(aiMesh* mesh, const aiScene* scene, int mesh_id) {
        int num_vertices = _vertices.size();
        for (int v = 0; v < mesh->mNumVertices; v++) {
            glm::vec3 pos(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            _vertices.push_back(pos);
            //printf("(%f,%f,%f)\n", pos.x, pos.y, pos.z);
            if (mesh->HasNormals()) {
                glm::vec3 normal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
                _normals.push_back(normal);
            }

            if (mesh->HasTextureCoords(0)) {
                auto tex = mesh->mTextureCoords[0][v];
                glm::vec3 texcoord(tex.x, tex.y, tex.z);
                _texcoords.push_back(texcoord);
            }
        }

        for (int f = 0; f < mesh->mNumFaces; f++) {
            Triangle t;
            auto indicies = mesh->mFaces[f].mIndices;
            // offset the indicies by how many vertices are already in the list
            // this is to be able to fit the indicies and vertices into 2 lists
            t = { indicies[0] + num_vertices, indicies[1] + num_vertices, indicies[2] + num_vertices };
            //printf("%d/%d/%d\n", t.v0, t.v1, t.v2);
            _triangles.push_back(t);
            _scene->_Triangle_Mesh_Map.push_back(mesh_id);
        }
    }

    Material GetMaterial(const aiScene* scene, unsigned int ai_material_id) {
        // cache the materials to avoid loading the same textures over again
        if (_cached_materials.count(ai_material_id)) return _cached_materials[ai_material_id];

        aiMaterial* mat = scene->mMaterials[ai_material_id];
        Material mesh_material;
        aiColor3D diffuse_col;
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_col);
        mesh_material.Diffuse = glm::vec3(diffuse_col.r, diffuse_col.g, diffuse_col.b);

        // load diffuse texture if any
        aiString texture_file;
        mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texture_file);
        if (texture_file.length) {
            std::string diffuse_path = _file_path.substr(0, _file_path.find_last_of("\\/")+1) + std::string(texture_file.C_Str());
            mesh_material.Diffuse_Index = LoadTexture(diffuse_path);
        }
        _cached_materials[ai_material_id] = mesh_material;
        return mesh_material;
    }

    int LoadTexture(std::string tex_path, int channel_type = GL_RGB) {
        
        int texture_index = _textures.size();
        stbi_set_flip_vertically_on_load(true);
        GLuint texture;
        int width, height, n_channels;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        unsigned char* data = stbi_load(tex_path.c_str(), &width, &height, &n_channels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, channel_type, width, height, 0, channel_type, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
        }
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // sets the texture wrap behaviour
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // sets how the filtering will work
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

        _textures.push_back(texture);

        return texture_index;
    }
public:
    void LoadScene(std::string path, Renderer* renderer, Scene *scene_tree) {
        Assimp::Importer importer;
        _file_path = path;
        const aiScene* ai_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_GenUVCoords);
        // std::cout << ai_scene->mNumMaterials << " " << ai_scene->mNumTextures << std::endl;
        if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        
        _scene = scene_tree;
        if (ai_scene->mRootNode != NULL) {
            int mesh_id = TraverseScene(ai_scene->mRootNode, ai_scene);
            _scene->_Meshes[mesh_id]->ResetModelMatrix();
            _scene->UpdateMeshMatrices(mesh_id);
        }
        renderer->SetData(_vertices, _normals, _texcoords, _triangles, _textures);
        renderer->Init();


        
        // tree = new BVH(_Triangles, _Vertices);
        // tree->BuildBVH();
    }
};
