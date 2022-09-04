#pragma once
#include "pch.h";
#include "bounds.h"
#include "primative.h"
#include <rendering/triangle.h>
#include <rendering/scene.h>

struct BVHNode {
	BVHNode(unsigned int start, unsigned int num,
		int l, int r) : start_prim(start), num_prims(num), left(l), right(r) {}
	BVHNode(unsigned int start, unsigned int num) : start_prim(start), num_prims(num), left(-1), right(-1) {}
	unsigned int start_prim, num_prims;
	int left, right; // the left child is guarenteed to be the next index (if it exists)
	Bound bounds;
};
struct BBucket {
	int count;
	Bound bounds;
};

class BVH {
public: 
	BVH(std::vector<Triangle> triangles, std::vector<glm::vec3> vertices, Scene *scene) {
		for (int i = 0; i < triangles.size(); i++) {
			auto tri = triangles[i];
			auto mesh = scene->_Meshes[scene->_Triangle_Mesh_Map[i]];
			auto model = mesh->_Model_Matrix;
			_primatives.push_back(Primative(
				ApplyMatrix(model, vertices[tri.v0]),
				ApplyMatrix(model, vertices[tri.v1]),
				ApplyMatrix(model, vertices[tri.v2]))
			);
			if (mesh->_Visible) {
				_indices.push_back(i);
			}
		}
		_nodes = std::vector<BVHNode *>();
	}

	std::vector<int> GetIndices() {
		return _indices;
	}

	std::vector<BVHNode*> GetNodes() {
		return _nodes;
	}

	/// <summary>
	/// returns the index of the triangle that intersects, -1 if none
	/// </summary>
	/// <param name="ray"></param>
	/// <returns></returns>
	int BVHIntersect(Ray ray) {
		if (_nodes.size() == 0) return -1;

		float min_dist = 10000000;
		int node_index = -1;
		int min_node = -1;
		int nodes_to_visit[64];
		int current_node = 0;
		nodes_to_visit[current_node] = 0;
		current_node++;
		while (current_node > 0) {
			current_node--;
			node_index = nodes_to_visit[current_node];
			BVHNode* node = _nodes[node_index];
			float dist;
			if (node->bounds.AABBIntersect(ray, dist) && dist < min_dist) {
				if (node->num_prims <= 1) {
					auto& prim = _primatives[_indices[node->start_prim]];
					if (prim.IntersectTri(ray, dist) && dist < min_dist) {
						min_dist = dist;
						min_node = node_index;
					}
				}
				else {
					if (node->left > 0) {
						nodes_to_visit[current_node] = node->left;
						current_node++;
					}

					if (node->right > 0) {
						nodes_to_visit[current_node] = node->right;
						current_node++;
					}
				}
			}
		}
		std::cout << min_node << std::endl;
		return min_node;
	}

	void BuildBVH() {
		Subdivide(0, _indices.size());
	}

private:
	std::vector<int> _indices;
	std::vector<Primative> _primatives;
	std::vector<BVHNode*> _nodes;
	int _max_prims = 1;

	glm::vec3 ApplyMatrix(glm::mat4 mat, glm::vec3 vec) {
		return glm::vec3(mat * glm::vec4(vec, 1));
	}

	int Subdivide(int start, int end) {

		int n_prims = end - start;
		if (n_prims == 0) return -1;
		int node_index = _nodes.size();
		auto node = new BVHNode(start, n_prims);
		_nodes.push_back(std::move(node));

		Bound b;
		for (int i = start; i < end; i++) {
			int index = _indices[i];
			b.Union(_primatives[index].GetBounds());
		}
		int axis = b.LongestAxis();
		float mid = (b.bmin[axis] + b.bmax[axis]) / 2.0f;
		_nodes[node_index]->bounds = b;
		if (n_prims <= _max_prims) {
			return node_index;
		}
		//for (auto ptr = _indices.begin() + start; ptr < _indices.begin() + end; ptr++) {
		//	auto tri = _primatives[*ptr];
		//	//std::cout << *ptr << ": " << glm::to_string(tri.vertex0) << " | " << glm::to_string(tri.vertex1) << " | " << glm::to_string(tri.vertex2) << "|" << glm::to_string(tri.centroid) << std::endl;
		//}
		int num_buckets = 12;
		int prim_mid = (start + end) / 2;
		// if the number of remaining primatives is less than a certain amount, just divide by half
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
				float normalized_position = (primitive.centroid[axis] - b.bmin[axis]) / extent_len;
				if (extent_len == 0) normalized_position = 0;
				// clamp range to size of the array
				int bi = glm::min((int)(num_buckets * normalized_position), num_buckets - 1);
				buckets[bi].count++;
				buckets[bi].bounds.Union(primitive.GetBounds());
			}
			float min_cost = std::numeric_limits<float>::infinity();
			int best_partition = 0;
			for (int bucket = 0; bucket < num_buckets - 1; bucket++) {
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
		int right = Subdivide(prim_mid, end);
		_nodes[node_index]->left = left;
		_nodes[node_index]->right = right;

		return node_index;
	}

	int GetHeight(int node_index) {
		if (node_index >= _nodes.size() || node_index == -1) return 0;
		auto& node = _nodes[node_index];
		int height = glm::max(GetHeight(node->left), GetHeight(node->right)) + 1;
		return height;
	}
};
