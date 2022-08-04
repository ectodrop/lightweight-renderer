#ifndef BVH_H
#define BVH_H
#include "pch.h";
#include <mesh3d.h>
struct Ray {
	Ray() : max_t(std::numeric_limits<float>::infinity()), origin(glm::vec3(0)), dir(glm::vec3(0)) {}
	glm::vec3 origin, dir;
	float max_t;
};

struct Bound {
	Bound(glm::vec3 aa, glm::vec3 bb) {
		bounds[0] = aa;
		bounds[1] = bb;
	}
	Bound() {
		bounds[0] = glm::vec3(std::numeric_limits<double>::infinity());
		bounds[1] = glm::vec3(-std::numeric_limits<double>::infinity());
	}

	void Union(Bound other) {
		bounds[0] = glm::min(bounds[0], other.bounds[0]);
		bounds[1] = glm::max(bounds[1], other.bounds[1]);
	}

	int LongestAxis() {
		auto extent = bounds[1] - bounds[0];
		int axis = 0;
		if (extent.y > extent.x) axis = 1;
		if (extent.z > extent.y) axis = 2;
		return axis;
	}

	float SurfaceArea() {
		return 2 * (extent().x*extent().y + extent().x*extent().z + extent().y*extent().z);
	}

	bool AABBIntersect(Ray r, float& t) {
        float tmin, tmax, tymin, tymax, tzmin, tzmax; 
		glm::vec3 invdir = 1.0f / r.dir;
		glm::bvec3 sign(invdir.x < 0, invdir.y < 0, invdir.z < 0);
        tmin = (bounds[sign[0]].x - r.origin.x) * invdir.x; 
        tmax = (bounds[1-sign[0]].x - r.origin.x) * invdir.x;
        tymin = (bounds[sign[1]].y - r.origin.y) * invdir.y;
        tymax = (bounds[1-sign[1]].y - r.origin.y) * invdir.y;
 
        if ((tmin > tymax) || (tymin > tmax)) 
            return false; 
 
        if (tymin > tmin) 
        tmin = tymin; 
        if (tymax < tmax) 
        tmax = tymax; 
 
        tzmin = (bounds[sign[2]].z - r.origin.z) * invdir.z;
        tzmax = (bounds[1-sign[2]].z - r.origin.z) * invdir.z;
 
        if ((tmin > tzmax) || (tzmin > tmax)) 
            return false; 
 
        if (tzmin > tmin) 
			tmin = tzmin; 
        if (tzmax < tmax) 
			tmax = tzmax; 
 
        t = tmin; 
 
        if (t < 0) { 
            t = tmax; 
            if (t < 0) return false; 
        } 
 
        return true; 
	}

	glm::vec3 extent() {
		return bounds[1] - bounds[0];
	}

	glm::vec3 centroid() {
		return (bounds[0] + bounds[1]) / 2.0f;
	}

	glm::vec3 bmin() {
		return bounds[0];
	}

	glm::vec3 bmax() {
		return bounds[1];
	}

	glm::vec3 bounds[2];
};


struct Primative{
	Primative(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2): vertex0(v0), vertex1(v1), vertex2(v2) {
		centroid = (v0+v1+v2)/3.0f;
		bcentroid = GetBounds().centroid();
	}
	Bound GetBounds() {
		return Bound(glm::min(vertex0, vertex1, vertex2), glm::max(vertex0, vertex1, vertex2));
	}
	bool IntersectTri(Ray ray, float &t) {
		glm::vec3 dir = ray.dir;
		glm::vec3 orig = ray.origin;
		auto v0 = vertex0, v1 = vertex1, v2 = vertex2;
		// compute plane's normal
		glm::vec3 v0v1 = v1 - v0;
		glm::vec3 v0v2 = v2 - v0;
		// no need to normalize
		glm::vec3 N = glm::cross(v0v1, v0v2);  //N 
		float area2 = N.length();

		// Step 1: finding P

		// check if ray and plane are parallel ?
		float NdotRayDirection = glm::dot(N, dir);
		if (fabs(NdotRayDirection) <= std::numeric_limits<float>::epsilon())  //almost 0 
			return false;  //they are parallel so they don't intersect ! 

		// compute d parameter using equation 2
		float d = glm::dot(-N,v0);

		// compute t (equation 3)
		t = -(glm::dot(N, orig) + d) / NdotRayDirection;

		// check if the triangle is in behind the ray
		if (t < 0) return false;  //the triangle is behind 

		// compute the intersection point using equation 1
		glm::vec3 P = orig + t * dir;

		// Step 2: inside-outside test
		glm::vec3 C;  //vector perpendicular to triangle's plane 

		// edge 0
		glm::vec3 edge0 = v1 - v0;
		glm::vec3 vp0 = P - v0;
		C = glm::cross(edge0,vp0);
		if (glm::dot(N,C) < 0) return false;  //P is on the right side 

		// edge 1
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 vp1 = P - v1;
		C = glm::cross(edge1,vp1);
		if (glm::dot(N,C) < 0)  return false;  //P is on the right side 

		// edge 2
		glm::vec3 edge2 = v0 - v2;
		glm::vec3 vp2 = P - v2;
		C = glm::cross(edge2,vp2);
		if (glm::dot(N,C) < 0) return false;  //P is on the right side; 

		return true;  //this ray hits the triangle
	}
	glm::vec3 vertex0, vertex1, vertex2, centroid, bcentroid;
};


struct BVHNode {
	BVHNode() {}
	BVHNode(unsigned int start, unsigned int num,
		int l, int r) : start_prim(start), num_prims(num), right(r) {}
	BVHNode(unsigned int start, unsigned int num) : start_prim(start), num_prims(num), right(-1) {}
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
	std::vector<int> _indicies;
	std::vector<Primative> _primatives;
	std::vector<std::unique_ptr<BVHNode>> _nodes;
	int _max_prims;
public: 
	BVH(std::vector<Triangle> primatives, std::vector<glm::vec3> vertices, int max_prims = 1) {
		for (int i = 0; i < primatives.size(); i++) {
			_indicies.push_back(i);
			auto prim = primatives[i];
			_primatives.push_back(Primative(vertices[prim.v0], vertices[prim.v1], vertices[prim.v2]));
		}
		_nodes = std::vector<std::unique_ptr<BVHNode>>();
		_max_prims = max_prims;
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
						auto& prim = _primatives[_indicies[i]];
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

		GLuint index_tbo, indices_tbo_tex;
		glGenBuffers(1, &index_tbo);
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_TEXTURE_BUFFER, index_tbo);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(GLint) * _indicies.size(), _indicies.data(), GL_STATIC_DRAW);
		glGenTextures(1, &indices_tbo_tex);
		glBindTexture(GL_TEXTURE_BUFFER, indices_tbo_tex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, index_tbo);
		shader.setInt("indicies", 0);
	}

	void BuildBVH() {
		_indicies.clear();
		for (int i = 0; i < _primatives.size(); i++) {
			_indicies.push_back(i);
		}
		Subdivide(0, _indicies.size());

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
			int index = _indicies[i];
			b.Union(_primatives[index].GetBounds());
		}
		int axis = b.LongestAxis();
		float mid = (b.bmin()[axis] + b.bmax()[axis]) / 2.0f;
		_nodes[node_index]->bounds = b;
		for (auto ptr = _indicies.begin() + start; ptr < _indicies.begin() + end; ptr++) {
			auto tri = _primatives[*ptr];
			//std::cout << *ptr << ": " << glm::to_string(tri.vertex0) << " | " << glm::to_string(tri.vertex1) << " | " << glm::to_string(tri.vertex2) << "|" << glm::to_string(tri.centroid) << std::endl;
		}
		int num_buckets = 12;
		int prim_mid = (start + end)/2;
		if (n_prims <= 6) {
			std::nth_element(_indicies.begin() + start, _indicies.begin() + prim_mid, _indicies.begin() + end,
				[=](int index0, int index1) {
					return _primatives[index0].centroid[axis] < _primatives[index1].centroid[axis];
				});
		}
		else {
			std::vector<BBucket> buckets(num_buckets);
			for (int i = start; i < end; i++) {
				auto primitive = _primatives[_indicies[i]];
				float extent_len = b.extent()[axis];
				float normalized_position = (primitive.centroid[axis] - b.bmin()[axis])/extent_len;
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
			glm::vec3 aa = b.bmin(), bb = b.bmax();
			auto it = std::partition(_indicies.begin() + start, _indicies.begin() + end,
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
			prim_mid = it - _indicies.begin();
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

#endif
