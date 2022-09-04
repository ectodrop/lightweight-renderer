#pragma once
#include "pch.h"
#include "camera.h"
#include "scene.h"
#include "shader.h"
#include <accel/bvh.h>

class Renderer {
protected:
	Scene* _Scene;
	Shader* _Shader;
	BVH* _bvh = NULL;

	std::vector<glm::vec3> _Vertices;
	std::vector<glm::vec3> _Normals;
	std::vector<glm::vec3> _Tangents;
	std::vector<glm::vec3> _Bitangents;
	std::vector<glm::vec3> _Texcoords;
	std::vector<Triangle> _Triangles;
	std::vector<GLuint> _Textures;
public:
	virtual void RenderScene() = 0;
	virtual void Init() = 0;
	virtual void CompileShaders() = 0;
	virtual GLuint GetScreenTexture() = 0;
	void SetData(
		std::vector<glm::vec3> vert,
		std::vector<glm::vec3> norm,
		std::vector<glm::vec3> tan,
		std::vector<glm::vec3> bitan,
		std::vector<glm::vec3> texc,
		std::vector<Triangle> tris,
		std::vector<GLuint> texs) {
		_Vertices = vert;
		_Normals = norm;
		_Tangents = tan;
		_Bitangents = bitan;
		_Texcoords = texc;
		_Triangles = tris;
		_Textures = texs;
	}
	BVH* GetBVH() {
		return _bvh;
	}
};