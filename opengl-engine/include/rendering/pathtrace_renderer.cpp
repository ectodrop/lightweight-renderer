#pragma once
#include "pch.h"
#include "renderer.h"


enum StorageObjects {
	VertexData = 0,
	BVHNodes,
	IndexData,
	TriangleData,
	MaterialData,
	LightData,
};


class PathtraceRenderer : public Renderer {
	using Renderer::Renderer;
	GLuint _tex_output = 0;
	GLuint _tex_accum = 0;
	int count = 0;
public:
	PathtraceRenderer(Scene* scene) {
		CompileShaders();
		_Scene = scene;
		Init();
	}

	void Init() override {
		InitScreenTexture();
	}

	void BuildScene() {
		InitScreenTexture();
		//_Scene->UpdateMeshMatrices(0);
		_bvh = new BVH(_Triangles, _Vertices, _Scene);
		_bvh->BuildBVH();
		FormatForShader();
		count = 0;
	}

	void RenderScene() override {
		std::cout << count++ << std::endl;
		// auto t1 = std::chrono::high_resolution_clock::now();
		_Shader->setMat4("clipToWorld", false, _Scene->GetCamera()->ClipToWorldSpace());
		_Shader->setVec3("camera_position", _Scene->GetCamera()->GetPosition());
		_Shader->setFloat("u_time", glfwGetTime());
		_Shader->setBool("camera_moved", _Scene->GetCamera()->MovedThisFrame());
		_Shader->useCompute(_Scene->GetCamera()->_Width/4, _Scene->GetCamera()->_Height/4, 1);
		//glFinish();
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//auto t2 = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		//std::cout << "Render: " << ms_double.count() << "ms" << std::endl;
		_Scene->GetCamera()->SetMovedFlag(false);
	}

	void CompileShaders() override {
		if (_Shader != nullptr) delete _Shader;
		_Shader = new Shader("shaders/ray.comp");
	}

	GLuint GetScreenTexture() override {
		return _tex_output;
	}

private:
	void InitScreenTexture() {
		
		glGenTextures(1, &_tex_output);
		glBindTexture(GL_TEXTURE_2D, _tex_output);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _Scene->GetCamera()->_Width, _Scene->GetCamera()->_Height, 0, GL_RGBA, GL_FLOAT,
			NULL);
		glBindImageTexture(0, _tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glGenTextures(1, &_tex_accum);
		glBindTexture(GL_TEXTURE_2D, _tex_accum);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _Scene->GetCamera()->_Width, _Scene->GetCamera()->_Height, 0, GL_RGBA, GL_FLOAT,
			NULL);
		glBindImageTexture(1, _tex_accum, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}

	void FormatForShader() {
		_Shader->use();
		auto indices = _bvh->GetIndices();
		auto b_nodes = _bvh->GetNodes();

		struct s_node {
			int start;
			int num;
			int left;
			int right;
			glm::vec4 bmin;
			glm::vec4 bmax;
		};
		static std::vector<s_node> nodes;
		nodes.clear();
		for (int i = 0; i < b_nodes.size(); i++) {
			auto bnode = b_nodes[i];
			s_node n;
			n.start = bnode->start_prim;
			n.num = bnode->num_prims;
			n.left = bnode->left;
			n.right = bnode->right;
			n.bmin = glm::vec4(bnode->bounds.bmin, 0);
			n.bmax = glm::vec4(bnode->bounds.bmax, 0);
			nodes.push_back(n);
		}
		GLuint nodes_ssbo;
		glGenBuffers(1, &nodes_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, nodes_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(s_node)* nodes.size(), nodes.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, StorageObjects::BVHNodes, nodes_ssbo);

		GLuint index_ssbo;
		glGenBuffers(1, &index_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, index_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int)* indices.size(), indices.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, StorageObjects::IndexData, index_ssbo);

		struct s_vertex {
			glm::vec4 vertex = glm::vec4(0.0f);
			glm::vec4 normal = glm::vec4(0.0f);
			glm::vec4 tangent = glm::vec4(0.0f);
			glm::vec4 bitangent = glm::vec4(0.0f);
			glm::vec4 texcoord = glm::vec4(0.0f);
		};

		static std::vector<s_vertex> vertices;
		vertices.resize(_Vertices.size());

		struct s_triangle {
			unsigned int v0, v1, v2, mesh_id;
		};
		static std::vector<s_triangle> triangles;
		triangles.clear();
		for (int i = 0; i < _Triangles.size(); i++) {
			s_triangle t;
			t.v0 = _Triangles[i].v0;
			t.v1 = _Triangles[i].v1;
			t.v2 = _Triangles[i].v2;
			t.mesh_id = _Scene->_Triangle_Mesh_Map[i];
			triangles.push_back(t);
			glm::mat4 model = _Scene->_Meshes[t.mesh_id]->_Model_Matrix;
			vertices[t.v0].vertex = model * glm::vec4(_Vertices[t.v0], 1);
			vertices[t.v1].vertex = model * glm::vec4(_Vertices[t.v1], 1);
			vertices[t.v2].vertex = model * glm::vec4(_Vertices[t.v2], 1);

			auto ti_model = glm::transpose(glm::inverse(model));
			vertices[t.v0].normal = glm::normalize(ti_model * glm::vec4(_Normals[t.v0], 1));
			vertices[t.v1].normal = glm::normalize(ti_model * glm::vec4(_Normals[t.v1], 1));
			vertices[t.v2].normal = glm::normalize(ti_model * glm::vec4(_Normals[t.v2], 1));
			
			vertices[t.v0].tangent = glm::normalize(ti_model * glm::vec4(_Tangents[t.v0], 1));
			vertices[t.v1].tangent = glm::normalize(ti_model * glm::vec4(_Tangents[t.v1], 1));
			vertices[t.v2].tangent = glm::normalize(ti_model * glm::vec4(_Tangents[t.v2], 1));

			vertices[t.v0].bitangent = glm::normalize(ti_model * glm::vec4(_Tangents[t.v0], 1));
			vertices[t.v1].bitangent = glm::normalize(ti_model * glm::vec4(_Bitangents[t.v1], 1));
			vertices[t.v2].bitangent = glm::normalize(ti_model * glm::vec4(_Bitangents[t.v2], 1));
		}

		GLuint vertex_ssbo;
		glGenBuffers(1, &vertex_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(s_vertex)* vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, StorageObjects::VertexData, vertex_ssbo);

		GLuint triangles_ssbo;
		glGenBuffers(1, &triangles_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangles_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(s_triangle)* triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, StorageObjects::TriangleData, triangles_ssbo);


		struct s_material {
			// texture indexes stored in 4th index
			glm::vec4 diffuse, specular, normal, emissive;
		};
		static std::vector<s_material> materials;
		materials.clear();
		for (int i = 0; i < _Scene->_Meshes.size(); i++) {
			s_material mat;
			auto mesh_mat = _Scene->_Meshes[i]->_Mesh_Material;
			mat.diffuse = glm::vec4(mesh_mat.Diffuse, mesh_mat.Diffuse_Index);
			mat.specular = glm::vec4(mesh_mat.Specular, mesh_mat.Specular_Index);
			mat.normal = glm::vec4(mesh_mat.Normal, mesh_mat.Normal_Index);
			mat.emissive = glm::vec4(mesh_mat.Emissive, mesh_mat.Emissive_Index);
			materials.push_back(mat);
		}
		GLuint material_ssbo;
		glGenBuffers(1, &material_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, material_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(s_material)* materials.size(), materials.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, StorageObjects::MaterialData, material_ssbo);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	}
};