#pragma once
#include "pch.h";
#include "bounds.h"
#include "primative.h"
#include <rendering/triangle.h>
#include <rendering/shader.h>


enum Uniforms {
	Indices = 0,
	Vertices,
	BVHNodes,
	Triangles,
	Normals,
	Texcoords,
};

struct BVHNode {
	BVHNode(unsigned int start, unsigned int num,
		int l, int r) : start_prim(start), num_prims(num), left(l), right(r) {}
	BVHNode(unsigned int start, unsigned int num) : start_prim(start), num_prims(num), left(-1), right(-1) {}
	unsigned int start_prim, num_prims;
	int left, right; // the left child is guarenteed to be the next index
	Bound bounds;
};
struct BBucket {
	int count;
	Bound bounds;
};

class BVH {
private:
	std::vector<int> _indices;
	std::vector<Primative> _primatives;
	std::vector<Triangle> _triangles;
	std::vector<std::unique_ptr<BVHNode>> _nodes;
	std::vector<glm::vec3> _vertices;
	int _max_prims;
public: 
	BVH(std::vector<Triangle> triangles, std::vector<glm::vec3> vertices, int max_prims = 1) {
		for (int i = 0; i < triangles.size(); i++) {
			_indices.push_back(i);
			auto prim = triangles[i];
			_primatives.push_back(Primative(vertices[prim.v0], vertices[prim.v1], vertices[prim.v2]));
		}
		_nodes = std::vector<std::unique_ptr<BVHNode>>();
		_vertices = vertices;
		_max_prims = max_prims;
		_triangles = triangles;
	}

	int Intersect(Ray ray) {
		// TODO implement depth
		float min_dist = -1;
		int node_index = -1;
		std::stack<int> nodes_to_visit;
		nodes_to_visit.push(0);
		while (!nodes_to_visit.empty()) {
			int node_index = nodes_to_visit.top();
			nodes_to_visit.pop();
			auto& node = _nodes[node_index];
			float dist;
			if (node->bounds.AABBIntersect(ray, dist)) {
				if (node->num_prims <= _max_prims) {
					for (int i = node->start_prim; i < node->start_prim + node->num_prims; i++) {
						auto& prim = _primatives[_indices[i]];
						if (prim.IntersectTri(ray, dist) && dist < min_dist) {
							min_dist = dist;
							node_index = i;
						}
					}
				}
				else {
					nodes_to_visit.push(node->left);
					nodes_to_visit.push(node->right);
				}
			}
		}
		return node_index;
	}

	int GetHeight(int node_index) {
		if (node_index >= _nodes.size() || node_index == -1) return 0;
		auto& node = _nodes[node_index];
		int height = glm::max(GetHeight(node->left), GetHeight(node->right)) + 1;
		return height;
	}

	void FormatForShader(Shader &shader) {
		shader.use();

		GLuint indices_tbo, indices_tbo_tex;
		glGenBuffers(1, &indices_tbo);
		glActiveTexture(GL_TEXTURE0 + Uniforms::Indices);
		glBindBuffer(GL_TEXTURE_BUFFER, indices_tbo);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint) * _indices.size(), _indices.data(), GL_STATIC_DRAW);
		glGenTextures(1, &indices_tbo_tex);
		glBindTexture(GL_TEXTURE_BUFFER, indices_tbo_tex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, indices_tbo);
		shader.setInt("indices", Uniforms::Indices);

		GLuint vertices_tbo, vertices_tbo_tex;
		glGenBuffers(1, &vertices_tbo);
		glActiveTexture(GL_TEXTURE0 + Uniforms::Vertices);
		glBindBuffer(GL_TEXTURE_BUFFER, vertices_tbo);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);
		glGenTextures(1, &vertices_tbo_tex);
		glBindTexture(GL_TEXTURE_BUFFER, vertices_tbo_tex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vertices_tbo);
		shader.setInt("vertices", Uniforms::Vertices);

		std::vector<float> flat;
		FlattenNodeData(flat);
		GLuint nodes_tbo, nodes_tbo_tex;
		glGenBuffers(1, &nodes_tbo);
		glActiveTexture(GL_TEXTURE0 + Uniforms::BVHNodes);
		glBindBuffer(GL_TEXTURE_BUFFER, nodes_tbo);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(GLfloat) * flat.size(), flat.data(), GL_STATIC_DRAW);
		glGenTextures(1, &nodes_tbo_tex);
		glBindTexture(GL_TEXTURE_BUFFER, nodes_tbo_tex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, nodes_tbo);
		shader.setInt("BVH", Uniforms::BVHNodes);

		GLuint triangles_tbo, triangles_tbo_tex;
		glGenBuffers(1, &triangles_tbo);
		glActiveTexture(GL_TEXTURE0 + Uniforms::Triangles);
		glBindBuffer(GL_TEXTURE_BUFFER, triangles_tbo);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::ivec3) * _triangles.size(), _triangles.data(), GL_STATIC_DRAW);
		glGenTextures(1, &triangles_tbo_tex);
		glBindTexture(GL_TEXTURE_BUFFER, triangles_tbo_tex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32I, triangles_tbo);
		shader.setInt("triangles", Uniforms::Triangles);
	}

	void FlattenNodeData(std::vector<float> &flattened) {
		for (auto& node : _nodes) {
			flattened.push_back(node->start_prim);
			flattened.push_back(node->num_prims);
			flattened.push_back(node->left);
			flattened.push_back(node->right);
			flattened.push_back(node->bounds.bmin.x);
			flattened.push_back(node->bounds.bmin.y);
			flattened.push_back(node->bounds.bmin.z);
			flattened.push_back(0);
			flattened.push_back(node->bounds.bmax.x);
			flattened.push_back(node->bounds.bmax.y);
			flattened.push_back(node->bounds.bmax.z);
			flattened.push_back(0);
		}
	}

	void BuildBVH() {
		_indices.clear();
		for (int i = 0; i < _primatives.size(); i++) {
			_indices.push_back(i);
		}
		Subdivide(0, _indices.size());
		//Ray ray;
		//ray.dir = glm::vec3(0,1,0);
		//ray.origin = glm::vec3(0);
		//int tri = Intersect(ray);
	}

	int Subdivide(int start, int end) {
		
		int n_prims = end - start;
		if (n_prims == 0) return -1;
		int node_index = _nodes.size();
		auto node =  std::make_unique<BVHNode>(start, n_prims);
		_nodes.push_back(std::move(node));

		if (n_prims <= _max_prims) {
			return node_index;
		}

		Bound b;
		for (int i = start; i < end; i++) {
			int index = _indices[i];
			b.Union(_primatives[index].GetBounds());
		}
		int axis = b.LongestAxis();
		float mid = (b.bmin[axis] + b.bmax[axis]) / 2.0f;
		_nodes[node_index]->bounds = b;
		//for (auto ptr = _indices.begin() + start; ptr < _indices.begin() + end; ptr++) {
		//	auto tri = _primatives[*ptr];
		//	//std::cout << *ptr << ": " << glm::to_string(tri.vertex0) << " | " << glm::to_string(tri.vertex1) << " | " << glm::to_string(tri.vertex2) << "|" << glm::to_string(tri.centroid) << std::endl;
		//}
		int num_buckets = 12;
		int prim_mid = (start + end)/2;
		if (n_prims <= 6) {
			std::nth_element(_indices.begin() + start, _indices.begin() + prim_mid, _indices.begin() + end,
				[=](int index0, int index1) {
					return _primatives[index0].centroid[axis] < _primatives[index1].centroid[axis];
				});
		}
		else {
			std::vector<BBucket> buckets(num_buckets);
			for (int i = start; i < end; i++) {
				auto primitive = _primatives[_indices[i]];
				float extent_len = b.extent()[axis];
				float normalized_position = (primitive.centroid[axis] - b.bmin[axis])/extent_len;
				// clamp range to size of the array
				int bi = glm::min((int)(num_buckets * normalized_position), num_buckets-1);
				buckets[bi].count++;
				buckets[bi].bounds.Union(primitive.GetBounds());
			}
			float min_cost = std::numeric_limits<float>::infinity();
			int best_partition = 0;
			for (int bucket = 0; bucket < num_buckets-1; bucket++) {
				int count0 = 0, count1 = 0;
				Bound bound0, bound1;
				for (int i = 0; i <= bucket; i++) {
					bound0.Union(buckets[i].bounds);
					count0 += buckets[i].count;
				}
				for (int j = bucket + 1; j < num_buckets; j++) {
					bound1.Union(buckets[j].bounds);
					count1 += buckets[j].count;
				}
				float cost = 0.125f + (count0 * bound0.SurfaceArea() + count1 * bound1.SurfaceArea()) / b.SurfaceArea();
				if (cost < min_cost) {
					min_cost = cost;
					best_partition = bucket;
				}
			}
			glm::vec3 aa = b.bmin, bb = b.bmax;
			auto it = std::partition(_indices.begin() + start, _indices.begin() + end,
				[=](int index) {
					glm::vec3 ext = bb - aa;
					auto primitive = _primatives[index];
					float extent_len = ext[axis];
					float normalized_position = (primitive.centroid[axis] - bb[axis]) / extent_len;
					// clamp range to size of the array
					int b = glm::min((int)(num_buckets * normalized_position), num_buckets - 1);

					return b < best_partition;
				}
			);
			prim_mid = it - _indices.begin();
		}

		if (prim_mid - start == 0 || end - prim_mid == 0) {
			prim_mid = (start + end) / 2;
		}
		int left = Subdivide(start, prim_mid);
		// only track the right child because the left child is guarenteed to be node_index+1
		int right = Subdivide(prim_mid, end);
		_nodes[node_index]->left = left;
		_nodes[node_index]->right = right;
		
		
		return node_index;
	}
};
