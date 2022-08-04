#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include "pch.h"

void TraverseTree(aiNode* root, const aiScene* scene, int depth = 0) {
    std::cout << root->mName.C_Str() << ", Depth: " << depth << std::endl;
    for (int i = 0; i < root->mNumMeshes; i++) {
        if (scene->HasMeshes()) {
            unsigned int index = root->mMeshes[i];
            aiMesh* mesh = scene->mMeshes[index];
            for (int j = 0; j < mesh->mNumFaces; j++) {
                /*for (int k = 0; k < mesh->mFaces[j].mNumIndices; k++) {
                    std::cout << mesh->mFaces[j].
                }*/
                //std::cout << mesh->mFaces[j].mIndices[0] << ", " << mesh->mFaces[j].mIndices[1] << ", " << mesh->mFaces[j].mIndices[2] << std::endl;
            }
        }
        if (scene->HasMaterials()) {
            int mat_id = scene->mMeshes[root->mMeshes[i]]->mMaterialIndex;
            std::cout << scene->mMaterials[mat_id]->GetName().C_Str() << std::endl;
        }
    }
    for (int i = 0; i < root->mNumChildren; i++) {
        TraverseTree(root->mChildren[i], scene, depth + 1);
    }
}


#endif