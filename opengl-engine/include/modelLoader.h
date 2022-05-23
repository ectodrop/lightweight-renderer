#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>

void loadModel(std::string path) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  std::cout << scene->mRootNode->mNumMeshes << scene->mRootNode->mNumChildren << std::endl;
  std::cout << scene->mNumMeshes << std::endl;
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
  }
  //for (int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
  //  printf("%f %f %f\n", scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z);
  //}
}

#endif