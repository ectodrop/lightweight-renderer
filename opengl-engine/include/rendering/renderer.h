#pragma once
#include "pch.h"
#include "camera.h"
#include "scene.h"

class Renderer {
protected:
	Scene* _Scene;
	Shader* _Shader;
	std::vector<glm::vec3> _Vertices;
	std::vector<glm::vec3> _Normals;
	std::vector<glm::vec3> _Texcoords;
	std::vector<Triangle> _Triangles;
	std::vector<GLuint> _Textures;
public:
	virtual void RenderScene() = 0;
	virtual void Init() = 0;
	virtual GLuint GetScreenTexture() = 0;
	virtual void ResizeTexture(int width, int height) = 0;
	void SetData(
		std::vector<glm::vec3> vert,
		std::vector<glm::vec3> norm,
		std::vector<glm::vec3> texc,
		std::vector<Triangle> tris,
		std::vector<GLuint> texs) {
		_Vertices = vert;
		_Normals = norm;
		_Texcoords = texc;
		_Triangles = tris;
		_Textures = texs;
	}
};